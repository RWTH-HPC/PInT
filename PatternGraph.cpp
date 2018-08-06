#include "PatternGraph.h"

#include "HPCParallelPattern.h"

#include <iostream>
#include "clang/AST/ODRHash.h"


/*
 * Function Declaration Database Entry functions
 */
FunctionNode::FunctionNode (std::string Name, unsigned Hash) : PatternGraphNode(GNK_FnCall), Children(), Parents()
{
	this->FnName = Name;
	this->Hash = Hash;
}	

void FunctionNode::AddChild(PatternGraphNode* Child)
{
	Children.push_back(Child);
}

void FunctionNode::AddParent(PatternGraphNode* Parent)
{
	Parents.push_back(Parent);
}



/*
 * Function Declaration Database functions
 */
FunctionDeclDatabase::FunctionDeclDatabase() : Entries()
{
}



/**
 * @brief Lookup function for the database entry that corresponds to the given function declaration.
 *
 * This function takes a clang function declaration object as input and calculates an ODR hash value from the object.
 * This value is then used for lookup of the corresponding entry in our function declaration database to enable linking of function calls and bodies with their declarations between translation units.
 * If no entry is found, a new entry is created and the object corresponding to this new entry is returned.
 *
 * @param Decl The clang object that belongs to a function declaration in the source code.
 *
 * @return The (new) function declaration database entry corresponding to the function declaration.
 **/
FunctionNode* FunctionDeclDatabase::Lookup(clang::FunctionDecl* Decl)
{
	clang::ODRHash Hash;
	Hash.AddDecl(Decl);
	unsigned HashVal = Hash.CalculateHash();		

	std::string FnName = Decl->getNameInfo().getName().getAsString();	

	// Search for an existing entry
	for (FunctionNode* e : Entries)
	{	
		if (e->GetHash() == HashVal)
		{
			return e;
		}
	}

	// We found nothing, therefore we allocate a new entry
	FunctionNode* FuncEntry;
	FuncEntry = new FunctionNode(FnName, HashVal);
	Entries.push_back(FuncEntry);

	if (Decl->isMain())
	{
		MainFnEntry = FuncEntry;
	}

	return FuncEntry;
}



/*
 * HPC Pattern Database Functions
 */
HPCPatternDatabase::HPCPatternDatabase() : Patterns()
{
	
}

/**
 * @brief This function is used to look for a HPCParallelPattern object in the pattern database using the design space and the pattern name as search criteria.
 *
 * @param DesignSp The design space of the pattern we are looking for.
 * @param PatternName The name of the pattern.
 *
 * @return HPCParallelPattern object that matches the search criteria or NULL.
 **/
HPCParallelPattern* HPCPatternDatabase::LookupParallelPattern(DesignSpace DesignSp, std::string PatternName)
{
	/* Go through the list of parallel patterns to find the parallel pattern with the given identifier */
	for (HPCParallelPattern* Pattern : Patterns)
	{
		if (DesignSp == Pattern->GetDesignSpace() && !PatternName.compare(Pattern->GetPatternName()))
		{
			return Pattern;
		}
	}

	return NULL;
}

/**
 * @brief Adds a parallel pattern to the database. Returns an error message if the pattern already exists in the database.
 *
 * @param Pattern The parallel pattern that is added.
 **/
void HPCPatternDatabase::AddParallelPattern(HPCParallelPattern* Pattern)
{
	if (LookupParallelPattern(Pattern->GetDesignSpace(), Pattern->GetPatternName()) != NULL)
	{
		return;
		std::cout << "\033[31m" << "Pattern already exists in the database." << "\033[0m" << std::endl;
	}

	Patterns.push_back(Pattern);
}


/**
 * @brief Finds a PatternOccurence object in the database by its ID.
 *
 * @param ID The ID of the PatternOccurence we are searching.
 *
 * @return The PatternOccurence object if successful, NULL else.
 **/
PatternOccurence* HPCPatternDatabase::LookupPatternOccurence(std::string ID)
{
	for (PatternOccurence* PatternOcc : PatternOccurences)
	{
		if (!ID.compare(PatternOcc->GetID()))
		{
			return PatternOcc;
		}
	}

	return NULL;
}

/**
 * @brief Adds a PatternOccurence to the database. Displays an error message if duplicate.
 *
 * @param PatternOcc The PatternOccurence to add.
 **/
void HPCPatternDatabase::AddPatternOccurence(PatternOccurence* PatternOcc)
{
	if (LookupPatternOccurence(PatternOcc->GetID()) != NULL)
	{
		return;
		std::cout << "\033[31m" << "PatternOccurence already exists in the database." << "\033[0m" << std::endl;
	}
	
	PatternOccurences.push_back(PatternOcc);
}

/**
 * @brief Collects all PatternCodeRegion objects and returns them.
 *
 * @return All PatternCodeRegion objects linked to this PatternOccurence
 **/
std::vector<PatternCodeRegion*> HPCPatternDatabase::GetAllPatternCodeRegions()
{
	std::vector<PatternCodeRegion*> CodeRegions;

	for (PatternOccurence* PatternOcc : PatternOccurences)
	{
		for (PatternCodeRegion* CodeRegion : PatternOcc->GetCodeRegions())
		{
			CodeRegions.push_back(CodeRegion);
		}
	}

	return CodeRegions;
}
