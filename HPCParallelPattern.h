#pragma once

#include <string>
#include <vector>
#include <stack>
#include "clang/AST/Decl.h"
#include "llvm/Support/Casting.h"



/*
 * Design Spaces
 */
enum DesignSpace { Unknown, FindingConcurrency, AlgorithmStructure, SupportingStructure, ImplementationMechanism };

DesignSpace StrToDesignSpace(std::string str);

std::string DesignSpaceToStr(DesignSpace DesignSp);


/*
 * Pattern Tree Node
 *
 * This abstract class is a prototype for all pattern tree node classes following.
 * Descendants have to implement functionality to manage parents and children in the pattern tree.
 * Also, some variables about the connected components is saved in this class.
 */
class PatternTreeNode {
public:
	enum TreeNodeKind 
	{
		TNK_FnCall, 
		TNK_Pattern
	};

	TreeNodeKind GetKind() const
	{
		return Kind;
	}

	PatternTreeNode(TreeNodeKind OK) : Kind(OK)
	{
		this->ComponentID = -1;
	}

	virtual void AddChild(PatternTreeNode* Child) = 0;

	virtual void AddParent(PatternTreeNode* Parent) = 0;

	virtual std::vector<PatternTreeNode*> GetChildren() = 0;

	virtual std::vector<PatternTreeNode*> GetParents() = 0;

	void SetConnectedComponent(int CID) { this->ComponentID = CID; }

	int GetConnectedComponent() { return this->ComponentID; }

private:
	int ComponentID;

	const TreeNodeKind Kind;
};



/*
 * Function Declaration Database Entry
 *
 * A function declaration database is a node in the pattern tree, and has children and parents.
 * It contains a hash value to uniquely identify a function declaration across compilation-units.
 */ 	
class FunctionDeclDatabaseEntry : public PatternTreeNode
{
public:
	FunctionDeclDatabaseEntry (std::string Name, unsigned Hash);

	void AddChild(PatternTreeNode* Child);
	
	void AddParent(PatternTreeNode* Parent);

	std::vector<PatternTreeNode*> GetChildren()
	{
		return Children;
	}	

	std::vector<PatternTreeNode*> GetParents()
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

	static bool classof(const PatternTreeNode* PatternOcc)
	{
		return PatternOcc->GetKind() == PatternTreeNode::TNK_FnCall;
	}
	
private:
	std::string FnName;
	unsigned Hash;

	std::vector<PatternTreeNode*> Children;
	std::vector<PatternTreeNode*> Parents;
};


/*
 * Function Declaration Database
 *
 * This singleton class implements a database for function entries.
 * It is needed to allow for cross-compilation-unit processing of the AST.
 * The class provides methods for lookup and addition of function declarations encountered in the code.
 */
class FunctionDeclDatabase
{
public:
	FunctionDeclDatabaseEntry* Lookup(clang::FunctionDecl* Decl);

	static FunctionDeclDatabase* GetInstance() 
	{
		static FunctionDeclDatabase Instance;
		return &Instance;
	}

	std::vector<FunctionDeclDatabaseEntry*> GetAllFunctionEntries() { return Entries; }

	FunctionDeclDatabaseEntry* GetMainFnEntry() { return MainFnEntry; }

private:
	FunctionDeclDatabase();
	
	// Prevent construction of another instance by copying
	FunctionDeclDatabase(const FunctionDeclDatabase&);
	FunctionDeclDatabase& operator = (const FunctionDeclDatabase&);

	std::vector<FunctionDeclDatabaseEntry*> Entries;

	FunctionDeclDatabaseEntry* MainFnEntry;
};



/* Forward declarations */
class PatternOccurence;
class PatternCodeRegion;

/*
 * HPC Parallel Pattern Class
 *
 * The pattern class describes the parallel pattern identified by the design space and the pattern name.
 * The pattern name is not the same as the pattern identifier.
 * This class contains references to all occurences of a pattern, hence indirectly to all code regions.
 */
class HPCParallelPattern
{
public:
	HPCParallelPattern(DesignSpace DesignSp, std::string PatternName);
	
	void Print();

	void AddOccurence(PatternOccurence* Occurence);

	std::vector<PatternOccurence*> GetAllOccurences() { return this->Occurences; }

	std::vector<PatternCodeRegion*> GetAllCodeRegions();
	
	std::string GetPatternName() { return this->PatternName; }

	std::string GetDesignSpaceStr() { return DesignSpaceToStr(this->DesignSp); }

	DesignSpace GetDesignSpace() { return DesignSp; }
	
	int GetTotalLinesOfCode();

private:	
	DesignSpace DesignSp;
	std::string PatternName;

	std::vector<PatternOccurence*> Occurences;
};



/* 
 * Pattern Occurence Class
 *
 * The pattern occurence is a hypothetical construct that represents a collection for all code regions
 * with the same identifier.
 * It is linked to a unique pattern, which all the code regions implement.
 */
class PatternOccurence
{
public:
	PatternOccurence(HPCParallelPattern* Pattern, std::string ID);

	HPCParallelPattern* GetPattern() { return this->Pattern; }

	void Print();

	std::string GetID() { return this->ID; }

	void AddCodeRegion(PatternCodeRegion* CodeRegion) { this->CodeRegions.push_back(CodeRegion); }

	std::vector<PatternCodeRegion*> GetCodeRegions() { return this->CodeRegions; }

private:
	HPCParallelPattern* Pattern;

	std::vector<PatternCodeRegion*> CodeRegions;
	
	std::string ID;
};



/*
 * Pattern Code Region Class
 *
 * This class represents the block of code that is enclosed with the instrumentation calls.
 * It is a node in the pattern tree, hence has children and parents in the tree.
 * Every region belongs to a pattern occurence.
 */
class PatternCodeRegion : public PatternTreeNode
{
public:
	PatternCodeRegion(PatternOccurence* PatternOcc);

	PatternOccurence* GetPatternOccurence() { return this->PatternOcc; }

	static bool classof(const PatternTreeNode* Node)
	{
		return Node->GetKind() == PatternTreeNode::TNK_Pattern;
	}

	void Print();

	void AddChild(PatternTreeNode* Child);

	void AddParent(PatternTreeNode* Parent);

	std::vector<PatternTreeNode*> GetChildren() { return this->Children; }

	std::vector<PatternTreeNode*> GetParents() { return this->Parents; }

	void SetFirstLine (int FirstLine);

	void SetLastLine (int LastLine);

	int GetLinesOfCode() { return this->LinesOfCode; }

	std::string GetID() { return this->PatternOcc->GetID(); }

private:
	PatternOccurence* PatternOcc;	

	std::vector<PatternTreeNode*> Parents;
	std::vector<PatternTreeNode*> Children;

	int LinesOfCode = 0;
};



/*
 * HPC Pattern Database Class
 *
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



/* 
 * Stack Management
 *
 * The pattern stack is used to keep track of the nesting of patterns.
 */
extern std::stack<PatternCodeRegion*> PatternContext;

void AddToPatternStack(PatternCodeRegion* PatternOcc);

PatternCodeRegion* GetTopPatternStack();

void RemoveFromPatternStack(std::string ID);
