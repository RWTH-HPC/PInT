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
	/* Do the same thing for the callTree*/
	if (Decl->isMain())
	{
		this->RootNode = Func;
		ClTre->setRootNode(HashVal);
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

CallTree* ClTre = new CallTree();

Identification::~Identification()
{
	std::cout << "Identification of:" <<IdentificationString <<
"or" << IdentificationUnsigned << "is deleted"<< '\n';
}

Identification::Identification(){
}

Identification::Identification(CallTreeNodeType type, std::string identification)
{

	if(type == Pattern_Begin || type == Pattern_End){
		this->IdentificationString = identification;
	}
}

Identification::Identification(CallTreeNodeType type, unsigned identification)
{
	if(type == Function || type == Function_Decl || type == Root){
		this->IdentificationUnsigned = identification;
	}
}

bool Identification::compare(Identification* ident)
{
	if(ident->IdentificationString.empty()){
		return IdentificationUnsigned == ident->IdentificationUnsigned;
	}
	return !IdentificationString.compare(ident->IdentificationString);
}

bool Identification::compare(unsigned Hash)
{
	return IdentificationUnsigned == Hash;
}

bool Identification::compare(std::string Id)
{
	return !IdentificationString.compare(Id);
}

std::ostream& operator<<(std::ostream &os, Identification const &ident)
{
	if((ident.getIdentificationString()).compare(""))
	{
		return os << ident.getIdentificationString();

	}
	if(ident.getIdentificationUnsigned() != 0){
		return os << ident.getIdentificationUnsigned();
	}
	return os << "Something went wrong the Identification is not set";
}

bool CallTree::everyPatternHasEnd()
{
	std::vector<CallTreeNode*> temp = this->PatternNodesOfCallTree;
  for(CallTreeNode* Node : temp){
		if(!Node->hasEnd()){
			return false;
		}
	}
	return true;
}

CallTree::CallTree()
{
		RootNode = NULL;
}

CallTree::~CallTree(){
}

void CallTree::registerNode(CallTreeNodeType NodeType, PatternCodeRegion* PatCodeReg, CallTreeNodeType LastVisited, PatternCodeRegion* TopOfStack, FunctionNode* surroundingFunc)
{
	CallTreeNode* Node = new CallTreeNode(NodeType, PatCodeReg->GetID());
	if(NodeType == Pattern_Begin || NodeType == Pattern_End)
	{
		if(LastVisited == Function_Decl)
		{
			appendCallerToNode(surroundingFunc, Node);
			std::cout << "Last visited Functionsdeklaration von " <<surroundingFunc->GetHash()<< '\n';
		}
		else if(LastVisited == Pattern_Begin)
		{
			if(TopOfStack == NULL || (NodeType == Pattern_End && !(TopOfStack->GetID().compare(PatCodeReg->GetID()))))
			{
				appendCallerToNode(surroundingFunc, Node);
			}
			else
			{
				appendCallerToNode(TopOfStack, Node);
				std::cout << "TopOfStack not empty" << '\n';
			}
			std::cout << "Last visited PatternCodeReg" << '\n';
		}
	}

}

void CallTree::registerNode(CallTreeNodeType NodeType, FunctionNode* FuncNode, CallTreeNodeType LastVisited, PatternCodeRegion* TopOfStack, FunctionNode* surroundingFunc)
{
	CallTreeNode* Node = new CallTreeNode(NodeType, FuncNode->GetHash());
	
	if(LastVisited == Function_Decl && !Node->compare(surroundingFunc->GetHash())){
		appendCallerToNode(surroundingFunc, Node);
	}
	else if(LastVisited == Pattern_Begin){
		if(TopOfStack == NULL){
			appendCallerToNode(surroundingFunc, Node);
		}
		else{
			appendCallerToNode(TopOfStack, Node);
		}
	}
}


void CallTree::setRootNode(unsigned identification)
{
	CallTreeNode* root = new CallTreeNode(Root, identification);
	RootNode = root;
	this->insertNodeIntoDeclVector(root);
}

void CallTree::appendCallerToNode(CallTreeNode* Caller, CallTreeNode* Node)
{
	if(RootNode && RootNode->compare(Caller)){
		Node->SetCaller(RootNode);
		RootNode->insertCallee(Node);
	}
	else{
		for(CallTreeNode* VecNode : DeclarationVector){
			if(VecNode->compare(Caller)){
			std::cout << "Hänge gerade "<< *VecNode->GetID()<< "an " << *Caller->GetID() << "an"<< std::endl;
				VecNode->SetCaller(Caller);
				Caller->insertCallee(VecNode);
				break;
			}
		}
	}
	std::cout << "getting now out of not infinite function" << std::endl;
}

void CallTree::appendCallerToNode(FunctionNode* Caller, CallTreeNode* Node)
{		
	if(RootNode && RootNode->compare(Caller->GetHash())){
		Node->SetCaller(RootNode);
		RootNode->insertCallee(Node);
	}
	else{
			for(CallTreeNode* DeclOfCaller : this->DeclarationVector){
				if(DeclOfCaller->compare(Caller->GetHash())){
					Node->SetCaller(DeclOfCaller);
					DeclOfCaller->insertCallee(Node);
					return;
				}
				std::cout << "Something went wrong could not find DeclOfCaller in DeclVector" << '\n';
			}
		}
	}

void CallTree::appendCallerToNode(PatternCodeRegion* Caller, CallTreeNode* Node)
{
	if(RootNode && RootNode->compare(Caller->GetID())){
		Node->SetCaller(RootNode);
		RootNode->insertCallee(Node);
	}
	else{
		for(CallTreeNode* VecNode : PatternNodesOfCallTree){
			if(VecNode->compare(Caller->GetID())){
				Node->SetCaller(VecNode);
			}
		}
	}
}

void CallTree::insertNodeIntoPatternVector(CallTreeNode* Node)
{
	PatternNodesOfCallTree.push_back(Node);
}

void CallTree::insertNodeIntoDeclVector(CallTreeNode* Node)
{
	DeclarationVector.push_back(Node);
}

void CallTree::appendAllDeclToCallTree(CallTreeNode* Node, int maxdepth)
{	std::cout << "appen AllDecl of " << *Node->GetID()<< std::endl;
	if(maxdepth > 0 ){
		CallTreeNodeType NodeType = Node->getNodeType();
		if(NodeType == Root || NodeType == Function_Decl){
		   for(CallTreeNode* DeclOfCallee : DeclarationVector){
			for(CallTreeNode* CalleeOfNode : *Node->GetCallees()){
				if(CalleeOfNode->compare(DeclOfCallee)){ //falls die Kinder von Node die selbe indentität haben wie eine deklaration im Declaration Vector dann...
					appendCallerToNode(CalleeOfNode, DeclOfCallee);
					appendAllDeclToCallTree(DeclOfCallee, maxdepth - 1);
				}
			}
		
		   }
		}
	}
}

std::vector<CallTreeNode*>* CallTree::GetDeclarationVector(){
	return &DeclarationVector;
}

CallTreeNode::~CallTreeNode(){
	std::cout << "loesche gerade Knoten" << '\n';

}

CallTreeNode::CallTreeNode(CallTreeNodeType type, std::string identification) : NodeType(type)
{
	if(NodeType == Pattern_Begin)
	{
		ClTre->insertNodeIntoPatternVector(this);
	}
	ident = new Identification(type, identification);
		std::cout << "Node of:" << identification <<
		"is created"<< '\n';
		if(type == Pattern_Begin)
		        std::cout << "NodeType = Pattern_Begin" << std::endl;
		 else if (type == Pattern_End)
		    	std::cout << "NodeType = Pattern_End" << std::endl;
}

CallTreeNode::CallTreeNode(CallTreeNodeType type ,unsigned identification) : NodeType(type)
{
	if(NodeType == Function_Decl)
	{
		ClTre->insertNodeIntoDeclVector(this);
	}
	ident = new Identification(type, identification);
		std::cout << "Node of:"<< identification << "is created"<< '\n';
		if(type == Function)
		   std::cout << "Node Type = Function" << std::endl;
		else if (type == Function_Decl)
		   std::cout << "Node Type = Function_Decl" << std::endl;

		else if(type == Root)
		   std::cout << "Node Type = Root" << std::endl;
		else
		   std::cout << "Was da los" << std::endl;
}

Identification* CallTreeNode::GetID()
{
	return this->ident;
}

std::vector<CallTreeNode*>* CallTreeNode::GetCallees(){
	return &Callees;
}

CallTreeNode* CallTreeNode::GetCaller(){
	return this->Caller;
}

void CallTreeNode::insertCallee(CallTreeNode* Node){
	(Callees).push_back(Node);
}

void CallTreeNode::SetCaller(CallTreeNode* Node)
{
	this->Caller = Node;
}

bool CallTreeNode::hasEnd()
{
	if(this->NodeType == Pattern_Begin)
	{
		std::vector<CallTreeNode*> tempQueue = this->Callees;
		CallTreeNode* tempNode;
		bool returnBool;
		for(int i = 0; i < tempQueue.size(); i++){
			tempNode = tempQueue[i];

			if(tempNode->compare(this) && tempNode->NodeType == Pattern_End){
				return true;
			}
			else{
				returnBool = tempNode->hasEnd();
			}
			if(returnBool){
				return returnBool;
			}
		}
		return false;
	}
	return false;
}

bool CallTreeNode::compare(CallTreeNode* otherNode)
{
	return ident->compare(otherNode->GetID());
}

bool CallTreeNode::compare(unsigned Hash)
{
	return ident->compare(Hash);
}

bool CallTreeNode::compare(std::string Id)
{
	return ident->compare(Id);
}

bool CallTreeNode::isCalleeOf(CallTreeNode* Caller){
	for(CallTreeNode* CalleeOfCaller : *Caller->GetCallees())
	{
		if(this->compare(CalleeOfCaller))
			return true;
		return false;
	}
}
