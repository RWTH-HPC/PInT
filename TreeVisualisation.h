#pragma once

#include "HPCParallelPattern.h"



class CallTreeVisualisation 
{
public:
	static void PrintPatternTree(int maxdepth);

private:
	static void PrintPattern(PatternOccurence* PatternOcc, int depth, int maxdepth);
	
	static void PrintFunctionTree(FunctionDeclDatabaseEntry* FnCall, int depth, int maxdepth);
	
	static void PrintIndent(int depth);
};
