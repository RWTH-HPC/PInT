#include "Helpers.h"
#include <vector>


/**
 * @brief A helper function to retrieve all PatternOccurrence objects from a list of PatternCodeRegions.
 * On demand, the list is turned into a set.
 *
 * @param CodeRegions List of PatternCodeRegion objects.
 * @param MakeUnique If true, all duplicates are removed from the container.
 *
 * @return A list of PatternOccurrence objects, free from duplicates iff flag is set.
 **/
std::vector<PatternOccurrence*> PatternHelpers::GetPatternOccurrences(std::vector<PatternCodeRegion*> CodeRegions, bool MakeUnique)
{
	/* Retrieve all pattern occurrences from the code regions */
	std::vector<PatternOccurrence*> PatternOccurrences;

	for (PatternCodeRegion* CodeReg : CodeRegions)
	{
		PatternOccurrences.push_back(CodeReg->GetPatternOccurrence());
	}

	/* Clean the list from duplicates if the parameter is set */
	if (MakeUnique)
	{
		PatternOccurrences = SetAlgorithms::GetUniquePatternOccList(PatternOccurrences);
	}

	return PatternOccurrences;
}

/**
 * @brief A tree operation that marks every tree node with the label corresponding to its connected component.
 * Calls GraphAlgorithms::MarkConnectedComponents(PatternGraphNode*, int).
 */
void GraphAlgorithms::MarkConnectedComponents()
{
	std::vector<PatternCodeRegion*> PatternCodeRegs = PatternGraph::GetInstance()->GetAllPatternCodeRegions();

	int ConnectedComponents = 0;

	for (PatternCodeRegion* PatternCodeReg : PatternCodeRegs)
	{
		if (PatternCodeReg->GetConnectedComponent() == -1)
		{
			MarkConnectedComponents(PatternCodeReg, ConnectedComponents);
			ConnectedComponents++;
		}
	}
}

/**
 * @brief Marks every tree node with label corresponding to connected component
 *
 * @param Node The current node.
 * @param ComponentID ID of the connected component.
 **/
void GraphAlgorithms::MarkConnectedComponents(PatternGraphNode* Node, int ComponentID)
{
	if (Node->GetConnectedComponent() == -1)
	{
		Node->SetConnectedComponent(ComponentID);

		for (PatternGraphNode* Child : Node->GetChildren())
		{
			MarkConnectedComponents(Child, ComponentID);
		}

		for (PatternGraphNode* Parent : Node->GetParents())
		{
			MarkConnectedComponents(Parent, ComponentID);
		}
	}
}

/**
 * @brief Finds the parent patterns, beginning from a PatternCodeRegion.
 * Saves the parent patterns in the list of PatternOccurrence passed as second parameter.
 *
 * @param Start Initial PatternCodeRegion from which the search is started.
 * @param Parents Reference to a std::vector of PatternOccurrence* in which the encountered occurrences are saved.
 * @param maxdepth Maximum depth of the recursion.
 **/
void GraphAlgorithms::FindParentPatternCodeRegions(PatternCodeRegion* Start, std::vector<PatternCodeRegion*>& Parents, int maxdepth)
{
	int a = 0;
	int* p = &a;
	FindNeighbourPatternCodeRegionss(Start, Parents, DIR_Parents, p, maxdepth);
}

/**
 * @brief Finds the child patterns from a starting point. See GraphAlgorithms::FindParentPatternCodeRegions().
 **/
void GraphAlgorithms::FindChildPatternCodeRegions(PatternCodeRegion* Start, std::vector<PatternCodeRegion*>& Children, int maxdepth)
{
	int a = 0;
	int* p = &a;
	FindNeighbourPatternCodeRegionss(Start, Children, DIR_Children, p, maxdepth);
}

/**
 * @brief Core functionality for finding neightbouring PatternCodeRegions of a PatternGraphNode (in this case: PatternCodeRegion).
 * The direction of the recursive descent is passed as a parameter.
 *
 * @param Current The starting point of the recursive descent.
 * @param Results Vector of PatternCodeRegion neighbours.
 * @param dir The search direction (Children or parents)
 * @param depth The current depth of recursion.
 * @param maxdepth The maximum recursion depth.
 **/
