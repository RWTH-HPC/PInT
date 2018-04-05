#include "HPCParallelPattern.h"

#include <iostream>


std::regex BeginParallelPatternRegex("([[:alnum:]]+)\\s([[:alnum:]]+)\\s([[:alnum:]]+)");
std::regex EndParallelPatternRegex("([[:alnum:]]+)");

std::stack<HPCParallelPattern*> Context;


HPCParallelPattern::HPCParallelPattern(std::string HPCPatternInstrString)
{
	std::smatch MatchRes;
	std::regex_search(HPCPatternInstrString, MatchRes, BeginParallelPatternRegex);

	this->DesignSp = StrToDesignSpace(MatchRes[1].str());
	this->PatternName = MatchRes[2].str();
	this->PatternID = MatchRes[3].str();

#ifdef PRINT_DEBUG
	std::cout << "Matching " << MatchRes[0].str() << std::endl;
	std::cout << "Pattern Design Space: " << MatchRes[1].str() << std::endl;
	std::cout << "Pattern Name: " << MatchRes[2].str() << std::endl;
	std::cout << "Pattern Identifier: " << MatchRes[3].str() << std::endl;
#endif
}



void HPCParallelPattern::Print() 
{
	std::cout << "Pattern Info:" << std::endl;
	std::cout << "Pattern Design Space: " << this->DesignSp << std::endl;
	std::cout << "Pattern Name: " << this->PatternName << std::endl;
	std::cout << "Pattern Identifier: " << this->PatternID << std::endl;
}

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
