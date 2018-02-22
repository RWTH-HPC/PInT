#include "FindHPCPatternConsumer.h"

#include "Debug.h"

#include <iostream>

#include "clang/AST/RawCommentList.h"
#include "llvm/ADT/StringRef.h"
#include "clang/Basic/SourceManager.h"



void FindHPCPatternConsumer::HandleTranslationUnit(clang::ASTContext &Ctxt) 
{
	/* Traverse the AST for comments and parse them */
	DEBUG_MESSAGE("Using Visitor to traverse from top translation declaration unit");
	Visitor.TraverseDecl(Ctxt.getTranslationUnitDecl());

	/* Print raw comment list for checking */
#ifdef PRINT_DEBUG
	clang::SourceManager& SrcMgr = Ctxt.getSourceManager();

	clang::RawCommentList &RCmtLst = Ctxt.getRawCommentList();
	llvm::ArrayRef<clang::RawComment*> RCmts = RCmtLst.getComments();
	
	if (!RCmts.empty())
	{
		llvm::ArrayRef<clang::RawComment*>::iterator RCmt;

		Decl->dump();
		std::cout << "List of raw comments attachted to this declaration:" << std::endl;

		for (RCmt = RCmts.begin(); RCmt != RCmts.end(); RCmt++)
		{
			llvm::StringRef RTxt = (*RCmt)->getRawText(SrcMgr);
			std::cout << RTxt.str() << std::endl;
		}
	}
#endif
}
