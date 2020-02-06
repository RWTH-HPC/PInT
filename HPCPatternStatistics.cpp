#include "HPCPatternStatistics.h"
#include <iostream>
#include <fstream>
#include "Debug.h"

//#define LOCDEBUG
//#define PRINT_DEBUG
/*
 * Methods for the Cyclomatic Complexity Statistic
 */
CyclomaticComplexityStatistic::CyclomaticComplexityStatistic() : VisitedNodes()
{

}

void CyclomaticComplexityStatistic::Calculate()
{
	Edges = CountEdges();
	Nodes = CountNodes();
	ConnectedComponents = CountConnectedComponents();

	/* C = #Edges - #Nodes + 2 * #ConnectedComponents */
	CyclomaticComplexity = (Edges - Nodes) + 2 * ConnectedComponents;
}

void CyclomaticComplexityStatistic::Print()
{
	std::cout << "\033[33m" << "WARNING: Results from the Cyclomatic Complexity Statistic might be inconsistent!" << "\033[0m" << std::endl;
	std::cout << "Number of Edges: " << Edges << std::endl;
	std::cout << "Number of Nodes: " << Nodes << std::endl;
	std::cout << "Number of Connected Components: " << ConnectedComponents << std::endl;
	std::cout << "Resulting \033[33mCyclomatic Complexity\033[0m: " << CyclomaticComplexity << std::endl;
}

void CyclomaticComplexityStatistic::CSVExport(std::string FileName)
{
	std::ofstream File;
	File.open(FileName, std::ios::app);

	File << "CyclComplexity" << CSV_SEPARATOR_CHAR << "NumEdges" << CSV_SEPARATOR_CHAR << "NumNodes" << CSV_SEPARATOR_CHAR << "NumConnectedComps\n";
	File << CyclomaticComplexity << CSV_SEPARATOR_CHAR << Edges << CSV_SEPARATOR_CHAR << Nodes << CSV_SEPARATOR_CHAR << ConnectedComponents;

	File.close();
}

void CyclomaticComplexityStatistic::SetNodeVisited(PatternGraphNode* Node)
{
	if (!IsNodeVisited(Node))
	{
		VisitedNodes.push_back(Node);
	}
}

bool CyclomaticComplexityStatistic::IsNodeVisited(PatternGraphNode* Node)
{
	for (PatternGraphNode* N : VisitedNodes)
	{
		if (N == Node)
		{
			return true;
		}
	}

	return false;
}

int CyclomaticComplexityStatistic::CountEdges()
{
	/* Start the tree traversal from all functions */
	std::vector<FunctionNode*> Functions = PatternGraph::GetInstance()->GetAllFunctions();

	int edges = 0;

	for (FunctionNode* Fn : Functions)
	{
		edges += CountEdges(Fn);
	}

	return edges;
}

int CyclomaticComplexityStatistic::CountEdges(PatternGraphNode* Current)
{
	int Edges = 0;

	/* If we visit a pattern, add the incoming edge */
	if (PatternCodeRegion* Pattern = clang::dyn_cast<PatternCodeRegion>(Current))
	{
		Edges = Edges + 1;
	}

	/* If we already visited this node, then just return 1 if this is a pattern, 0 else */
	if (IsNodeVisited(Current))
	{
		return Edges;
	}
	else
	{
		SetNodeVisited(Current);

		/* Count the Edges beginning from the children */
		for (PatternGraphNode* Child : Current->GetChildren())
		{
			Edges += CountEdges(Child);
		}
	}

	return Edges;
}

int CyclomaticComplexityStatistic::CountNodes()
{
	std::vector<HPCParallelPattern*> Patterns = PatternGraph::GetInstance()->GetAllPatterns();

	int nodes = 0;

	/* Count all occurrences for all patterns */
	for (HPCParallelPattern* Pattern : Patterns)
	{
		std::vector<PatternCodeRegion*> CodeRegs = Pattern->GetCodeRegions();
		nodes += CodeRegs.size();
	}

	return nodes;
}

