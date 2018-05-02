#pragma once

#include "HPCParallelPattern.h"



class HPCPatternStatistic
{
public:
	virtual void Calculate() = 0;

	virtual void Print() = 0;

private:
	virtual void VisitFunctionCall(FunctionDeclDatabaseEntry* FnEntry) = 0;

	virtual void VisitPattern(HPCParallelPattern* Pattern) = 0;
};



class SimplePatternCountStatistic : public HPCPatternStatistic
{
public:
	SimplePatternCountStatistic();	

	void Calculate();

	void Print();

private:
	struct PatternOccurenceCounter
	{
		DesignSpace PatternDesignSp;
		std::string PatternName;
		int Count = 0;
	};

	void VisitFunctionCall(FunctionDeclDatabaseEntry* FnEntry);

	void VisitPattern(HPCParallelPattern* Pattern);

	PatternOccurenceCounter* LookupPatternOcc(HPCParallelPattern* Pattern);

	PatternOccurenceCounter* AddPatternOcc(HPCParallelPattern* Pattern);
	
	std::vector<PatternOccurenceCounter*> PatternOccCounter;
};
