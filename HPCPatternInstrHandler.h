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
	/**
	 * returns the pattern on top of the pattern stack. Is used to keept track within wich pattern we are while traversing.
	 */
	PatternCodeRegion* GetLastPattern() { return GetTopPatternStack(); };
	/**
	 * @brief Analyse the match results from the pattern begin matcher to extract information about the pattern.
	 * After extracting design space, pattern name and pattern identifier, HPCParallelPattern and PatternOccurrence objects are looked up in the database.
	 * If they do not already exist, they are created.
	 * Then, a PatternCodeRegion object is created for this particular encounter.
	 *
	 * @param Result Match results from the pattern begin matcher.
	 **/
	virtual void run (const clang::ast_matchers::MatchFinder::MatchResult &Result);

private:
	/**
	 * Is used to keep track within which function we are while traversing.
	 */
	FunctionNode* CurrentFnEntry;
};


/**
 * See HPCPatternBeginInstrHandler and HPCPatternEndInstrHandler::run()
 */
class HPCPatternEndInstrHandler : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
	void SetCurrentFnEntry(FunctionNode* FnEntry);
	/**
	 * Pattern end is closing a Pattern. Possibly a pattern within another pattern. This is used to get the outer pattern.
	 */
	PatternCodeRegion* GetLastPattern() { return LastPattern; };
	/**
	 * returns the ID of the last encountered Pattern_End (which is not necessarily the LastPattern)
	 */
	std::string GetLastPatternID(){ return LastPatternID;};
	/**
	 * @brief Extracts the pattern identifier string from the match results and removes the PatternCodeRegion from the pattern stack.
	 *
	 * @param Result Match results from the pattern end matcher.
	 **/
	virtual void run (const clang::ast_matchers::MatchFinder::MatchResult &Result);

private:
	/**
	 * @brief See PatternBeginInstrHandler::SetCurrentFnEntry().
	 *
	 * @param FnEntry Current function declaration database entry.
	 **/
	FunctionNode* CurrentFnEntry;

	PatternCodeRegion* LastPattern;

	std::string LastPatternID;

	PatternCodeRegion* LastOnlyPattern;
};
