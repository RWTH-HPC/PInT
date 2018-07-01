#include "SimilarityMetrics.h"

/* 
 * Methods for the Similarity Metric Interface
 */
SimilarityMeasure::SimilarityMeasure(HPCParallelPattern* RootPattern, int maxlength, SearchDirection dir)
{
	this->RootPattern = RootPattern;
	this->maxlength = maxlength;
	this->dir = dir;
}

std::vector<SimilarityMeasure::PatternSequence*> SimilarityMeasure::FindPatternSeqs(PatternCodeRegion* PatternNode, SearchDirection dir, int maxdepth)
{
	std::vector<PatternSequence*> Seqs;

	PatternSequence* CurSeq;
	CurSeq = new PatternSequence;
	CurSeq->Patterns.push_back(PatternNode->GetPatternOccurence()->GetPattern());

	std::vector<PatternTreeNode*> Neighbours;

	/* determine the direction in which to build the sequences */
	if (dir ==  DIR_Children)
	{
		Neighbours = PatternNode->GetChildren();
	}
	else
	{
		Neighbours = PatternNode->GetParents();
	}

	/* Start with visiting the neighbours */
	for (PatternTreeNode* Neighbour : Neighbours)
	{
		VisitPatternTreeNode(Neighbour, CurSeq, &Seqs, dir, 1, maxdepth);
	}

	return Seqs;
}

void SimilarityMeasure::VisitPatternTreeNode(PatternTreeNode* CurrentNode, PatternSequence* CurrentSequence, std::vector<PatternSequence*>* Sequences, SearchDirection dir, int depth, int maxdepth)
{
	/* Check if the current node is a pattern occurence node */
	if (PatternCodeRegion* CurrentCodeReg = clang::dyn_cast<PatternCodeRegion>(CurrentNode))
	{
		/* Branch a new sequence from the previous */
		PatternSequence* NewSequence = CurrentSequence->Fork();
		NewSequence->Patterns.push_back(CurrentCodeReg->GetPatternOccurence()->GetPattern());
		Sequences->push_back(NewSequence);

		CurrentSequence = NewSequence;
	}

	/* If we can still add new occurences, then continue */	
	if (CurrentSequence->Patterns.size() < this->maxlength && depth < maxdepth)
	{
		/* Get neighbours */
		std::vector<PatternTreeNode*> Neighbours;

		if (dir == DIR_Children)
		{
			Neighbours = CurrentNode->GetChildren();
		}
		else
		{
			Neighbours = CurrentNode->GetParents();
		}
		
		/* Visit Neighbours */
		for (PatternTreeNode* Neighbour : Neighbours)
		{	
			VisitPatternTreeNode(Neighbour, CurrentSequence, Sequences, dir, 1, maxdepth);
		}
	}
}



/*
 * Methods for the Jaccard Similarity Statistic
 */
JaccardSimilarityStatistic::JaccardSimilarityStatistic(HPCParallelPattern* RootPattern, int length, SearchDirection dir) : SimilarityMeasure(RootPattern, length, dir)
{

}

void JaccardSimilarityStatistic::Calculate()
{

}

void JaccardSimilarityStatistic::Print()
{

}

void JaccardSimilarityStatistic::CSVExport()
{

}
