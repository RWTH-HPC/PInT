#pragma once

#include "HPCParallelPattern.h"


/**
 * SearchDirection for recursive descent.
 */
enum SearchDirection
{
	DIR_Children, DIR_Parents
};

/**
 * A helper class that provides algorithms operating on the pattern tree whose functionality might be useful for a handful of different other classes.
 */
class TreeAlgorithms
{
public:
	static void MarkConnectedComponents();

	static void MarkConnectedComponents(PatternTreeNode* Node, int ComponentID);
};
