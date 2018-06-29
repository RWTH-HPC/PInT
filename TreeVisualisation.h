#pragma once

#include "HPCParallelPattern.h"



class CallTreeVisualisation 
{
public:
	static void PrintCallTree(int maxdepth);

private:
	static void PrintPattern(PatternCodeRegion* PatternCodeRegion, int depth, int maxdepth);
	
	static void PrintFunction(FunctionDeclDatabaseEntry* FnCall, int depth, int maxdepth);
	
	static void PrintIndent(int depth);
};
