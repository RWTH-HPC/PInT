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
 * Abstract dummy class for nesting
 */
class PatternTreeNode {
public:
	enum OccurenceKind 
	{
		OK_FnCall, 
		OK_Pattern
	};

	OccurenceKind GetKind() const
	{
		return Kind;
	}

	PatternTreeNode(OccurenceKind OK) : Kind(OK)
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

	const OccurenceKind Kind;
};



/*
 * A struct for the database entries
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
		return PatternOcc->GetKind() == PatternTreeNode::OK_FnCall;
	}
	
private:
	std::string FnName;
	unsigned Hash;

	std::vector<PatternTreeNode*> Children;
	std::vector<PatternTreeNode*> Parents;
};



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



/* Forward declaration for Pattern Occurences */
class PatternOccurence;

/*
 * HPC Parallel Pattern Class
 */
class HPCParallelPattern
{
public:
	HPCParallelPattern(DesignSpace DesignSp, std::string PatternName);
	
	void Print();

	void AddChild(PatternTreeNode* Child);

	void AddParent(PatternTreeNode* Parent);

	void AddOccurence(PatternOccurence* Occurence);

	std::vector<PatternOccurence*> GetAllOccurences() { return this->Occurences; }

	std::vector<PatternOccurence*> GetOccurencesWithID(std::string ID);
	
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
 */
class PatternOccurence : public PatternTreeNode
{
public:
	PatternOccurence(HPCParallelPattern* Pattern, std::string ID);

	HPCParallelPattern* GetPattern() { return this->Pattern; }

	static bool classof(const PatternTreeNode* Node)
	{
		return Node->GetKind() == PatternTreeNode::OK_Pattern;
	}

	void AddChild(PatternTreeNode* Child);

	void AddParent(PatternTreeNode* Parent);

	std::vector<PatternTreeNode*> GetChildren() { return this->Children; }

	std::vector<PatternTreeNode*> GetParents() { return this->Parents; }

	void SetFirstLine (int FirstLine);

	void SetLastLine (int LastLine);

	int GetLinesOfCode() { return this->LinesOfCode; }

	std::string GetID() { return this->ID; }

private:
	HPCParallelPattern* Pattern;	

	std::string ID;

	std::vector<PatternTreeNode*> Parents;
	std::vector<PatternTreeNode*> Children;

	int LinesOfCode = 0;
};



/*
 * HPC Pattern Database Class
 */
class HPCPatternDatabase 
{
public:
	HPCParallelPattern* LookupParallelPattern(DesignSpace DesignSp, std::string PatternName);
	
	void AddParallelPattern(HPCParallelPattern* Pattern);

	std::vector<HPCParallelPattern*> GetAllPatterns() { return Patterns; }

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
};



/* 
 * Stack Management
 */
extern std::stack<PatternOccurence*> PatternContext;

void AddToPatternStack(PatternOccurence* PatternOcc);

PatternOccurence* GetTopPatternStack();

void RemoveFromPatternStack(std::string ID);
