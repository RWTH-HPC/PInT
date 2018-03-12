#include "ParallelPattern.h"

#include <iostream>


std::regex ParallelPatternRegex("(Begin|End)\\s(FindingConcurrency|AlgorithmStructure|SupportingStructure|ImplementationMechanism)\\.([[:alpha:]]+)\\s([[:alnum:]]+)");


ParallelPattern::ParallelPattern(std::string PatternName)
{
	std::smatch MatchRes;
	std::regex_search(PatternName, MatchRes, ParallelPatternRegex);

#ifdef PRINT_DEBUG
	std::cout << "Matching " << MatchRes[0].str() << std::endl;
	std::cout << MatchRes[1].str() << std::endl;
	std::cout << "Pattern Design Space: " << MatchRes[2].str() << std::endl;
	std::cout << "Pattern Name: " << MatchRes[3].str() << std::endl;
	std::cout << "Pattern Identifier: " << MatchRes[4].str() << std::endl;
#endif
}
