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
struct FunctionDeclDatabaseEntry
{
public:
	FunctionDeclDatabaseEntry (std::string Name, unsigned Hash) : Patterns()
	{
		this->BodyVisited = false;
		this->FnName = Name;
		this->Hash = Hash;
	}	

	bool BodyVisited;	
	std::string FnName;
	unsigned Hash;
	std::vector<HPCParallelPattern*> Patterns;
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


private:
	FunctionDeclDatabase();
	
	// Prevent construction of another instance by copying
	FunctionDeclDatabase(const FunctionDeclDatabase&);
	FunctionDeclDatabase& operator = (const FunctionDeclDatabase&);

	std::vector<FunctionDeclDatabaseEntry*> Entries;
};

