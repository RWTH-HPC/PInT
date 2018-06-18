#include "HPCPatternStatistics.h"
#include <iostream>
#include <fstream>
#include "TreeAlgorithms.h"
#include "Debug.h"



/*
 * Methods for the Cyclomatic Complexity Statistic
 */
CyclomaticComplexityStatistic::CyclomaticComplexityStatistic() : VisitedNodes()
{

}

void CyclomaticComplexityStatistic::Calculate()
{	
	Edges = CountEdges();
	Nodes = CountNodes();
	ConnectedComponents = CountConnectedComponents();
	
	/* C = #Edges - #Nodes + 2 * #ConnectedComponents */
	CyclomaticComplexity = (Edges - Nodes) + 2 * ConnectedComponents; 
}

void CyclomaticComplexityStatistic::Print()
{
	std::cout << "\033[33m" << "WARNING: Results from the Cyclomatic Complexity Statistic might be inconsistent!" << "\033[0m" << std::endl;
	std::cout << "Number of Edges: " << Edges << std::endl;
	std::cout << "Number of Nodes: " << Nodes << std::endl;
	std::cout << "Number of Connected Components: " << ConnectedComponents << std::endl;
	std::cout << "Resulting \033[33mCyclomatic Complexity\033[0m: " << CyclomaticComplexity << std::endl;
}

void CyclomaticComplexityStatistic::CSVExport(std::string FileName)
{
	std::ofstream File;
	File.open(FileName, std::ios::app);

	File << "CyclComplexity" << CSV_SEPARATOR_CHAR << "NumEdges" << CSV_SEPARATOR_CHAR << "NumNodes" << CSV_SEPARATOR_CHAR << "NumConnectedComps\n";
	File << CyclomaticComplexity << CSV_SEPARATOR_CHAR << Edges << CSV_SEPARATOR_CHAR << Nodes << CSV_SEPARATOR_CHAR << ConnectedComponents;
	
	File.close();
}

void CyclomaticComplexityStatistic::SetNodeVisited(PatternTreeNode* Node)
{
	if (!IsNodeVisited(Node))
	{
		VisitedNodes.push_back(Node);
	}
}

bool CyclomaticComplexityStatistic::IsNodeVisited(PatternTreeNode* Node)
{
	for (PatternTreeNode* N : VisitedNodes)
	{
		if (N == Node)
		{
			return true;
		}
	}

	return false;
}

int CyclomaticComplexityStatistic::CountEdges()
{
	/* Start the tree traversal from the Main Function */
	std::vector<FunctionDeclDatabaseEntry*> Functions = FunctionDeclDatabase::GetInstance()->GetAllFunctionEntries();

	int edges = 0;

	for (FunctionDeclDatabaseEntry* Fn : Functions)
	{
		edges += CountEdges(Fn);
	}

	return edges;
}

int CyclomaticComplexityStatistic::CountEdges(PatternTreeNode* Current)
{
	int Edges = 0;
	
	/* If we visit a pattern, add the incoming edge */
	if (PatternOccurence* PatternOcc = clang::dyn_cast<PatternOccurence>(Current))
	{
		Edges = Edges + 1;
	}

	/* If we already visited this node, then just return 1 if this is a pattern, 0 else */
	if (IsNodeVisited(Current))
	{
		return Edges;
	}
	else
	{
		SetNodeVisited(Current);
	
		/* Count the Edges beginning from the children */
		for (PatternTreeNode* Child : Current->GetChildren())
		{
			Edges += CountEdges(Child);
		}
	}

	return Edges;
}

int CyclomaticComplexityStatistic::CountNodes()
{
	HPCPatternDatabase* PDB = HPCPatternDatabase::GetInstance();
	std::vector<HPCParallelPattern*> Patterns = PDB->GetAllPatterns();

	int nodes = 0;

	/* Count all occurences for all patterns */
	for (HPCParallelPattern* Pattern : Patterns)
	{
		std::vector<PatternOccurence*> Occurences = Pattern->GetAllOccurences();
		nodes += Occurences.size();
	}

	return nodes;
}

int CyclomaticComplexityStatistic::CountConnectedComponents()
{
	TreeAlgorithms::MarkConnectedComponents();

	std::vector<PatternOccurence*> PatternOccs = HPCPatternDatabase::GetInstance()->GetAllPatternOccurences();

	int ConnectedComponents = 0;

	for (PatternOccurence* PatternOcc : PatternOccs)
	{
		if (PatternOcc->GetConnectedComponent() > ConnectedComponents)
		{
			ConnectedComponents = PatternOcc->GetConnectedComponent();
		}
	}

	return ConnectedComponents + 1;
}



