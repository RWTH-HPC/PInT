#pragma once

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/RawCommentList.h"
#include "clang/AST/ASTContext.h"

#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include <iostream>
#include "llvm/ADT/StringRef.h"

class FindHPCPatternConsumer : public clang::ASTConsumer
{
public:
	explicit FindHPCPatternConsumer(clang::ASTContext *Context)
	{
	}

	void HandleTranslationUnit(clang::ASTContext &Context);

private:
	/* FindHPCPatternVisitor Visitor; */
};
