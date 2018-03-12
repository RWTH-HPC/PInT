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
