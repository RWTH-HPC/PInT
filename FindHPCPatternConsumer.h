#pragma once

#include "clang/AST/ASTConsumer.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/ASTContext.h"

#include "FindHPCPatternVisitor.h"


class FindHPCPatternConsumer : public clang::ASTConsumer
{
public:
	explicit FindHPCPatternConsumer(clang::ASTContext *Context) : Visitor(Context)
	{
	}

	void HandleTranslationUnit(clang::ASTContext &Context);

private:
	FindHPCPatternVisitor Visitor;
};
