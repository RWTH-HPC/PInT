#pragma once

#include <string>
#include <regex>


enum DesignSpace { FindingConcurrency, AlgorithmStructure, SupportingStructure, ImplementationMechanism };


extern std::regex ParallelPatternRegex;


class ParallelPattern {
public:
	ParallelPattern(std::string PatternComment);
	
	DesignSpace DesignSp;
	std::string PatternName;
	std::string PatternIdentifier;
};
