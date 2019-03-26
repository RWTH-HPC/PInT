#include "TreeVisualisation.h"

#include <iostream>



/**
 * @brief Prints the call tree recursively, beginning with the main function.
 *
 * @param maxdepth The maximum recursion (i.e., output depth)
 **/
void CallTreeVisualisation::PrintCallTree(int maxdepth, bool onlyPattern)
{
		if(onlyPattern){
			PatternGraph* actlGraph = PatternGraph::GetInstance();
			for(PatternCodeRegion* RootNode : actlGraph->GetAllPatternCodeRegions()){
				if(!RootNode->hasPatternParent()){
					PrintPattern(RootNode, 0, maxdepth, onlyPattern);
					}
			}
		}

		else{
			PatternGraphNode* RootNode = PatternGraph::GetInstance()->GetRootNode();
			if (FunctionNode* Func = clang::dyn_cast<FunctionNode>(RootNode))
			{
				PrintFunction(Func, 0, maxdepth, onlyPattern);
			}
			else if (PatternCodeRegion* CodeRegion = clang::dyn_cast<PatternCodeRegion>(RootNode))
			{
				PrintPattern(CodeRegion, 0, maxdepth, onlyPattern);
			}
		}
}

/**
 * @brief Prints a pattern in the pattern tree with spacing according to the recursion depth.
 *
 * @param CodeRegion The code region from which the pattern is printed.
 * @param depth The current depth of recursion.
 * @param maxdepth The maximum depth of recursion.
 **/
void CallTreeVisualisation::PrintPattern(PatternCodeRegion* CodeRegion, int depth, int maxdepth, bool onlyPattern)
{

		if (depth > maxdepth)
		{
			return;
		}

		PrintIndent(depth);

		HPCParallelPattern* Pattern = CodeRegion->GetPatternOccurrence()->GetPattern();
		std::cout << "\033[36m" << Pattern->GetDesignSpaceStr() << ":\33[33m " << Pattern->GetPatternName() << "\33[0m";

		std::cout << "(" << CodeRegion->GetPatternOccurrence()->GetID() << ")" << std::endl;

	if(!onlyPattern){
		for (PatternGraphNode* Child : CodeRegion->GetChildren())
		{ 
			if (FunctionNode* FnCall = clang::dyn_cast<FunctionNode>(Child))
			{
				PrintFunction(FnCall, depth + 1, maxdepth, onlyPattern);
			}
			else if (PatternCodeRegion* CodeRegion = clang::dyn_cast<PatternCodeRegion>(Child))
			{
				PrintPattern(CodeRegion, depth + 1, maxdepth, onlyPattern);
			}
		}
	}
	else{
		for (PatternGraphNode* Child : CodeRegion->GetChildren())
		{
		 	if (PatternCodeRegion* CodeRegion = clang::dyn_cast<PatternCodeRegion>(Child))
			{

				PrintPattern(CodeRegion, depth + 1 , maxdepth, onlyPattern);
			}
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
void CallTreeVisualisation::PrintFunction(FunctionNode* FnCall, int depth, int maxdepth,  bool onlyPattern)
{
	if (depth > maxdepth)
	{
		return;
	}
	if (!onlyPattern){
		PrintIndent(depth);
		std::cout << "\033[31m" << FnCall->GetFnName() << "\033[0m" << " (Hash: " << FnCall->GetHash() << ")" << std::endl;
	}
	std::vector<PatternGraphNode*> pointerToChildren = FnCall->GetChildren();
	for (PatternGraphNode* Child : FnCall->GetChildren())
	{
		if (FunctionNode* FnCall = clang::dyn_cast<FunctionNode>(Child))
		{
			PrintFunction(FnCall, depth + 1, maxdepth, onlyPattern);
		}
		else if (PatternCodeRegion* CodeRegion = clang::dyn_cast<PatternCodeRegion>(Child))
		{
			++depth;
			PrintPattern(CodeRegion, depth + 1, maxdepth, onlyPattern);
		}
	}
	return;
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
