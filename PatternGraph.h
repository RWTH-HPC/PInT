#pragma once

#include "DesignSpaces.h"

#include <string>
#include <vector>
#include <stack>
#include "clang/AST/Decl.h"
#include "llvm/Support/Casting.h"



/* Forward declarations */
class HPCParallelPattern;
class PatternOccurrence;
class PatternCodeRegion;



/**
 * This abstract class is a prototype for all pattern graph node classes following.
 * Descendants have to implement functionality to manage parents and children in the pattern tree/graph.
 * Also, some variables about the connected components are saved in this class.
 */
class PatternGraphNode {
public:
	/**
 	* This enum is needed for LLVM type checking. New classes inheriting from this class should add their own enum values here.
 	*/
	enum GraphNodeKind
	{
		GNK_FnCall,
		GNK_Pattern
	};

	GraphNodeKind GetKind() const
	{
		return Kind;
	}

	PatternGraphNode(GraphNodeKind OK) : Kind(OK)
	{
		this->ComponentID = -1;
	}

	virtual void AddChild(PatternGraphNode* Child) = 0;

	virtual void AddParent(PatternGraphNode* Parent) = 0;

	virtual std::vector<PatternGraphNode*> GetChildren() = 0;

	virtual std::vector<PatternGraphNode*> GetParents() = 0;

	void SetConnectedComponent(int CID) { this->ComponentID = CID; }

	int GetConnectedComponent() { return this->ComponentID; }

private:
	int ComponentID;

	const GraphNodeKind Kind;
};



/**
 * A FunctionNode is a node in the pattern graph (i.e. inherits from PatternGraphNode), and has children and parents.
 * It contains a hash value to uniquely identify a function declaration across compilation-units.
 * This is useful if a function is called and we need information from the function body but the function is not defined within the current translation unit.
 * Then, the reference is saved for later until the definition belonging to the function declaration is encountered.
 */
class FunctionNode : public PatternGraphNode
{
public:
	FunctionNode (std::string Name, unsigned Hash);

	void AddChild(PatternGraphNode* Child);

	void AddParent(PatternGraphNode* Parent);

	void AddPatternParent(PatternGraphNode* PatParent);

	void AddPatternParents(std::vector<PatternCodeRegion*> PatternParents);

	void AddPatternChild(PatternGraphNode* PatChild);

	std::vector<PatternCodeRegion*> GetPatternParents();

	std::vector<PatternCodeRegion*> GetPatternChildren();

	bool HasNoPatternParents();

	bool HasNoPatternChildren();

	void registerPatChildrenToPatParents();
	
	void PrintVecOfPattern(std::vector<PatternCodeRegion*> RegionVec);

	std::vector<PatternGraphNode*> GetChildren()
	{
		return Children;
	}

	std::vector<PatternGraphNode*> GetParents()
	{
		return Parents;
	}

	unsigned GetHash()
	{
		return Hash;
	}

	std::string GetFnName()
	{
		return FnName;
	}

	static bool classof(const PatternGraphNode* Node)
	{
		return Node->GetKind() == PatternGraphNode::GNK_FnCall;
	}

private:
	std::string FnName;
	unsigned Hash;
	// we need only one Parents to trace down the reletion chip of the patterns through different Functions

	std::vector<PatternCodeRegion*> PatternParents;
	std::vector<PatternCodeRegion*> PatternChildren;

	std::vector<PatternGraphNode*> Children;
	std::vector<PatternGraphNode*> Parents;
};



class PatternGraph
{
public:
	PatternGraphNode* GetRootNode();
	std::vector<PatternGraphNode*> GetOnlyPatternRootNodes();
	/* Access to patterns */
	bool RegisterPattern(HPCParallelPattern* Pattern);

	HPCParallelPattern* GetPattern(DesignSpace DesignSp, std::string Name);

	/**
	 * @brief
	 *
	 * @return All patterns registered in the graph.
	 **/
	std::vector<HPCParallelPattern*> GetAllPatterns() { return Patterns; }


	/* Access to pattern occurrences */
	bool RegisterPatternOccurrence(PatternOccurrence* PatternOcc);

	PatternOccurrence* GetPatternOccurrence(std::string ID);

	/**
	 * @brief
	 *
	 * @return All pattern occurrences registered in the graph.
	 **/
	std::vector<PatternOccurrence*> GetAllPatternOccurrence() { return PatternOccurrences; }

	std::vector<PatternCodeRegion*> GetAllPatternCodeRegions();

	/* Access to functions */
	bool RegisterFunction(clang::FunctionDecl* Decl);

	FunctionNode* GetFunctionNode(clang::FunctionDecl* Decl);

	FunctionNode* GetFunctionNode(std::string Name);

	void RegisterOnlyPatternRootNode(PatternCodeRegion* CodeReg);

	/**
	 * @brief
	 *
	 * @return All functions registered in the graph.
	 **/
	std::vector<FunctionNode*> GetAllFunctions() { return Functions; }

	/**
	 * @brief Get the instance of the PatternGraph
	 *
	 * @return PatternGraph instance
	 **/
	static PatternGraph* GetInstance()
	{
		static PatternGraph Graph;
		return &Graph;
	}

private:
	/* Save patterns, patternoccurrences and functions for later requests and linear access. */
	std::vector<HPCParallelPattern*> Patterns;
	std::vector<PatternOccurrence*> PatternOccurrences;

	std::vector<FunctionNode*> Functions;

	/* Designated root node for output in "Treeifyed" display */
	PatternGraphNode* RootNode;
	/* When using the OnlyPattern flag we can have multiple rootPatterns*/
	std::vector<PatternGraphNode*> OnlyPatternRootNodes;

	/* Prevent object creation */
	PatternGraph();
	PatternGraph(const PatternGraph&);
	PatternGraph& operator = (const PatternGraph&);
};
