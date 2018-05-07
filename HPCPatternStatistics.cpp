#include "HPCPatternStatistics.h"
#include <iostream>



/*
 * Methods for the simple pattern counter
 */
SimplePatternCountStatistic::SimplePatternCountStatistic() : PatternOccCounter()
{

}	

void SimplePatternCountStatistic::Calculate()
{
	/* Start the computation with the main function */
	FunctionDeclDatabase* FnDb = FunctionDeclDatabase::GetInstance();
	FunctionDeclDatabaseEntry* MainFn = FnDb->GetMainFnEntry();

	VisitFunctionCall(MainFn, 0, 10);
}

void SimplePatternCountStatistic::Print()
{
	for (SimplePatternCountStatistic::PatternOccurenceCounter* Counter : PatternOccCounter)
	{
		std::cout << "Pattern \033[33m" << Counter->PatternName << "\033[0m occurs " << Counter->Count << " times" << std::endl;
	}	
}

void SimplePatternCountStatistic::VisitFunctionCall(FunctionDeclDatabaseEntry* FnEntry, int depth, int maxdepth)
{
	if (depth > maxdepth)
	{
		return;
	}

	/* Do nothing, but visit the children */
	for (PatternOccurence* Child : FnEntry->GetChildren())
	{
		if (FunctionDeclDatabaseEntry* FnCall = clang::dyn_cast<FunctionDeclDatabaseEntry>(Child))
		{
			VisitFunctionCall(FnCall, depth + 1, maxdepth);
		}
		else if (HPCParallelPattern* Pattern = clang::dyn_cast<HPCParallelPattern>(Child))
		{
			VisitPattern(Pattern, depth + 1, maxdepth);
		}
	}
}

void SimplePatternCountStatistic::VisitPattern(HPCParallelPattern* Pattern, int depth, int maxdepth)
{
	if (depth > maxdepth)
	{
		return;
	}

	/* Look up this pattern. If there is no entry, create a new one */
	SimplePatternCountStatistic::PatternOccurenceCounter* Counter = LookupPatternOcc(Pattern);

	if (Counter != NULL)
	{
		Counter->Count++;
	}
	else 
	{
		Counter = AddPatternOcc(Pattern);	
		Counter->Count++;
	}

	/* Visit children */
	for (PatternOccurence* Child : Pattern->GetChildren())
	{
		if (FunctionDeclDatabaseEntry* FnCall = clang::dyn_cast<FunctionDeclDatabaseEntry>(Child))
		{
			VisitFunctionCall(FnCall, depth + 1, maxdepth);
		}
		else if (HPCParallelPattern* Pattern = clang::dyn_cast<HPCParallelPattern>(Child))
		{
			VisitPattern(Pattern, depth + 1, maxdepth);
		}
	}
}

SimplePatternCountStatistic::PatternOccurenceCounter* SimplePatternCountStatistic::LookupPatternOcc(HPCParallelPattern* Pattern)
{
	/* Look up the Pattern Counter for this pattern */
	for (SimplePatternCountStatistic::PatternOccurenceCounter* Counter : PatternOccCounter)
	{
		if (Pattern->GetDesignSpace() == Counter->PatternDesignSp && !Pattern->GetPatternName().compare(Counter->PatternName))
		{
			return Counter;
		}
	}

	return NULL;
}

SimplePatternCountStatistic::PatternOccurenceCounter* SimplePatternCountStatistic::AddPatternOcc(HPCParallelPattern* Pattern)
{
	/* Create a new Pattern Counter for this Pattern */
	PatternOccurenceCounter* Counter = new PatternOccurenceCounter;
	Counter->PatternDesignSp = Pattern->GetDesignSpace();	
	Counter->PatternName = Pattern->GetPatternName();
	PatternOccCounter.push_back(Counter);
	return Counter;
}



/*
 * Methods fot the Fan In, Fan Out Statistic
 */
FanInFanOutStatistic::FanInFanOutStatistic() : FIFOCounter()
{

} 

void FanInFanOutStatistic::Calculate()
{
	/* Start the computation with the main function */
	FunctionDeclDatabase* FnDb = FunctionDeclDatabase::GetInstance();
	FunctionDeclDatabaseEntry* MainFn = FnDb->GetMainFnEntry();

	VisitFunctionCall(MainFn, 0, 10);
}

void FanInFanOutStatistic::Print()
{

}

void FanInFanOutStatistic::VisitFunctionCall(FunctionDeclDatabaseEntry* FnEntry, int depth, int maxdepth)
{
	/* Do nothing, but visit the children */
	for (PatternOccurence* Child : FnEntry->GetChildren())
	{
		if (FunctionDeclDatabaseEntry* FnCall = clang::dyn_cast<FunctionDeclDatabaseEntry>(Child))
		{
			VisitFunctionCall(FnCall, depth + 1, maxdepth);
		}
		else if (HPCParallelPattern* Pattern = clang::dyn_cast<HPCParallelPattern>(Child))
		{
			VisitPattern(Pattern, depth + 1, maxdepth);
		}
	}
}

void FanInFanOutStatistic::VisitPattern(HPCParallelPattern* Pattern, int depth, int maxdepth)
{
	FanInFanOutCounter* Counter = LookupFIFOCounter(Pattern);

	if (Counter == NULL)
	{
		Counter = AddFIFOCounter(Pattern);
	}

	/* Search in child direction */

	/* Search in parent direction */

	// TODO Implement actual computation

	/* Continue with visiting the children */
	for (PatternOccurence* Child : Pattern->GetChildren())
	{
		if (FunctionDeclDatabaseEntry* FnCall = clang::dyn_cast<FunctionDeclDatabaseEntry>(Child))
		{
			VisitFunctionCall(FnCall, depth + 1, maxdepth);
		}
		else if (HPCParallelPattern* Pattern = clang::dyn_cast<HPCParallelPattern>(Child))
		{
			VisitPattern(Pattern, depth + 1, maxdepth);
		}
	}
}

FanInFanOutStatistic::FanInFanOutCounter* FanInFanOutStatistic::LookupFIFOCounter(HPCParallelPattern* Pattern)
{
	/* Look up the Pattern Counter for this pattern */
	for (FanInFanOutStatistic::FanInFanOutCounter* Counter : FIFOCounter)
	{
		if (Pattern->GetDesignSpace() == Counter->PatternDesignSp && !Pattern->GetPatternName().compare(Counter->PatternName))
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
	Counter->PatternDesignSp = Pattern->GetDesignSpace();	
	Counter->PatternName = Pattern->GetPatternName();
	FIFOCounter.push_back(Counter);
	return Counter;
}
