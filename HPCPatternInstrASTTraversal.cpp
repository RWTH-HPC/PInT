#include "HPCPatternInstrASTTraversal.h"
#include "Debug.h"
#include "HPCParallelPattern.h"

#include <iostream>
#include "clang/AST/RawCommentList.h"
#include "llvm/ADT/StringRef.h"
#include "clang/Basic/SourceManager.h"
#include <string>



/* 
 * Visitor function implementations
 */
bool HPCPatternInstrVisitor::VisitFunctionDecl(clang::FunctionDecl *Decl)
{
	CurrentFn = Decl;
	CurrentFnEntry = FunctionDB->Lookup(Decl);
#ifdef PRINT_DEBUG
	std::cout << CurrentFnEntry->GetFnName() << " (" << CurrentFnEntry->GetHash() << ")" << std::endl;
#endif
	PatternBeginHandler.SetCurrentFnEntry(CurrentFnEntry);
	PatternEndHandler.SetCurrentFnEntry(CurrentFnEntry);

	return true;
}

bool HPCPatternInstrVisitor::VisitCallExpr(clang::CallExpr *CallExpr)
{
	if (!CallExpr->getBuiltinCallee() && CallExpr->getDirectCallee() && !CallExpr->getDirectCallee()->isInStdNamespace()) 
	{	
		clang::FunctionDecl* Callee = CallExpr->getDirectCallee();

#ifdef PRINT_DEBUG
		std::cout << Callee->getNameInfo().getName().getAsString() << std::endl;
#endif	
	
		std::string FnName = Callee->getNameInfo().getName().getAsString();	

		// Is this a call to our pattern functions?
		if (!FnName.compare(PATTERN_BEGIN_CXX_FNNAME) || !FnName.compare(PATTERN_BEGIN_C_FNNAME))
		{
			clang::Expr** Args = CallExpr->getArgs();
#ifdef PRINT_DEBUG
			Args[0]->dump();
#endif
			PatternBeginFinder.match(*Args[0], *Context);
		}
		else if (!FnName.compare(PATTERN_END_CXX_FNNAME) || !FnName.compare(PATTERN_END_C_FNNAME))
		{
			clang::Expr** Args = CallExpr->getArgs();
#ifdef PRINT_DEBUG
			Args[0]->dump();
#endif
			PatternEndFinder.match(*Args[0], *Context);
		}
		// If no: search the called function for patterns
		else
		{
			/* Look up the database entry for this function */
			FunctionDeclDatabaseEntry* DBEntry = FunctionDB->Lookup(Callee);
#ifdef PRINT_DEBUG
			std::cout << DBEntry->GetFnName() << " (" << DBEntry->GetHash() << ")" << std::endl;
#endif
			HPCParallelPattern* Top;
			if ((Top = GetTopPatternStack()) != NULL)
			{
				Top->AddChild(DBEntry);
			}
			else
			{
				CurrentFnEntry->AddChild(DBEntry);
			}
		}
	}

	return true;
}

HPCPatternInstrVisitor::HPCPatternInstrVisitor (clang::ASTContext* Context) : Context(Context)
{
	using namespace clang::ast_matchers;	
	StatementMatcher StringArgumentMatcher = hasDescendant(stringLiteral().bind("patternstr"));	
	
	PatternBeginFinder.addMatcher(StringArgumentMatcher, &PatternBeginHandler);
	PatternEndFinder.addMatcher(StringArgumentMatcher, &PatternEndHandler);
	
	FunctionDB = FunctionDeclDatabase::GetInstance();
}



/* 
 * Consumer function implementations+
 */
void HPCPatternInstrConsumer::HandleTranslationUnit(clang::ASTContext &Context) 
{
	/* Traverse the AST for comments and parse them */
	DEBUG_MESSAGE("Using Visitor to traverse from top translation declaration unit");
	Visitor.TraverseDecl(Context.getTranslationUnitDecl());
}



/*
 * Frontend action function implementations
 */
std::unique_ptr<clang::ASTConsumer> HPCPatternInstrAction::CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile)
{
	DEBUG_MESSAGE("Creating consumer object!")	
	return std::unique_ptr<clang::ASTConsumer>(new HPCPatternInstrConsumer(&Compiler.getASTContext()));	
}
