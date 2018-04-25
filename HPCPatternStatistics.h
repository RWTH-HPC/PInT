#pragma once

#include "HPCParallelPattern.h"

class HPCPatternStatistic
{
public:
	virtual void Print() = 0;

private:
	virtual void VisitFunctionDeclDatabaseEntry(FunctionDeclDatabaseEntry* Entry);

	virtual void VisitPattern(HPCParallelPattern* Pattern);
}



class SimplePatternCountStatistic : public HPCPatternStatistic
{
public:
	SimplePatternCountStatistic() : PatternOccCounter()
	{
	}	

	void Print();

private:
	void VisitFunctionDeclDatabaseEntry(FunctionDeclDatabaseEntry* Entry);

	void VisitPattern(HPCParallelPattern* Pattern);

	struct PatternOccurenceCounter
	{
		std::string PatternName;
		int Count;
	}

	std::vector<PatternOccurenceCounter> PatternOccCounter;
}
