#include "PatternGraph.h"

#include "HPCParallelPattern.h"

#include <iostream>
#include "clang/AST/ODRHash.h"
#include "HPCError.h"

#define SUITEDFORSTATSDEBUG
//#define TEST
//#define CURRDEBUG
//#define DEBUG
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
	}

	return Func;
}


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

bool PatternGraph::RegisterPattern(HPCParallelPattern* Pattern)
{
	if (GetPattern(Pattern->GetDesignSpace(), Pattern->GetPatternName()) != NULL)
	{
		return false;
	}

	Patterns.push_back(Pattern);
	return true;
}

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

bool PatternGraph::RegisterPatternOccurrence(PatternOccurrence* PatternOcc)
{
	if (GetPatternOccurrence(PatternOcc->GetID()) != NULL)
	{
		return false;
	}

	PatternOccurrences.push_back(PatternOcc);

	return true;
}

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

CallTree::CallTree()
{
		RootNode = NULL;
}

CallTree::~CallTree(){
}

CallTreeNode* CallTree::registerNode(CallTreeNodeType NodeType, PatternCodeRegion* PatCodeReg, CallTreeNodeType LastVisited, PatternCodeRegion* TopOfStack, FunctionNode* surroundingFunc)
{
	CallTreeNode* Node = new CallTreeNode(NodeType, PatCodeReg);
	if(NodeType == Pattern_Begin || NodeType == Pattern_End)
	{
		if(LastVisited == Function_Decl)
		{
			appendCallerToNode(surroundingFunc, Node);
			#ifdef DEBUG
				std::cout << "Last visited Functionsdeklaration von " <<surroundingFunc->GetHash()<< '\n';
			#endif
		}
		else if(LastVisited == Pattern_Begin)
		{
			if(TopOfStack == NULL)
			{
				appendCallerToNode(surroundingFunc, Node);
				#ifdef DEBUG
					std::cout << "Last visited PatternCodereg von " <<PatCodeReg->GetID()<< " and Stack not empty!"<< '\n';
				#endif
			}
			else
			{
				appendCallerToNode(TopOfStack, Node);
				#ifdef DEBUG
					std::cout << "TopOfStack not empty" << '\n';
				#endif
			}
			#ifdef DEBUG
				std::cout << "Last visited PatternCodeReg" << '\n';
			#endif
		}
		#ifdef DEBUG
			std::cout << "Last visited probably not set" << '\n';
		#endif
	}
	return Node;
}

CallTreeNode* CallTree::registerNode(CallTreeNodeType NodeType, FunctionNode* FuncNode, CallTreeNodeType LastVisited, PatternCodeRegion* TopOfStack, FunctionNode* surroundingFunc)
{
	CallTreeNode* Node = new CallTreeNode(NodeType, FuncNode);
 if(NodeType == Function){
  // warunung das hier muss später ersetzt werden so wird auch die Rekursion ausgeschlossen
	 if(LastVisited == Function_Decl){
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
	return Node;
}

CallTreeNode* CallTree::registerEndNode(CallTreeNodeType NodeType, std::string identification, CallTreeNodeType LastVisited, PatternCodeRegion* TopOfStack, FunctionNode* surroundingFunc){
	PatternCodeRegion* CorrespReg = PatternIDisUsed(identification);
	CallTreeNode* Node;
	if(CorrespReg!=NULL)
		Node = new CallTreeNode(NodeType, CorrespReg);
	else
		Node = new CallTreeNode(NodeType, identification);
		#ifdef DEBUG
			std::cout << "LastVisited = "<< LastVisited << '\n';
		#endif
		if(LastVisited == Function_Decl)
		{
			appendCallerToNode(surroundingFunc, Node);
			#ifdef DEBUG
				std::cout << "Last visited Functionsdeklaration von " <<surroundingFunc->GetHash()<< '\n';
			#endif
		}
		else if(LastVisited == Pattern_Begin)
		{
			if(TopOfStack == NULL)
			{
				appendCallerToNode(surroundingFunc, Node);
				#ifdef DEBUG
					std::cout << "Last visited " <<surroundingFunc->GetHash()<< " and Stack is empty!"<< '\n';
				#endif
			}
			else
			{
				appendCallerToNode(TopOfStack, Node);
				#ifdef DEBUG
					std::cout << "TopOfStack not empty" << '\n';
				#endif
			}
			#ifdef DEBUG
				std::cout << "Last visited PatternCodeReg" << '\n';
			#endif
		}
		#ifdef DEBUG
			std::cout << "Last visited probably not set" << '\n';
		#endif
		return Node;
}


void CallTree::setRootNode(CallTreeNode* root)
{
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
			// it is not allowed to append a Node to itself, in the Code this is also not possible
			if(VecNode->compare(Caller) && VecNode != Caller){
				#ifdef DEBUG
					std::cout << "Hänge gerade "<< *VecNode->GetID()<<" Typ: "<< VecNode->GetNodeType() << " an " << *Caller->GetID() << " Typ: " << Caller->GetNodeType() <<" an"<< std::endl;
				#endif
				VecNode->SetCaller(Caller);
				Caller->insertCallee(VecNode);
				break;
			}

		}
	}
}

