#include "FindHPCPatternVisitor.h"

#include "Debug.h"

#include <iostream> 
#include "clang/AST/RawCommentList.h"
#include "llvm/ADT/StringRef.h"
#include "clang/Basic/SourceManager.h"

bool FindHPCPatternVisitor::VisitDecl(clang::Decl *Decl)
{
	clang::ASTContext& Ctxt = Decl->getASTContext();
	clang::SourceManager& SrcMgr = Ctxt.getSourceManager();

	clang::RawComment* RCmt = Ctxt.getRawCommentForDeclNoCache(Decl);
	if (RCmt)
	{
		Decl->dump();
		std::cout << RCmt->getRawText(SrcMgr).str() << std::endl;
	}

	/* continue */
	return true;
}