void GraphAlgorithms::FindNeighbourPatternCodeRegions(PatternGraphNode* Current, std::vector<PatternCodeRegion*>& Results, GraphSearchDirection dir, int depth, int maxdepth)
{
	/* Check, if we reached the maximum depth */
	if (depth >= maxdepth)
	{
		return;
	}

	PatternCodeRegion* PatternReg = clang::dyn_cast<PatternCodeRegion>(Current);

	if (depth > 0 && PatternReg != NULL)
	{
		Results.push_back(PatternReg);
	}
	else
	{
		/* Get the neighbouring nodes depending on the defined search direction */
		std::vector<PatternGraphNode*> Neighbours;

		if (dir == DIR_Parents)
		{
			Neighbours = Current->GetParents();
		}
		else if (dir == DIR_Children)
		{
			Neighbours = Current->GetChildren();
		}



		/* Visit all the neighbouring nodes according to the given direction */
		if (Neighbours.size() > 0 && Neighbours[0] != NULL && Neighbours[0] != 0){
			for (PatternGraphNode* Neighbour : Neighbours)
			{
				FindNeighbourPatternCodeRegions(Neighbour, Results, dir, depth + 1, maxdepth);
			}
		}
	}
		return;
}

void GraphAlgorithms::FindNeighbourPatternCodeRegionss(PatternGraphNode* Current, std::vector<PatternCodeRegion*>& Results, GraphSearchDirection dir, int* depth, int maxdepth)
{
	/* Check, if we reached the maximum depth */
	if (*depth >= maxdepth)
	{
		return;
	}

	PatternCodeRegion* PatternReg = clang::dyn_cast<PatternCodeRegion>(Current);

	if (*depth > 0 && PatternReg != NULL)
	{
		Results.push_back(PatternReg);
	}
	else
	{
		/* Get the neighbouring nodes depending on the defined search direction */
		std::vector<PatternGraphNode*> Neighbours;

		if (dir == DIR_Parents)
		{
			Neighbours = Current->GetParents();
		}
		else if (dir == DIR_Children)
		{
			Neighbours = Current->GetChildren();
		}



		/* Visit all the neighbouring nodes according to the given direction */
		if (Neighbours.size() > 0 && Neighbours[0] != NULL && Neighbours[0] != 0){
			for (PatternGraphNode* Neighbour : Neighbours)
			{
				*depth = *depth + 1;
				FindNeighbourPatternCodeRegionss(Neighbour, Results, dir, depth , maxdepth);
			}
		}
	}
		return;
}

/**
 * @brief Remove duplicates from a list of PatternOccurrence.
 * The criterion is defined by PatternOccurrence::Equals().
 *
 * @param PatternOccs List of PatternOccurrence objects containing duplicates.
 *
 * @return List of PatternOccurrence objects free from duplicates.
 **/
std::vector<PatternOccurrence*> SetAlgorithms::GetUniquePatternOccList(std::vector<PatternOccurrence*> PatternOccs)
{
	std::vector<PatternOccurrence*> Res;

	for (PatternOccurrence* PatternOcc : PatternOccs)
	{
		/* Search the pattern list, whether this is a duplicate */
		bool duplicate = false;

		for (PatternOccurrence* ResOcc : Res)
		{
			if (PatternOcc == ResOcc || PatternOcc->Equals(ResOcc))
			{
				duplicate = true;
				break;
			}
		}

		if (!duplicate)
		{
			Res.push_back(PatternOcc);
		}
	}

	return Res;
}

/**
 * @brief Removes duplicates from the input set.
 * The criterion is HPCParallelPattern::Equals().
 *
 * @param Patterns Input set.
 *
 * @return Set free of duplicates.
 **/
std::vector<HPCParallelPattern*> SetAlgorithms::GetUniquePatternList(std::vector<HPCParallelPattern*> Patterns)
{
	std::vector<HPCParallelPattern*> Res;

	for (HPCParallelPattern* Pattern : Patterns)
	{
		bool duplicate = false;

		/* Check, if the pattern has already been added to the output set */
		for (HPCParallelPattern* Pattern2 : Res)
		{
			if (Pattern->Equals(Pattern2))
			{
				duplicate = true;
				break;
			}
		}

		if (!duplicate)
		{
			Res.push_back(Pattern);
		}
	}

	return Res;
}
