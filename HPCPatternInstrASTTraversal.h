#pragma once

#include "HPCPatternInstrHandler.h"
#include "HPCParallelPattern.h"

#include "clang/Frontend/FrontendActions.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/ADT/StringRef.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Comment.h"
#include "clang/AST/Expr.h"
#include "clang/AST/Decl.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#define PATTERN_BEGIN_FNNAME "Pattern_Begin"
#define PATTERN_END_FNNAME "Pattern_End"


class HPCPatternInstrVisitor : public clang::RecursiveASTVisitor<HPCPatternInstrVisitor> 
{
public:
	explicit HPCPatternInstrVisitor(clang::ASTContext *Context);
	
	bool VisitFunctionDecl(clang::FunctionDecl *Decl);

	bool VisitCallExpr(clang::CallExpr *CallExpr);

private:
	clang::ASTContext *Context;
	
	clang::ast_matchers::MatchFinder PatternBeginFinder;
	clang::ast_matchers::MatchFinder PatternEndFinder;
	
	HPCPatternBeginInstrHandler PatternBeginHandler;
	HPCPatternEndInstrHandler PatternEndHandler;

	FunctionDeclDatabase* FunctionDB;

	clang::FunctionDecl* CurrentFn;
	FunctionDeclDatabaseEntry* CurrentFnEntry;
};



class HPCPatternInstrConsumer : public clang::ASTConsumer
{
public:
	explicit HPCPatternInstrConsumer(clang::ASTContext *Context) : Visitor(Context)
	{
	}

	void HandleTranslationUnit(clang::ASTContext &Context);

private:
	HPCPatternInstrVisitor Visitor;
};



class HPCPatternInstrAction : public clang::ASTFrontendAction 
{
public:
	virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile);
};
