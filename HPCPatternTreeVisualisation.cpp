#include "HPCPatternTreeVisualisation.h"

#include <iostream>



void HPCPatternTreeVisualisation::PrintPatternTree()
{
	FunctionDeclDatabase* FuncDB = FunctionDeclDatabase::GetInstance();
	
	FunctionDeclDatabaseEntry* MainFnEntry = FuncDB->GetMainFnEntry();
	PrintFunctionTree(MainFnEntry, 0);
}

void HPCPatternTreeVisualisation::PrintPattern(HPCParallelPattern* Pattern, int depth)
{
	PrintIndent(depth);
	std::cout << Pattern->GetPatternID() << std::endl;

	for (HPCParallelPattern* Child : Pattern->GetChildren())
	{
		PrintPattern(Child, depth + 1);	
	}

	for (FunctionDeclDatabaseEntry* FnCall : Pattern->GetFnCalls())
	{
		PrintFunctionTree(FnCall, depth + 1);
	}
}
	
void HPCPatternTreeVisualisation::PrintFunctionTree(FunctionDeclDatabaseEntry* FnCall, int depth)
{
	PrintIndent(depth);
	std::cout << FnCall->FnName << std::endl;

	for (HPCParallelPattern* Pattern : FnCall->Patterns)
	{
		PrintPattern(Pattern, depth + 1);
	}
}
	
void HPCPatternTreeVisualisation::PrintIndent(int depth)
{
	for (int i = 0; i < depth; i++)
	{
		std::cout << "\t";
	}
}
