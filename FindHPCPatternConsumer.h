#pragma once

#include "clang/AST/ASTConsumer.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/ASTContext.h"


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
