#include "TreeVisualisation.h"
#include <tuple>
#include <iostream>
//#define LOCDEBUG

/**
 * @brief Prints the call tree recursively, beginning with the main function.
 *
 * @param maxdepth The maximum recursion (i.e., output depth)
 **/
void CallTreeVisualisation::PrintRelationTree(int maxdepth, bool onlyPattern)
{
	std::cout << "\n RELATION TREE VISUALISATION \n";
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

void CallTreeVisualisation::PrintCallTree(int maxdepth, CallTree* CalTre, bool onlyPattern){
	std::cout << "\n CALL TREE VISUALISATION \n";
	std::vector<std::tuple<int, CallTreeNode*>> CallTreeHelp;
	int CallTreeHelpKey = 0;
	CallTreeNode* currentNode = CalTre->getRoot();
#ifdef DEBUG
	const Identification* currentIdent = currentNode->GetID();
  std::cout << *currentIdent << '\n';
#endif
#ifdef LOCDEBUG
	std::cout << currentNode << '\n';
#endif
	PrintCallTreeRecursively (CallTreeHelpKey, CallTreeHelp, currentNode, 0, maxdepth, onlyPattern);
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

void CallTreeVisualisation::PrintCallTreeRecursively(int &HelpKey, std::vector<std::tuple<int, CallTreeNode*>> &CallTreeHelp, CallTreeNode* ClTrNode, int depth, int maxdepth, bool onlyPattern){
	if(depth > maxdepth){
		return;
	}
	CallTreeNodeType nodeTypeOfClTr = ClTrNode->GetNodeType();
	if(nodeTypeOfClTr == Pattern_Begin){
		CallTreeHelp.push_back(std::make_tuple(depth, ClTrNode));
	}

	if(onlyPattern){
		if(nodeTypeOfClTr == Pattern_End||nodeTypeOfClTr == Pattern_Begin){
			if(nodeTypeOfClTr == Pattern_End){
				int depthForEnd = searchDepthInCallTreeHelp(CallTreeHelp, ClTrNode);
				PrintIndent(depthForEnd);
				#ifdef LOCDEBUG
					std::cout <<"Adress of CallTreeNode: "<< ClTrNode << '\n';
				#endif
			}
			else{
				PrintIndent(depth);
			}
			ClTrNode->print();
		}
	}
	else if(nodeTypeOfClTr!= Function_Decl){
		if(nodeTypeOfClTr != Pattern_End){
			PrintIndent(depth);
		}
		else{
			int depthForEnd = searchDepthInCallTreeHelp(CallTreeHelp, ClTrNode);
			PrintIndent(depthForEnd);
		}
		ClTrNode->print();
		#ifdef LOCDEBUG
			std::cout <<"Adress of CallTreeNode: "<< ClTrNode << '\n';
		#endif
	}

	#ifdef DEBUG
		std::cout << "\033[36m" << *ClTrNode->GetID() << ":\33[33m" << std::endl;

		if(ClTrNode->GetCaller())
			std::cout << "Caller:" << *(ClTrNode->GetCaller()->GetID())<<" Type: "<< ClTrNode->GetCaller()->GetNodeType() << std::endl;
		  std::cout << "Callees:" << std::endl;
			for(CallTreeNode* Callee : *ClTrNode->GetCallees())
			{
				std::cout << *Callee->GetID() << " Type: " << Callee->GetNodeType() << std::endl;
			}
	#endif
	for(const auto &CalleePair : *(ClTrNode->GetCallees())){
		CallTreeNode* Callee = CalleePair.second;
		if(nodeTypeOfClTr == Function_Decl){
				PrintCallTreeRecursively(HelpKey, CallTreeHelp, Callee, depth, maxdepth, onlyPattern);
		}
		else{
			PrintCallTreeRecursively(HelpKey, CallTreeHelp, Callee, depth + 1, maxdepth, onlyPattern);
		}
	}
}

/**
 * @brief Prints an indent according to the passed depth.
 *
 * @param depth Depth of indent.
 **/
void CallTreeVisualisation::PrintIndent(int depth)
{
	if(depth!=0){
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
}

int CallTreeVisualisation::searchDepthInCallTreeHelp(std::vector<std::tuple<int, CallTreeNode*>> &CallTreeHelp, CallTreeNode* EndNode){
	if(EndNode->GetNodeType()!= Pattern_End){
		std::cout << "This function should be only used for CallTreeNodes of the Type Pattern_End. Returnning 0 by default." << '\n';
		return 0;
	}
	int largestDepth = 0;
	int depthOfNode;
	for(std::tuple<int, CallTreeNode*> tup : CallTreeHelp){
		if(std::get<1>(tup) == EndNode->getCorrespCallTreeNodeRelation()){
			depthOfNode = std::get<0>(tup);
			if(largestDepth <= depthOfNode)
				largestDepth = depthOfNode;
		}
	}
	return largestDepth;
}
