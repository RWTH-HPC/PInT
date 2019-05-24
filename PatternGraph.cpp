#include "PatternGraph.h"

#include "HPCParallelPattern.h"

#include <iostream>
#include "clang/AST/ODRHash.h"

//#define DEBUG_PATTERNREGISTRATIION


/*
 * Function Declaration Database Entry functions
 */
FunctionNode::FunctionNode (std::string Name, unsigned Hash) : PatternGraphNode(GNK_FnCall), Children(), Parents()
{
	this->FnName = Name;
	this->Hash = Hash;
}

void FunctionNode::AddChild(PatternGraphNode* Child)
{
	Children.push_back(Child);
}

void FunctionNode::AddParent(PatternGraphNode* Parent)
{
	Parents.push_back(Parent);
}

void FunctionNode::AddPatternParent(PatternGraphNode* PatParent)
{/*Before we add a parent we look if this parent has not been registered already
	*/
	PatternCodeRegion* PatternParent = clang::dyn_cast<PatternCodeRegion>(PatParent);
	for(PatternCodeRegion* PatRegFor : this->PatternParents){
		if(PatRegFor->GetID() == PatternParent->GetID()) return;
	}
	this->PatternParents.push_back(PatternParent);
}

void FunctionNode::AddPatternParents(std::vector<PatternCodeRegion*> PatternParents){
	for(PatternCodeRegion* PatParent : PatternParents){
		this->AddPatternParent(PatParent);
	}
}

void FunctionNode::AddPatternChild(PatternGraphNode* PatChild)
{/*Before we add a child we look if this child has not been registered already
	*/
	PatternCodeRegion* PatternChild = clang::dyn_cast<PatternCodeRegion>(PatChild);
	for(PatternCodeRegion* PatRegFor : this->PatternChildren){
		if(PatRegFor->GetID() == PatternChild->GetID()) return;
	}
	this->PatternChildren.push_back(PatternChild);
}

std::vector<PatternCodeRegion*> FunctionNode::GetPatternParents()
{
	return this->PatternParents;
}

std::vector<PatternCodeRegion*> FunctionNode::GetPatternChildren()
{
	return this->PatternChildren;
}

bool FunctionNode::HasNoPatternParents(){
	if(this->PatternParents.size()){
		return false;
	}
	return true;
}

bool FunctionNode::HasNoPatternChildren(){
	if(this->PatternChildren.size()){
		return false;
	}
	return true;
}

void FunctionNode::registerPatChildrenToPatParents(){
	for(PatternCodeRegion* PChild : this->PatternChildren)
	{
		 for(PatternCodeRegion* PParent : this->PatternParents)
		 {
			 PParent->AddOnlyPatternChild(PChild);
			 PChild->AddOnlyPatternParent(PParent);
			 #ifdef DEBUG_PATTERNREGISTRATIION
				 HPCParallelPattern* ParentPattern = PParent->GetPatternOccurrence()->GetPattern();
		 		 std::cout << "\033[36m" << ParentPattern->GetDesignSpaceStr() << ":\33[33m " << ParentPattern->GetPatternName() << "\33[0m";
				 std::cout << "(" << PParent->GetPatternOccurrence()->GetID() << ")" << " has now the child: " <<std::endl;

					HPCParallelPattern* ChildPattern = PChild->GetPatternOccurrence()->GetPattern();
				  std::cout << "\033[36m" << ChildPattern->GetDesignSpaceStr() << ":\33[33m " << ChildPattern->GetPatternName() << "\33[0m";
				  std::cout << "(" << PChild->GetPatternOccurrence()->GetID() << ")" << std::endl;
			#endif
		 }
	}
}

void FunctionNode::PrintVecOfPattern(std::vector<PatternCodeRegion*> RegionVec)
{
		for(PatternCodeRegion* PatReg : RegionVec)
		{
			 HPCParallelPattern* Pattern = PatReg->GetPatternOccurrence()->GetPattern();
			 std::cout << "\033[36m" << Pattern->GetDesignSpaceStr() << ":\33[33m " << Pattern->GetPatternName() << "\33[0m";
			 std::cout << "(" << PatReg->GetPatternOccurrence()->GetID() << ")" << " has now the child: " <<std::endl;
		}
}

PatternGraph::PatternGraph() : Functions(), Patterns(), PatternOccurrences()
{

}

/**
 * @brief Selects and returns the root node for a tree representation.
 *
 * @return Root Node for tree representation.
 **/
PatternGraphNode* PatternGraph::GetRootNode()
{
	if (this->RootNode != NULL)
	{
		return this->RootNode;
	}

	/* Return the first pattern if there is no designated root node. */
	return (PatternGraphNode*)Patterns.front();
}

