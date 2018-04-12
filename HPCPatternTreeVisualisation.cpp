#include "HPCPatternTreeVisualisation.h"

#include <iostream>



void HPCPatternTreeVisualisation::PrintPatternTree(int maxdepth)
{
	FunctionDeclDatabase* FuncDB = FunctionDeclDatabase::GetInstance();
	
	FunctionDeclDatabaseEntry* MainFnEntry = FuncDB->GetMainFnEntry();
	PrintFunctionTree(MainFnEntry, 0, maxdepth);
}

void HPCPatternTreeVisualisation::PrintPattern(HPCParallelPattern* Pattern, int depth, int maxdepth)
{
	if (depth > maxdepth)
	{	
		return;
	}
	
	PrintIndent(depth);
	std::cout << Pattern->GetPatternID() << std::endl;

	for (HPCParallelPattern* Child : Pattern->GetChildren())
	{
		PrintPattern(Child, depth + 1, maxdepth);	
	}

	for (FunctionDeclDatabaseEntry* FnCall : Pattern->GetFnCalls())
	{
		PrintFunctionTree(FnCall, depth + 1, maxdepth);
	}
}
	
void HPCPatternTreeVisualisation::PrintFunctionTree(FunctionDeclDatabaseEntry* FnCall, int depth, int maxdepth)
{
	if (depth > maxdepth)
	{	
		return;
	}

	PrintIndent(depth);
	std::cout << "\033[31m" << FnCall->GetFnName() << "\033[0m" << " (" << FnCall->GetHash() << ")" << std::endl;

	for (HPCParallelPattern* Pattern : FnCall->GetPatterns())
	{
		PrintPattern(Pattern, depth + 1, maxdepth);
	}

	for (FunctionDeclDatabaseEntry* FnCall : FnCall->GetFnCalls())
	{
		PrintFunctionTree(FnCall, depth + 1, maxdepth);
	}
}
	
void HPCPatternTreeVisualisation::PrintIndent(int depth)
{
	int i = 0;

	for (; i < depth - 1; i++)
	{
		std::cout << "---";
	}

	for (; i < depth; i++)
	{
		std::cout << "--> ";
	}
}
