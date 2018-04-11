#include "HPCParallelPattern.h"

#include <iostream>


/*
 * Regular Expressions
 */
std::regex BeginParallelPatternRegex("([[:alnum:]]+)\\s([[:alnum:]]+)\\s([[:alnum:]]+)");
std::regex EndParallelPatternRegex("([[:alnum:]]+)");



/*
 * HPC Parallel Pattern Class Functions
 */
HPCParallelPattern::HPCParallelPattern(std::string HPCPatternInstrString) : Parents(), Children(), FnCalls()
{
	/* Match Regex and save info in member variables */
	std::smatch MatchRes;
	std::regex_search(HPCPatternInstrString, MatchRes, BeginParallelPatternRegex);

	this->DesignSp = StrToDesignSpace(MatchRes[1].str());
	this->PatternName = MatchRes[2].str();
	this->PatternID = MatchRes[3].str();

	/* Initialise children and parent vectors */
	if (!Context.empty())
	{
		HPCParallelPattern* Parent = GetTopPatternStack();
		Parents.push_back(Parent);
		Parent->AddChild(this);
	}
}

void HPCParallelPattern::Print() 
{
	std::cout << "Pattern Info:" << std::endl;
	std::cout << "Pattern Design Space: " << this->DesignSp << std::endl;
	std::cout << "Pattern Name: " << this->PatternName << std::endl;
	std::cout << "Pattern Identifier: " << this->PatternID << std::endl;
}

void HPCParallelPattern::AddChild(HPCParallelPattern* Child) 
{
	Children.push_back(Child);
}

void HPCParallelPattern::AddParent(HPCParallelPattern* Parent)
{
	Parents.push_back(Parent);
}

void HPCParallelPattern::AddFnCall(FunctionDeclDatabaseEntry* Entry)
{
	FnCalls.push_back(Entry);
}



/*
 * Design Space Helper Functions
 */
DesignSpace StrToDesignSpace(std::string str)
{
	if (str.compare("FindingConcurrency")) 
	{
		return FindingConcurrency;
	}
	else if (str.compare("AlgorithmStructure"))
	{
		return AlgorithmStructure;
	}
	else if (str.compare("SupportingStructure"))
	{
		return SupportingStructure;	
	}
	else if (str.compare("ImplementationMechanism"))
	{
		return ImplementationMechanism;
	}
	else
	{
		return Unknown;
	}
}



/*
 * Pattern Stack Management
 */
std::stack<HPCParallelPattern*> Context;

void AddToPatternStack(HPCParallelPattern* Pattern)
{
	Context.push(Pattern);
}

HPCParallelPattern* GetTopPatternStack() 
{
	if (!Context.empty())
	{
		return Context.top();
	}
	else
	{
		return NULL;
	}
}

void RemoveFromPatternStack(std::string ID)
{
	if (!Context.empty())
	{
		HPCParallelPattern* Top = Context.top();	
		
		if (!Top->GetPatternID().compare(ID))
		{
			// TODO Thorw an Exception here
		}
		
		Context.pop();
	}
}

