#pragma once

#include "DesignSpaces.h"

#include <string>
#include <vector>
#include <stack>
#include <queue>
#include "clang/AST/Decl.h"
#include "llvm/Support/Casting.h"
#include <map>



/* Forward declarations */
class HPCParallelPattern;
class PatternOccurrence;
class PatternCodeRegion;
class CallTreeNode;


/**
 * This abstract class is a prototype for all pattern graph node classes following.
 * Descendants have to implement functionality to manage parents and children in the pattern tree/graph.
 * Also, some variables about the connected components are saved in this class.
 */
class PatternGraphNode {
public:
	/**
 	* This enum is needed for LLVM type checking. New classes inheriting from this class should add their own enum values here.
 	*/
	enum GraphNodeKind
	{
		GNK_FnCall,
		GNK_Pattern
	};

	GraphNodeKind GetKind() const
	{
		return Kind;
	}

	PatternGraphNode(GraphNodeKind OK) : Kind(OK)
	{
		this->ComponentID = -1;
	}

	virtual void AddChild(PatternGraphNode* Child) = 0;

	virtual void AddParent(PatternGraphNode* Parent) = 0;

	virtual std::vector<PatternGraphNode*> GetChildren() = 0;

	virtual std::vector<PatternGraphNode*> GetParents() = 0;

	void SetConnectedComponent(int CID) { this->ComponentID = CID; }

	int GetConnectedComponent() { return this->ComponentID; }

private:
	int ComponentID;

	const GraphNodeKind Kind;
};



/**
 * A FunctionNode is a node in the pattern graph (i.e. inherits from PatternGraphNode), and has children and parents.
 * It contains a hash value to uniquely identify a function declaration across compilation-units.
 * This is useful if a function is called and we need information from the function body but the function is not defined within the current translation unit.
 * Then, the reference is saved for later until the definition belonging to the function declaration is encountered.
 */
class FunctionNode : public PatternGraphNode
{
public:
	FunctionNode (std::string Name, unsigned Hash);

	void AddChild(PatternGraphNode* Child);

	void AddParent(PatternGraphNode* Parent);

	void AddPatternParent(PatternGraphNode* PatParent);

	void AddPatternParents(std::vector<PatternCodeRegion*> PatternParents);

	void AddPatternChild(PatternGraphNode* PatChild);

	std::vector<PatternCodeRegion*> GetPatternParents();

	std::vector<PatternCodeRegion*> GetPatternChildren();

	bool HasNoPatternParents();

	bool HasNoPatternChildren();

	void registerPatChildrenToPatParents();

	void PrintVecOfPattern(std::vector<PatternCodeRegion*> RegionVec);

	std::vector<PatternGraphNode*> GetChildren()
	{
		return Children;
	}

	std::vector<PatternGraphNode*> GetParents()
	{
		return Parents;
	}

	unsigned GetHash()
	{
		return Hash;
	}

	std::string GetFnName()
	{
		return FnName;
	}

	static bool classof(const PatternGraphNode* Node)
	{
		return Node->GetKind() == PatternGraphNode::GNK_FnCall;
	}

	void insertCorrespondingCallTreeNode(CallTreeNode* Node){
		CorrespondingCallTreeNodes.push_back(Node);
	}

private:
	std::string FnName;
	unsigned Hash;
	// we need only one Parents to trace down the reletion chip of the patterns through different Functions

	std::vector<PatternCodeRegion*> PatternParents;
	std::vector<PatternCodeRegion*> PatternChildren;

	std::vector<PatternGraphNode*> Children;
	std::vector<PatternGraphNode*> Parents;

	std::vector<CallTreeNode*> CorrespondingCallTreeNodes;
};



class PatternGraph
{
public:
	/**
	 * @brief Selects and returns the root node for a tree representation.
	 *
	 * @return Root Node for tree representation.
	 **/
	PatternGraphNode* GetRootNode();
	std::vector<PatternGraphNode*> GetOnlyPatternRootNodes();
	/**
	 * @brief Adds a parallel pattern to the database.
	 *
	 * @param Pattern The parallel pattern that is added.
	 *
	 * @return False if the pattern is already registered. Else, true.
	 **/
	bool RegisterPattern(HPCParallelPattern* Pattern);
	/**
	 * @brief This function is used to look for a HPCParallelPattern object in the pattern database using the design space and the pattern name as search criteria.
	 *
	 * @param DesignSp The design space of the pattern we are looking for.
	 * @param PatternName The name of the pattern.
	 *
	 * @return HPCParallelPattern object that matches the search criteria or NULL.
	 **/
	HPCParallelPattern* GetPattern(DesignSpace DesignSp, std::string Name);

