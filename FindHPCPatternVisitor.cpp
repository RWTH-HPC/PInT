#include "FindHPCPatternVisitor.h"

#include "Debug.h"

#include <iostream> 
#include "llvm/ADT/StringRef.h"
#include "ParallelPattern.h"
#include <string>
#include "clang/AST/Expr.h"


bool FindHPCPatternVisitor::VisitCallExpr(clang::CallExpr *CallExpr)
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
			FindHPCPatternVisitor RecVisitor(Context);	
#ifdef PRINT_DEBUG
			Callee->getBody()->dump();
#endif
			RecVisitor.TraverseStmt(Callee->getBody());
		}
		else
		{
			std::cout << "Function is not defined!" << std::endl;
		}	
	}

	return true;
}


FindHPCPatternVisitor::FindHPCPatternVisitor (clang::ASTContext* Context) : Context(Context)
{
	clang::ast_matchers::StatementMatcher StringLiteralMatcher = clang::ast_matchers::hasDescendant(clang::ast_matchers::stringLiteral().bind("patternstr"));	

	Finder.addMatcher(StringLiteralMatcher, &Handler);
}
