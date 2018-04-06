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
bool HPCPatternInstrVisitor::VisitCallExpr(clang::CallExpr *CallExpr)
{
	if (!CallExpr->getBuiltinCallee() && CallExpr->getDirectCallee() && !CallExpr->getDirectCallee()->isInStdNamespace()) 
	{
#ifdef PRINT_DEBUG
		CallExpr->dumpPretty(*Context);
		std::cout << std::endl;
#endif
		
		clang::FunctionDecl* Callee = CallExpr->getDirectCallee();

#ifdef PRINT_DEBUG
		std::cout << Callee->getNameInfo().getName().getAsString() << std::endl;
#endif	
	
		std::string FnName = Callee->getNameInfo().getName().getAsString();	

		// TODO distinguish Begin and End Pattern

		// Is this a call to our pattern functions?
		if (!FnName.compare(PATTERN_BEGIN_FNNAME) || !FnName.compare(PATTERN_END_FNNAME))
		{
			clang::Expr** Args = CallExpr->getArgs();
#ifdef PRINT_DEBUG
			Args[0]->dump();
#endif
			Finder.match(*Args[0], *Context);
		}
		// If no: search the called function for patterns
		else if (Callee->isDefined())
		{
			HPCPatternInstrVisitor RecVisitor(Context);	
#ifdef PRINT_DEBUG
			Callee->getBody()->dump();
#endif
			RecVisitor.TraverseStmt(Callee->getBody());
		}
		else
		{
			DEBUG_MESSAGE("Function is not defined!");
		}	
	}

	return true;
}


HPCPatternInstrVisitor::HPCPatternInstrVisitor (clang::ASTContext* Context) : Context(Context)
{
	using namespace clang::ast_matchers;	

	StatementMatcher StringLiteralMatcher = hasDescendant(stringLiteral().bind("patternstr"));	

	Finder.addMatcher(StringLiteralMatcher, &Handler);
}



/* 
 * Consumer function implementations+
 */
void HPCPatternInstrConsumer::HandleTranslationUnit(clang::ASTContext &Context) 
{
	/* Traverse the AST for comments and parse them */
	DEBUG_MESSAGE("Using Visitor to traverse from top translation declaration unit");
	Visitor.TraverseDecl(Context.getTranslationUnitDecl());

	/* Print raw comment list for checking */
#ifdef PRINT_DEBUG
	clang::SourceManager& SrcMgr = Context.getSourceManager();

	clang::RawCommentList &RCmtLst = Context.getRawCommentList();
	llvm::ArrayRef<clang::RawComment*> RCmts = RCmtLst.getComments();
	
	if (!RCmts.empty())
	{
		llvm::ArrayRef<clang::RawComment*>::iterator RCmt;

		std::cout << "List of raw comments in this translation unit:" << std::endl;

		for (RCmt = RCmts.begin(); RCmt != RCmts.end(); RCmt++)
		{
			llvm::StringRef RTxt = (*RCmt)->getRawText(SrcMgr);
			std::cout << RTxt.str() << std::endl;
		}
	}
#endif
}



/*
 * Frontend action function implementations
 */
std::unique_ptr<clang::ASTConsumer> HPCPatternInstrAction::CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile)
{
	DEBUG_MESSAGE("Creating consumer object!")
	
	return std::unique_ptr<clang::ASTConsumer>(new HPCPatternInstrConsumer(&Compiler.getASTContext()));	
}
