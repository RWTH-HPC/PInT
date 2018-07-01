#pragma once

#include "HPCPatternStatistics.h"
#include "HPCParallelPattern.h"
#include <vector>



enum SimilarityCriterion
{
	DesignSpace, Pattern
};


class SimilarityMeasure
{
public:
	/* Datastructure to save a sequence of PatternCodeRegions and link them with similarities */
	struct SimilarityPair;

	struct PatternSequence
	{
		std::vector<HPCParallelPattern*> Patterns;
		std::vector<SimilarityPair*> Similarities;
	
		/* A little fork operator, so that we can branch one sequence from another */
		PatternSequence* Fork()
		{
			PatternSequence* PS;
			PS = new PatternSequence;
		
			PS->Patterns = this->Patterns;
			return PS;
		}
	};

	/* Datastructure to save the similarity between two sequences */
	struct SimilarityPair
	{
		PatternSequence* Seq1;
		PatternSequence* Seq2;
		float Similarity;
	};


protected:
	SimilarityMeasure(HPCParallelPattern* RootPattern, int maxlength, SearchDirection dir);


	std::vector<SimilarityPair*> SortBySimilarity(std::vector<SimilarityPair*> Sims);


	std::vector<PatternSequence*> PatternSequences;

	std::vector<SimilarityPair*> Similarities;

	/* Functions to build the sequences of pattern occurences */
	std::vector<PatternSequence*> FindPatternSeqs(PatternCodeRegion* PatternNode, SearchDirection dir, int maxdepth);

	void VisitPatternTreeNode(PatternTreeNode* CurrentNode, PatternSequence* CurrentSequence, std::vector<PatternSequence*>* Sequences, SearchDirection dir, int depth, int maxdepth);


	HPCParallelPattern* RootPattern;

	int maxlength;
	
	SearchDirection dir;
};



class JaccardSimilarityStatistic : public HPCPatternStatistic, public SimilarityMeasure
{
public:
	JaccardSimilarityStatistic(HPCParallelPattern* RootPattern, int length, SearchDirection dir, SimilarityCriterion Crit);

	void Calculate();

	void Print();

	void CSVExport();

private:
	/* Functions to calculate the Jaccard Similarity */
	float Similarity(PatternSequence* Seq1, PatternSequence* Seq2);

	std::vector<HPCParallelPattern*> IntersectByDesignSp(std::vector<HPCParallelPattern*> Seq1, std::vector<HPCParallelPattern*> Seq2);

	std::vector<HPCParallelPattern*> IntersectByPatternName(std::vector<HPCParallelPattern*> Seq1, std::vector<HPCParallelPattern*> Seq2);

	std::vector<HPCParallelPattern*> UnionByDesignSp(std::vector<HPCParallelPattern*> Seq1, std::vector<HPCParallelPattern*> Seq2);

	std::vector<HPCParallelPattern*> UnionByPatternName(std::vector<HPCParallelPattern*> Seq1, std::vector<HPCParallelPattern*> Seq2);	

	std::vector<HPCParallelPattern*> RemoveDuplicates(std::vector<HPCParallelPattern*> InSet);

	SimilarityCriterion Crit;
};