/*
 * Methods for the lines of code statistic
 */
void LinesOfCodeStatistic::Calculate()
{

}

void LinesOfCodeStatistic::Print()
{
	HPCPatternDatabase* PDB = HPCPatternDatabase::GetInstance();
	std::vector<HPCParallelPattern*> Patterns = PDB->GetAllPatterns();

	for (HPCParallelPattern* Pattern : Patterns)
	{
		std::cout << "\033[33m" << Pattern->GetPatternName() << "\033[0m" << " has " << Pattern->GetTotalLinesOfCode() << " line(s) of code in total." << std::endl;

		std::vector<PatternOccurence*> Occurences = Pattern->GetAllOccurences();
		std::cout << Occurences.size() << " Occurences in code." << std::endl;

		for (PatternOccurence* PatternOcc : Occurences)
		{
			std::cout << PatternOcc->GetID() << ": " << PatternOcc->GetLinesOfCode() << std::endl;
		}

		std::cout << "Line(s) of code respectively." << std::endl << std::endl;
	}
}

void LinesOfCodeStatistic::CSVExport(std::string FileName)
{
	std::ofstream File;
	File.open(FileName, std::ios::app);

	File << "Patternname" << CSV_SEPARATOR_CHAR << "NumRegions" << CSV_SEPARATOR_CHAR << "LOCByRegions" << CSV_SEPARATOR_CHAR << "TotalLOCs\n";

	HPCPatternDatabase* PDB = HPCPatternDatabase::GetInstance();
	std::vector<HPCParallelPattern*> Patterns = PDB->GetAllPatterns();

	for (HPCParallelPattern* Pattern : Patterns)
	{
		File << Pattern->GetPatternName()  << CSV_SEPARATOR_CHAR;
		
		std::vector<PatternOccurence*> PatternOccurences = Pattern->GetAllOccurences();
		File << PatternOccurences.size() << CSV_SEPARATOR_CHAR;	

		/* Print the list of lines of code for this pattern */
		File << "\"";	
	
		for (int i = 0; i < PatternOccurences.size() - 1; i++)
		{
			File << PatternOccurences.at(i)->GetLinesOfCode() << ", ";
		}

		File << PatternOccurences.at(PatternOccurences.size() - 1)->GetLinesOfCode();
		File << "\"" << CSV_SEPARATOR_CHAR;
	
		File << Pattern->GetTotalLinesOfCode() << "\n";
	}	

	File.close();
}



/*
 * Methods for the simple pattern counter
 */
SimplePatternCountStatistic::SimplePatternCountStatistic()
{

}	

void SimplePatternCountStatistic::Calculate()
{

}

void SimplePatternCountStatistic::Print()
{
	std::vector<HPCParallelPattern*> Patterns = HPCPatternDatabase::GetInstance()->GetAllPatterns();

	for (HPCParallelPattern* Pattern : Patterns)
	{
		std::cout << "Pattern \033[33m" << Pattern->GetPatternName() << "\033[0m occurs " << Pattern->GetAllOccurences().size() << " times." << std::endl;
	}
}

void SimplePatternCountStatistic::CSVExport(std::string FileName)
{
	std::ofstream File;
	File.open(FileName, std::ios::app);

	File << "Patternname" << CSV_SEPARATOR_CHAR << "Count\n";

	std::vector<HPCParallelPattern*> Patterns = HPCPatternDatabase::GetInstance()->GetAllPatterns();

	for (HPCParallelPattern* Pattern : Patterns)
	{
		File << Pattern->GetPatternName() << CSV_SEPARATOR_CHAR << Pattern->GetAllOccurences().size() << "\n";
	}	

	File.close();
}



/*
 * Methods fot the Fan In, Fan Out Statistic
 */
FanInFanOutStatistic::FanInFanOutStatistic(int maxdepth) : FIFOCounter()
{
	this->maxdepth = maxdepth;
} 

