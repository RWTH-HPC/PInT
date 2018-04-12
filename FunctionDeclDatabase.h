#pragma once

#include "HPCParallelPattern.h"

#include "clang/AST/Decl.h"
#include <vector>
#include <string>


/* Forward declarations */
class HPCParallelPattern;



/*
 * A struct for the database entries
 */ 	
class FunctionDeclDatabaseEntry
{
public:
	FunctionDeclDatabaseEntry (std::string Name, unsigned Hash);	

	void AddCallerPattern(HPCParallelPattern* Pattern)
	{
		CallerPatterns.push_back(Pattern);
	}

	void AddPattern(HPCParallelPattern* Pattern)
	{
		Patterns.push_back(Pattern);
	}

	void AddFnCall(FunctionDeclDatabaseEntry* FnCall)
	{
		FnCalls.push_back(FnCall);
	}

	std::vector<FunctionDeclDatabaseEntry*> GetFnCalls()
	{
		return FnCalls;
	}	

	unsigned GetHash() 
	{
		return Hash;
	}

	std::string GetFnName()
	{
		return FnName;
	}

	std::vector<HPCParallelPattern*> GetPatterns()
	{
		return Patterns;
	}
	
private:
	bool BodyVisited;	
	std::string FnName;
	unsigned Hash;
	std::vector<HPCParallelPattern*> CallerPatterns;
	std::vector<HPCParallelPattern*> Patterns;
	std::vector<FunctionDeclDatabaseEntry*> FnCalls;
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