int CyclomaticComplexityStatistic::CountConnectedComponents()
{
	GraphAlgorithms::MarkConnectedComponents();

	std::vector<PatternCodeRegion*> CodeRegs = PatternGraph::GetInstance()->GetAllPatternCodeRegions();
	std::vector<FunctionNode*> Functions = PatternGraph::GetInstance()->GetAllFunctions();

	int ConnectedComponents = 0;

	/* Get CCs from the code regions */
	for (PatternCodeRegion* Reg : CodeRegs)
	{
		if (Reg->GetConnectedComponent() > ConnectedComponents)
		{
			ConnectedComponents = Reg->GetConnectedComponent();
		}
	}

	/* Also check the function calls */
	for (FunctionNode* Fn : Functions)
	{
		if (Fn->GetConnectedComponent() > ConnectedComponents)
		{
			ConnectedComponents = Fn->GetConnectedComponent();
		}
	}

	/* Return highest index + 1 (null based index) */
	return ConnectedComponents + 1;
}



/*
 * Methods for the lines of code statistic
 */
void LinesOfCodeStatistic::Calculate()
{

}

void LinesOfCodeStatistic::Print()
{
	std::vector<HPCParallelPattern*> Patterns = PatternGraph::GetInstance()->GetAllPatterns();

	for (HPCParallelPattern* Pattern : Patterns)
	{
		std::cout << "\033[33m" << Pattern->GetPatternName() << "\033[0m" << " has " << Pattern->GetTotalLinesOfCode() << " line(s) of code in total." << std::endl;

		std::vector<PatternOccurrence*> Occurrences = Pattern->GetOccurrences();
		std::cout << Occurrences.size() << " occurrences in code." << std::endl;

		for (PatternOccurrence* PatternOcc : Occurrences)
		{
				std::cout << PatternOcc->GetID() << ": " << PatternOcc->GetTotalLinesOfCode() << " LOC in " << PatternOcc->GetNumberOfCodeRegions() << " regions." << std::endl;
		}

		std::cout << "Line(s) of code respectively." << std::endl << std::endl;
	}
}

void LinesOfCodeStatistic::CSVExport(std::string FileName)
{
	std::ofstream File;
	File.open(FileName, std::ios::app);

	File << "Patternname" << CSV_SEPARATOR_CHAR << "NumRegions" << CSV_SEPARATOR_CHAR << "LOCByRegions" << CSV_SEPARATOR_CHAR << "TotalLOCs\n";

	std::vector<HPCParallelPattern*> Patterns = PatternGraph::GetInstance()->GetAllPatterns();

	for (HPCParallelPattern* Pattern : Patterns)
	{
		File << Pattern->GetPatternName()  << CSV_SEPARATOR_CHAR;

		std::vector<PatternCodeRegion*> PatternCodeRegions = Pattern->GetCodeRegions();
		File << PatternCodeRegions.size() << CSV_SEPARATOR_CHAR;

		/* Print the list of lines of code for this pattern */
		File << "\"";

		for (unsigned long i = 0; i < PatternCodeRegions.size() - 1; i++)
		{
			File << PatternCodeRegions.at(i)->GetLinesOfCode() << ", ";
		}

		File << PatternCodeRegions.at(PatternCodeRegions.size() - 1)->GetLinesOfCode();
		File << "\"" << CSV_SEPARATOR_CHAR;

		File << Pattern->GetTotalLinesOfCode() << "\n";
	}

	File.close();
}



/*
 * Methods for the simple pattern counter
 */
SimplePatternCountStatistic::SimplePatternCountStatistic()
{

}

void SimplePatternCountStatistic::Calculate()
{

}

void SimplePatternCountStatistic::Print()
{
	std::vector<HPCParallelPattern*> Patterns = PatternGraph::GetInstance()->GetAllPatterns();

	for (HPCParallelPattern* Pattern : Patterns)
	{
		std::cout << "Pattern \033[33m" << Pattern->GetPatternName() << "\033[0m occurs " << Pattern->GetOccurrences().size() << " times." << std::endl;
	}
}