	/**
	 * @brief
	 *
	 * @return All patterns registered in the graph.
	 **/
	std::vector<HPCParallelPattern*> GetAllPatterns() { return Patterns; }
	/**
	 * @brief Adds a PatternOccurrence to the database.
	 *
	 * @param PatternOcc The PatternOccurrence to add.
	 *
	 * @return False if the pattern occurrence is already registered. Else, true.
	 **/
	bool RegisterPatternOccurrence(PatternOccurrence* PatternOcc);
	/**
	 * @brief Finds a PatternOccurrence object in the database by its ID.
	 *
	 * @param ID The ID of the PatternOccurrence we are searching.
	 *
	 * @return The PatternOccurrence object if successful, NULL else.
	 **/
	PatternOccurrence* GetPatternOccurrence(std::string ID);

	/**
	 * @brief
	 *
	 * @return All pattern occurrences registered in the graph.
	 **/
	std::vector<PatternOccurrence*> GetAllPatternOccurrence() { return PatternOccurrences; }
	/**
	 * @brief Collects all PatternCodeRegion objects and returns them.
	 *
	 * @return All PatternCodeRegion objects linked to this PatternOccurrence
	 **/
	std::vector<PatternCodeRegion*> GetAllPatternCodeRegions();

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
	bool RegisterFunction(clang::FunctionDecl* Decl);
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
	FunctionNode* GetFunctionNode(clang::FunctionDecl* Decl);

	FunctionNode* GetFunctionNode(std::string Name);

	void RegisterOnlyPatternRootNode(PatternCodeRegion* CodeReg);

	/**
	 * @brief
	 *
	 * @return All functions registered in the graph.
	 **/
	std::vector<FunctionNode*> GetAllFunctions() { return Functions; }

	/**
	 * @brief Get the instance of the PatternGraph
	 *
	 * @return PatternGraph instance
	 **/
	static PatternGraph* GetInstance()
	{
		static PatternGraph Graph;
		return &Graph;
	}

private:
	/* Save patterns, patternoccurrences and functions for later requests and linear access. */
	std::vector<HPCParallelPattern*> Patterns;
	std::vector<PatternOccurrence*> PatternOccurrences;

	std::vector<FunctionNode*> Functions;

	/* Designated root node for output in "Treeifyed" display */
	PatternGraphNode* RootNode;
	/* When using the OnlyPattern flag we can have multiple rootPatterns*/
	std::vector<PatternGraphNode*> OnlyPatternRootNodes;

	/* Prevent object creation */
	PatternGraph();
	PatternGraph(const PatternGraph&);
	PatternGraph& operator = (const PatternGraph&);
};

/**
	* enum is needed to distinguish between the different types of CAllTreeNodes
  **/

enum CallTreeNodeType{
	Function, Pattern_Begin, Pattern_End, Function_Decl, Root
};

/**
	* Whith the help of the Identification class we are able to easily print and compare the IDs of the Pattern/ Hash values of the Functions.
  **/

class Identification
{
public:
	~Identification();
	Identification();
	/**
		* Constructor for Identifications of CallTreeNodes which have a pattern as basis
	  **/
	Identification(CallTreeNodeType type, std::string identification);
	/**
		* Constructor for Identifications of CallTreeNodes which have a function as basis
	  **/
	Identification(CallTreeNodeType type, unsigned identification);
	/**
		* Helps to compare the different Call TreeNodes without having to make
		* a distinction between the different types of the nodes.
		* if(calltreenode.GetID()->compare(anothercalltreenode) == true)
		* then the nodes are having the same pattern or function as basis.
		* (overloaded function)
		**/
	bool compare(Identification* ident);
	/**
		* Helps to compare the different Call TreeNodes without having to make
		* a distinction between the different types of the nodes.
		* if(calltreenode.GetID()->compare(anothercalltreenode) == true)
		* then the nodes are having the same pattern or function as basis
		* (overloaded function)
		**/
	bool compare(unsigned Hash);
	/**
		* Helps to compare the different Call TreeNodes without having to make
		* a distinction between the different types of the nodes.
		* if(calltreenode.GetID()->compare(anothercalltreenode) == true)
		* then the nodes are having the same pattern or function as basis
		* (overloaded function)
		**/
	bool compare(std::string Id);
	/**
		* returns the IdentificationString which is equivalent to the ID of a PatternCodeRegion
		**/
	std::string  getIdentificationString() const {return IdentificationString;};
	/**
		* returns the IdentificationUnsigned which is equivalent to the hash value of a Function
		**/
	unsigned getIdentificationUnsigned() const {return IdentificationUnsigned;};

private:
	std::string IdentificationString = "";
	unsigned IdentificationUnsigned = 0;
};

