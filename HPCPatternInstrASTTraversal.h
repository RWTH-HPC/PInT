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

#define PATTERN_BEGIN_C_FNNAME "PatternInstrumentation_Pattern_Begin"
#define PATTERN_END_C_FNNAME "PatternInstrumentation_Pattern_End"

#define PATTERN_BEGIN_CXX_FNNAME "Pattern_Begin"
#define PATTERN_END_CXX_FNNAME "Pattern_End"



/**
 * A custom visitor, overriding functions from the RecursiveASTVisitor.
 * It searches for function declarations to build connections between function declarations and calls.
 * It also looks for call expressions in the code and links these expressions to the corresponding function declarations.
 * If a pattern instrumentation call is encountered, a PatternCodeRegion is created/closed and registered with the HPCPatternDatabase.
 */
class HPCPatternInstrVisitor : public clang::RecursiveASTVisitor<HPCPatternInstrVisitor> 
{
public:
	explicit HPCPatternInstrVisitor(clang::ASTContext *Context);
	
	bool VisitFunctionDecl(clang::FunctionDecl *Decl);

	bool VisitCallExpr(clang::CallExpr *CallExpr);

private:
	clang::ASTContext *Context;
	
	/**
 	 * This is a match finder to extract the string argument from the pattern instrumentation call and pass it to the HPCPatternBeginInstrHandler
 	 */
	clang::ast_matchers::MatchFinder PatternBeginFinder;
	
	/**
 	 * See HPCPatternInstrVisitor::PatternBeginFinder
 	 */ 	
	clang::ast_matchers::MatchFinder PatternEndFinder;
	
	HPCPatternBeginInstrHandler PatternBeginHandler;
	HPCPatternEndInstrHandler PatternEndHandler;

	clang::FunctionDecl* CurrentFn;
	FunctionNode* CurrentFnEntry;
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
