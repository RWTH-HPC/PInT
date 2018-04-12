#include "FunctionDeclDatabase.h"

#include "clang/AST/ODRHash.h"



/*
 * Function Declaration Database Entry functions
 */
FunctionDeclDatabaseEntry::FunctionDeclDatabaseEntry (std::string Name, unsigned Hash) : Patterns(), CallerPatterns(), FnCalls()
{
	this->BodyVisited = false;
	this->FnName = Name;
	this->Hash = Hash;
}	



/*
 * Function Declaration Database functions
 */
FunctionDeclDatabase::FunctionDeclDatabase() : Entries()
{
}

FunctionDeclDatabaseEntry* FunctionDeclDatabase::Lookup(clang::FunctionDecl* Decl)
{
	clang::ODRHash Hash;
	Hash.AddDecl(Decl);
	unsigned HashVal = Hash.CalculateHash();		

	std::string FnName = Decl->getNameInfo().getName().getAsString();	

	// Search for an existing entry
	for (FunctionDeclDatabaseEntry* e : Entries)
	{	
		if (e->GetHash() == HashVal)
		{
			return e;
		}
	}

	// We found nothing, therefore we allocate a new entry
	FunctionDeclDatabaseEntry* FuncEntry;
	FuncEntry = new FunctionDeclDatabaseEntry(FnName, HashVal);
	Entries.push_back(FuncEntry);

	if (Decl->isMain())
	{
		MainFnEntry = FuncEntry;
	}

	return FuncEntry;
}
