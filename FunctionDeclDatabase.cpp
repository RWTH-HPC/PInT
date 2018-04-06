#include "FunctionDeclDatabase.h"

#include "clang/AST/ODRHash.h"


FunctionDeclDatabase::FunctionDeclDatabase() : Entries()
{
}

FunctionDeclDatabase::Entry* FunctionDeclDatabase::Lookup(clang::FunctionDecl* Decl)
{
	clang::ODRHash Hash;
	Hash.AddDecl(Decl);
	unsigned HashVal = Hash.CalculateHash();		

	std::string FnName = Decl->getNameInfo().getName().getAsString();	

	// When the list of entries is empty, we can immediately return
	if (Entries.empty())
	{
		Entry* FuncEntry;
		FuncEntry = new Entry(FnName, HashVal);
		Entries.push_back(FuncEntry);
		return FuncEntry;
	}
	else
	{
		// Search for an existing entry
		for (Entry* e : Entries)
		{	
			if (e->Hash == HashVal)
			{
				return e;
			}
		}

		// We found nothing, therefore we allocate a new entry
		Entry* FuncEntry;
		FuncEntry = new Entry(FnName, HashVal);
		Entries.push_back(FuncEntry);
		return FuncEntry;
	}
}
