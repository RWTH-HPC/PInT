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
	/**
		* Prints the relation Tree
		**/
	static void PrintRelationTree(int maxdepth, bool onlyPattern);
	/**
		* Prints the CallTree
		**/
	static void PrintCallTree(int maxdepth, CallTree* CalTre, bool onlyPattern);

private:
	/**
		* Prints the OnlyPatterntree and is called from PrintRelationTree.
		**/
	static void PrintOnlyPatternTree(int maxdepth);
	/**
		* Prints a Pattern
		**/
	static void PrintPattern(PatternCodeRegion* PatternCodeRegion, int depth, int maxdepth);
	/**
		* Prints a Function
		**/
	static void PrintFunction(FunctionNode* FnCall, int depth, int maxdepth);
	/**
		* Prints recursevely the OnlyPatternTree
		**/
	static void PrintRecursiveOnlyPattern(PatternCodeRegion* CodeRegion,int depth,int maxdepth);
	/**
		* Prints recursevely the CallTree
		**/
 	static void PrintCallTreeRecursively(int &HelpKey, std::vector<std::tuple<int, CallTreeNode*>> &CallTreeHelp, CallTreeNode* ClTrNode, int depth, int maxdepth, bool onlyPattern);
	/**
		* Prints a indent in dependent on depth.
		**/
	static void PrintIndent(int depth);

	static int searchDepthInCallTreeHelp(std::vector<std::tuple<int, CallTreeNode*>> &CallTreeHelp, CallTreeNode* EndNode);

};
