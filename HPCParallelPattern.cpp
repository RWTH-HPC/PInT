#include "HPCParallelPattern.h"

#include <iostream>
#include "clang/AST/ODRHash.h"



/*
 * HPC Parallel Pattern Class Functions
 */
HPCParallelPattern::HPCParallelPattern(DesignSpace DesignSp, std::string PatternName)
{
	this->DesignSp = DesignSp;
	this->PatternName = PatternName;
	this->Occurrences = std::vector<PatternOccurrence*>();
}

/**
 * @brief Prints design space, pattern name and number of occurrences.
 **/
void HPCParallelPattern::Print()
{
	std::cout << "Pattern Info" << std::endl;
	std::cout << "Pattern Design Space: " << DesignSpaceToStr(this->DesignSp) << std::endl;
	std::cout << "Pattern Name: " << this->PatternName << std::endl;
	std::cout << this->Occurrences.size() << " Occurrences." << std::endl;
}

/**
 * @brief Like Print() but output is only a single line.
 **/
void HPCParallelPattern::PrintShort()
{
	std::cout << "\033[33m" << DesignSpaceToStr(this->DesignSp) << "\033[0m" << this->PatternName;
}

void HPCParallelPattern::AddOccurrence(PatternOccurrence* Occurrence)
{
	this->Occurrences.push_back(Occurrence);
}

/**
 * @brief Returns the sum of lines of code from all pattern occurrences.
 *
 * @return Sum of lines of code.
 **/
int HPCParallelPattern::GetTotalLinesOfCode()
{
	int LOC = 0;

	for (PatternOccurrence* PatternOcc : this->Occurrences)
	{
		LOC += PatternOcc->GetTotalLinesOfCode();
	}

	return LOC;
}

/**
 * @brief Compares two patterns for design space and pattern name.
 *
 * @return True if equal, else false.
 **/
bool HPCParallelPattern::Equals(HPCParallelPattern* Pattern)
{
	if (this->DesignSp == Pattern->GetDesignSpace() && !this->PatternName.compare(Pattern->GetPatternName()))
	{
		return true;
	}

	return false;
}

std::vector<PatternOccurrence*> HPCParallelPattern::GetOccurrences() {
	if(!this->Occurrences.empty()){
	  	return this->Occurrences;
	}
	else{
		  return std::vector<PatternOccurrence*>();
	}
}

/**
 * @brief Get all code regions from all pattern occurrences.
 *
 * @return Return pointers to all PatternCodeRegion objects from all PatternOccurrence objects.
 **/
std::vector<PatternCodeRegion*> HPCParallelPattern::GetCodeRegions()
{
	std::vector<PatternCodeRegion*> CodeRegions = std::vector<PatternCodeRegion*>();

	for (PatternOccurrence* PatternOcc : this->GetOccurrences())
	{
		for (PatternCodeRegion* CodeReg : PatternOcc->GetCodeRegions())
		{
			CodeRegions.push_back(CodeReg);
		}
	}

	return CodeRegions;
}

void HPCParallelPattern::incrementNumOfOperators(){
	this->numOfOperators++;
}

int HPCParallelPattern::GetNumOfOperators(){
	return this->numOfOperators;
}

/*
 * Pattern Occurrence Class Functions
 */
PatternOccurrence::PatternOccurrence(HPCParallelPattern* Pattern, std::string ID)
{
	this->Pattern = Pattern;
	this->ID = ID;
}

/**
 * @brief Get the lines of code for all PatternCodeRegion objects registered with this PatternOccurrence.
 *
 * @return Sum of lines of code.
 **/
int PatternOccurrence::GetTotalLinesOfCode()
{
	int LOC = 0;

	for (PatternCodeRegion* CodeReg : this->CodeRegions)
	{
		LOC += CodeReg->GetLinesOfCode();
	}

	return LOC;
}

/**
 * @brief Compare a PatternOccurrence object with this object. The ID and the underlying HPCParallelPattern are compared.
 *
 * @param PatternOcc The PatternOccurrence object to compare with.
 *
 * @return True if equal, false elsewise.
 **/
bool PatternOccurrence::Equals(PatternOccurrence* PatternOcc)
{
	if (!this->ID.compare(PatternOcc->GetID()) && this->Pattern->Equals(PatternOcc->GetPattern()))
	{
		return true;
	}

	return false;
}

/**
 * @brief Prints the ID of this pattern occurrence as well as all information from HPCParallelPattern::Print().
 **/
void PatternOccurrence::Print()
{
	this->Pattern->Print();
	std::cout << this->GetID() << std::endl;
}



/*
 * Pattern Code Region Class Functions
 */
PatternCodeRegion::PatternCodeRegion(PatternOccurrence* PatternOcc) : PatternGraphNode(GNK_Pattern), Parents(), Children()
{
	this->PatternOcc = PatternOcc;
}

void PatternCodeRegion::AddChild(PatternGraphNode* Child)
{
	Children.push_back(Child);
}

void PatternCodeRegion::AddParent(PatternGraphNode* Parent)
{
	Parents.push_back(Parent);
}

void PatternCodeRegion::AddPatternChild(PatternGraphNode* PatChild)
{
	PatternChildren.push_back(PatChild);
}

void PatternCodeRegion::AddPatternParent(PatternGraphNode* PatParent)
{
	PatternParents.push_back(PatParent);
}


/**
 * @brief Save the first line of the code region to keep track of the lines of code.
 **/
void PatternCodeRegion::SetFirstLine(int FirstLine)
{
	this->LinesOfCode = FirstLine;
}

/**
 * @brief See PatternCodeRegion::SetFirstLine.
 **/
void PatternCodeRegion::SetLastLine(int LastLine)
{
	this->LinesOfCode = (LastLine - this->LinesOfCode) - 1;
}

void PatternCodeRegion::SetStartSourceLoc(clang::SourceLocation StartLoc)
{
	this->StartSLocation = StartLoc;
}

void PatternCodeRegion::SetEndSourceLoc(clang::SourceLocation EndLoc){
	this->EndSLocation = EndLoc;
}

/**
 * @brief Print the lines of code plus all information from PatternOccurrence::Print().
 **/
void PatternCodeRegion::Print()
{
	this->PatternOcc->Print();
	std::cout << this->GetLinesOfCode() << " lines of code." << std::endl;
}

clang::SourceLocation PatternCodeRegion::GetStartLoc(){
	return this->StartSLocation;
}

clang::SourceLocation PatternCodeRegion::GetEndLoc(){
	return this->EndSLocation;
}


/*
 * Pattern Stack Management
 */
std::stack<PatternCodeRegion*> PatternContext;

/**
 * @brief Add a PatternCodeRegion to the top of the pattern context stack.
 *
 * @param PatternReg Code Region to be placed on the stack.
 **/
void AddToPatternStack(PatternCodeRegion* PatternReg)
{
	PatternContext.push(PatternReg);
}

/**
 * @brief Get the top of the pattern context stack.
 *
 * @return Top PatternCodeRegion or NULL if stack is empty.
 **/
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

/**
 * @brief Remove top of the pattern context from the stack if the ID matches with the function input. Prints an error message if not.
 *
 * @param ID The suspected ID of the pattern context top.
 **/
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

/*Stack for Halstead */
std::vector<PatternOccurrence*> OccStackForHalstead;