/**
	* This Class is used to display the CallTree and check the correct instrumentation
	* of the code. Also keeps trak of the relations between the different
	* CallTreeNodes
	**/


class CallTree
{
public:
	/**
		* Destructor of a CallTree
		**/
	~CallTree();
	/**
		* Constructor of a CallTree
		**/
	CallTree();
	/**
		* Registers a CallTreeNode with the correct relation to the Caller. For Nodes which have
		* a PatternCodeRegion(Pattern) as basis
		**/
	CallTreeNode* registerNode(CallTreeNodeType NodeType, PatternCodeRegion* PatCodeReg, CallTreeNodeType LastVisited, PatternCodeRegion* TopOfStack, FunctionNode* surroundingFunc);
	/**
		* Registers a CallTreeNode with the correct relation to the Caller. For Nodes which have
		* a function call or a function declaration as basis as basis
		**/
	CallTreeNode* registerNode(CallTreeNodeType NodeType, FunctionNode* FuncNode, CallTreeNodeType LastVisited, PatternCodeRegion* TopOfStack, FunctionNode* surroundingFunc);
	/**
		* Registers a CallTreeNode with the correct relation to the Caller. For Nodes which have
		* a Pattern_End as basis as basis. It is not necessary to use this function as it is possible to
		* register Pattern_Ends correctly with the other functions.
		**/
	CallTreeNode* registerEndNode(CallTreeNodeType NodeType,std::string identification, CallTreeNodeType LastVisited, PatternCodeRegion* TopOfStack, FunctionNode* surroundingFunc);
	/**
		* sets the rootNode for a CallTree. This root node will be the starting point for our analysis
		* if the code is instrumented correctly. This is usually the main function.
		**/
	void setRootNode(CallTreeNode* root);
	/**
		*Overloaded function. Registers the Caller as caller of Node in the CallTree.
		**/
	void appendCallerToNode(CallTreeNode* Caller, CallTreeNode* Node);
	/**
		*Overloaded function. Registers the Caller as caller of Node in the CallTree.
		**/
	void appendCallerToNode(FunctionNode* Caller, CallTreeNode* Node);
	/**
		*Overloaded function. Registers the Caller as caller of Node in the CallTree.
		**/
	void appendCallerToNode(PatternCodeRegion* Caller, CallTreeNode* Node);
	/**
		*The CallTreeNodes which corresponds to a Pattern_End have to be registered in the Pattern_EndVector. Later this Vector is used to check if every Pattern_Begin has a Pattern_End and if we don´t have to much Pattern_Ends or Pattern_Begins.
		**/
	void insertNodeIntoPattern_EndVector(CallTreeNode* Node);
	/**
		*The CallTreeNodes which corresponds to a function declaration have to be registered in the DeclVector this is also used to store CallTreeNodes which corresponding to a Pattern_Begin. Only CallTreeNodes corresponding to a function declaration or to a Pattern_Begin can have children because of that we are collecting them together.
		**/
	void insertNodeIntoDeclVector(CallTreeNode* Node);
	/**
		*During the traversation we are not able to tell which CallTreeNode is called by another node. That is because the traversation of the code is not in call order. This function ensures the right relation of the different CallTreeNode and is called after the traversation of the code.
		**/
	void appendAllDeclToCallTree(CallTreeNode* Root, int maxdepth);
	/**
		*@brief  Only after calling appendAllDeclToCallTree it is possible to see if the Pattern_Begin have the correct Children. Highly likely they have too much children.
		In this function we trace back the path to from a CallTreeNode corresponding to a Pattern_End to the fitting CallTreeNode which corresponds to a Pattern_Begin.
		All children of the CallTreeNode corresponding to the Pattern_Begin after the children through we reached the CallTreeNode  are deleted as children from this CallTreeNode and assigned to its Caller.
		**/
	void setUpTree();
	/**
		* Finds a CallTreeNode corresponding to a Pattern_Begin for another CallTreeNode corresponding to a Pattern_End. If EndNode does not respond to a Pattern_End it returns NULL.
		**/
	CallTreeNode* findCorrespBegin(CallTreeNode* EndNode);
	/**
		* returns a pointer at the Vector which contains all the CallTreeNodes corresponding eather to a Pattern_Begin or a function declaration.
		**/
	std::vector<CallTreeNode*>* GetDeclarationVector();
	/**
		* returns the root of the CallTree.
		**/
	CallTreeNode* getRoot(){return RootNode;};
private:
	//we store the Pattern in a Vector so we can go up to the parents
	std::vector<CallTreeNode*> Pattern_EndVector;
	// the RootNode is the main function, which is probably named differently
	CallTreeNode* RootNode = NULL;
	/**
		*This vector stores all CallTreeNodes which are corresponding to a function declaration or a Pattern_Begin
		**/
	std::vector<CallTreeNode*> DeclarationVector;
};

