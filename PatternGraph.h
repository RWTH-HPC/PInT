#pragma once

#include "DesignSpaces.h"

#include <string>
#include <vector>
#include <stack>
#include "clang/AST/Decl.h"
#include "llvm/Support/Casting.h"



/* Forward declarations */
class HPCParallelPattern;
class PatternOccurence;
class PatternCodeRegion;



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
	
private:
	std::string FnName;
	unsigned Hash;

	std::vector<PatternGraphNode*> Children;
	std::vector<PatternGraphNode*> Parents;
};


/**
 * This singleton class implements a database for function entries.
 * It is needed to allow for cross-compilation-unit processing of the AST.
 * The class provides methods for lookup and addition of function declarations encountered in the code.
 * For encountered function calls, bodies or declarations, a FunctionNode is created in this database.
 * Through ODR hashing, the entry can later be found again.
 */
class FunctionDeclDatabase
{
public:
	FunctionNode* Lookup(clang::FunctionDecl* Decl);

	static FunctionDeclDatabase* GetInstance() 
	{
		static FunctionDeclDatabase Instance;
		return &Instance;
	}

	std::vector<FunctionNode*> GetAllFunctionEntries() { return Entries; }

	FunctionNode* GetMainFnEntry() { return MainFnEntry; }

private:
	FunctionDeclDatabase();
	
	// Prevent construction of another instance by copying
	FunctionDeclDatabase(const FunctionDeclDatabase&);
	FunctionDeclDatabase& operator = (const FunctionDeclDatabase&);

	std::vector<FunctionNode*> Entries;

	FunctionNode* MainFnEntry;
};



/**
 * This class contains references to all patterns and pattern occurences currently known.
 * It allows for comfortable and reliable lookup of patterns and occurences by name, resp. ID.
 */
class HPCPatternDatabase 
{
public:
	/* Search and add patterns */
	HPCParallelPattern* LookupParallelPattern(DesignSpace DesignSp, std::string PatternName);
	
	void AddParallelPattern(HPCParallelPattern* Pattern);

	std::vector<HPCParallelPattern*> GetAllPatterns() { return Patterns; }


	/* Search and add pattern occurences */
	PatternOccurence* LookupPatternOccurence(std::string ID);

	void AddPatternOccurence(PatternOccurence* PatternOcc);

	std::vector<PatternOccurence*> GetAllPatternOccurences() { return PatternOccurences; }

	std::vector<PatternCodeRegion*> GetAllPatternCodeRegions();


	static HPCPatternDatabase* GetInstance() 
	{
		static HPCPatternDatabase Instance;
		return &Instance;
	}

private:
	HPCPatternDatabase();

	// Prevent construction of another instance by copying
	HPCPatternDatabase(const HPCPatternDatabase&);
	HPCPatternDatabase& operator = (const HPCPatternDatabase&);
	
	std::vector<HPCParallelPattern*> Patterns;

	std::vector<PatternOccurence*> PatternOccurences;
};