void FanInFanOutStatistic::Calculate()
{
	HPCPatternDatabase* DB = HPCPatternDatabase::GetInstance();
	std::vector<HPCParallelPattern*> Patterns = DB->GetAllPatterns();

	for (HPCParallelPattern* Pattern : Patterns)
	{
		FanInFanOutCounter* Counter = LookupFIFOCounter(Pattern);

		if (Counter == NULL)
		{
			Counter = AddFIFOCounter(Pattern);
		}
		
		/* Gather all parents and children */
		std::vector<PatternOccurence*> Parents;
		std::vector<PatternOccurence*> Children;

		for (PatternOccurence* PatternOcc : Pattern->GetAllOccurences())
		{
#ifdef PRINT_DEBUG
			PatternOcc->Print();
			std::cout << std::endl;
#endif
			/* Search in Parent and Child Directions */
			FindParentPatterns(PatternOcc, Parents, maxdepth);
#ifdef PRINT_DEBUG
			std::cout << "List of parents: " << std::endl;			

			for (PatternOccurence* Parent : Parents)
			{
				Parent->Print();
				std::cout << std::endl;
			}
#endif
			FindChildPatterns(PatternOcc, Children, maxdepth);
#ifdef PRINT_DEBUG
			std::cout << "List of children: " << std::endl;

			for (PatternOccurence* Child : Children)
			{
				Child->Print();
				std::cout << std::endl;
			}

			std::cout << std::endl;
#endif
		}

		/* Filter out the duplicates */
		Parents = GetUniquePatternOccList(Parents);
		Children = GetUniquePatternOccList(Children);

		/* Calculate the resulting fan-in and fan-out numbers */
		Counter->FanIn += Parents.size();
		Counter->FanOut += Children.size();
	}
}

std::vector<PatternOccurence*> FanInFanOutStatistic::GetUniquePatternOccList(std::vector<PatternOccurence*> PatternOccs)
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

void FanInFanOutStatistic::Print()
{
	for (FanInFanOutCounter* Counter : FIFOCounter)
	{
		std::cout << "Pattern \033[33m" << Counter->Pattern->GetPatternName() << "\033[0m has" << std::endl;
		std::cout << "Fan-In: " << Counter->FanIn << std::endl;
		std::cout << "Fan-Out: " << Counter->FanOut << std::endl;
	}
}

void FanInFanOutStatistic::CSVExport(std::string FileName)
{
	std::ofstream File;
	File.open(FileName, std::ios::app);

	File << "Patternname" << CSV_SEPARATOR_CHAR << "FanIn" << CSV_SEPARATOR_CHAR << "FanOut" << "\n";

	for (FanInFanOutCounter* Counter : FIFOCounter)
	{
		File << Counter->Pattern->GetPatternName() << CSV_SEPARATOR_CHAR << Counter->FanIn << CSV_SEPARATOR_CHAR << Counter->FanOut << "\n";
	}	

	File.close();
}

FanInFanOutStatistic::FanInFanOutCounter* FanInFanOutStatistic::LookupFIFOCounter(HPCParallelPattern* Pattern)
{
	/* Look up the Pattern Counter for this pattern */
	for (FanInFanOutStatistic::FanInFanOutCounter* Counter : FIFOCounter)
	{
		if (Counter->Pattern == Pattern)
		{
			return Counter;
		}
	}

	return NULL;
}

FanInFanOutStatistic::FanInFanOutCounter* FanInFanOutStatistic::AddFIFOCounter(HPCParallelPattern* Pattern)
{
	/* Create a new Pattern Counter for this Pattern */
	FanInFanOutStatistic::FanInFanOutCounter* Counter = new FanInFanOutCounter;
	Counter->Pattern = Pattern;
	FIFOCounter.push_back(Counter);
	return Counter;
}

void FanInFanOutStatistic::FindParentPatterns(PatternOccurence* Start, std::vector<PatternOccurence*>& Parents, int maxdepth)
{
	FindNeighbourPatternsRec(Start, Parents, DIR_Parents, 0, maxdepth);
}

void FanInFanOutStatistic::FindChildPatterns(PatternOccurence* Start, std::vector<PatternOccurence*>& Children, int maxdepth)
{
	FindNeighbourPatternsRec(Start, Children, DIR_Children, 0, maxdepth);
}

void FanInFanOutStatistic::FindNeighbourPatternsRec(PatternTreeNode* Current, std::vector<PatternOccurence*>& Results, SearchDirection dir, int depth, int maxdepth)
{
	/* Check, if we reached the maximum depth */
	if (depth >= maxdepth)
	{
		return;
	}

	PatternOccurence* Pattern = clang::dyn_cast<PatternOccurence>(Current);

	if (depth > 0 && Pattern != NULL)
	{
		Results.push_back(Pattern);
	}
	else
	{
		/* Get the neighbouring nodes depending on the defined search direction */
		std::vector<PatternTreeNode*> Neighbours;

		if (dir == DIR_Parents)
		{
			Neighbours = Current->GetParents();
		}
		else if (dir == DIR_Children)
		{
			Neighbours = Current->GetChildren();
		}

		/* Visit all the neighbouring nodes according to the given direction */
		for (PatternTreeNode* Neighbour : Neighbours)
		{
			FindNeighbourPatternsRec(Neighbour, Results, dir, depth + 1, maxdepth);
		}
	}
}
