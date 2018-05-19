#pragma once

#include <string>
#include <vector>
#include <stack>
#include "clang/AST/Decl.h"
#include "llvm/Support/Casting.h"


// TODO Implement Exception for wrong pattern nesting



/*
 * Design Spaces
 */
enum DesignSpace { Unknown, FindingConcurrency, AlgorithmStructure, SupportingStructure, ImplementationMechanism };

DesignSpace StrToDesignSpace(std::string str);

std::string DesignSpaceToStr(DesignSpace DesignSp);


/*
 * Abstract dummy class for nesting
 */
class PatternOccurence {
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

	PatternOccurence(OccurenceKind OK) : Kind(OK)
	{

	}

	virtual void AddChild(PatternOccurence* Child) = 0;

	virtual void AddParent(PatternOccurence* Parent) = 0;

	virtual std::vector<PatternOccurence*> GetChildren() = 0;

	virtual std::vector<PatternOccurence*> GetParents() = 0;

private:
	const OccurenceKind Kind;
};



/*
 * A struct for the database entries
 */ 	
class FunctionDeclDatabaseEntry : public PatternOccurence
{
public:
	FunctionDeclDatabaseEntry (std::string Name, unsigned Hash);

	void AddChild(PatternOccurence* Child);
	
	void AddParent(PatternOccurence* Parent);

	std::vector<PatternOccurence*> GetChildren()
	{
		return Children;
	}	

	std::vector<PatternOccurence*> GetParents()
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

	static bool classof(const PatternOccurence* PatternOcc)
	{
		return PatternOcc->GetKind() == PatternOccurence::OK_FnCall;
	}
	
private:
	bool BodyVisited;	
	std::string FnName;
	unsigned Hash;
	std::vector<PatternOccurence*> Children;
	std::vector<PatternOccurence*> Parents;
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
class HPCParallelPattern : public PatternOccurence {
public:
	HPCParallelPattern(DesignSpace DesignSp, std::string PatternName, std::string PatternID);
	
	void Print();

	void AddChild(PatternOccurence* Child);

	void AddParent(PatternOccurence* Parent);

	std::string GetPatternID() { return this->PatternID; }

	std::string GetPatternName() { return this->PatternName; }

	std::string GetDesignSpaceStr() { return DesignSpaceToStr(this->DesignSp); }

	DesignSpace GetDesignSpace() { return DesignSp; }

	std::vector<PatternOccurence*> GetChildren() { return this->Children; }

	std::vector<PatternOccurence*> GetParents() { return this->Parents; }

	void SetFirstLine (int FirstLine) { this->FirstLine = FirstLine; }

	int GetFirstLine () { return this->FirstLine; }

	void SetLastLine (int LastLine) { this->LastLine = LastLine; }
	
	int GetLastLine () { return this->LastLine; }
	
	static bool classof(const PatternOccurence* PatternOcc)
	{
		return PatternOcc->GetKind() == PatternOccurence::OK_Pattern;
	}
	
private:	
	DesignSpace DesignSp;
	std::string PatternName;
	std::string PatternID;

	int FirstLine;
	int LastLine;
	
	std::vector<PatternOccurence*> Parents;
	std::vector<PatternOccurence*> Children;
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
