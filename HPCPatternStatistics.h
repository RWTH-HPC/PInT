#pragma once

#include "HPCParallelPattern.h"

#define CSV_SEPARATOR_CHAR ","



class HPCPatternStatistic
{
public:
	virtual void Calculate() = 0;

	virtual void Print() = 0;

	virtual void CSVExport(std::string FileName) = 0;
};



class CyclomaticComplexityStatistic : public HPCPatternStatistic
{
public:
	CyclomaticComplexityStatistic();
	
	void Calculate();

	void Print();

	void CSVExport(std::string FileName);

private:
	void SetNodeVisited(PatternTreeNode* Pattern);

	bool IsNodeVisited(PatternTreeNode* Pattern);

	std::vector<PatternTreeNode*> VisitedNodes;

	int CountEdges();

	int CountEdges(PatternTreeNode* Root);

	int CountNodes();

	int Nodes, Edges = 0;

	int CyclomaticComplexity = 0;
};



class LinesOfCodeStatistic : public HPCPatternStatistic
{
public:
	void Calculate();
	
	void Print();

	void CSVExport(std::string FileName);
};



class SimplePatternCountStatistic : public HPCPatternStatistic
{
public:
	SimplePatternCountStatistic();	

	void Calculate();

	void Print();

	void CSVExport(std::string FileName);

private:
	struct PatternTreeNodeCounter
	{
		DesignSpace PatternDesignSp;
		std::string PatternName;
		int Count = 0;
	};

	void VisitFunctionCall(FunctionDeclDatabaseEntry* FnEntry, int depth, int maxdepth);

	void VisitPattern(HPCParallelPattern* Pattern, int depth, int maxdepth);

	PatternTreeNodeCounter* LookupPatternOcc(HPCParallelPattern* Pattern);

	PatternTreeNodeCounter* AddPatternOcc(HPCParallelPattern* Pattern);
	
	std::vector<PatternTreeNodeCounter*> PatternOccCounter;
};



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
		DesignSpace PatternDesignSp;
		std::string PatternName;
		int FanIn = 0;
		int FanOut = 0;
	};

	enum SearchDirection
	{
		DIR_Children, DIR_Parents
	};

	void VisitFunctionCall(FunctionDeclDatabaseEntry* FnEntry, int depth, int maxdepth);

	void VisitPattern(HPCParallelPattern* Pattern, int depth, int maxdepth);

	FanInFanOutCounter* LookupFIFOCounter(HPCParallelPattern* Pattern);

	FanInFanOutCounter* AddFIFOCounter(HPCParallelPattern* Pattern);

	void FindParentPatterns(HPCParallelPattern* Start, std::vector<HPCParallelPattern*>& Parents, int maxdepth);
	
	void FindChildPatterns(HPCParallelPattern* Start, std::vector<HPCParallelPattern*>& Children, int maxdepth);

	void FindNeighbourPatternsRec(PatternTreeNode* Current, std::vector<HPCParallelPattern*>& Results, SearchDirection dir, int depth, int maxdepth);

	int maxdepth;

	std::vector<FanInFanOutCounter*> FIFOCounter;
};