void CallTree::appendCallerToNode(FunctionNode* Caller, CallTreeNode* Node)
{
	if(RootNode && RootNode->compare(Caller->GetHash())){
		Node->SetCaller(RootNode);
		RootNode->insertCallee(Node);
	}
	else{
			for(CallTreeNode* DeclOfCaller : this->DeclarationVector){
				#ifdef DEBUG
					if(Node->GetID()->getIdentificationString() == "TQ1"){
						std::cout << "for TQ1 checking Declaration: "<< *DeclOfCaller->GetID() << '\n';
					}
				#endif
				if(DeclOfCaller->compare(Caller->GetHash())){
					#ifdef DEBUG
						std::cout << "comparison successful. Appending "<<*DeclOfCaller->GetID()<<"as Caller" << '\n';
					#endif
					Node->SetCaller(DeclOfCaller);
					DeclOfCaller->insertCallee(Node);
					return;
				}
			}
			std::cout << "Something went wrong could not find DeclOfCaller in DeclVector (Function)" << '\n';
		}
	}

void CallTree::appendCallerToNode(PatternCodeRegion* Caller, CallTreeNode* Node)
{
	if(RootNode && RootNode->compare(Caller->GetID())){
		Node->SetCaller(RootNode);
		RootNode->insertCallee(Node);
	}
	else{
			for(CallTreeNode* DeclOfCaller : this->DeclarationVector){
				if(DeclOfCaller->compare(Caller->GetID())){
					Node->SetCaller(DeclOfCaller);
					DeclOfCaller->insertCallee(Node);
					return;
				}
			}
			std::cout << "Something went wrong could not find DeclOfCaller in DeclVector (Pattern)" << '\n';
	}
}

void CallTree::insertNodeIntoPattern_EndVector(CallTreeNode* Node)
{
	Pattern_EndVector.push_back(Node);
}

void CallTree::insertNodeIntoDeclVector(CallTreeNode* Node)
{
	DeclarationVector.push_back(Node);
}

void CallTree::appendAllDeclToCallTree(CallTreeNode* Node, int maxdepth)
{
	#ifdef DEBUG
		std::cout << "appen AllDecl of " << *Node->GetID()<< std::endl;
	#endif
	if(maxdepth > 0 ){
	 for(CallTreeNode* DeclOfCallee : DeclarationVector){
		for(const auto &CalleeOfNodePair : *Node->GetCallees()){
			CallTreeNode* CalleeOfNode = CalleeOfNodePair.second;
			if(CalleeOfNode->GetNodeType() != Pattern_End  && CalleeOfNode->compare(DeclOfCallee)){
				//falls die Kinder von Node die gleiche indentität haben wie eine deklaration im Declaration Vector dann...
				#ifdef DEBUG
					std::cout << "appended "<< *CalleeOfNode->GetID()<< "as a Caller to "<<*DeclOfCallee->GetID() << '\n';
				#endif
				appendCallerToNode(CalleeOfNode, DeclOfCallee);
				appendAllDeclToCallTree(DeclOfCallee, maxdepth - 1);
			}
		 }
		}
	}
}

