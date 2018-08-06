#include "TreeAlgorithms.h"
#include <vector>



/**
 * @brief A tree operation that marks every tree node with the label corresponding to its connected component.
 * Calls TreeAlgorithms::MarkConnectedComponents(PatternGraphNode*, int).
 */
void TreeAlgorithms::MarkConnectedComponents()
{
	std::vector<PatternCodeRegion*> PatternOccs = HPCPatternDatabase::GetInstance()->GetAllPatternCodeRegions();

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
void TreeAlgorithms::MarkConnectedComponents(PatternGraphNode* Node, int ComponentID)
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

