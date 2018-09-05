#pragma once

#include <string>
#include <vector>
#include <stack>
#include "clang/AST/Decl.h"
#include "llvm/Support/Casting.h"

#include "DesignSpaces.h"
#include "PatternGraph.h"



/* Forward declarations */
class PatternOccurrence;
class PatternCodeRegion;



/**
 * This class describes a parallel pattern identified by the design space and the pattern name.
 * The pattern name is not the same as the pattern identifier, as only PatternOccurrence can have an identifier.
 * A parallel pattern can have one or multiple PatternOccurrences which are registered in this object.
 */
class HPCParallelPattern
{
public:
	HPCParallelPattern(DesignSpace DesignSp, std::string PatternName);
	
	void Print();

	void PrintShort();

	void AddOccurrence(PatternOccurrence* Occurrence);

	std::vector<PatternOccurrence*> GetOccurrences() { return this->Occurrences; }

	std::vector<PatternCodeRegion*> GetCodeRegions();
	
	std::string GetPatternName() { return this->PatternName; }

	std::string GetDesignSpaceStr() { return DesignSpaceToStr(this->DesignSp); }

	DesignSpace GetDesignSpace() { return DesignSp; }
	
	int GetTotalLinesOfCode();

	bool Equals(HPCParallelPattern* Pattern);

private:	
	DesignSpace DesignSp;
	std::string PatternName;

	std::vector<PatternOccurrence*> Occurrences;
};



/**
 * The PatternOccurrence is a hypothetical construct that represents a collection for all code regions
 * with the same identifier.
 * It is linked to a unique HPCParallelPattern.
 * Each PatternCodeRegion with this identifier is accessible from this object.
 */
class PatternOccurrence
{
public:
	PatternOccurrence(HPCParallelPattern* Pattern, std::string ID);

	HPCParallelPattern* GetPattern() { return this->Pattern; }

	void Print();

	std::string GetID() { return this->ID; }

	void AddCodeRegion(PatternCodeRegion* CodeRegion) { this->CodeRegions.push_back(CodeRegion); }

	std::vector<PatternCodeRegion*> GetCodeRegions() { return this->CodeRegions; }

	int GetTotalLinesOfCode();

	int GetNumberOfCodeRegions() { return this->CodeRegions.size(); }

	bool Equals(PatternOccurrence* PatternOcc);
	
private:
	HPCParallelPattern* Pattern;

	std::vector<PatternCodeRegion*> CodeRegions;
	
	std::string ID;
};



/**
 * This class represents a block of code that is enclosed with the instrumentation calls.
 * It is a node in the pattern tree, hence has children and parents in the tree.
 * A PatternCodeRegion belongs to a PatternOccurrence.
 */
class PatternCodeRegion : public PatternGraphNode
{
public:
	PatternCodeRegion(PatternOccurrence* PatternOcc);

	PatternOccurrence* GetPatternOccurrence() { return this->PatternOcc; }

	static bool classof(const PatternGraphNode* Node)
	{
		return Node->GetKind() == PatternGraphNode::GNK_Pattern;
	}

	void Print();

	void AddChild(PatternGraphNode* Child);

	void AddParent(PatternGraphNode* Parent);

	std::vector<PatternGraphNode*> GetChildren() { return this->Children; }

	std::vector<PatternGraphNode*> GetParents() { return this->Parents; }

	void SetFirstLine (int FirstLine);

	void SetLastLine (int LastLine);

	int GetLinesOfCode() { return this->LinesOfCode; }

	std::string GetID() { return this->PatternOcc->GetID(); }

private:
	PatternOccurrence* PatternOcc;	

	std::vector<PatternGraphNode*> Parents;
	std::vector<PatternGraphNode*> Children;

	int LinesOfCode = 0;
};



/**
 * The pattern stack is used to keep track of the nesting of patterns.
 */
extern std::stack<PatternCodeRegion*> PatternContext;

void AddToPatternStack(PatternCodeRegion* PatternOcc);

PatternCodeRegion* GetTopPatternStack();

void RemoveFromPatternStack(std::string ID);
