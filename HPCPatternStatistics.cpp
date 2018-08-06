#include "HPCPatternStatistics.h"
#include <iostream>
#include <fstream>
#include "Debug.h"



/*
 * Methods for the Cyclomatic Complexity Statistic
 */
CyclomaticComplexityStatistic::CyclomaticComplexityStatistic() : VisitedNodes()
{

}

/**
 * @brief Calls CyclomaticComplexityStatistic::CountEdges(), CyclomaticComplexityStatistic::CountNodes() and CyclomaticComplexityStatistic::CountConnectedComponents() to calculate the Cyclomatic Complexity Statistic C = (Edges - Nodes) + 2 * ConnectedComponents
 */
void CyclomaticComplexityStatistic::Calculate()
{	
	Edges = CountEdges();
	Nodes = CountNodes();
	ConnectedComponents = CountConnectedComponents();
	
	/* C = #Edges - #Nodes + 2 * #ConnectedComponents */
	CyclomaticComplexity = (Edges - Nodes) + 2 * ConnectedComponents; 
}

/**
 * @brief Prints the number of edges, nodes, connected components and the resulting cyclomatic complexity.
 */
void CyclomaticComplexityStatistic::Print()
{
	std::cout << "\033[33m" << "WARNING: Results from the Cyclomatic Complexity Statistic might be inconsistent!" << "\033[0m" << std::endl;
	std::cout << "Number of Edges: " << Edges << std::endl;
	std::cout << "Number of Nodes: " << Nodes << std::endl;
	std::cout << "Number of Connected Components: " << ConnectedComponents << std::endl;
	std::cout << "Resulting \033[33mCyclomatic Complexity\033[0m: " << CyclomaticComplexity << std::endl;
}

/**
 * @brief Function for csv output of the form "cycl. complexity, edges, nodes, connectedcomponents".
 *
 * @param FileName The file name of the desired output file.
 **/
void CyclomaticComplexityStatistic::CSVExport(std::string FileName)
{
	std::ofstream File;
	File.open(FileName, std::ios::app);

	File << "CyclComplexity" << CSV_SEPARATOR_CHAR << "NumEdges" << CSV_SEPARATOR_CHAR << "NumNodes" << CSV_SEPARATOR_CHAR << "NumConnectedComps\n";
	File << CyclomaticComplexity << CSV_SEPARATOR_CHAR << Edges << CSV_SEPARATOR_CHAR << Nodes << CSV_SEPARATOR_CHAR << ConnectedComponents;
	
	File.close();
}

/**
 * @brief Set a node to visited for calculation of the number of edges in the pattern tree.
 *
 * @param Node The node that is set visited.
 **/
void CyclomaticComplexityStatistic::SetNodeVisited(PatternGraphNode* Node)
{
	if (!IsNodeVisited(Node))
	{
		VisitedNodes.push_back(Node);
	}
}

/**
 * @brief Enquire if the node is visited (see CyclomaticComplexityStatistic::SetNodeVisited()).
 *
 * @param Node Node that is checked.
 *
 * @return True if visited, false else.
 **/
bool CyclomaticComplexityStatistic::IsNodeVisited(PatternGraphNode* Node)
{
	for (PatternGraphNode* N : VisitedNodes)
	{
		if (N == Node)
		{
			return true;
		}
	}

	return false;
}

/**
 * @brief Calls CyclomaticComplexityStatistic::CountEdges(PatternGraphNode*) with the entries of all functions in the FunctionDeclDatabase.
 *
 * @return The number of edges in the pattern tree.
 **/
int CyclomaticComplexityStatistic::CountEdges()
{
	/* Start the tree traversal from all functions */
	std::vector<FunctionNode*> Functions = FunctionDeclDatabase::GetInstance()->GetAllFunctionEntries();

	int edges = 0;

	for (FunctionNode* Fn : Functions)
	{
		edges += CountEdges(Fn);
	}

	return edges;
}

/**
 * @brief Counts the number of edges in the pattern tree (Disregarding function calls) recursively.
 *
 * @param Current The node with which the counting starts.
 *
 * @return The number of edges.
 **/
int CyclomaticComplexityStatistic::CountEdges(PatternGraphNode* Current)
{
	int Edges = 0;
	
	/* If we visit a pattern, add the incoming edge */
	if (PatternCodeRegion* Pattern = clang::dyn_cast<PatternCodeRegion>(Current))
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
		for (PatternGraphNode* Child : Current->GetChildren())
		{
			Edges += CountEdges(Child);
		}
	}

	return Edges;
}

/**
 * @brief Counts the number of nodes in the pattern tree (i.e. only PatternCodeRegions) by requesting from the HPCPatternDatabase.
 *
 * @return The number of nodes.
 **/
int CyclomaticComplexityStatistic::CountNodes()
{
	HPCPatternDatabase* PDB = HPCPatternDatabase::GetInstance();
	std::vector<HPCParallelPattern*> Patterns = PDB->GetAllPatterns();

	int nodes = 0;

	/* Count all occurences for all patterns */
	for (HPCParallelPattern* Pattern : Patterns)
	{
		std::vector<PatternCodeRegion*> CodeRegs = Pattern->GetCodeRegions();
		nodes += CodeRegs.size();
	}

	return nodes;
}

