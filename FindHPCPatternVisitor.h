#pragma once

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Comment.h"
#include "clang/AST/ASTContext.h"

class FindHPCPatternVisitor : public clang::RecursiveASTVisitor<FindHPCPatternVisitor> 
{
public:
	explicit FindHPCPatternVisitor(clang::ASTContext *Context) : Context(Context)
	{
	}

	bool VisitPragmaCommentDecl(clang::PragmaCommentDecl *CmtDecl);

private:
	clang::ASTContext *Context;
};