void SimplePatternCountStatistic::CSVExport(std::string FileName)
{
	std::ofstream File;
	File.open(FileName, std::ios::app);

	File << "Patternname" << CSV_SEPARATOR_CHAR << "Count\n";

	std::vector<HPCParallelPattern*> Patterns = PatternGraph::GetInstance()->GetAllPatterns();

	for (HPCParallelPattern* Pattern : Patterns)
	{
		File << Pattern->GetPatternName() << CSV_SEPARATOR_CHAR << Pattern->GetOccurrences().size() << "\n";
	}

	File.close();
}


FanInFanOutStatistic::FanInFanOutStatistic(int maxdepth) : FIFOCounter()
{
	this->maxdepth = maxdepth;
}

void FanInFanOutStatistic::Calculate()
{
	std::vector<HPCParallelPattern*> Patterns = PatternGraph::GetInstance()->GetAllPatterns();

	for (HPCParallelPattern* Pattern : Patterns)
	{
		FanInFanOutCounter* Counter = LookupFIFOCounter(Pattern);

		if (Counter == NULL)
		{
			Counter = AddFIFOCounter(Pattern);
		}

		/* We want to count the number of pattern occurrences */
		std::vector<PatternOccurrence*> Parents;
		std::vector<PatternOccurrence*> Children;

		for (PatternCodeRegion* CodeReg : GetCodeRegions(Pattern))
		{
#ifdef PRINT_DEBUG
			CodeReg->Print();
			std::cout << std::endl;
#endif
			/* Search in Parent and Child Directions */
			FindParentPatterns(CodeReg, Parents, maxdepth);
#ifdef PRINT_DEBUGPARENTS
			std::cout << "LIST OF PARENTS " << std::endl;

			for (PatternOccurrence* Parent : Parents)
			{
				Parent->Print();
				std::cout << std::endl;
			}
#endif
			FindChildPatterns(CodeReg, Children, maxdepth);
#ifdef PRINT_DEBUG
			std::cout << "List of children: " << std::endl;

			for (PatternOccurrence* Child : Children)
			{
				Child->Print();
				std::cout << std::endl;
			}

			std::cout << std::endl;
#endif
		}

		/* Filter out the duplicates */
		Parents = SetAlgorithms::GetUniquePatternOccList(Parents);
		Children = SetAlgorithms::GetUniquePatternOccList(Children);

		/* Calculate the resulting fan-in and fan-out numbers */
		Counter->FanIn += Parents.size();
		Counter->FanOut += Children.size();
	}
}

void FanInFanOutStatistic::Print()
{
	for (FanInFanOutCounter* Counter : FIFOCounter)
	{
		std::cout << "Pattern \033[33m" << Counter->Pattern->GetPatternName() << "\033[0m has" << std::endl;
		std::cout << "Fan-In: " << Counter->FanIn << std::endl;
		std::cout << "Fan-Out: " << Counter->FanOut << std::endl;
	}
}

void FanInFanOutStatistic::CSVExport(std::string FileName)
{
	std::ofstream File;
	File.open(FileName, std::ios::app);

	File << "Patternname" << CSV_SEPARATOR_CHAR << "FanIn" << CSV_SEPARATOR_CHAR << "FanOut" << "\n";

	for (FanInFanOutCounter* Counter : FIFOCounter)
	{
		File << Counter->Pattern->GetPatternName() << CSV_SEPARATOR_CHAR << Counter->FanIn << CSV_SEPARATOR_CHAR << Counter->FanOut << "\n";
	}

	File.close();
}

std::vector<PatternCodeRegion*> FanInFanOutStatistic::GetCodeRegions(HPCParallelPattern* Pattern){
	std::vector<PatternCodeRegion*> returnVector;
	for(PatternCodeRegion* CodeReg : Pattern->GetCodeRegions()){
		if(CodeReg->isSuitedForNestingStatistics){
			returnVector.push_back(CodeReg);
		}
	}
	return returnVector;
}


