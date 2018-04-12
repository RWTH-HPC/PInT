#pragma once

#include "HPCParallelPattern.h"
#include "FunctionDeclDatabase.h"

class HPCPatternTreeVisualisation 
{
public:
	static void PrintPatternTree(int maxdepth);

private:
	static void PrintPattern(HPCParallelPattern* Pattern, int depth, int maxdepth);
	
	static void PrintFunctionTree(FunctionDeclDatabaseEntry* FnCall, int depth, int maxdepth);
	
	static void PrintIndent(int depth);
};
