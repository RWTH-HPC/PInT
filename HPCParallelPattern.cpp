#include "HPCParallelPattern.h"

#include <iostream>
#include "clang/AST/ODRHash.h"



/*
 * Function Declaration Database Entry functions
 */
FunctionDeclDatabaseEntry::FunctionDeclDatabaseEntry (std::string Name, unsigned Hash) : PatternTreeNode(TNK_FnCall), Children(), Parents()
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

int HPCParallelPattern::GetTotalLinesOfCode()
{
	int LOC = 0;

	for (PatternOccurence* PatternOcc : this->Occurences)
	{
		LOC += PatternOcc->GetTotalLinesOfCode();
	}

	return LOC;
}

bool HPCParallelPattern::Equals(HPCParallelPattern* Pattern)
{
	if (this->DesignSp == Pattern->GetDesignSpace() && !this->PatternName.compare(Pattern->GetPatternName()))
	{
		return true;
	}
	
	return false;
}



/*
 * Pattern Occurence Class Functions
 */
int PatternOccurence::GetTotalLinesOfCode() 
{
	int LOC = 0;	

	for (PatternCodeRegion* CodeReg : this->CodeRegions)
	{
		LOC += CodeReg->GetLinesOfCode();
	}

	return LOC;
}

bool PatternOccurence::Equals(PatternOccurence* PatternOcc)
{
	if (!this->ID.compare(PatternOcc->GetID()) && this->Pattern->Equals(PatternOcc->GetPattern()))
	{
		return true;
	}
	
	return false;
}



/*
 * Pattern Code Region Class Functions
 */
PatternCodeRegion::PatternCodeRegion(PatternOccurence* PatternOcc) : PatternTreeNode(TNK_Pattern), Parents(), Children()
{
	this->PatternOcc = PatternOcc;
}

void PatternCodeRegion::AddChild(PatternTreeNode* Child) 
{
	Children.push_back(Child);
}

void PatternCodeRegion::AddParent(PatternTreeNode* Parent)
{
	Parents.push_back(Parent);
}

void PatternCodeRegion::SetFirstLine(int FirstLine)
{
	this->LinesOfCode = FirstLine;
}

void PatternCodeRegion::SetLastLine(int LastLine)
{
	this->LinesOfCode = (LastLine - this->LinesOfCode) - 1;
}

void PatternCodeRegion::Print()
{
	this->PatternOcc->GetPattern()->Print();
	this->PatternOcc->Print();
	std::cout << this->GetLinesOfCode() << " lines of code." << std::endl;
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

/* Search and add pattern occurences */
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

void HPCPatternDatabase::AddPatternOccurence(PatternOccurence* PatternOcc)
{
	if (LookupPatternOccurence(PatternOcc->GetID()) != NULL)
	{
		return;
		std::cout << "\033[31m" << "PatternOccurence already exists in the database." << "\033[0m" << std::endl;
	}
	
	PatternOccurences.push_back(PatternOcc);
}

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
std::stack<PatternCodeRegion*> PatternContext;

void AddToPatternStack(PatternCodeRegion* PatternOcc)
{
	PatternContext.push(PatternOcc);
}

PatternCodeRegion* GetTopPatternStack() 
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
		PatternCodeRegion* Top = PatternContext.top();	
		
		if (ID.compare(Top->GetID()))
		{
			std::cout << "\033[31m" << "Inconsistency in the pattern stack detected. Results may not be correct. Check the structure of the instrumentation in the application code!" << "\033[0m" << std::endl;
		}
		
		PatternContext.pop();
	}
}
