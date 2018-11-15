#include "SimilarityMetrics.h"



/**
 * @brief Constructor for the abstract similarity measure class.
 *
 * @param RootPattern The pattern from which the pattern sequences are constructed.
 * @param maxlength The maximum sequence length.
 * @param dir The direction (children/parents) in which the sequences are extracted.
 **/
SimilarityMeasure::SimilarityMeasure(std::vector<HPCParallelPattern*> RootPatterns, int maxlength, GraphSearchDirection dir)
{
	this->RootPatterns = RootPatterns;
	this->maxlength = maxlength;
	this->dir = dir;
}

/**
 * @brief Sorts the sequence pairs in place.
 *
 * @param Sims The similarity pairs that are meant to be sorted.
 **/
void SimilarityMeasure::SortBySimilarity(std::vector<SimilarityPair*>& Sims)
{
	std::sort(Sims.begin(), Sims.end(), SimilarityMeasure::CompareBySimilarity);
}

/**
 * @brief Compares two similarity pairs (Needed for SimilarityMeasure::SortBySimilarity)
 *
 * @param SimPair1 First pair.
 * @param SimPair2 Second pair.
 *
 * @return True if similarity of first pair is greater than of second. False, else.
 **/
bool SimilarityMeasure::CompareBySimilarity(const SimilarityPair* SimPair1, const SimilarityPair* SimPair2)
{
	return (SimPair1->Similarity > SimPair2->Similarity);
}

/**
 * @brief Extracts the pattern sequences starting with the root pattern and saves them in SimilarityMeasure::PatternSequence objects.
 * Calls SimilarityMeasure::VisitPatternGraphNode().
 *
 * @param PatternNode The starting code region.
 * @param dir Direction (children/parent) of descent.
 * @param maxdepth The maximum depth of the recursion.
 *
 * @return A list of pattern sequences.
 **/
std::vector<SimilarityMeasure::PatternSequence*> SimilarityMeasure::FindPatternSeqs(PatternCodeRegion* PatternNode, GraphSearchDirection dir, int maxdepth)
{
	std::vector<PatternSequence*> Seqs;

	PatternSequence* CurSeq;
	CurSeq = new PatternSequence;
	CurSeq->Patterns.push_back(PatternNode->GetPatternOccurrence()->GetPattern());

	std::vector<PatternGraphNode*> Neighbours;

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
	for (PatternGraphNode* Neighbour : Neighbours)
	{
		VisitPatternGraphNode(Neighbour, CurSeq, &Seqs, dir, 1, maxdepth);
	}

	return Seqs;
}

/**
 * @brief Recursive function for extraction of the pattern sequences.
 *
 * @param CurrentNode The current pattern tree node.
 * @param CurrentSequence The current pattern sequence to which we add further patterns.
 * @param Sequences A pointer to a vector of sequences to which new sequences are added.
 * @param dir Direction of recursive descent.
 * @param depth The current recursion depth.
 * @param maxdepth The maximum recursion depth.
 **/
void SimilarityMeasure::VisitPatternGraphNode(PatternGraphNode* CurrentNode, PatternSequence* CurrentSequence, std::vector<PatternSequence*>* Sequences, GraphSearchDirection dir, int depth, int maxdepth)
{
	/* Check if the current node is a pattern occurrence node */
	if (PatternCodeRegion* CurrentCodeReg = clang::dyn_cast<PatternCodeRegion>(CurrentNode))
	{
		/* Branch a new sequence from the previous */
		PatternSequence* NewSequence = CurrentSequence->Fork();
		NewSequence->Patterns.push_back(CurrentCodeReg->GetPatternOccurrence()->GetPattern());
		Sequences->push_back(NewSequence);

		CurrentSequence = NewSequence;
	}

	/* If we can still add new occurrences, then continue */
	if (CurrentSequence->Patterns.size() < this->maxlength && depth < maxdepth)
	{
		/* Get neighbours */
		std::vector<PatternGraphNode*> Neighbours;

		if (dir == DIR_Children)
		{
			Neighbours = CurrentNode->GetChildren();
		}
		else
		{
			Neighbours = CurrentNode->GetParents();
		}

		/* Visit Neighbours */
		for (PatternGraphNode* Neighbour : Neighbours)
		{
			VisitPatternGraphNode(Neighbour, CurrentSequence, Sequences, dir, depth + 1, maxdepth);
		}
	}
}

/**
 * @brief Filters a list of sequences so that only PatternSequence objects with a specific length are retained.
 *
 * @param PatternSequences Input list of PatternSequence objects.
 * @param minlength The minimum length.
 * @param maxlength The maximum length.
 *
 * @return Input list clear of sequences that are longer than maxlength and shorter than minlength.
 **/
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



