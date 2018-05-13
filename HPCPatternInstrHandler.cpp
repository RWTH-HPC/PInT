#include "HPCPatternInstrHandler.h"
#include "HPCParallelPattern.h"

#include <iostream>
#include <regex>


/*
 * Regular Expressions
 */
std::regex BeginParallelPatternRegex("([[:alnum:]]+)\\s([[:alnum:]]+)\\s([[:alnum:]]+)");
std::regex EndParallelPatternRegex("([[:alnum:]]+)");



/* 
 * Pattern Begin Instrumentation Call Handler
 */
void HPCPatternBeginInstrHandler::SetCurrentFnEntry(FunctionDeclDatabaseEntry* FnEntry) 
{
	CurrentFnEntry = FnEntry;
}

void HPCPatternBeginInstrHandler::run(const clang::ast_matchers::MatchFinder::MatchResult &Result)
{
	const clang::StringLiteral* patternstr = Result.Nodes.getNodeAs<clang::StringLiteral>("patternstr");	

	/* Match Regex and save info*/
	std::smatch MatchRes;
	std::string PatternInfoStr = patternstr->getString().str();
	std::regex_search(PatternInfoStr, MatchRes, BeginParallelPatternRegex);

	DesignSpace DesignSp = StrToDesignSpace(MatchRes[1].str());
	std::string PatternName = MatchRes[2].str();
	std::string PatternID = MatchRes[3].str();
	
	/* Look if a pattern with this ID already exists */
	HPCParallelPattern* Pattern = HPCPatternDatabase::GetInstance()->LookupParallelPattern(PatternID);

	if (Pattern == NULL)
	{
		Pattern = new HPCParallelPattern(DesignSp, PatternName, PatternID);
		HPCPatternDatabase::GetInstance()->AddParallelPattern(Pattern);
	}

	HPCParallelPattern* Top = GetTopPatternStack();
	
	/* Connect the child and parent links between the objects */
	if (Top != NULL)
	{
		Top->AddChild(Pattern);
		Pattern->AddParent(Top);
	}
	else
	{	
		CurrentFnEntry->AddChild(Pattern);
		Pattern->AddParent(CurrentFnEntry);
	}

	AddToPatternStack(Pattern);
	LastPattern = Pattern;

#if PRINT_DEBUG
	Pattern->Print();	
#endif
}



/*
 * Pattern End Instrumentation Call Handler
 */
void HPCPatternEndInstrHandler::run(const clang::ast_matchers::MatchFinder::MatchResult &Result)
{
	const clang::StringLiteral* patternstr = Result.Nodes.getNodeAs<clang::StringLiteral>("patternstr");	
	
	HPCParallelPattern* Pattern = HPCPatternDatabase::GetInstance()->LookupParallelPattern(patternstr->getString().str());
	
	RemoveFromPatternStack(Pattern);
	LastPattern = Pattern;
}

void HPCPatternEndInstrHandler::SetCurrentFnEntry(FunctionDeclDatabaseEntry* FnEntry) 
{
	CurrentFnEntry = FnEntry;
}
