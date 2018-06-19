#pragma once

#include "HPCParallelPattern.h"



enum SearchDirection
{
	DIR_Children, DIR_Parents
};

class TreeAlgorithms
{
public:
	static void MarkConnectedComponents();

	static void MarkConnectedComponents(PatternTreeNode* Node, int ComponentID);
};
