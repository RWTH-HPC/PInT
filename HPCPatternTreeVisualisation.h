#pragma once

#include "HPCParallelPattern.h"
#include "FunctionDeclDatabase.h"

class HPCPatternTreeVisualisation 
{
public:
	static void PrintPatternTree();

private:
	static void PrintPattern(HPCParallelPattern* Pattern, int depth);
	
	static void PrintFunctionTree(FunctionDeclDatabaseEntry* FnCall, int depth);
	
	static void PrintIndent(int depth);
};