/**
	* There are five types of CallTreeNodes Function, Function_Decl, Pattern_Begin, Pattern_End and Root.
	* A CallTreeNode with the NodeType Function is generated when we encounter a function call during the traversation. A CallTreeNode with the NodeType Function_Decl is gernerated when we encounter a function declaration, a Pattern_Begin when a Pattern_Begin call is encountered and a Pattern_End when a Pattern_End call is encountered. The CallTreeNode with NodeType Root is crated when the main is encountered. In general multiple CallTreeNodes can relate to a single function call or Pattern_Begin or Pattern_End.
	**/

class CallTreeNode
{
public:
	/**
		*Destructor.
		**/
	~CallTreeNode();
	/**
		*Constructor of a CallTreeNode eather corresponding to a Pattern_End or a Pattern_Begin.
		**/
	CallTreeNode(CallTreeNodeType type,  PatternCodeRegion* CorrespondingPat);
	/**
		* Constructor of a CallTreeNode eather corresponding to a function call or a function declaration.
		**/
	CallTreeNode(CallTreeNodeType type, FunctionNode* CorrespondingFunction);
	/**
		* Constructor of a CallTreeNode eather corresponding to a Pattern_End or a Pattern_Begin. When the specific PatternCodeRegion is not known during the generation of the CallTreeNode.
		**/
	CallTreeNode(CallTreeNodeType type, std::string identification);
	/**
		* Returns the Identification of a CallTreeNode
		**/
	Identification* GetID();
	/**
		* Returns a pointer of the Callee std::map.
		**/
	std::map<double, CallTreeNode*>* GetCallees();
	/**
		* Returns a pointer of the CallTreeNode Caller.
		**/
	CallTreeNode* GetCaller();
	/**
		* Inserts a CallTreeNode into the Callee std::map.
		**/
	void insertCallee(CallTreeNode* Node,double key = 0.0);
	/**
		* Proofes if a CallTreeNode Callee is already in the Callee std::map.
		* returns 1 if Callee is already stored in the Callee std::map otherwise 0.
		**/
	bool isAlreadyCallee(CallTreeNode* Callee);
	/**
		* Declares the CallTreeNode Node to the Caller of this.
		**/
	void SetCaller(CallTreeNode* Node);
	/**
		* returns 1 if the node has the same underlying function/pattern otherwise 0
		**/
	bool compare(CallTreeNode* otherNode);
	/**
		* returns 1 if the node has the same underlying function/pattern otherwise 0
		**/
	bool compare(unsigned Hash);
	/**
		* returns 1 if the node has the same underlying function/pattern otherwise 0
		**/
	bool compare(std::string Id);
	/**
		* returns 1 if this is a Callee of Caller
		**/
	bool isCalleeOf(CallTreeNode* Caller);
	/**
		* For pattern it prints the ID (hashValue/String), the pattern name, the DesignSpace and END if the CallTreeNode corresponds to an Pattern_End.
		* For function ist prints the name of the function and a HashValue.
		* For CallTreeNodes from wich we dont, know the corresponding pattern or function it prints out the Identification which is always declared.
		**/
	void print();
	/**
		* Returns the NodeType of this.
		**/
	CallTreeNodeType GetNodeType(){return NodeType;};
	/**
		* Declares the int lineNumber of this.
		**/
	void SetLineNumber(int Loc){lineNumber = Loc;};
	/**
		* Returns the int lineNumber of this.
		**/
	int getLineNumber(){return lineNumber;};
	/**
		* Sets the GraphNode CorrespondingNode to Node. This declares wo which GraphNode (PatternCodeRegion of FunctionNode) <b>this</b> belongs.
		**/
	void setCorrespondingNode(PatternGraphNode* Node){CorrespondingNode = Node;};
	/**
		* Returns the CorrespondingNode of this.
		**/
	PatternGraphNode* GetCorrespondingNode(){return CorrespondingNode;};
	/**
		* This sets correspPatCallNode to PatCallNode. It is used to store the belonging Pattern_Begin to a Pattern_End or the other way  arround. For CallTreeNodes with the type Function or Function_Decl this should be NULL.
		**/
	void setCorrespCallTreeNodeRelation(CallTreeNode* PatCallNode){correspPatCallNode = PatCallNode;}
	/**
		* Returns correspPatCallNode.
		**/
	CallTreeNode* getCorrespCallTreeNodeRelation(){return correspPatCallNode;}
	/**
		* This function stores amount of lines between this and EndNode. Child has to be a direct Child of this and EndNode has to be already within the std::map LocTillEnds of Child.
		* If the Child is a Function_Decl the belonging entry (the entry of EndNode) is only copied in the std::map LocTillEnds of this.
		**/
	void setLOCTillPatternEnd(CallTreeNode* Child,  CallTreeNode* EndNode);
	/**
		* This stores the number of lines between a Patten_End and a Pattern_Begin. But it should be only read  in the CallTreeNode corresponding to the Pattern_Begin.
		**/
	int* getLOCTillPatternEnd(){return &locTillPatternEnd;};
	/**
		* Returns the std::map LocTillEnds which at the end of the traversation should contain all lines of code to all Pattern_Ends which surround  this CallTreeNode. (All the patterns in which are a parent of this CallTreeNode.)
		**/
	std::map<CallTreeNode*, int>* getMapLOCToPatternEnds(){return &LocTillEnds;};
	/**
		* Inserts a Node (key) and a number in LocTillEnds.
		**/
	void insertLOCToPatternEnd(CallTreeNode* Node, int Loc);