void CallTree::setUpTree(){
	#ifdef LOCDEBUG
		std::cout << "PRINTING PATTNERN_ENDVECTOR:" << '\n';
		for(CallTreeNode* Node : Pattern_EndVector){
			std::cout << *Node->GetID() << '\n';
		}
	#endif
	for(CallTreeNode* EndNode : Pattern_EndVector){
		if(EndNode->getCorrespondingCodeRegion()== NULL){
			PatternCodeRegion* CorrespReg = PatternIDisUsed(EndNode->GetID()->getIdentificationString());
			EndNode->setCorrespondingNode(CorrespReg);
		}
		else{
			#ifdef DEBUG
			 std::cout << "EndNode->setCorrespondingNode(CorrespReg) is not NULL in setUpTree" << '\n';
			#endif
		}
		#ifdef CURRDEBUG
			std::cout << "Current End Node "<< '\n';
			EndNode->print();
		#endif
		CallTreeNode* CorrespBegin  = findCorrespBegin(EndNode);
		try{
			if(CorrespBegin != NULL){
				CorrespBegin->setCorrespCallTreeNodeRelation(EndNode);
			}
			else
				throw TooManyEndsException();
		}
		catch(TooManyEndsException &e){
			e.what();
			throw TerminateEarlyException();
		}
	}
	//check if we have too many Begins
	try{
	for(CallTreeNode* BeginNode : *GetDeclarationVector()){
		if(BeginNode->GetNodeType()!= Pattern_Begin)
			break;
		if(BeginNode->getCorrespCallTreeNodeRelation()==NULL){
			throw TooManyBeginsException(BeginNode->GetID()->getIdentificationString());
		}
	}
	}
	catch(TooManyBeginsException& exept){
		exept.what();
		throw TerminateEarlyException();
	}
}

CallTreeNode* CallTree::findCorrespBegin(CallTreeNode* EndNode){
	if(EndNode->GetNodeType() != Pattern_End)
		return NULL;

	CallTreeNode* Caller = EndNode->GetCaller();
	CallTreeNode* Callertemp = EndNode;

	#ifdef CURRDEBUG
		std::cout << "Actual Caller: "<< *Caller->GetID()<<std::endl;
	#endif

	bool hasBegin = false;

	while(!hasBegin){
		if (Caller == NULL)
			throw TooManyEndsException(EndNode->GetID()->getIdentificationString());

		#ifdef CURRDEBUG
			std::cout << "Actual Caller: " << '\n';
			if(Caller != NULL)
			std::cout<<	*Caller->GetID();
			else
				std::cout << "The Caller does not exist." << '\n';
			std::cout << "Actual Node: "<< '\n';
			Callertemp->print();
		#endif

		Caller->setLOCTillPatternEnd(Callertemp, EndNode);
		hasBegin = Caller->compare(EndNode);

		if(Caller->GetNodeType() == Pattern_Begin && !hasBegin){
			EndNode->setSuitedForNestingStatisticsTo(false);
			Caller->setSuitedForNestingStatisticsTo(false);

			std::cout << "PRINTING PATTERN THAT ARE NOT SUITED FOR STATISTICS WHICH NEED CLEAR NESTING" << '\n';
			std::cout << "Pattern " << *EndNode->GetID()<<" and "<< *Caller->GetID()<< " is not suited for statistics which need clear nesting of Pattern. " << '\n';
			std::cout << "The first Pattern_Begin occurence before the Pattern_End of "<<*EndNode->GetID()<<" is "<< *Caller->GetID() << '\n';
		}

		if(hasBegin){
			Caller->setCorrespCallTreeNodeRelation(EndNode);
			EndNode->setCorrespCallTreeNodeRelation(Caller);
			break;
		}

		Callertemp = Caller;
		Caller = Callertemp->GetCaller();
	}

	if(hasBegin){
		return Caller;
	}
	return NULL;
}

 bool CallTree::lookIfTreeIsCorrect(){
	 for(CallTreeNode* BeginNode : DeclarationVector){
		 if(BeginNode->GetNodeType()== Pattern_Begin){
			 if(!BeginNode->getCorrespCallTreeNodeRelation())
			 throw TooManyBeginsException(BeginNode->GetID()->getIdentificationString());
		 }
	 }
	 for(CallTreeNode* EndNode : Pattern_EndVector){
		 if(EndNode->GetNodeType() == Pattern_End){
			 if(!EndNode->getCorrespCallTreeNodeRelation())
			 	throw TooManyEndsException(EndNode->GetID()->getIdentificationString());
		 }
	 }
	 return true;
 }

std::vector<CallTreeNode*>* CallTree::GetDeclarationVector(){
	return &DeclarationVector;
}

CallTreeNode::~CallTreeNode(){
	std::cout << "loesche gerade Knoten" << '\n';
}

CallTreeNode::CallTreeNode(CallTreeNodeType type, PatternCodeRegion* CorrespondingPat) : NodeType(type)
{
	if(NodeType == Pattern_Begin)
	{
		ClTre->insertNodeIntoDeclVector(this);
	}
	else if(NodeType == Pattern_End)
		ClTre->insertNodeIntoPattern_EndVector(this);
	ident = new Identification(type, CorrespondingPat->GetID());
	this->setCorrespondingNode(CorrespondingPat);
	CorrespondingPat->insertCorrespondingCallTreeNode(this);

	#ifdef DEBUG
		std::cout << "Node of: " << CorrespondingPat->GetID() <<
		" is created"<< '\n';
		if(type == Pattern_Begin)
		        std::cout << "NodeType = Pattern_Begin" << std::endl;
		 else if (type == Pattern_End)
		    	std::cout << "NodeType = Pattern_End" << std::endl;
	#endif
}

