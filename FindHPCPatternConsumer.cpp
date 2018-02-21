#include "FindHPCPatternConsumer.h"

#include <iostream>

#include "clang/AST/RawCommentList.h"
#include "llvm/ADT/StringRef.h"
#include "clang/Basic/SourceManager.h"



void FindHPCPatternConsumer::HandleTranslationUnit(clang::ASTContext &Context) 
{
	/* Traverse the AST for comments and parse them */
	/* Visitor.TraverseDecl(Context.getTranslationUnitDecl()); */

	/* Print raw comment list for checking */
	clang::SourceManager &SrcManager = Context.getSourceManager();
	clang::RawCommentList &RawCommentList = Context.getRawCommentList();

	llvm::ArrayRef<clang::RawComment*> RawComments = RawCommentList.getComments();
	llvm::ArrayRef<clang::RawComment*>::iterator RawComment;

	/* TODO find out why there is no output */
	Context.PrintStats();

	std::cout << "List of raw comments contains " << RawComments.size() << " entries." << std::endl;

	for (RawComment = RawComments.begin(); RawComment != RawComments.end(); RawComment++)
	{
		llvm::StringRef RawText = (*RawComment)->getRawText(SrcManager);
		std::cout << RawText.str() << std::endl;
	}
}
