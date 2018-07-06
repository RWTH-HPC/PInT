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
			VisitPatternTreeNode(Neighbour, CurrentSequence, Sequences, dir, depth + 1, maxdepth);
		}
	}
}

std::vector<SimilarityMeasure::PatternSequence*> SimilarityMeasure::FilterSequencesByLength(std::vector<PatternSequence*> PatternSequences, int minlength, int maxlength)
{
	std::vector<PatternSequence*> FilteredSequences;	

	/* Iterate over all sequences and check the length of the pattern list */
	for (PatternSequence* Seq : PatternSequences)
	{
		if (Seq->Patterns.size() >= minlength && Seq->Patterns.size() <= maxlength)
		{
			FilteredSequences.push_back(Seq);
		}
	}

	return FilteredSequences;
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
	/* Iterate over all occurences and all code regions of the root pattern to find all sequences starting from this pattern */
	for (PatternCodeRegion* CodeRegion : RootPattern->GetCodeRegions())
	{
		std::vector<PatternSequence*> Seqs = FindPatternSeqs(CodeRegion, this->dir, 10);

		for (PatternSequence* Seq : Seqs)
		{
			this->PatternSequences.push_back(Seq);
		}
	}

	this->PatternSequences = FilterSequencesByLength(this->PatternSequences, 3, 3);	
}

void JaccardSimilarityStatistic::Print()
{
	for (PatternSequence* Seq : this->PatternSequences)
	{
		Seq->Print();
	}
}

void JaccardSimilarityStatistic::CSVExport(std::string FileName)
{

}

/* Functions to calculate the Jaccard Similarity */
float JaccardSimilarityStatistic::Similarity(PatternSequence* Seq1, PatternSequence* Seq2)
{
	int num, denom;
	std::vector<HPCParallelPattern*> numset, denomset;
	
	denomset = UnionSet(Seq1->Patterns, Seq2->Patterns);

	switch (Crit)
	{
		/* Calculate the metric by using the Design Space as criterion */
		case DesignSpace:
			numset = IntersectByDesignSp(Seq1->Patterns, Seq2->Patterns);
			break;

		/* Use the pattern as intersection and union criterion */
		case Pattern:
			numset = IntersectByPattern(Seq1->Patterns, Seq2->Patterns);
			break;
	}

	/* Remove duplicates and calculate the numerator and denominator */
	num = numset.size();
	denom = denomset.size();

	return (float)(num) / (float)(denom);
}

std::vector<HPCParallelPattern*> JaccardSimilarityStatistic::IntersectByDesignSp(std::vector<HPCParallelPattern*> Seq1, std::vector<HPCParallelPattern*> Seq2)
{
	std::vector<HPCParallelPattern*> Intersection;

	/* Iterate over all patterns and add all combinations of patterns where the design spaces match */
	for (HPCParallelPattern* Pattern1 : Seq1)
	{
		for (HPCParallelPattern* Pattern2 : Seq2)
		{
			if (Pattern1->GetDesignSpace() == Pattern2->GetDesignSpace())
			{
				Intersection.push_back(Pattern1);
				Intersection.push_back(Pattern2);
			}
		}
	}

	return RemoveDuplicates(Intersection);
}

std::vector<HPCParallelPattern*> JaccardSimilarityStatistic::IntersectByPattern(std::vector<HPCParallelPattern*> Seq1, std::vector<HPCParallelPattern*> Seq2)
{
	std::vector<HPCParallelPattern*> Intersection;
	
	/* Add all patterns which are equal to one another */
	for (HPCParallelPattern* Pattern1 : Seq1)
	{
		for (HPCParallelPattern* Pattern2 : Seq2)
		{
			if (Pattern1->Equals(Pattern2))
			{
				Intersection.push_back(Pattern1);
			}
		}
	}

	return RemoveDuplicates(Intersection);
}

std::vector<HPCParallelPattern*> JaccardSimilarityStatistic::UnionSet(std::vector<HPCParallelPattern*> Seq1, std::vector<HPCParallelPattern*> Seq2)
{
	std::vector<HPCParallelPattern*> Union;

	/* Unite both sets by adding all patterns */	
	for (HPCParallelPattern* Pattern : Seq1)
	{
		Union.push_back(Pattern);
	}

	for (HPCParallelPattern* Pattern : Seq2)
	{
		Union.push_back(Pattern);
	}

	return RemoveDuplicates(Union);
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

