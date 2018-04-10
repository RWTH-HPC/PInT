#pragma once

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"



class HPCPatternBeginInstrHandler : public clang::ast_matchers::MatchFinder::MatchCallback 
{
public:
	virtual void run (const clang::ast_matchers::MatchFinder::MatchResult &Result);
};



class HPCPatternEndInstrHandler : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
	virtual void run (const clang::ast_matchers::MatchFinder::MatchResult &Result);
};
