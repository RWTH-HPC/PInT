#pragma once

#include "HPCParallelPattern.h"


/**
 * A class that implements (recursive) visualisation of the call tree.
 */
class CallTreeVisualisation
{
public:
	static void PrintCallTree(int maxdepth, bool onlyPattern);

private:
	static void PrintOnlyPatternTree(int maxdepth);

	static void PrintPattern(PatternCodeRegion* PatternCodeRegion, int depth, int maxdepth);

	static void PrintFunction(FunctionNode* FnCall, int depth, int maxdepth);

	static void PrintRecursiveOnlyPattern(PatternCodeRegion* CodeRegion,int depth,int maxdepth);

	static void PrintIndent(int depth);

};
