#include "TreeVisualisation.h"

#include <iostream>
//#define PRINT_ONLYPATTERNDENUG


/**
 * @brief Prints the call tree recursively, beginning with the main function.
 *
 * @param maxdepth The maximum recursion (i.e., output depth)
 **/
void CallTreeVisualisation::PrintCallTree(int maxdepth, bool onlyPattern)
{
	PatternGraphNode* RootNode = PatternGraph::GetInstance()->GetRootNode();
	if(onlyPattern){
			PrintOnlyPatternTree(maxdepth);
	}
	else{
		if (FunctionNode* Func = clang::dyn_cast<FunctionNode>(RootNode))
		{
			PrintFunction(Func, 0, maxdepth);
		}
		else if (PatternCodeRegion* CodeRegion = clang::dyn_cast<PatternCodeRegion>(RootNode))
		{
			PrintPattern(CodeRegion, 0, maxdepth);
		}
	}
}

void CallTreeVisualisation::PrintOnlyPatternTree(int maxdepth)
{
	//PatternGraph::GetInstance()->SetOnlyPatternRootNodes();

	for(PatternCodeRegion* OnlyPatRootNode : PatternGraph::GetInstance()->GetAllPatternCodeRegions())
	//hier selbst raussortieren welche Pattern als RootNode gelten(Pattern die in der Main sind und keine Eltern haben)
	{
#ifdef DEBUG
		std::cout << "(" << OnlyPatRootNode->GetPatternOccurrence()->GetID() << ")" << std::endl;
		std::cout << "HasNoPatternParents: " << OnlyPatRootNode->HasNoPatternParents() << std::endl;
		std::cout << "HasNoPatternChildren: " << OnlyPatRootNode->HasNoPatternChildren() << std::endl;
#endif

		PatternCodeRegion* CodeRegion = clang::dyn_cast<PatternCodeRegion>(OnlyPatRootNode);
		if(CodeRegion->isInMain && CodeRegion->HasNoPatternParents()){
			PrintRecursiveOnlyPattern(CodeRegion,0,maxdepth);
		}
	}

#ifdef DEBUG
	for(FunctionNode* FuncNode : PatternGraph::GetInstance()->GetAllFunctions())
	{
		if(!FuncNode->HasNoPatternParents()||!FuncNode->HasNoPatternChildren())
		std::cout << "NEUE FUNKTION" << '\n';
		std::cout << "(" << FuncNode->GetFnName() << ")" << std::endl;
		std::cout << "HasNoPatternParents: " << FuncNode->HasNoPatternParents() << std::endl;
		std::cout << "HasNoPatternChildren: " << FuncNode->HasNoPatternChildren() << std::endl;
		FuncNode->PrintVecOfPattern(FuncNode->GetPatternChildren());
	}
	#endif
}

/**
 * @brief Prints a pattern in the pattern tree with spacing according to the recursion depth.
 *
 * @param CodeRegion The code region from which the pattern is printed.
 * @param depth The current depth of recursion.
 * @param maxdepth The maximum depth of recursion.
 **/
void CallTreeVisualisation::PrintPattern(PatternCodeRegion* CodeRegion, int depth, int maxdepth)
{
	if (depth > maxdepth)
	{
		return;
	}

	PrintIndent(depth);

	HPCParallelPattern* Pattern = CodeRegion->GetPatternOccurrence()->GetPattern();
	std::cout << "\033[36m" << Pattern->GetDesignSpaceStr() << ":\33[33m " << Pattern->GetPatternName() << "\33[0m";

	std::cout << "(" << CodeRegion->GetPatternOccurrence()->GetID() << ")" << std::endl;

	for (PatternGraphNode* Child : CodeRegion->GetChildren())
	{
		if (FunctionNode* FnCall = clang::dyn_cast<FunctionNode>(Child))
		{
			PrintFunction(FnCall, depth + 1, maxdepth);
		}
		else if (PatternCodeRegion* CodeRegion = clang::dyn_cast<PatternCodeRegion>(Child))
		{
			PrintPattern(CodeRegion, depth + 1, maxdepth);
		}
	}
}

/**
 * @brief Prints a function in the pattern tree with indent.
 *
 * @param FnCall Function call.
 * @param depth Current recursion depth.
 * @param maxdepth Maximum recursion depth.
 **/
void CallTreeVisualisation::PrintFunction(FunctionNode* FnCall, int depth, int maxdepth)
{
	if (depth > maxdepth)
	{
		return;
	}
		PrintIndent(depth);
		std::cout << "\033[31m" << FnCall->GetFnName() << "\033[0m" << " (Hash: " << FnCall->GetHash() << ")" << std::endl;
	for (PatternGraphNode* Child : FnCall->GetChildren())
	{
		if (FunctionNode* FnCall = clang::dyn_cast<FunctionNode>(Child))
		{
			PrintFunction(FnCall, depth + 1, maxdepth);
		}
		else if (PatternCodeRegion* CodeRegion = clang::dyn_cast<PatternCodeRegion>(Child))
		{
			PrintPattern(CodeRegion, depth + 1, maxdepth);
		}
	}
}

void CallTreeVisualisation::PrintRecursiveOnlyPattern(PatternCodeRegion* CodeRegion,int depth,int maxdepth)
{
	if (depth > maxdepth)
	{
		return;
	}
	PrintIndent(depth);
	HPCParallelPattern* Pattern = CodeRegion->GetPatternOccurrence()->GetPattern();
	std::cout << "\033[36m" << Pattern->GetDesignSpaceStr() << ":\33[33m " << Pattern->GetPatternName() << "\33[0m";
		std::cout << "(" << CodeRegion->GetPatternOccurrence()->GetID() << ")" << std::endl;

	for (PatternGraphNode* Child : CodeRegion->GetOnlyPatternChildren())
	{
		PatternCodeRegion* CodeRegion = clang::dyn_cast<PatternCodeRegion>(Child);
		PrintRecursiveOnlyPattern(CodeRegion, depth + 1, maxdepth);
	}
}

/**
 * @brief Prints an indent according to the passed depth.
 *
 * @param depth Depth of indent.
 **/
void CallTreeVisualisation::PrintIndent(int depth)
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
