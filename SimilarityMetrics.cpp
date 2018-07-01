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

std::vector<SimilarityMeasure::SimilarityPair*> SimilarityMeasure::SortBySimilarity(std::vector<SimilarityPair*> Sims)
{
	// TODO Implement sorting function
	std::vector<SimilarityPair*> res;
	return res;
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
JaccardSimilarityStatistic::JaccardSimilarityStatistic(HPCParallelPattern* RootPattern, int length, SearchDirection dir, SimilarityCriterion Crit) : SimilarityMeasure(RootPattern, length, dir)
{
	this->Crit = Crit;
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

/* Functions to calculate the Jaccard Similarity */
float JaccardSimilarityStatistic::Similarity(PatternSequence* Seq1, PatternSequence* Seq2)
{
	int num, denom;
	std::vector<HPCParallelPattern*> numset, denomset;

	switch (Crit)
	{
		/* Calculate the metric by using the Design Space as criterion */
		case DesignSpace:
			numset = IntersectByDesignSp(Seq1->Patterns, Seq2->Patterns);
			denomset = UnionByDesignSp(Seq1->Patterns, Seq2->Patterns);
			break;

		/* Use the pattern as intersection and union criterion */
		case Pattern:
			numset = IntersectByPatternName(Seq1->Patterns, Seq2->Patterns);
			denomset = UnionByPatternName(Seq1->Patterns, Seq2->Patterns);		
			break;
	}

	/* Remove duplicates and calculate the numerator and denominator */
	numset = RemoveDuplicates(numset);
	num = numset.size();

	denomset = RemoveDuplicates(denomset);
	denom = denomset.size();

	return (float)(num) / (float)(denom);
}

std::vector<HPCParallelPattern*> JaccardSimilarityStatistic::IntersectByDesignSp(std::vector<HPCParallelPattern*> Seq1, std::vector<HPCParallelPattern*> Seq2)
{
	return Seq1;
}

std::vector<HPCParallelPattern*> JaccardSimilarityStatistic::IntersectByPatternName(std::vector<HPCParallelPattern*> Seq1, std::vector<HPCParallelPattern*> Seq2)
{
	return Seq1;
}

std::vector<HPCParallelPattern*> JaccardSimilarityStatistic::UnionByDesignSp(std::vector<HPCParallelPattern*> Seq1, std::vector<HPCParallelPattern*> Seq2)
{
	return Seq1;
}

std::vector<HPCParallelPattern*> JaccardSimilarityStatistic::UnionByPatternName(std::vector<HPCParallelPattern*> Seq1, std::vector<HPCParallelPattern*> Seq2)
{
	return Seq1;
}

std::vector<HPCParallelPattern*> JaccardSimilarityStatistic::RemoveDuplicates(std::vector<HPCParallelPattern*> InSet)
{
	std::vector<HPCParallelPattern*> OutSet;

	for (HPCParallelPattern* Pattern : InSet)
	{
		bool duplicate = false;

		/* Check, if the pattern has already been added to the output set */
		for (HPCParallelPattern* Pattern2 : OutSet)
		{
			if (Pattern->Equals(Pattern2))
			{
				duplicate = true;
				break;
			}
		}

		if (!duplicate)
		{
			OutSet.push_back(Pattern);
		}
	}

	return OutSet;
}

