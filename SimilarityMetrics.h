#pragma once

#include "HPCPatternStatistics.h"
#include "HPCParallelPattern.h"
#include <vector>
#include <iostream>
#include <algorithm>


/**
 * Similarity Criterion for intersection of pattern sets.
 * Intersection by design space means that both patterns have the same design space.
 * By Pattern means pattern identity.
 */
enum SimilarityCriterion
{
	DesignSpace, Pattern
};

/**
 * An abstract class for similarity measures which provides mutual methods and standard implementations.
 * All similarity measures should inherit from this class.
 * Most importantly, extraction of pattern sequences from the pattern tree for later similarity investigations is implemented.
 * Furthermore, miscellaneous implementations are provided here.
 * E.g. sorting by similarity etc.
 */
class SimilarityMeasure
{
public:
	/* Datastructure to save a sequence of PatternCodeRegions and link them with similarities */
	struct SimilarityPair;

	/**
 	 * Class that contains a sequence of patterns and similarities to other PatternSequence objects.
 	 */
	struct PatternSequence
	{
		/**
 		 * The patterns contained in this pattern sequence
 		 */
		std::vector<HPCParallelPattern*> Patterns;
		/**
 		 * Similarities to other Pattern Sequences
 		 */
		std::vector<SimilarityPair*> Similarities;

		/**
 		 * A convenient fork operator which copies the patterns to the new objects.
 		 * Similarities are not copied.
 		 *
 		 * @returns A new PatternSequence object with same patterns.
		 */
		PatternSequence* Fork()
		{
			PatternSequence* PS;
			PS = new PatternSequence;

			PS->Patterns = this->Patterns;
			return PS;
		}

		/**
 		 * Prints all the information for this pattern sequence.
 		 * Calls HPCParallelPattern::PrintShort()
 		 */
		void Print()
		{
			for (HPCParallelPattern* Pattern : Patterns)
			{
				Pattern->PrintShort();
				std::cout << std::endl;
			}

			std::cout << std::endl;
		}

		/**
 		 * Function for comparison of PatternSequence objects.
 		 * @param Seq Sequence to compare against.
 		 *
 		 * @returns True if equal, false else.
 		 */
		bool Equals (PatternSequence* Seq)
		{
			/* Check for equality by iterating over the list of patterns until a difference is found */
			if (this->Patterns.size() == Seq->Patterns.size())
			{
				for (unsigned long i = 0; i < this->Patterns.size(); i++)
				{
					if (!this->Patterns.at(i)->Equals(Seq->Patterns.at(i)))
					{
						return false;
					}
				}

				return true;
			}

			return false;
		}

		int Size()
		{
			return this->Patterns.size();
		}
	};

	/**
 	 * Datastructure to save the similarity between two sequences
 	 */
	struct SimilarityPair
	{
		PatternSequence* Seq1;
		PatternSequence* Seq2;
		float Similarity;

		/**
 		 * Constructor for similarity pairs.
 		 *
 		 * @param Seq1 First sequence.
 		 * @param Seq2 Second sequence.
 		 * @param Similarity Calculated similarity.
 		 */
		SimilarityPair(PatternSequence* Seq1, PatternSequence* Seq2, float Similarity)
		{
			this->Seq1 = Seq1;
			this->Seq2 = Seq2;
			this->Similarity = Similarity;
		}

		/**
 		 * Prints the information contained in this object.
 		 */
		void Print()
		{
			int maxlength = std::max(this->Seq1->Size(), this->Seq2->Size());

			/* Print both sequences next to each other */
			for (int i = 0; i < maxlength; i++)
			{
				if (i < this->Seq1->Size())
				{
					this->Seq1->Patterns.at(i)->PrintShort();
				}

				std::cout << "\033[31m  - \033[0m";

				if (i < this->Seq2->Size())
				{
					this->Seq2->Patterns.at(i)->PrintShort();
				}

				std::cout << std::endl;
			}

			std::cout << "Resulting similarity: " << this->Similarity << std::endl << std::endl;
		}
	};

	static bool CompareBySimilarity(const SimilarityPair* SimPair1, const SimilarityPair* SimPair2);

	SimilarityMeasure(std::vector<HPCParallelPattern*> RootPattern, int maxlength, GraphSearchDirection dir);

	static void SortBySimilarity(std::vector<SimilarityPair*>& Sims);

	std::vector<PatternSequence*> FindPatternSeqs(PatternCodeRegion* PatternNode, GraphSearchDirection dir, int maxdepth);

	std::vector<PatternSequence*> FilterSequencesByLength(std::vector<PatternSequence*> Sequences, int minlength, int maxlength);

protected:
	std::vector<PatternSequence*> PatternSequences;

	std::vector<SimilarityPair*> Similarities;

	void VisitPatternGraphNode(PatternGraphNode* CurrentNode, PatternSequence* CurrentSequence, std::vector<PatternSequence*>* Sequences, GraphSearchDirection dir, int depth, int maxdepth);

	std::vector<HPCParallelPattern*> RootPatterns;

	int maxlength;

	GraphSearchDirection dir;
};



/**
 * This class implements the Jaccard similarity statistic.
 * Jaccard coefficient is a metric that can be used to quantify similarity between two sets.
 * We adapted this metric for parallel patterns.
 * It is calculated as the size of the intersection divided by size of the union.
 * J = size(intersection(A, B)) / size(union(A, B)).
 * The intersection criterion can be chosen upon statistic object creation.
 * The order of patterns in the pattern sequences is irrelevant for this set-based similarity measure.
 */
class JaccardSimilarityStatistic : public HPCPatternStatistic, public SimilarityMeasure
{
public:
	JaccardSimilarityStatistic(std::vector<HPCParallelPattern*> RootPattern, int minlength, int maxlength, GraphSearchDirection dir, SimilarityCriterion Crit, int outputlen);

	void Calculate();

	void Print();

	void CSVExport(std::string FileName);

private:
	/* Functions to calculate the Jaccard Similarity */
	float Similarity(PatternSequence* Seq1, PatternSequence* Seq2);

	std::vector<HPCParallelPattern*> IntersectByDesignSp(std::vector<HPCParallelPattern*> Seq1, std::vector<HPCParallelPattern*> Seq2);

	std::vector<HPCParallelPattern*> IntersectByPattern(std::vector<HPCParallelPattern*> Seq1, std::vector<HPCParallelPattern*> Seq2);

	std::vector<HPCParallelPattern*> UnionSet(std::vector<HPCParallelPattern*> Seq1, std::vector<HPCParallelPattern*> Seq2);

	std::vector<HPCParallelPattern*> RemoveDuplicates(std::vector<HPCParallelPattern*> InSet);

	int outputlen;

	int minlength;

	SimilarityCriterion Crit;
};
