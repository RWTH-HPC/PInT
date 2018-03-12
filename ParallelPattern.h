#pragma once

#include <string>
#include <regex>
#include <vector>


enum DesignSpace { Unknown, FindingConcurrency, AlgorithmStructure, SupportingStructure, ImplementationMechanism };


extern std::regex ParallelPatternRegex;


class ParallelPattern {
public:
	ParallelPattern(std::string PatternComment);
	
	DesignSpace DesignSp;
	std::string PatternName;
	std::string PatternID;
};


DesignSpace StrToDesignSpace(std::string str);
