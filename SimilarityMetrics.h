#pragma once

#include "HPCPatternStatistics.h"
#include "HPCParallelPattern.h"
#include <vector>
#include <iostream>
#include <algorithm>



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
	
		/* Prints all the information for this pattern sequence */
		void Print()
		{
			for (HPCParallelPattern* Pattern : Patterns)
			{
				Pattern->PrintShort();
				std::cout << std::endl;
			}
	
			std::cout << std::endl;
		}
	
		bool Equals (PatternSequence* Seq)
		{
			/* Check for equality by iterating over the list of patterns until a difference is found */
			if (this->Patterns.size() == Seq->Patterns.size())
			{
				for (int i = 0; i < this->Patterns.size(); i++)
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

	/* Datastructure to save the similarity between two sequences */
	struct SimilarityPair
	{
		PatternSequence* Seq1;
		PatternSequence* Seq2;
		float Similarity;

		SimilarityPair(PatternSequence* Seq1, PatternSequence* Seq2, float Similarity)
		{
			this->Seq1 = Seq1;
			this->Seq2 = Seq2;
			this->Similarity = Similarity;
		}	

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

	SimilarityMeasure(HPCParallelPattern* RootPattern, int maxlength, SearchDirection dir);

	std::vector<SimilarityPair*> SortBySimilarity(std::vector<SimilarityPair*> Sims);

	std::vector<PatternSequence*> FindPatternSeqs(PatternCodeRegion* PatternNode, SearchDirection dir, int maxdepth);

	std::vector<PatternSequence*> FilterSequencesByLength(std::vector<PatternSequence*> Sequences, int minlength, int maxlength);

protected:
	std::vector<PatternSequence*> PatternSequences;

	std::vector<SimilarityPair*> Similarities;

	void VisitPatternTreeNode(PatternTreeNode* CurrentNode, PatternSequence* CurrentSequence, std::vector<PatternSequence*>* Sequences, SearchDirection dir, int depth, int maxdepth);

	HPCParallelPattern* RootPattern;

	int maxlength;
	
	SearchDirection dir;
};



class JaccardSimilarityStatistic : public HPCPatternStatistic, public SimilarityMeasure
{
public:
	JaccardSimilarityStatistic(HPCParallelPattern* RootPattern, int minlength, int maxlength, SearchDirection dir, SimilarityCriterion Crit, int outputlen);

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