std::vector<PatternGraphNode*> PatternGraph::GetOnlyPatternRootNodes(){
		return this->OnlyPatternRootNodes;
}
/**
 * @brief Lookup function for the database entry that corresponds to the given function declaration.
 *
 * This function takes a clang function declaration object as input and calculates an ODR hash value from the object.
 * This value is then used for lookup of the corresponding entry in our function declaration database to enable linking of function calls and bodies with their declarations between translation units.
 *
 * @param Decl The clang object that belongs to a function declaration in the source code.
 *
 * @return The (new) function declaration database entry corresponding to the function declaration.
 **/
FunctionNode* PatternGraph::GetFunctionNode(clang::FunctionDecl* Decl)
{
	clang::ODRHash Hash;
	Hash.AddDecl(Decl);
	unsigned HashVal = Hash.CalculateHash();

	std::string FnName = Decl->getNameInfo().getName().getAsString();

	// Search for an existing entry
	for (FunctionNode* Func : Functions)
	{
		if (Func->GetHash() == HashVal)
		{
			return Func;
		}
	}

	return NULL;
}


	void PatternGraph::RegisterOnlyPatternRootNode(PatternCodeRegion* CodeReg)
	{
		this->OnlyPatternRootNodes.push_back(CodeReg);
	}
/**
 * @brief Registers a function with the database in the PatternGraph.
 *
 * This function registers a FunctionNode object in the PatternGraph class.
 * The object is created based on the data extracted from the Clang FunctionDecl object.
 * A unique ODR hash value is calculated for identification.
 *
 * @param Decl The clang function declaration object.
 *
 * @return False if the function is already registered. Else, true.
 **/
bool PatternGraph::RegisterFunction(clang::FunctionDecl* Decl)
{
	if (GetFunctionNode(Decl) != NULL)
	{
		return false;
	}

	/* Extract information from the clang object */
	clang::ODRHash Hash;
	Hash.AddDecl(Decl);
	unsigned HashVal = Hash.CalculateHash();

	std::string FnName = Decl->getNameInfo().getName().getAsString();


	/* Allocate a new entry */
	FunctionNode* Func;
	Func = new FunctionNode(FnName, HashVal);
	Functions.push_back(Func);


	/* Set as root node if this is the main function */
	if (Decl->isMain())
	{
		this->RootNode = Func;
	}

	return Func;
}

/**
 * @brief This function is used to look for a HPCParallelPattern object in the pattern database using the design space and the pattern name as search criteria.
 *
 * @param DesignSp The design space of the pattern we are looking for.
 * @param PatternName The name of the pattern.
 *
 * @return HPCParallelPattern object that matches the search criteria or NULL.
 **/
HPCParallelPattern* PatternGraph::GetPattern(DesignSpace DesignSp, std::string PatternName)
{
	/* Go through the list of parallel patterns to find the parallel pattern with the given identifier */
	for (HPCParallelPattern* Pattern : Patterns)
	{
		if (DesignSp == Pattern->GetDesignSpace() && !PatternName.compare(Pattern->GetPatternName()))
		{
			return Pattern;
		}
	}

	return NULL;
}

/**
 * @brief Adds a parallel pattern to the database.
 *
 * @param Pattern The parallel pattern that is added.
 *
 * @return False if the pattern is already registered. Else, true.
 **/
bool PatternGraph::RegisterPattern(HPCParallelPattern* Pattern)
{
	if (GetPattern(Pattern->GetDesignSpace(), Pattern->GetPatternName()) != NULL)
	{
		return false;
	}

	Patterns.push_back(Pattern);
	return true;
}


/**
 * @brief Finds a PatternOccurrence object in the database by its ID.
 *
 * @param ID The ID of the PatternOccurrence we are searching.
 *
 * @return The PatternOccurrence object if successful, NULL else.
 **/
PatternOccurrence* PatternGraph::GetPatternOccurrence(std::string ID)
{
	for (PatternOccurrence* PatternOcc : PatternOccurrences)
	{
		if (!ID.compare(PatternOcc->GetID()))
		{
			return PatternOcc;
		}
	}

	return NULL;
}

/**
 * @brief Adds a PatternOccurrence to the database.
 *
 * @param PatternOcc The PatternOccurrence to add.
 *
 * @return False if the pattern occurrence is already registered. Else, true.
 **/
bool PatternGraph::RegisterPatternOccurrence(PatternOccurrence* PatternOcc)
{
	if (GetPatternOccurrence(PatternOcc->GetID()) != NULL)
	{
		return false;
	}

	PatternOccurrences.push_back(PatternOcc);

	return true;
}

/**
 * @brief Collects all PatternCodeRegion objects and returns them.
 *
 * @return All PatternCodeRegion objects linked to this PatternOccurrence
 **/
std::vector<PatternCodeRegion*> PatternGraph::GetAllPatternCodeRegions()
{
	std::vector<PatternCodeRegion*> CodeRegions;

	for (PatternOccurrence* PatternOcc : PatternOccurrences)
	{
		for (PatternCodeRegion* CodeRegion : PatternOcc->GetCodeRegions())
		{
			CodeRegions.push_back(CodeRegion);
		}
	}

	return CodeRegions;
}