FanInFanOutStatistic::FanInFanOutCounter* FanInFanOutStatistic::LookupFIFOCounter(HPCParallelPattern* Pattern)
{
	/* Look up the Pattern Counter for this pattern */
	for (FanInFanOutStatistic::FanInFanOutCounter* Counter : FIFOCounter)
	{
		if (Counter->Pattern == Pattern)
		{
			return Counter;
		}
	}

	return NULL;
}

FanInFanOutStatistic::FanInFanOutCounter* FanInFanOutStatistic::AddFIFOCounter(HPCParallelPattern* Pattern)
{
	/* Create a new Pattern Counter for this Pattern */
	FanInFanOutStatistic::FanInFanOutCounter* Counter = new FanInFanOutCounter;
	Counter->Pattern = Pattern;
	FIFOCounter.push_back(Counter);
	return Counter;
}

void FanInFanOutStatistic::FindParentPatterns(PatternCodeRegion* Start, std::vector<PatternOccurrence*>& Parents, int maxdepth)
{
	/* Find the parent pattern code regions using API functionality */
	std::vector<PatternCodeRegion*> CodeRegions;
	GraphAlgorithms::FindParentPatternCodeRegions(Start, CodeRegions, maxdepth);

	int i = 0;
	for(PatternCodeRegion* CodeReg : CodeRegions){
		if(!CodeReg->isSuitedForNestingStatistics){
			CodeRegions.erase(CodeRegions.begin()+i);
		}
		i++;
	}

	std::vector<PatternOccurrence*> TempParents = PatternHelpers::GetPatternOccurrences(CodeRegions, true);

	Parents.insert(Parents.end(),TempParents.begin(), TempParents.end());
}

void FanInFanOutStatistic::FindChildPatterns(PatternCodeRegion* Start, std::vector<PatternOccurrence*>& Children, int maxdepth)
{
	/* Find child pattern code regions using API */
	std::vector<PatternCodeRegion*> CodeRegions;
	GraphAlgorithms::FindChildPatternCodeRegions(Start, CodeRegions, maxdepth);

	std::vector<PatternOccurrence*> TempChildren = PatternHelpers::GetPatternOccurrences(CodeRegions, true);

	Children.insert(Children.end(),TempChildren.begin(), TempChildren.end());
}


Halstead::Halstead () {
	int numOfOperators = 0;
	//clang::tooling::runToolOnCode(new HalsteadClassAction, "main.cpp");
}

void Halstead::Calculate(){
}

void Halstead::Print(){
/*	printf("Halstead metric\n\n");

	for(int i = 0; i < HPatterns.size(); i++){

		std::cout << "Pattern Design Space: " << HPatterns[i]->GetDesignSpaceStr() << std::endl;
		std::cout << "Pattern Name: " << HPatterns[i]->GetPatternName() << std::endl;
		std::cout <<"Pattern number of operants: "<< HPatterns[i]->GetNumOfOperators() <<'\n'<<std::endl;
	}*/

}

void Halstead::CSVExport(std::string FileName){

}

void Halstead::insertPattern(HPCParallelPattern* Pat){
	bool patRegistered = false;
	for(int i = 0; i < (int) HPatterns.size(); i++){
		if(HPatterns[i]->Equals(Pat)){
			patRegistered = true;
		}
	}
	if(!patRegistered){
		HPatterns.push_back(Pat);
	}
}











