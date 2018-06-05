#pragma once

#include "HPCParallelPattern.h"


class TreeAlgorithms
{
public:
	static void MarkConnectedComponents();

	static void MarkConnectedComponents(PatternTreeNode* Node, int ComponentID);
};
