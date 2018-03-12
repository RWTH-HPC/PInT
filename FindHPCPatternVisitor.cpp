#include "FindHPCPatternVisitor.h"

#include "Debug.h"

#include <iostream> 
#include "llvm/ADT/StringRef.h"
#include "ParallelPattern.h"

bool FindHPCPatternVisitor::VisitPragmaCommentDecl(clang::PragmaCommentDecl *CmtDecl)
{	
#ifdef PRINT_DEBUG
	CmtDecl->dump();
#endif

	llvm::StringRef arg = CmtDecl->getArg();
	std::cout << arg.str() << std::endl;
	
	ParallelPattern pattern = ParallelPattern(arg.str());
	
	return true;
}

bool FindHPCPatternVisitor::VisitCallExpr(clang::CallExpr *CallExpr)
{
	if (!CallExpr->getBuiltinCallee() && CallExpr->getDirectCallee() && !CallExpr->getDirectCallee()->isInStdNamespace()) 
	{
#ifdef PRINT_DEBUG
		CallExpr->dumpPretty(*Context);
#endif
		
		if (CallExpr->getDirectCallee()->isDefined())
		{
			FindHPCPatternVisitor RecVisitor(Context);
			RecVisitor.VisitDecl(CallExpr->getDirectCallee()->getDefinition());
		}
		else
		{
			std::cout << "Function is not defined!" << std::endl;
		}	
	}

	return true;
}
