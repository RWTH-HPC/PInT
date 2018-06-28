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

	
	/* Look if a pattern with this Design Space and Name already exists */
	HPCParallelPattern* Pattern = HPCPatternDatabase::GetInstance()->LookupParallelPattern(DesignSp, PatternName);

	if (Pattern == NULL)
	{
		Pattern = new HPCParallelPattern(DesignSp, PatternName);
		HPCPatternDatabase::GetInstance()->AddParallelPattern(Pattern);
	}


	/* Check if this code regions is part of an existing pattern occurence */
	PatternOccurence* PatternOcc = HPCPatternDatabase::GetInstance()->LookupPatternOccurence(PatternID);

	if (PatternOcc == NULL)
	{
		PatternOcc = new PatternOccurence(Pattern, PatternID);
		HPCPatternDatabase::GetInstance()->AddPatternOccurence(PatternOcc);
		Pattern->AddOccurence(PatternOcc);
	}


	/* Create a new object for pattern occurence */
	PatternCodeRegion* CodeRegion = new PatternCodeRegion(PatternOcc);
	PatternOcc->AddCodeRegion(CodeRegion);


	/* Connect the child and parent links between the objects */
	PatternCodeRegion* Top = GetTopPatternStack();
	
	if (Top != NULL)
	{
		Top->AddChild(CodeRegion);
		CodeRegion->AddParent(Top);
	}
	else
	{	
		CurrentFnEntry->AddChild(CodeRegion);
		CodeRegion->AddParent(CurrentFnEntry);
	}

	AddToPatternStack(CodeRegion);
	LastPattern = CodeRegion;

#if PRINT_DEBUG
	Pattern->Print();
	PatternOcc->Print();
	CodeRegion->Print();
#endif
}



/*
 * Pattern End Instrumentation Call Handler
 */
void HPCPatternEndInstrHandler::run(const clang::ast_matchers::MatchFinder::MatchResult &Result)
{
	const clang::StringLiteral* patternstr = Result.Nodes.getNodeAs<clang::StringLiteral>("patternstr");	
	
	std::string PatternID = patternstr->getString().str();

	LastPattern = GetTopPatternStack();	
	RemoveFromPatternStack(PatternID);
}

void HPCPatternEndInstrHandler::SetCurrentFnEntry(FunctionDeclDatabaseEntry* FnEntry) 
{
	CurrentFnEntry = FnEntry;
}
