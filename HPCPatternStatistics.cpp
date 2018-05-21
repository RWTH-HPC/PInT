#include "HPCPatternStatistics.h"
#include <iostream>
#include <fstream>



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
		std::cout << Pattern->GetPatternName() << " \033[33m" << Pattern->GetPatternID() << "\033[0m"  << " has " << Pattern->GetTotalLinesOfCode() << " line(s) of code in total:" << std::endl;
		
		std::vector<int> LinesOfCode = Pattern->GetLinesOfCode();
		int numregions = LinesOfCode.size();

		std::cout << numregions << " region(s) with ";

		for (int i = 0; i < numregions - 1; i++)
		{
			std::cout << LinesOfCode.at(i) << ", ";
		}

		std::cout << LinesOfCode.at(numregions - 1) << " line(s) of code respectively." << std::endl;
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
		File << Pattern->GetPatternName() << " " << Pattern->GetPatternID() << CSV_SEPARATOR_CHAR;
		
		std::vector<int> LinesOfCode = Pattern->GetLinesOfCode();
		int numregions = LinesOfCode.size();

		File << numregions << CSV_SEPARATOR_CHAR;

		/* Print the list of LOCs per pattern region with quotes */
		File << "\"";	
		for (int i = 0; i < numregions - 1; i++)
		{
			File << LinesOfCode.at(i) << ", ";
		}

		File << LinesOfCode.at(numregions - 1);
		File << "\"" << CSV_SEPARATOR_CHAR;
	
		File << Pattern->GetTotalLinesOfCode() << "\n";
	}	

	File.close();
}



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

void SimplePatternCountStatistic::CSVExport(std::string FileName)
{
	std::ofstream File;
	File.open(FileName, std::ios::app);

	File << "Patternname" << CSV_SEPARATOR_CHAR << "Count\n";

	for (PatternOccurenceCounter* Counter : PatternOccCounter)
	{
		File << Counter->PatternName << CSV_SEPARATOR_CHAR << Counter->Count << "\n";
	}	

	File.close();
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
		
		/* Search in Parent and Child Directions */
		std::vector<HPCParallelPattern*> Parents;
		FindParentPatterns(Pattern, Parents, maxdepth);

		std::vector<HPCParallelPattern*> Children;
		FindChildPatterns(Pattern, Children, maxdepth);

		/* Calculate the resulting fan-in and fan-out numbers */
		Counter->FanIn = Parents.size();
		Counter->FanOut = Children.size();
	}
}

void FanInFanOutStatistic::Print()
{
	for (FanInFanOutCounter* Counter : FIFOCounter)
	{
		std::cout << "Pattern \033[33m" << Counter->PatternName << "\033[0m has" << std::endl;
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
		File << Counter->PatternName << CSV_SEPARATOR_CHAR << Counter->FanIn << CSV_SEPARATOR_CHAR << Counter->FanOut << "\n";
	}	

	File.close();
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

void FanInFanOutStatistic::FindParentPatterns(HPCParallelPattern* Start, std::vector<HPCParallelPattern*>& Parents, int maxdepth)
{
	FindNeighbourPatternsRec(Start, Parents, DIR_Parents, 0, maxdepth);
}

void FanInFanOutStatistic::FindChildPatterns(HPCParallelPattern* Start, std::vector<HPCParallelPattern*>& Children, int maxdepth)
{
	FindNeighbourPatternsRec(Start, Children, DIR_Children, 0, maxdepth);
}

void FanInFanOutStatistic::FindNeighbourPatternsRec(PatternOccurence* Current, std::vector<HPCParallelPattern*>& Results, SearchDirection dir, int depth, int maxdepth)
{
	/* Check, if we reached the maximum depth */
	if (depth >= maxdepth)
	{
		return;
	}

	HPCParallelPattern* Pattern = clang::dyn_cast<HPCParallelPattern>(Current);

	if (depth > 0 && Pattern != NULL)
	{
		Results.push_back(Pattern);
	}
	else
	{
		/* Get the neighbouring nodes depending on the defined search direction */
		std::vector<PatternOccurence*> Neighbours;

		if (dir == DIR_Parents)
		{
			Neighbours = Current->GetParents();
		}
		else if (dir == DIR_Children)
		{
			Neighbours = Current->GetChildren();
		}

		/* Visit all the neighbouring nodes according to the given direction */
		for (PatternOccurence* Neighbour : Neighbours)
		{
			FindNeighbourPatternsRec(Neighbour, Results, dir, depth + 1, maxdepth);
		}
	}
}
