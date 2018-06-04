#include "HPCPatternTreeVisualisation.h"

#include <iostream>



void HPCPatternTreeVisualisation::PrintPatternTree(int maxdepth)
{
	FunctionDeclDatabase* FuncDB = FunctionDeclDatabase::GetInstance();
	
	FunctionDeclDatabaseEntry* MainFnEntry = FuncDB->GetMainFnEntry();
	PrintFunctionTree(MainFnEntry, 0, maxdepth);
}

void HPCPatternTreeVisualisation::PrintPattern(PatternOccurence* PatternOcc, int depth, int maxdepth)
{
	if (depth > maxdepth)
	{	
		return;
	}
	
	PrintIndent(depth);

	HPCParallelPattern* Pattern = PatternOcc->GetPattern();
	std::cout << "\033[36m" << Pattern->GetDesignSpaceStr() << ":\33[33m " << Pattern->GetPatternName() << "\33[0m";

	std::cout << "(" << PatternOcc->GetID() << ")" << std::endl;

	for (PatternTreeNode* Child : PatternOcc->GetChildren())
	{
		if (FunctionDeclDatabaseEntry* FnCall = clang::dyn_cast<FunctionDeclDatabaseEntry>(Child))
		{
			PrintFunctionTree(FnCall, depth + 1, maxdepth);
		}
		else if (PatternOccurence* PatternOcc = clang::dyn_cast<PatternOccurence>(Child))
		{
			PrintPattern(PatternOcc, depth + 1, maxdepth);
		}
	}
}
	
void HPCPatternTreeVisualisation::PrintFunctionTree(FunctionDeclDatabaseEntry* FnCall, int depth, int maxdepth)
{
	if (depth > maxdepth)
	{	
		return;
	}

	PrintIndent(depth);
	std::cout << "\033[31m" << FnCall->GetFnName() << "\033[0m" << " (Hash: " << FnCall->GetHash() << ")" << std::endl;

	for (PatternTreeNode* Child : FnCall->GetChildren())
	{
		if (FunctionDeclDatabaseEntry* FnCall = clang::dyn_cast<FunctionDeclDatabaseEntry>(Child))
		{
			PrintFunctionTree(FnCall, depth + 1, maxdepth);
		}
		else if (PatternOccurence* PatternOcc = clang::dyn_cast<PatternOccurence>(Child))
		{
			PrintPattern(PatternOcc, depth + 1, maxdepth);
		}
	}
}
	
void HPCPatternTreeVisualisation::PrintIndent(int depth)
{
	int i = 0;

	for (; i < depth - 1; i++)
	{
		std::cout << "    ";
	}

	for (; i < depth; i++)
	{
		std::cout << "--> ";
	}
}