CallTreeNode::CallTreeNode(CallTreeNodeType type ,FunctionNode* CorrespondingFunction) : NodeType(type)
{
	if(NodeType == Function_Decl)
	{
		ClTre->insertNodeIntoDeclVector(this);
	}
	ident = new Identification(type, CorrespondingFunction->GetHash());
	this->setCorrespondingNode(CorrespondingFunction);
	CorrespondingFunction->insertCorrespondingCallTreeNode(this);

	#ifdef DEBUG
		std::cout << "Node of: "<< CorrespondingFunction->GetHash() << " is created"<< '\n';
		std::cout << "Node Type = " << type << std::endl;
	#endif
}

CallTreeNode::CallTreeNode(CallTreeNodeType type, std::string identification): NodeType(type)
{
	if(NodeType == Pattern_Begin)
	{
		ClTre->insertNodeIntoDeclVector(this);
	}
	else if(NodeType == Pattern_End){
		ClTre->insertNodeIntoPattern_EndVector(this);
	}
	ident = new Identification(type, identification);
	#ifdef DEBUG
		std::cout << "Node of:"<<identification<< " is created"<< '\n';
		std::cout << "Node Type = " << type << std::endl;
	#endif
}

Identification* CallTreeNode::GetID()
{
	return this->ident;
}

std::map<double, CallTreeNode*>* CallTreeNode::GetCallees(){
	return &Callees;
}

CallTreeNode* CallTreeNode::GetCaller(){
	return this->Caller;
}

void CallTreeNode::insertCallee(CallTreeNode* Node, double key){
	#ifdef DEBUG
	std::cout << "in insertCallee" << '\n';
	#endif
	// it is not allowed to appent the same object (with the same adress) twice
	if(!isAlreadyCallee(Node)){
		#ifdef DEBUG
		std::cout << "!isAlreadyCallee = " << !isAlreadyCallee(Node) << '\n';
		#endif
		if(key != 0.0){
			#ifdef DEBUG
			std::cout << "inserting "<< *Node->GetID()<< " at position "<<key << '\n';
			#endif
			Callees[key] = Node;
		}
		else{
			#ifdef DEBUG
			std::cout << "inserting "<< *Node->GetID()<< " at position "<<actNumOfChild << '\n';
			#endif
			Callees[actNumOfChild] = Node;
			actNumOfChild++;
		}
	}
}

bool CallTreeNode::isAlreadyCallee(CallTreeNode* Callee){
	for(const auto &calleePair: Callees){
		if(calleePair.second == Callee)
			return true;
	}
	return false;
}

void CallTreeNode::SetCaller(CallTreeNode* Node)
{
	this->Caller = Node;
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
	for(const auto &CalleeOfCallerPair : *Caller->GetCallees())
	{
		CallTreeNode* CalleeOfCaller = CalleeOfCallerPair.second;
		if(this->compare(CalleeOfCaller))
			return true;
		return false;
	}
}

void CallTreeNode::print(){
	if((NodeType == Pattern_Begin || NodeType == Pattern_End)&& CorrespondingNode!= NULL && clang::dyn_cast<PatternCodeRegion>(CorrespondingNode)){
		PatternCodeRegion* CorrespRegion = clang::dyn_cast<PatternCodeRegion>(CorrespondingNode);
		std::cout << "\033[36m";
		if(NodeType == Pattern_End){
			std::cout << "END ";
		}
		std::cout << CorrespRegion->GetPatternOccurrence()->GetPattern()->GetDesignSpaceStr() << ":\33[33m " << CorrespRegion->GetPatternOccurrence()->GetPattern()->GetPatternName() << "\33[0m";
			std::cout << "(" << *ident << ")" << std::endl;
	}
	else if((NodeType == Function || NodeType == Root) && CorrespondingNode!=NULL && clang::dyn_cast<FunctionNode>(CorrespondingNode)){
		FunctionNode* CorrespFunc = clang::dyn_cast<FunctionNode>(CorrespondingNode);
		std::cout << "\033[31m" << CorrespFunc->GetFnName() << "\033[0m" << " (Hash: " << *ident << ")" << std::endl;
	}
	else if(CorrespondingNode == NULL){
		std::cout << "\033[36m" << "Only Printing Identification "<< ":\33[33m"<< *ident << "\033[0m" << std::endl;
	}
}

