#include "FindHPCPatternConsumer.h"

#include "Debug.h"

#include <iostream>

#include "clang/AST/RawCommentList.h"
#include "llvm/ADT/StringRef.h"
#include "clang/Basic/SourceManager.h"



void FindHPCPatternConsumer::HandleTranslationUnit(clang::ASTContext &Context) 
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