/**
 * @brief Connected components are marked by call of TreeAlgorithms::MarkConnectedComponents().
 * Then, the number of connected components is calculated.
 *
 * @return The number of connected components.
 **/
int CyclomaticComplexityStatistic::CountConnectedComponents()
{
	TreeAlgorithms::MarkConnectedComponents();

	std::vector<PatternCodeRegion*> CodeRegs = HPCPatternDatabase::GetInstance()->GetAllPatternCodeRegions();
	std::vector<FunctionNode*> Functions = FunctionDeclDatabase::GetInstance()->GetAllFunctionEntries();

	int ConnectedComponents = 0;

	/* Get CCs from the code regions */
	for (PatternCodeRegion* Reg : CodeRegs)
	{
		if (Reg->GetConnectedComponent() > ConnectedComponents)
		{
			ConnectedComponents = Reg->GetConnectedComponent();
		}
	}

	/* Also check the function calls */
	for (FunctionNode* Fn : Functions)
	{
		if (Fn->GetConnectedComponent() > ConnectedComponents)
		{
			ConnectedComponents = Fn->GetConnectedComponent();
		}
	}

	/* Return highest index + 1 (null based index) */
	return ConnectedComponents + 1;
}



/*
 * Methods for the lines of code statistic
 */
void LinesOfCodeStatistic::Calculate()
{

}

/**
 * @brief Prints statistics about lines of code for each HPCParallelPattern and PatternOccurence.
 */
void LinesOfCodeStatistic::Print()
{
	HPCPatternDatabase* PDB = HPCPatternDatabase::GetInstance();
	std::vector<HPCParallelPattern*> Patterns = PDB->GetAllPatterns();

	for (HPCParallelPattern* Pattern : Patterns)
	{
		std::cout << "\033[33m" << Pattern->GetPatternName() << "\033[0m" << " has " << Pattern->GetTotalLinesOfCode() << " line(s) of code in total." << std::endl;

		std::vector<PatternOccurence*> Occurences = Pattern->GetOccurences();
		std::cout << Occurences.size() << " occurences in code." << std::endl;

		for (PatternOccurence* PatternOcc : Occurences)
		{
			std::cout << PatternOcc->GetID() << ": " << PatternOcc->GetTotalLinesOfCode() << " LOC in " << PatternOcc->GetNumberOfCodeRegions() << " regions." << std::endl;
		}

		std::cout << "Line(s) of code respectively." << std::endl << std::endl;
	}
}

/**
 * @brief CSV export of the statistic. Format "Patternname, NumRegions, LOCByRegions (list), TotalLOCs".
 *
 * @param FileName File name of the output file.
 **/
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
		
		std::vector<PatternCodeRegion*> PatternCodeRegions = Pattern->GetCodeRegions();
		File << PatternCodeRegions.size() << CSV_SEPARATOR_CHAR;	

		/* Print the list of lines of code for this pattern */
		File << "\"";	
	
		for (int i = 0; i < PatternCodeRegions.size() - 1; i++)
		{
			File << PatternCodeRegions.at(i)->GetLinesOfCode() << ", ";
		}

		File << PatternCodeRegions.at(PatternCodeRegions.size() - 1)->GetLinesOfCode();
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

/**
 * @brief Dummy function.
 */
void SimplePatternCountStatistic::Calculate()
{

}

/**
 * @brief Prints for each pattern, how often it occurs in the code.
 */
void SimplePatternCountStatistic::Print()
{
	std::vector<HPCParallelPattern*> Patterns = HPCPatternDatabase::GetInstance()->GetAllPatterns();

	for (HPCParallelPattern* Pattern : Patterns)
	{
		std::cout << "Pattern \033[33m" << Pattern->GetPatternName() << "\033[0m occurs " << Pattern->GetOccurences().size() << " times." << std::endl;
	}
}

/**
 * @brief CSV export of the static. Format "Patternname, Count".
 *
 * @param FileName File name of the output file.
 **/
void SimplePatternCountStatistic::CSVExport(std::string FileName)
{
	std::ofstream File;
	File.open(FileName, std::ios::app);

	File << "Patternname" << CSV_SEPARATOR_CHAR << "Count\n";

	std::vector<HPCParallelPattern*> Patterns = HPCPatternDatabase::GetInstance()->GetAllPatterns();

	for (HPCParallelPattern* Pattern : Patterns)
	{
		File << Pattern->GetPatternName() << CSV_SEPARATOR_CHAR << Pattern->GetOccurences().size() << "\n";
	}	

	File.close();
}



/**
 * @brief Constructor for the Fan-In Fan-Out statistic.
 *
 * @param maxdepth The maximum recursion depth when descending in the tree.
 **/
FanInFanOutStatistic::FanInFanOutStatistic(int maxdepth) : FIFOCounter()
{
	this->maxdepth = maxdepth;
} 

