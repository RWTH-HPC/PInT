#pragma once

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"


class PatternStringHandler : public clang::ast_matchers::MatchFinder::MatchCallback 
{
public:
	virtual void run (const clang::ast_matchers::MatchFinder::MatchResult &Result);
};
