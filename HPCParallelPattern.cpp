#include "HPCParallelPattern.h"

#include <iostream>
#include "clang/AST/ODRHash.h"



/*
 * Function Declaration Database Entry functions
 */
FunctionDeclDatabaseEntry::FunctionDeclDatabaseEntry (std::string Name, unsigned Hash) : PatternTreeNode(OK_FnCall), Children(), Parents()
{
	this->FnName = Name;
	this->Hash = Hash;
}	

void FunctionDeclDatabaseEntry::AddChild(PatternTreeNode* Child)
{
	Children.push_back(Child);
}

void FunctionDeclDatabaseEntry::AddParent(PatternTreeNode* Parent)
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
HPCParallelPattern::HPCParallelPattern(DesignSpace DesignSp, std::string PatternName) : Occurences()
{
	this->DesignSp = DesignSp;
	this->PatternName = PatternName;
}

void HPCParallelPattern::Print() 
{
	std::cout << "Pattern Info" << std::endl;
	std::cout << "Pattern Design Space: " << DesignSpaceToStr(this->DesignSp) << std::endl;
	std::cout << "Pattern Name: " << this->PatternName << std::endl;
	std::cout << this->Occurences.size() << " Occurences." << std::endl;
}

void HPCParallelPattern::AddOccurence(PatternOccurence* Occurence)
{
	this->Occurences.push_back(Occurence);
}

std::vector<PatternOccurence*> HPCParallelPattern::GetOccurencesWithID(std::string ID)
{
	std::vector<PatternOccurence*> res;

	for (PatternOccurence* Occ : Occurences)
	{
		if (!ID.compare(Occ->GetID()))
		{
			res.push_back(Occ);
		}
	}

	return res;
}

int HPCParallelPattern::GetTotalLinesOfCode()
{
	int LOC = 0;

	for (PatternOccurence* PatternOcc : this->Occurences)
	{
		LOC += PatternOcc->GetLinesOfCode();
	}

	return LOC;
}



/*
 * Pattern Occurence Class Functions
 */
PatternOccurence::PatternOccurence(HPCParallelPattern* Pattern, std::string ID) : PatternTreeNode(OK_Pattern), Parents(), Children()
{
	this->Pattern = Pattern;
	this->ID = ID;
}

void PatternOccurence::AddChild(PatternTreeNode* Child) 
{
	Children.push_back(Child);
}

void PatternOccurence::AddParent(PatternTreeNode* Parent)
{
	Parents.push_back(Parent);
}

void PatternOccurence::SetFirstLine(int FirstLine)
{
	this->LinesOfCode = FirstLine;
}

void PatternOccurence::SetLastLine(int LastLine)
{
	this->LinesOfCode = (LastLine - this->LinesOfCode) - 1;
}

bool PatternOccurence::Equals(PatternOccurence* PatternOcc)
{
	if (!this->ID.compare(PatternOcc->ID) && this->Pattern == PatternOcc->Pattern)
	{
		return true;
	}
	
	return false;
}

void PatternOccurence::Print()
{
	std::cout << "Pattern Identifier: " << this->ID << std::endl;
	this->Pattern->Print();
}



/*
 * HPC Pattern Database Functions
 */
HPCPatternDatabase::HPCPatternDatabase() : Patterns()
{
	
}

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

void HPCPatternDatabase::AddParallelPattern(HPCParallelPattern* Pattern)
{
	if (LookupParallelPattern(Pattern->GetDesignSpace(), Pattern->GetPatternName()) != NULL)
	{
		return;
		std::cout << "\033[31m" << "Pattern already exists in the database." << "\033[0m" << std::endl;
	}

	Patterns.push_back(Pattern);
}

std::vector<PatternOccurence*> HPCPatternDatabase::GetAllPatternOccurences()
{
	std::vector<PatternOccurence*> Occs;

	for (HPCParallelPattern* Pattern : Patterns)
	{
		for (PatternOccurence* PatternOcc : Pattern->GetAllOccurences())
		{
			Occs.push_back(PatternOcc);
		}
	}

	return Occs;
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
std::stack<PatternOccurence*> PatternContext;

void AddToPatternStack(PatternOccurence* PatternOcc)
{
	PatternContext.push(PatternOcc);
}

PatternOccurence* GetTopPatternStack() 
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

void RemoveFromPatternStack(std::string ID)
{
	if (!PatternContext.empty())
	{
		PatternOccurence* Top = PatternContext.top();	
		
		if (ID.compare(Top->GetID()))
		{
			std::cout << "\033[31m" << "Inconsistency in the pattern stack detected. Results may not be correct. Check the structure of the instrumentation in the application code!" << "\033[0m" << std::endl;
		}
		
		PatternContext.pop();
	}
}