	private:
	/**The identification does not identify the CallTreeNode but it identifies the
	  *belonging Pattern or Function. This class makes it easier to get the ID wich is a string or a hash value without the need to distinguish between the different NodeTypes.
		**/
	Identification* ident;
	/**
		* Identifies the Caller of the current CallTreeNode.
		**/
	CallTreeNode* Caller = NULL;
	/**
		* Identifies the Corresponding PatternCodeRegion or FunctionNode
		**/
	PatternGraphNode* CorrespondingNode = NULL;
	/**
		* This is an std::map which holds all Callees at the end of the traversation.
		**/
	std::map<double, CallTreeNode*> Callees;
	/**
		*	This is used to define the order of the children of a CallTreeNode. A child which appears earlier in the analyzed sourcecode has a lower int value in LocTillEns.
		**/
	int actNumOfChild = 0;
	/**
		* This holds the Lines of Code between a PattenBegin to the CorespondingPatternEnd. Only redable from the CallTreeNode corresponding to the Pattern_Begin.
		**/
	int locTillPatternEnd = 0;
	/**
		* After the traversation this should store the distance in lines of Code, of all enclosing Pattern, between a CallTreeNode coresponding to a Pattern_End and this.
		**/
	std::map<CallTreeNode*, int> LocTillEnds;
	/**
		* Stores the type of this CallTreeNode. Can range between Root, Function_Decl, Function, Pattern_Begin and Pattern_End
		**/
	const CallTreeNodeType NodeType;
	/**
		* stores the line Number in the analyzed sourcecode belonging to this CallTreeNode.
		**/
	int lineNumber;
	/**
		* Stores the corresponding Pattern_Begin/Pattern_End to a Pattern_End/Pattern_Begin.
		**/
	CallTreeNode* correspPatCallNode;
};

//
extern CallTree* ClTre;
/**
	* Makes it possible to print the Identification directly.
	**/
std::ostream& operator<<(std::ostream &os, Identification const &ident);
/**
	* Makes it possible to print the CallTreeNodeType directly.
	**/
std::ostream& operator<<(std::ostream &os, CallTreeNodeType const &NodeType);
