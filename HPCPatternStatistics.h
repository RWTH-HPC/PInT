#pragma once

#include "HPCParallelPattern.h"
#include <string>
#include "Helpers.h"

#define NUMOFDIFFSTATS 5
#define CSV_SEPARATOR_CHAR ","

#ifndef HPCPATTERNASTTREVERSAL_H

#include "HPCPatternInstrHandler.h"

#endif
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

	/**
	 * @brief Calls CyclomaticComplexityStatistic::CountEdges(), CyclomaticComplexityStatistic::CountNodes() and CyclomaticComplexityStatistic::CountConnectedComponents() to calculate the Cyclomatic Complexity Statistic C = (Edges - Nodes) + 2 * ConnectedComponents
	 */
	void Calculate();
	/**
	 * @brief Prints the number of edges, nodes, connected components and the resulting cyclomatic complexity.
	 */
	void Print();
	/**
	 * @brief Function for csv output of the form "cycl. complexity, edges, nodes, connectedcomponents".
	 *
	 * @param FileName The file name of the desired output file.
	 **/
	void CSVExport(std::string FileName);

private:

	/**
	 * @brief Set a node to visited for calculation of the number of edges in the pattern tree.
	 *
	 * @param Node The node that is set visited.
	 **/
	void SetNodeVisited(PatternGraphNode* Node);
	/**
	 * @brief Enquire if the node is visited (see CyclomaticComplexityStatistic::SetNodeVisited()).
	 *
	 * @param Node Node that is checked.
	 *
	 * @return True if visited, false else.
	 **/
	bool IsNodeVisited(PatternGraphNode* Node);

	std::vector<PatternGraphNode*> VisitedNodes;
	/**
	 * @brief Calls CyclomaticComplexityStatistic::CountEdges(PatternGraphNode*) with the entries of all functions in the FunctionDeclDatabase.
	 *
	 * @return The number of edges in the pattern tree.
	 **/
	int CountEdges();
	/**
	 * @brief Counts the number of edges in the pattern tree (Disregarding function calls) recursively.
	 *
	 * @param Current The node with which the counting starts.
	 *
	 * @return The number of edges.
	 **/
	int CountEdges(PatternGraphNode* Root);
	/**
	 * @brief Counts the number of nodes in the pattern tree (i.e. only PatternCodeRegions) by requesting from the HPCPatternDatabase.
	 *
	 * @return The number of nodes.
	 **/
	int CountNodes();
	/**
	 * @brief Connected components are marked by call of GraphAlgorithms::MarkConnectedComponents().
	 * Then, the number of connected components is calculated.
	 *
	 * @return The number of connected components.
	 **/
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
	/**
	 * @brief Prints statistics about lines of code for each HPCParallelPattern and PatternOccurrence.
	 */
	void Print();
	/**
	 * @brief CSV export of the statistic. Format "Patternname, NumRegions, LOCByRegions (list), TotalLOCs".
	 *
	 * @param FileName File name of the output file.
	 **/
	void CSVExport(std::string FileName);
};


/**
 * A statistic that prints for each pattern how many occurrences there are.
 */
class SimplePatternCountStatistic : public HPCPatternStatistic
{
public:
	SimplePatternCountStatistic();
	/**
	 * @brief Dummy function.
	 */
	void Calculate();
	/**
	 * @brief Prints for each pattern, how often it occurs in the code.
	 */
	void Print();
	/**
	 * @brief CSV export of the static. Format "Patternname, Count".
	 *
	 * @param FileName File name of the output file.
	 **/
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
	/**
	 * @brief Constructor for the Fan-In Fan-Out statistic.
	 *
	 * @param maxdepth The maximum recursion depth when descending in the tree.
	 **/
	FanInFanOutStatistic(int maxdepth);
	/**
	 * @brief Calculates the Fan-In and Fan-Out statistic for each Pattern.
	 * First, all children and parents for all PatternOccurrence and PatternCodeRegions are gathered.
	 * Then, duplicates are removed.
	 * The remaining patterns are counted and saved in FanInFanOutStatistic::FanInFanOutCounter objects.
	 */
	void Calculate();
	/**
	 * @brief Print Fan-In and Fan-Out for all patterns previously encountered.
	 */
	void Print();
	/**
	 * @brief CSV export of the results. Format "PatternName, Fan-In, Fan-Out".
	 *
	 * @param FileName File name of the output file.
	 **/
	void CSVExport(std::string FileName);
	/**
	 *@brief this method is used to gether the CallTreeNodes which are suitable to be used by this statistic.
	 **/
	 std::vector<PatternCodeRegion*> GetCodeRegions(HPCParallelPattern* Pattern);
private:
	struct FanInFanOutCounter
	{
		HPCParallelPattern* Pattern;
		int FanIn = 0;
		int FanOut = 0;
	};

	void VisitFunctionCall(FunctionNode* FnEntry, int depth, int maxdepth);

	void VisitPattern(PatternCodeRegion* PatternOcc, int depth, int maxdepth);
	/**
	 * @brief Retrieve the fan-in fan-out counter for a specific pattern.
	 *
	 * @param Pattern The pattern to find.
	 *
	 * @return The counter that belongs to the pattern.
	 **/
	FanInFanOutCounter* LookupFIFOCounter(HPCParallelPattern* Pattern);
	/**
	 * @brief Create a fan-in fan-out counter for a pattern.
	 * Adds the counter to the list of counters.
	 *
	 * @param Pattern
	 *
	 * @return The counter created.
	 **/
	FanInFanOutCounter* AddFIFOCounter(HPCParallelPattern* Pattern);
	/**
	 * @brief Finds the parent patterns, beginning from a PatternCodeRegion.
	 * Saves the parent patterns in the list of PatternOccurrence passed as second parameter.
	 *
	 * @param Start Initial PatternCodeRegion from which the search is started.
	 * @param Parents Reference to a std::vector of PatternOccurrence* in which the encountered occurrences are saved.
	 * @param maxdepth Maximum depth of the recursion.
	 **/
	void FindParentPatterns(PatternCodeRegion* Start, std::vector<PatternOccurrence*>& Parents, int maxdepth);
	/**
	 * @brief Finds the child patterns from a starting point. See FanInFanOutStatistic::FindParentPatterns().
	 **/
	void FindChildPatterns(PatternCodeRegion* Start, std::vector<PatternOccurrence*>& Children, int maxdepth);

	int maxdepth;
	std::vector<HPCParallelPattern*> Pattern;

	std::vector<FanInFanOutCounter*> FIFOCounter;
};

//int HalsteadAnzOperator;

class Halstead : public HPCPatternStatistic{
public:

	Halstead();

	void Calculate();

	void Print();

	void CSVExport(std::string FileName);

	void insertPattern(HPCParallelPattern* Pat);

private:
	int numOfOperators;
	std::vector <HPCParallelPattern*> HPatterns;
};

/*
class HalsteadMetrikPattern : public HPCPatternStatistic
{
public:
 HalsteadMetrikPattern();

 void Calculate();

 void Print();

 void CSVExport(std::string FileName);

private:
int CalculateProgrammLength();

int CalculateVolume();

int CalculateDifficulty();

int CalculateEffort();

int CalculateHalsteadTime();

int CalculateNumberOfOperands();

int CalculateNumberOfOperators();

bool IsOperand(HPCParallelPattern* Pattern);

bool IsOperator(HPCParallelPattern* Pattern);
}*/
