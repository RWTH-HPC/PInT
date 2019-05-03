#pragma once

#include "HPCParallelPattern.h"

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"


/**
 * This class handles the callback that is issued when a pattern begin instrumentation call is encountered and the string argument is matched.
 * It extracts all information about the pattern and patternoccurrence from the string argument and initiates creation of all involved objects.
 */
class HPCPatternBeginInstrHandler : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
	void SetCurrentFnEntry(FunctionNode* FnEntry);

	PatternCodeRegion* GetLastPattern() { return LastPattern; }

	virtual void run (const clang::ast_matchers::MatchFinder::MatchResult &Result);

private:
	FunctionNode* CurrentFnEntry;

	PatternCodeRegion* LastPattern;
};


/**
 * See HPCPatternBeginInstrHandler and HPCPatternEndInstrHandler::run()
 */
class HPCPatternEndInstrHandler : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
	void SetCurrentFnEntry(FunctionNode* FnEntry);

	PatternCodeRegion* GetLastPattern() { return LastPattern; }

	virtual void run (const clang::ast_matchers::MatchFinder::MatchResult &Result);

private:
	FunctionNode* CurrentFnEntry;

	PatternCodeRegion* LastPattern;

	PatternCodeRegion* LastOnlyPattern;
};
