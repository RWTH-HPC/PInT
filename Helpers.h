#pragma once

#include "HPCParallelPattern.h"
#include "PatternGraph.h"
#include <vector>

/**
 * GraphSearchDirection for recursive descent.
 */
enum GraphSearchDirection
{
	DIR_Children, DIR_Parents
};

namespace PatternHelpers
{
	extern std::vector<PatternOccurence*> GetPatternOccurences(std::vector<PatternCodeRegion*> CodeRegions, bool MakeUnique);
}

namespace GraphAlgorithms
{
	extern void MarkConnectedComponents();

	extern void MarkConnectedComponents(PatternGraphNode* Node, int ComponentID);

	extern void FindParentPatternCodeRegions(PatternCodeRegion* Start, std::vector<PatternCodeRegion*>& Parents, int maxdepth);

	extern void FindChildPatternCodeRegions(PatternCodeRegion* Start, std::vector<PatternCodeRegion*>& Children, int maxdepth);

	extern void FindNeighbourPatternCodeRegions(PatternGraphNode* Current, std::vector<PatternCodeRegion*>& Results, GraphSearchDirection dir, int depth, int maxdepth);
}

namespace SetAlgorithms
{
	extern std::vector<PatternOccurence*> GetUniquePatternOccList(std::vector<PatternOccurence*> PatternOccs);
}
