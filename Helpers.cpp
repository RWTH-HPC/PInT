#include "Helpers.h"
#include <vector>


std::vector<PatternOccurence*> PatternHelpers::GetPatternOccurences(std::vector<PatternCodeRegion*> CodeRegions, bool MakeUnique)
{
	/* Retrieve all pattern occurences from the code regions */
	std::vector<PatternOccurence*> PatternOccurences;

	for (PatternCodeRegion* CodeReg : CodeRegions)
	{
		PatternOccurences.push_back(CodeReg->GetPatternOccurence());
	}

	/* Clean the list from duplicates if the parameter is set */
	if (MakeUnique)
	{
		PatternOccurences = SetAlgorithms::GetUniquePatternOccList(PatternOccurences);
	}

	return PatternOccurences;
}

/**
 * @brief A tree operation that marks every tree node with the label corresponding to its connected component.
 * Calls GraphAlgorithms::MarkConnectedComponents(PatternGraphNode*, int).
 */
void GraphAlgorithms::MarkConnectedComponents()
{
	std::vector<PatternCodeRegion*> PatternOccs = PatternGraph::GetInstance()->GetAllPatternCodeRegions();

	int ConnectedComponents = 0;

	for (PatternCodeRegion* PatternOcc : PatternOccs)
	{
		if (PatternOcc->GetConnectedComponent() == -1)
		{
			MarkConnectedComponents(PatternOcc, ConnectedComponents);
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
 * Saves the parent patterns in the list of PatternOccurence passed as second parameter.
 *
 * @param Start Initial PatternCodeRegion from which the search is started.
 * @param Parents Reference to a std::vector of PatternOccurence* in which the encountered occurences are saved.
 * @param maxdepth Maximum depth of the recursion.
 **/
void GraphAlgorithms::FindParentPatternCodeRegions(PatternCodeRegion* Start, std::vector<PatternCodeRegion*>& Parents, int maxdepth)
{
	FindNeighbourPatternCodeRegions(Start, Parents, DIR_Parents, 0, maxdepth);
}

/**
 * @brief Finds the child patterns from a starting point. See FindParentPatternCodeRegions().
 **/
void GraphAlgorithms::FindChildPatternCodeRegions(PatternCodeRegion* Start, std::vector<PatternCodeRegion*>& Children, int maxdepth)
{
	FindNeighbourPatternCodeRegions(Start, Children, DIR_Children, 0, maxdepth);
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
		for (PatternGraphNode* Neighbour : Neighbours)
		{
			FindNeighbourPatternCodeRegions(Neighbour, Results, dir, depth + 1, maxdepth);
		}
	}
}

/**
 * @brief Remove duplicates from a list of PatternOccurence.
 * The criterion is defined by PatternOccurence::Equals().
 *
 * @param PatternOccs List of PatternOccurence objects containing duplicates.
 *
 * @return List of PatternOccurence objects free from duplicates.
 **/
std::vector<PatternOccurence*> SetAlgorithms::GetUniquePatternOccList(std::vector<PatternOccurence*> PatternOccs)
{
	std::vector<PatternOccurence*> Res;

	for (PatternOccurence* PatternOcc : PatternOccs)
	{
		/* Search the pattern list, whether this is a duplicate */
		bool duplicate = false;

		for (PatternOccurence* ResOcc : Res)
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
