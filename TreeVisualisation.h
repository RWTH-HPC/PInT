#pragma once

#include "HPCParallelPattern.h"
#ifndef PATTERNGRAPH_H
	#include "PatternGraph.h"
#endif

/**
 * A class that implements (recursive) visualisation of the call tree.
 */
class CallTreeVisualisation
{
public:
	static void PrintRelationTree(int maxdepth, bool onlyPattern);
	static void PrintCallTree(int maxdepth, CallTree* CalTre);

private:
	static void PrintOnlyPatternTree(int maxdepth);

	static void PrintPattern(PatternCodeRegion* PatternCodeRegion, int depth, int maxdepth);

	static void PrintFunction(FunctionNode* FnCall, int depth, int maxdepth);

	static void PrintRecursiveOnlyPattern(PatternCodeRegion* CodeRegion,int depth,int maxdepth);

	static void PrintIndent(int depth);



};
