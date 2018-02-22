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
	clang::SourceManager &SrcManager = Context.getSourceManager();
	clang::RawCommentList &RawCommentList = Context.getRawCommentList();

	llvm::ArrayRef<clang::RawComment*> RawComments = RawCommentList.getComments();
	llvm::ArrayRef<clang::RawComment*>::iterator RawComment;

	SrcManager.dump();

	std::cout << "List of raw comments contains " << RawComments.size() << " entries." << std::endl;
	
	std::cout << "Now printing list of raw comments:" << std::endl;

	for (RawComment = RawComments.begin(); RawComment != RawComments.end(); RawComment++)
	{
		llvm::StringRef RawText = (*RawComment)->getRawText(SrcManager);
		std::cout << RawText.str() << std::endl;
	}
#endif
}
