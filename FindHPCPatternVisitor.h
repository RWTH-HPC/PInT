#pragma once

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Comment.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "HPCPatternHandler.h"

#define PATTERN_BEGIN_FNNAME "Pattern_Begin"
#define PATTERN_END_FNNAME "Pattern_End"


class FindHPCPatternVisitor : public clang::RecursiveASTVisitor<FindHPCPatternVisitor> 
{
public:
	explicit FindHPCPatternVisitor(clang::ASTContext *Context);

	bool VisitCallExpr(clang::CallExpr *CallExpr);

private:
	clang::ASTContext *Context;
	clang::ast_matchers::MatchFinder Finder;
	PatternStringHandler Handler;
};
