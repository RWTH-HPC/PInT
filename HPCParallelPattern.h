#pragma once

#include "FunctionDeclDatabase.h"

#include <string>
#include <regex>
#include <vector>
#include <stack>


/* Forward Declarations */
class FunctionDeclDatabase;
struct FunctionDeclDatabaseEntry;

// TODO Implement Exception for wrong pattern nesting



/*
 * Design Spaces
 */
enum DesignSpace { Unknown, FindingConcurrency, AlgorithmStructure, SupportingStructure, ImplementationMechanism };

DesignSpace StrToDesignSpace(std::string str);



/*
 * HPC Parallel Pattern Class
 */
class HPCParallelPattern {
public:
	HPCParallelPattern(std::string HPCPatternInstrString);
	
	void Print();

	void AddChild(HPCParallelPattern* Child);

	void AddParent(HPCParallelPattern* Pattern);

	void AddFnCall(FunctionDeclDatabaseEntry* Entry);

	std::string GetPatternID() { return this->PatternID; }

private:	
	DesignSpace DesignSp;
	std::string PatternName;
	std::string PatternID;

	std::vector<HPCParallelPattern*> Parents;
	std::vector<HPCParallelPattern*> Children;

	std::vector<FunctionDeclDatabaseEntry*> FnCalls;
};



/*
 * Regular Expressions
 */
extern std::regex BeginParallelPatternRegex;

extern std::regex EndParallelPatternRegex;



/* 
 * Stack Management
 */
extern std::stack<HPCParallelPattern*> Context;

void AddToPatternStack(HPCParallelPattern* Pattern);

HPCParallelPattern* GetTopPatternStack();

void RemoveFromPatternStack(std::string ID);
