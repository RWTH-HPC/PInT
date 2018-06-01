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

	}

	virtual void AddChild(PatternTreeNode* Child) = 0;

	virtual void AddParent(PatternTreeNode* Parent) = 0;

	virtual std::vector<PatternTreeNode*> GetChildren() = 0;

	virtual std::vector<PatternTreeNode*> GetParents() = 0;

private:
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
	bool BodyVisited;	
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

	FunctionDeclDatabaseEntry* GetMainFnEntry() { return MainFnEntry; }

private:
	FunctionDeclDatabase();
	
	// Prevent construction of another instance by copying
	FunctionDeclDatabase(const FunctionDeclDatabase&);
	FunctionDeclDatabase& operator = (const FunctionDeclDatabase&);

	std::vector<FunctionDeclDatabaseEntry*> Entries;

	FunctionDeclDatabaseEntry* MainFnEntry;
};



/*
 * HPC Parallel Pattern Class
 */
class HPCParallelPattern : public PatternTreeNode 
{
public:
	HPCParallelPattern(DesignSpace DesignSp, std::string PatternName, std::string PatternID);
	
	void Print();

	void AddChild(PatternTreeNode* Child);

	void AddParent(PatternTreeNode* Parent);

	std::string GetPatternID() { return this->PatternID; }

	std::string GetPatternName() { return this->PatternName; }

	std::string GetDesignSpaceStr() { return DesignSpaceToStr(this->DesignSp); }

	DesignSpace GetDesignSpace() { return DesignSp; }

	std::vector<PatternTreeNode*> GetChildren() { return this->Children; }

	std::vector<PatternTreeNode*> GetParents() { return this->Parents; }

	void SetFirstLine (int FirstLine);

	void SetLastLine (int LastLine);
	
	std::vector<int> GetLinesOfCode () { return this->LinesOfCode; }
	
	int GetTotalLinesOfCode();

	static bool classof(const PatternTreeNode* PatternOcc)
	{
		return PatternOcc->GetKind() == PatternTreeNode::OK_Pattern;
	}
	
private:	
	DesignSpace DesignSp;
	std::string PatternName;
	std::string PatternID;

	std::stack<int> FirstLineStack;
	std::vector<int> LinesOfCode;	

	std::vector<PatternTreeNode*> Parents;
	std::vector<PatternTreeNode*> Children;
};



class HPCPatternDatabase 
{
public:
	HPCParallelPattern* LookupParallelPattern(std::string ID);
	
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
extern std::stack<HPCParallelPattern*> PatternContext;

void AddToPatternStack(HPCParallelPattern* Pattern);

HPCParallelPattern* GetTopPatternStack();

void RemoveFromPatternStack(HPCParallelPattern* Pattern);
