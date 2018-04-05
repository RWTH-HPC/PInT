#pragma once

#include <string>
#include <regex>
#include <vector>
#include <stack>


enum DesignSpace { Unknown, FindingConcurrency, AlgorithmStructure, SupportingStructure, ImplementationMechanism };


extern std::regex BeginParallelPatternRegex;
extern std::regex EndParallelPatternRegex;


class HPCParallelPattern {
public:
	HPCParallelPattern(std::string HPCPatternInstrString);
	
	void Print();

private:	
	DesignSpace DesignSp;
	std::string PatternName;
	std::string PatternID;
}
;

extern std::stack<HPCParallelPattern*> Context;

DesignSpace StrToDesignSpace(std::string str);
