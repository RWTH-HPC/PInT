#include "HPCParallelPattern.h"

#include <iostream>


std::regex ParallelPatternRegex("(Begin|End)\\s(FindingConcurrency|AlgorithmStructure|SupportingStructure|ImplementationMechanism)\\.([[:alpha:]]+)\\s([[:alnum:]]+)");



ParallelPattern::ParallelPattern(std::string PatternName)
{
	std::smatch MatchRes;
	std::regex_search(PatternName, MatchRes, ParallelPatternRegex);

	this->DesignSp = StrToDesignSpace(MatchRes[2].str());
	this->PatternName = MatchRes[3].str();
	this->PatternID = MatchRes[4].str();

#ifdef PRINT_DEBUG
	std::cout << "Matching " << MatchRes[0].str() << std::endl;
	std::cout << MatchRes[1].str() << std::endl;
	std::cout << "Pattern Design Space: " << MatchRes[2].str() << std::endl;
	std::cout << "Pattern Name: " << MatchRes[3].str() << std::endl;
	std::cout << "Pattern Identifier: " << MatchRes[4].str() << std::endl;
#endif
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
