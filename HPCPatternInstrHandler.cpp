#include "HPCPatternInstrHandler.h"
#include "HPCParallelPattern.h"
#include "HPCError.h"
#include <iostream>
#include <regex>
//#define PRINT_ONLYPATTERNDENUG


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

 // describes what has to happen if we encounter the beginning of a pattern
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

	/*We look if this patternCodeRegion ID is already used*/
	try{
		if(PatternIDisUsed(PatternID)){
			throw TooManyBeginsException(PatternID);
		}
	}
	catch(TooManyBeginsException& e){
		e.what();
		throw TerminateEarlyException();
	}
	/* Look if a pattern with this Design Space and Name already exists */
	HPCParallelPattern* Pattern = PatternGraph::GetInstance()->GetPattern(DesignSp, PatternName);

	/*If Pattern does not exist register it.*/
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
		try{
			if (!PatternOcc->GetPattern()->Equals(Pattern))
			{
				throw WrongSyntaxException(PatternOcc);
			}
		}
		catch(WrongSyntaxException& wrongSyn){
			std::cout << wrongSyn.what() <<" ende "<< std::endl;
			throw TerminateEarlyException();
		}
	}

	/* register the PatternOcc in the Pattern Stack for the Halstead metric*/
	OccStackForHalstead.push_back(PatternOcc);

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

		/*Register the PatternChildren of the Functions too*/
		CurrentFnEntry->AddPatternChild(CodeRegion);
		CurrentFnEntry->registerPatChildrenToPatParents();
	}

	AddToPatternStack(CodeRegion);

	PatternCodeRegion* OnlyPatternTop = GetTopOnlyPatternStack();

	if(OnlyPatternTop != NULL)
	{
		OnlyPatternTop->AddOnlyPatternChild(CodeRegion);
		CodeRegion->AddOnlyPatternParent(OnlyPatternTop);
	}
	else
	{
		PatternGraph::GetInstance()->RegisterOnlyPatternRootNode(CodeRegion);
	}

	AddToOnlyPatternStack(CodeRegion);

#if PRINT_DEBUG
	Pattern->Print();
	PatternOcc->Print();
	CodeRegion->Print();
#endif
}

void HPCPatternEndInstrHandler::run(const clang::ast_matchers::MatchFinder::MatchResult &Result)
{
	const clang::StringLiteral* patternstr = Result.Nodes.getNodeAs<clang::StringLiteral>("patternstr");

	LastPatternID = patternstr->getString().str();
	LastPattern = GetTopPatternStack();

	RemoveFromPatternStack(LastPatternID);

	LastOnlyPattern = GetTopOnlyPatternStack();
	RemoveFromOnlyPatternStack(LastPatternID);
}

void HPCPatternEndInstrHandler::SetCurrentFnEntry(FunctionNode* FnEntry)
{
	CurrentFnEntry = FnEntry;
}