/*  HALSTEAD METRIK (Patternbezogen)

HalsteadMetrikPattern::HalsteadMetrikPattern()
{
}

void HalsteadMetrikPattern::Calculate()
{
}

void HalsteadMetrikPattern::Print()
{
}

void HalsteadMetrikPattern::CSVExport()
{
}

--------------- private Funktionen ---------------
int HalsteadMetrikPattern::CalculateProgrammLength()
{
	int Operand = 0;
	int Operator = 0;

	Vektor mit allen im Code auftauchenden Pattern
	std::vector<HPCParallelPattern*> Patterns = PatternGraph::GetInstance()->GetAllPatterns();

	for(HPCParallelPattern* Patterns : Pattern)
	{
		für jedes erscheinen (Occurrence) des Pattern schauen wir ob die Occurrence ein Operand ist oder ein Operator
		std::vector<PatternOccurrence*> PatternOcc;
		PatternOcc = Pattern->GetOccurrence();

		for(PatternOccurrence* PatternOcc : Occ)
		{
			std::vector<PatternCodeRegion*> CodeRegOfOcc = Occ->GetCodeRegions();

			for(PatternCodeRegion* CodeRegOfOcc : OccCodeReg)
			{
				std::vector<PatternCodeRegion*> Parents;
				GraphAlgorithms::FindParentPatternCodeRegions(OccCodeReg, Parents, 1);

				if(!(Parents.empty())) //--> PatternOcc ist Operand
				{
					Operand++;
				}

				std::vector<PatternCodeRegion*> Children;
				GraphAlgorithms::FindChildPatternCodeRegions(OccCodeReg, Children, 1)

				if(!(Children.empty())) //--> PatternOcc ist ein Operator
				{
					Operator++;
				}

			}
		}
	}
 return Operand + Operator;
}

int HalsteadMetrikPattern::CalculateVolume()
{
 int n1 = 0; // Anzahl einzigartiger Operatoren
 int n2 = 0; // Anzahl einzigartiger Operanden

 std::vector<HPCParallelPattern*> Patterns = PatternGraph::GetInstance()->GetAllPatterns();

 for(HPCParallelPattern* Patterns : Pattern)
 {
	 if(IsOperator(Pattern)){ n1++; }
	 if(IsOperand(Pattern)){ n2++; }
 }

 // das Volumen ist N log n. wobei N die Halstead-änge des Programm ist
return HalsteadMetrikPattern::CalculateVolume() * std::log2(n1+n2);
}

int HalsteadMetrikPattern::CalculateDifficulty()
{

}

int HalsteadMetrikPattern::CalculateEffort()
{
}

int HalsteadMetrikPattern::CalculateHalsteadTime()
{
}

bool IsOperand(HPCParallelPattern* Pattern)
{
	std::vector<HPCParallelPattern*> Patterns = PatternGraph::GetInstance()->GetAllPatterns();

	for(HPCParallelPattern* Patterns : Pattern)
	{
		std::vector<PatternOccurrence*> PatternOcc;
		PatternOcc = Pattern->GetOccurrence();

		for(PatternOccurrence* PatternOcc : Occ)
		{
			std::vector<PatternCodeRegion*> CodeRegOfOcc = Occ->GetCodeRegions();

			for(PatternCodeRegion* CodeRegOfOcc : OccCodeReg)
			{
				std::vector<PatternCodeRegion*> Parents;
				GraphAlgorithms::FindParentPatternCodeRegions(OccCodeReg, Parents, 1);

				if(!(Parents.empty())) //--> PatternOcc ist Operand
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool IsOperator(HPCParallelPattern* Pattern)
{
	std::vector<HPCParallelPattern*> Patterns = PatternGraph::GetInstance()->GetAllPatterns();

	for(HPCParallelPattern* Patterns : Pattern)
	{
		std::vector<PatternOccurrence*> PatternOcc;
		PatternOcc = Pattern->GetOccurrence();

		for(PatternOccurrence* PatternOcc : Occ)
		{
			std::vector<PatternCodeRegion*> CodeRegOfOcc = Occ->GetCodeRegions();

			for(PatternCodeRegion* CodeRegOfOcc : OccCodeReg)
			{
				std::vector<PatternCodeRegion*> Children;
				GraphAlgorithms::FindChildPatternCodeRegions(OccCodeReg, Parents, 1);

				if(!(Children.empty())) //--> PatternOcc ist Operator
				{
					return true;
				}
			}
		}
	}
 }
*/
