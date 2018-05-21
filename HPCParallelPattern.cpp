#include "HPCParallelPattern.h"

#include <iostream>
#include "clang/AST/ODRHash.h"



/*
 * Function Declaration Database Entry functions
 */
FunctionDeclDatabaseEntry::FunctionDeclDatabaseEntry (std::string Name, unsigned Hash) : PatternOccurence(OK_FnCall), Children(), Parents()
{
	this->BodyVisited = false;
	this->FnName = Name;
	this->Hash = Hash;
}	

void FunctionDeclDatabaseEntry::AddChild(PatternOccurence* Child)
{
	Children.push_back(Child);
}

void FunctionDeclDatabaseEntry::AddParent(PatternOccurence* Parent)
{
	Parents.push_back(Parent);
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



/*
 * HPC Parallel Pattern Class Functions
 */
HPCParallelPattern::HPCParallelPattern(DesignSpace DesignSp, std::string PatternName, std::string PatternID) : PatternOccurence(OK_Pattern), Parents(), Children()
{
	this->DesignSp = DesignSp;
	this->PatternName = PatternName;
	this->PatternID = PatternID;
}

void HPCParallelPattern::Print() 
{
	std::cout << "Pattern Info" << std::endl;
	std::cout << "Pattern Design Space: " << DesignSpaceToStr(this->DesignSp) << std::endl;
	std::cout << "Pattern Name: " << this->PatternName << std::endl;
	std::cout << "Pattern Identifier: " << this->PatternID << std::endl;
}

void HPCParallelPattern::AddChild(PatternOccurence* Child) 
{
	Children.push_back(Child);
}

void HPCParallelPattern::AddParent(PatternOccurence* Parent)
{
	Parents.push_back(Parent);
}



/*
 * HPC Pattern Database Functions
 */
HPCPatternDatabase::HPCPatternDatabase() : Patterns()
{
	
}

HPCParallelPattern* HPCPatternDatabase::LookupParallelPattern(std::string ID)
{
	/* Go through the list of parallel patterns to find the parallel pattern with the given identifier */
	for (HPCParallelPattern* Pattern : Patterns)
	{
		if (!ID.compare(Pattern->GetPatternID()))
		{
			return Pattern;
		}
	}

	return NULL;
}

void HPCPatternDatabase::AddParallelPattern(HPCParallelPattern* Pattern)
{
	if (LookupParallelPattern(Pattern->GetPatternID()) != NULL)
	{
		return;
		// TODO throw exception because of duplicate pattern!
	}

	Patterns.push_back(Pattern);
}



/*
 * Design Space Helper Functions
 */
DesignSpace StrToDesignSpace(std::string str)
{
	if (!str.compare("FindingConcurrency")) 
	{
		return FindingConcurrency;
	}
	else if (!str.compare("AlgorithmStructure"))
	{
		return AlgorithmStructure;
	}
	else if (!str.compare("SupportingStructure"))
	{
		return SupportingStructure;	
	}
	else if (!str.compare("ImplementationMechanism"))
	{
		return ImplementationMechanism;
	}
	else
	{
		return Unknown;
	}
}

std::string DesignSpaceToStr(DesignSpace DesignSp)
{
	if (DesignSp == FindingConcurrency)
	{
		return "FindingConcurrency";
	}
	else if (DesignSp == AlgorithmStructure)
	{
		return "AlgorithmStructure";
	}
	else if (DesignSp == SupportingStructure)
	{
		return "SupportingStructre";
	}
	else if (DesignSp == ImplementationMechanism)
	{
		return "ImplementationMechanism";
	}
	else
	{
		return "Unknown";
	}
}



/*
 * Pattern Stack Management
 */
std::stack<HPCParallelPattern*> PatternContext;

void AddToPatternStack(HPCParallelPattern* Pattern)
{
	PatternContext.push(Pattern);
}

HPCParallelPattern* GetTopPatternStack() 
{
	if (!PatternContext.empty())
	{
		return PatternContext.top();
	}
	else
	{
		return NULL;
	}
}

void RemoveFromPatternStack(HPCParallelPattern* Pattern)
{
	if (!PatternContext.empty())
	{
		HPCParallelPattern* Top = PatternContext.top();	
		
		if (Top != Pattern)
		{
			std::cout << "\033[31m" << "Inconsistency in the pattern stack detected. Results may not be correct. Check the structure of the instrumentation in the application code!" << "\033[0m" << std::endl;
		}
		
		PatternContext.pop();
	}
}

