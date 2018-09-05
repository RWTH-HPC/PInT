#pragma once

#include "HPCParallelPattern.h"
#include <string>
#include "Helpers.h"

#define CSV_SEPARATOR_CHAR ","


/**
 * Abstract class for pattern statistics.
 * Statistics have to inherit from this class and implement the virtual methods.
 */
class HPCPatternStatistic
{
public:
	/**
	 * Calculate the statistics value(s).
	 */
	virtual void Calculate() = 0;

	/**
 	 * Print the statistic in human-readable form to std::cout
 	 */ 	
	virtual void Print() = 0;

	/**
 	 * Export the statistic value(s) to a csv file.
 	 */
	virtual void CSVExport(std::string FileName) = 0;
};



/**
 * A statistic class that calculates the cyclomatic complexity as defined by McCabe.
 * We adapted this software engineering metric for our pattern use case.
 * The cyclomatic complexity is calculated as C = (edges - nodes) + 2 * ConnectedComponents
 * Edges are countes as follows: the connection between two PatternCodeRegion is defined as an edge.
 * Function nodes are ignored.
 * The number of nodes is the number of unique PatternCodeRegions.
 */
class CyclomaticComplexityStatistic : public HPCPatternStatistic
{
public:
	CyclomaticComplexityStatistic();
	
	void Calculate();

	void Print();

	void CSVExport(std::string FileName);

private:
	void SetNodeVisited(PatternGraphNode* Node);

	bool IsNodeVisited(PatternGraphNode* Node);

	std::vector<PatternGraphNode*> VisitedNodes;

	int CountEdges();

	int CountEdges(PatternGraphNode* Root);

	int CountNodes();

	int CountConnectedComponents();

	void MarkConnectedComponent(PatternGraphNode * Node, int ComponentID);

	int Nodes, Edges, ConnectedComponents = 0;

	int CyclomaticComplexity = 0;
};


/**
 * A simple statistic class that prints the lines of code for each pattern.
 */
class LinesOfCodeStatistic : public HPCPatternStatistic
{
public:
	void Calculate();
	
	void Print();

	void CSVExport(std::string FileName);
};


/**
 * A statistic that prints for each pattern how many occurrences there are.
 */
class SimplePatternCountStatistic : public HPCPatternStatistic
{
public:
	SimplePatternCountStatistic();	

	void Calculate();

	void Print();

	void CSVExport(std::string FileName);
};



/**
 * This statistic calculates the fan-in and fan-out numbers for every pattern which occurs in the code.
 * The fan-in number is calculated as the number of patterns which are a direct parent to the pattern in question.
 * The fan-out number is the number of patterns which are direct children to this pattern.
 */
class FanInFanOutStatistic : public HPCPatternStatistic
{
public:
	FanInFanOutStatistic(int maxdepth);

	void Calculate();

	void Print();

	void CSVExport(std::string FileName);

private:
	struct FanInFanOutCounter
	{
		HPCParallelPattern* Pattern;
		int FanIn = 0;
		int FanOut = 0;
	};

	void VisitFunctionCall(FunctionNode* FnEntry, int depth, int maxdepth);

	void VisitPattern(PatternCodeRegion* PatternOcc, int depth, int maxdepth);

	FanInFanOutCounter* LookupFIFOCounter(HPCParallelPattern* Pattern);

	FanInFanOutCounter* AddFIFOCounter(HPCParallelPattern* Pattern);

	void FindParentPatterns(PatternCodeRegion* Start, std::vector<PatternOccurrence*>& Parents, int maxdepth);
	
	void FindChildPatterns(PatternCodeRegion* Start, std::vector<PatternOccurrence*>& Children, int maxdepth);

	int maxdepth;

	std::vector<FanInFanOutCounter*> FIFOCounter;
};
