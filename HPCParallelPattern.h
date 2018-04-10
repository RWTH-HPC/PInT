#pragma once

#include <string>
#include <regex>
#include <vector>
#include <stack>


// TODO Implement Exception for wrong pattern nesting


/*
 * HPC Parallel Pattern Class
 */
class HPCParallelPattern {
public:
	HPCParallelPattern(std::string HPCPatternInstrString, std::stack<HPCParallelPattern*> Context);
	
	void Print();

	void AddChild(ParallelPattern* Child);

	void AddParent(ParallelPattern* Pattern);

private:	
	DesignSpace DesignSp;
	std::string PatternName;
	std::string PatternID;
	std::vector<HPCParallelPattern*> Parents;
	std::vector<HPCParallelPattern*> Children;
};



/*
 * Design Spaces
 */
enum DesignSpace { Unknown, FindingConcurrency, AlgorithmStructure, SupportingStructure, ImplementationMechanism };

DesignSpace StrToDesignSpace(std::string str);



/*
 * Regular Expressions
 */
extern std::regex BeginParallelPatternRegex;

extern std::regex EndParallelPatternRegex;



/* 
 * Stack Management
 */
extern std::stack<HPCParallelPattern*> Context;

void AddToPatternStack(ParallelPattern* Pattern);

HPCParallelPattern* GetTopPatternStack();

HPCParallelPattern* RemoveFromPatternStack(std::string ID);