/**
 * @brief Constructor for the Jaccard similarity statistic.
 *
 * @param RootPattern The root pattern for all pattern sequences.
 * @param minlength The minimum length for a sequence to be considered.
 * @param maxlength The max length of a sequence.
 * @param dir Direction of recursive descent.
 * @param Crit The similarity criterion used.
 * @param outputlen Length of the textual output: how many entries are displayed.
 **/
JaccardSimilarityStatistic::JaccardSimilarityStatistic(std::vector<HPCParallelPattern*> RootPattern, int minlength, int maxlength, GraphSearchDirection dir, SimilarityCriterion Crit, int outputlen) : SimilarityMeasure(RootPattern, maxlength, dir)
{
	this->minlength = minlength;
	this->Crit = Crit;
	this->outputlen = outputlen;
}

/**
 * @brief Calculates the Jaccard similarity statistic for all sequences extracted.
 * First, the sequences are filtered by length.
 * Then, a similarity is calculated for all pairs of sequences.
 */
void JaccardSimilarityStatistic::Calculate()
{
	/* Iterate over all occurrences and all code regions of the root patterns to find all sequences starting from this pattern */
	for (HPCParallelPattern* RootPattern : RootPatterns)
	{
		for (PatternCodeRegion* CodeRegion : RootPattern->GetCodeRegions())
		{
			std::vector<PatternSequence*> Seqs = FindPatternSeqs(CodeRegion, this->dir, 10);

			for (PatternSequence* Seq : Seqs)
			{
				this->PatternSequences.push_back(Seq);
			}
		}
	}

	this->PatternSequences = FilterSequencesByLength(this->PatternSequences, this->minlength, this->maxlength);


	/* Calculate the similarities for all pairs of pattern sequences */
	for (PatternSequence* Seq1 : this->PatternSequences)
	{
		for (PatternSequence* Seq2 : this->PatternSequences)
		{
			if (Seq1 != Seq2 && !Seq1->Equals(Seq2))
			{
				float sim = Similarity(Seq1, Seq2);

				SimilarityPair* SimPair = new SimilarityPair(Seq1, Seq2, sim);
				this->Similarities.push_back(SimPair);
			}
		}
	}

	SortBySimilarity(this->Similarities);
}

/**
 * @brief Prints similarities for all pairs of pattern sequences.
 */
void JaccardSimilarityStatistic::Print()
{
#if PRINT_DEBUG
	for (PatternSequence* Seq : this->PatternSequences)
	{
		Seq->Print();
	}
#endif

	std::cout << "Outputlen: " << outputlen << " Similarities: " << this->Similarities.size() << std::endl;

	for (int i = 0; i < std::min((ulong)outputlen, this->Similarities.size()); i++)
	{
		this->Similarities.at(i)->Print();
	}
}

/**
 * @brief Dummy function
 *
 * @param FileName File name of the output file.
 **/
void JaccardSimilarityStatistic::CSVExport(std::string FileName)
{

}

/**
 * @brief Calculates the Jaccard similarity for two given pattern sequences.
 * Calls JaccardSimilarityStatistic::UnionSet(), JaccardSimilarityStatistic::IntersectByDesignSp() and JaccardSimilarityStatistic::IntersectByPattern() to build a union set and an intersection of both sets constructed from the sequences.
 * Set sizes are then used to calculate the metric.
 *
 * @param Seq1 First sequence.
 * @param Seq2 Second sequence.
 *
 * @return The similarity of the sequences.
 **/
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

/**
 * @brief Builds an intersection of the input sets.
 * The critetion for intersection is that both patterns have the same design space.
 * They are added to an intermediate set, which is then cleansed from duplicates.
 *
 * @param Seq1 First Set.
 * @param Seq2 Second Set.
 *
 * @return The intersection of both sets.
 **/
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

	return SetAlgorithms::GetUniquePatternList(Intersection);
}

/**
 * @brief See JaccardSimilarityStatistic::IntersectByDesignSp().
 * Intersection criterion is the pattern name, hence pattern equality.
 *
 * @param Seq1 First Set.
 * @param Seq2 Second Set.
 *
 * @return The intersection of both sets.
 **/
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

	return SetAlgorithms::GetUniquePatternList(Intersection);
}

/**
 * @brief Computes the union of both sets.
 * All patterns are saved in an intermediate set and duplicates are removed.
 *
 * @param Seq1 First set.
 * @param Seq2 Second set.
 *
 * @return The union of both sets.
 **/
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

	return SetAlgorithms::GetUniquePatternList(Union);
}
