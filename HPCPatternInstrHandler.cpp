#include "HPCPatternInstrHandler.h"
#include "HPCParallelPattern.h"

#include <iostream>
#include <regex>


/*
 * Regular Expressions
 */
std::regex BeginParallelPatternRegex("([[:alnum:]]+)\\s([[:alnum:]]+)\\s([[:alnum:]]+)");
std::regex EndParallelPatternRegex("([[:alnum:]]+)");



/**
 * @brief Keep track of the currently encountered function.
 *
 * @param FnEntry The function decl. of the body that is currently analysed.
 **/
void HPCPatternBeginInstrHandler::SetCurrentFnEntry(FunctionNode* FnEntry) 
{
	CurrentFnEntry = FnEntry;
}

/**
 * @brief Analyse the match results from the pattern begin matcher to extract information about the pattern.
 * After extracting design space, pattern name and pattern identifier, HPCParallelPattern and PatternOccurrence objects are looked up in the database.
 * If they do not already exist, they are created.
 * Then, a PatternCodeRegion object is created for this particular encounter.
 *
 * @param Result Match results from the pattern begin matcher.
 **/
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
	HPCParallelPattern* Pattern = PatternGraph::GetInstance()->GetPattern(DesignSp, PatternName);

	if (Pattern == NULL)
	{
		Pattern = new HPCParallelPattern(DesignSp, PatternName);
		PatternGraph::GetInstance()->RegisterPattern(Pattern);
	}


	/* Check if this code regions is part of an existing pattern occurrence */
	PatternOccurrence* PatternOcc = PatternGraph::GetInstance()->GetPatternOccurrence(PatternID);

	if (PatternOcc == NULL)
	{
		PatternOcc = new PatternOccurrence(Pattern, PatternID);
		PatternGraph::GetInstance()->RegisterPatternOccurrence(PatternOcc);
		Pattern->AddOccurrence(PatternOcc);
	}
	else
	{
		if (!PatternOcc->GetPattern()->Equals(Pattern))
		{
			std::cout << "\033[31m" << "Pattern Occurrences with same identifier have different underlying pattern:\033[0m" << PatternID << std::endl;
		}
	}

	/* Create a new object for pattern occurrence */
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



/**
 * @brief Extracts the pattern identifier string from the match results and removes the PatternCodeRegion from the pattern stack.
 *
 * @param Result Match results from the pattern end matcher.
 **/
void HPCPatternEndInstrHandler::run(const clang::ast_matchers::MatchFinder::MatchResult &Result)
{
	const clang::StringLiteral* patternstr = Result.Nodes.getNodeAs<clang::StringLiteral>("patternstr");	
	
	std::string PatternID = patternstr->getString().str();

	LastPattern = GetTopPatternStack();	
	RemoveFromPatternStack(PatternID);
}

/**
 * @brief See PatternBeginInstrHandler::SetCurrentFnEntry().
 *
 * @param FnEntry Current function declaration database entry.
 **/
void HPCPatternEndInstrHandler::SetCurrentFnEntry(FunctionNode* FnEntry) 
{
	CurrentFnEntry = FnEntry;
}
