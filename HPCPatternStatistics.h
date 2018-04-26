#pragma once

#include "HPCParallelPattern.h"

class HPCPatternStatistic
{
public:
	virtual void Calculate() = 0;

	virtual void Print() = 0;

private:
	virtual void VisitFunction(FunctionDeclDatabaseEntry* FnEntry);

	virtual void VisitPattern(HPCParallelPattern* Pattern);
}



class SimplePatternCountStatistic : public HPCPatternStatistic
{
public:
	SimplePatternCountStatistic() : PatternOccCounter()
	{
	}	

	void Calculate();

	void Print();

private:
	void VisitFunction(FunctionDeclDatabaseEntry* FnEntry);

	void VisitPattern(HPCParallelPattern* Pattern);

	PatternOccurenceCounter* LookupPatternOcc(HPCParallelPattern* Pattern);

	AddPatternOcc(HPCParallelPattern* Pattern);
	
	struct PatternOccurenceCounter
	{
		DesignSpace PatternDesignSp;
		std::string PatternName;
		int Count = 0;
	}

	std::vector<PatternOccurenceCounter*> PatternOccCounter;
}