void CallTreeNode::setLOCTillPatternEnd(CallTreeNode* Child, CallTreeNode* EndNode){
	int locNodeToEndNode;
	if(Child == EndNode){
		#ifdef LOCDEBUG
			std::cout << "Child and EndNode are the same." << '\n';
		#endif
		EndNode->insertLOCToPatternEnd(EndNode, 0);
	}
	if(Child->GetNodeType() == Function_Decl){
		//copy corresponding entry of Function_Decl
		auto correspEntry = (Child->getMapLOCToPatternEnds())->find(EndNode);
		insertLOCToPatternEnd(correspEntry->first, correspEntry->second);
		#ifdef LOCDEBUG
			std::cout << "Copy operation:" << '\n';
			std::cout << "copied "<<  *(correspEntry->first)->GetID()<<" "<< correspEntry->second<< " to "<< *GetID()<< '\n';
		#endif
	}
	else{
		int correspEntry = (Child->getMapLOCToPatternEnds())->find(EndNode)->second;
		locNodeToEndNode = correspEntry + (Child->getLineNumber() - getLineNumber());
		insertLOCToPatternEnd(EndNode, locNodeToEndNode);

		#ifdef LOCDEBUG
			int storedLineNumber = (getMapLOCToPatternEnds()->find(EndNode))->second;
			std::cout << "FOR ENDNODE: "<<*EndNode->GetID() << '\n';
			std::cout << "in setLOCTillPatternEnd: "<< correspEntry << " + "<<  *Child->GetID()<<" "<< Child->getLineNumber() << " - "<< *this->GetID()<<" "<<this->getLineNumber() << "= "<<locNodeToEndNode<< " verification" <<storedLineNumber << '\n';
		#endif
	}
	if(this->compare(EndNode)){
		locTillPatternEnd = getMapLOCToPatternEnds()->find(EndNode)->second;

		/*delete all Children of the PatternBegin which are't really children
		 * that means erasing all children which where assigned after this one
		 * (have higher keys)
		*/
		std::map<double, CallTreeNode*>* MapCallees = GetCallees();
		int childKey;
		CallTreeNode* callerOfThis = GetCaller();
		double callerChildKey;
		std::map<double, CallTreeNode*>* mapCallerCallees = callerOfThis->GetCallees();
		for(auto nodeEntry = mapCallerCallees->begin() ; nodeEntry != mapCallerCallees->end();){
			if(nodeEntry->second == this){
				callerChildKey = nodeEntry->first;
				break;
			}
			else
				nodeEntry++;
		}

		for(auto nodeEntry = MapCallees->begin() ; nodeEntry != MapCallees->end();){
			if(nodeEntry->second == Child){
				childKey = nodeEntry->first;
			}//for every Child wich comes after the Child which contains the as the end as an
			//successor. Delete the Child as A child of Pattern_Begin and add his child
			// to the Caller of Pattern_Begin
			if(nodeEntry->first > childKey){
				#ifdef CHILDDEBUG
					std::cout << "ERASING "<<nodeEntry->first << *((nodeEntry.second)->GetID()) << '\n';
				#endif
				callerChildKey += 0.1;
				callerOfThis->insertCallee(nodeEntry->second, callerChildKey);
				nodeEntry = MapCallees->erase(nodeEntry);
			}
			else{
				nodeEntry++;
			}

		}
		#ifdef LOCDEBUG
			std::cout << "set locTillPatternEnd from "<< *GetID()<< " to "<< locTillPatternEnd << '\n';
		#endif
		}
}

void CallTreeNode::insertLOCToPatternEnd(CallTreeNode* Node, int Loc){
	LocTillEnds.insert({Node, Loc});
}

void CallTreeNode::setSuitedForNestingStatisticsTo(bool suited){
	isSuitedForNestingStatistics = suited;
	PatternGraphNode* GraphNode = getCorrespondingCodeRegion();
	PatternCodeRegion* CodeReg = clang::dyn_cast<PatternCodeRegion>(GraphNode);
	CodeReg->isSuitedForNestingStatistics = suited;
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

std::ostream& operator<<(std::ostream &os, CallTreeNodeType const &NodeType)
{
	if(NodeType == Function){
		return os << "Function";
	}
	if(NodeType == Pattern_Begin){
		return os << "Pattern_Begin";
	}
	if(NodeType == Pattern_End){
		return os << "Pattern_End";
	}
	if(NodeType == Function_Decl){
		return os << "Function_Decl";
	}
	if(NodeType == Root){
		return os << "Function_Decl (Root)";
	}
}