/**
 * @brief Calculates the Fan-In and Fan-Out statistic for each Pattern.
 * First, all children and parents for all PatternOccurence and PatternCodeRegions are gathered.
 * Then, duplicates are removed.
 * The remaining patterns are counted and saved in FanInFanOutStatistic::FanInFanOutCounter objects.
 */
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
		
		/* We want to count the number of pattern occurences */
		std::vector<PatternOccurence*> Parents;
		std::vector<PatternOccurence*> Children;

		for (PatternCodeRegion* CodeReg : Pattern->GetCodeRegions())
		{
#ifdef PRINT_DEBUG
			CodeReg->Print();
			std::cout << std::endl;
#endif
			/* Search in Parent and Child Directions */
			FindParentPatterns(CodeReg, Parents, maxdepth);
#ifdef PRINT_DEBUG
			std::cout << "List of parents: " << std::endl;			

			for (PatternOccurence* Parent : Parents)
			{
				Parent->Print();
				std::cout << std::endl;
			}
#endif
			FindChildPatterns(CodeReg, Children, maxdepth);
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

/**
 * @brief Remove duplicates from a list of PatternOccurence.
 * The criterion is defined by PatternOccurence::Equals().
 *
 * @param PatternOccs List of PatternOccurence objects containing duplicates.
 *
 * @return List of PatternOccurence objects free from duplicates.
 **/
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

/**
 * @brief Print Fan-In and Fan-Out for all patterns previously encountered.
 */
void FanInFanOutStatistic::Print()
{
	for (FanInFanOutCounter* Counter : FIFOCounter)
	{
		std::cout << "Pattern \033[33m" << Counter->Pattern->GetPatternName() << "\033[0m has" << std::endl;
		std::cout << "Fan-In: " << Counter->FanIn << std::endl;
		std::cout << "Fan-Out: " << Counter->FanOut << std::endl;
	}
}

/**
 * @brief CSV export of the results. Format "PatternName, Fan-In, Fan-Out".
 *
 * @param FileName File name of the output file.
 **/
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

/**
 * @brief Retrieve the fan-in fan-out counter for a specific pattern.
 *
 * @param Pattern The pattern to find.
 *
 * @return The counter that belongs to the pattern.
 **/
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

/**
 * @brief Create a fan-in fan-out counter for a pattern.
 * Adds the counter to the list of counters.
 *
 * @param Pattern 
 *
 * @return The counter created.
 **/
FanInFanOutStatistic::FanInFanOutCounter* FanInFanOutStatistic::AddFIFOCounter(HPCParallelPattern* Pattern)
{
	/* Create a new Pattern Counter for this Pattern */
	FanInFanOutStatistic::FanInFanOutCounter* Counter = new FanInFanOutCounter;
	Counter->Pattern = Pattern;
	FIFOCounter.push_back(Counter);
	return Counter;
}

/**
 * @brief Finds the parent patterns, beginning from a PatternCodeRegion.
 * Saves the parent patterns in the list of PatternOccurence passed as second parameter.
 *
 * @param Start Initial PatternCodeRegion from which the search is started.
 * @param Parents Reference to a std::vector of PatternOccurence* in which the encountered occurences are saved.
 * @param maxdepth Maximum depth of the recursion.
 **/
void FanInFanOutStatistic::FindParentPatterns(PatternCodeRegion* Start, std::vector<PatternOccurence*>& Parents, int maxdepth)
{
	FindNeighbourPatternsRec(Start, Parents, DIR_Parents, 0, maxdepth);
}

/**
 * @brief Finds the child patterns from a starting point. See FanInFanOutStatistic::FindParentPatterns().
 **/
void FanInFanOutStatistic::FindChildPatterns(PatternCodeRegion* Start, std::vector<PatternOccurence*>& Children, int maxdepth)
{
	FindNeighbourPatternsRec(Start, Children, DIR_Children, 0, maxdepth);
}

/**
 * @brief Core functionality for finding neightbours of a PatternGraphNode (in this case: PatternCodeRegion).
 * The direction of the recursive descent is passed as a parameter.
 *
 * @param Current The starting point of the recursive descent.
 * @param Results Vector of PatternOccurence neighbours.
 * @param dir The search direction (Children or parents)
 * @param depth The current depth of recursion.
 * @param maxdepth The maximum recursion depth.
 **/
void FanInFanOutStatistic::FindNeighbourPatternsRec(PatternGraphNode* Current, std::vector<PatternOccurence*>& Results, SearchDirection dir, int depth, int maxdepth)
{
	/* Check, if we reached the maximum depth */
	if (depth >= maxdepth)
	{
		return;
	}

	PatternCodeRegion* Pattern = clang::dyn_cast<PatternCodeRegion>(Current);

	if (depth > 0 && Pattern != NULL)
	{
		Results.push_back(Pattern->GetPatternOccurence());
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
			FindNeighbourPatternsRec(Neighbour, Results, dir, depth + 1, maxdepth);
		}
	}
}
