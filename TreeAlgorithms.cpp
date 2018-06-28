#include "TreeAlgorithms.h"
#include <vector>



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

void TreeAlgorithms::MarkConnectedComponents(PatternTreeNode* Node, int ComponentID)
{
	if (Node->GetConnectedComponent() == -1)
	{
		Node->SetConnectedComponent(ComponentID);
		
		for (PatternTreeNode* Child : Node->GetChildren())
		{
			MarkConnectedComponents(Child, ComponentID);
		}

		for (PatternTreeNode* Parent : Node->GetParents())
		{
			MarkConnectedComponents(Parent, ComponentID);
		}
	}
}

