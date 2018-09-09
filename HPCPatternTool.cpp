#include "HPCPatternInstrASTTraversal.h"
#include "TreeVisualisation.h"
#include "HPCPatternStatistics.h"
#include "HPCParallelPattern.h"
#include "Helpers.h"
#include "SimilarityMetrics.h"

#include <iostream>
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"
#include "clang/Tooling/ArgumentsAdjusters.h"



/** 
 * @mainpage Clang Pattern Instrumentation Tool
 *
 * @section intro_sec Introduction
 *
 * This a a clang-based tool for automatic evaluation of instrumented codes containing parallel patterns.
 * For a tutorial on how to use the tool, please consider: https://git.rwth-aachen.de/swienke/patternInstrumentation
 * 
 * @section source_sec About the source
 *
 * The source code can mainly be divided into two parts.
 *  -# The detection of instrumentation calls, program and pattern structure in the source code using the clang libTooling library.
 *  -# Further processing of the extracted information, i.e. inferring statistics about the patterns or identifying most common nested patterns.
 *
 * If you want to improve the tool or if you're just interested in how the interaction with clang works, take a look at the HPCPatternInstrVisitor.
 *
 * To implement your own statistics or similarity measures, take a look at HPCPatternStatistic or SimilarityMeasure interfaces. Examples are given by the actual statistic implementations.
 * 
 * Our internal data structure hierarchy for patterns looks as this:
 * -# HPCParallelPattern is the class that represents actual patterns. A pattern is like a template that can occur anywhere in the code but it has no concrete location in the source code. It is more a theoretical construct.
 * -# One instance of a pattern, i.e. an occurrence of a pattern is represented by the PatternOccurrence class. This pattern occurrence still does not match to a specific source location. 
 *  A pattern occurrence can rather consist of many PatternCodeRegion with the same identifier.
 * -# The code regions instrumented in the code are represnted by PatternCodeRegion objects in our tool. Every code region belongs to an occurrence which 'has' a pattern.
 * 
 * Another central class is the PatternGraph singleton class, which holds structual information about the patterns extracted from the analysed sourcecode. It provides access to lists of all patterns, all occurrences and all code regions. Further, it holds a reference to a designated root node for tree or analysis purposes.
 * 
 * Own statistics or similarity measures can easily be implemented. Statistics should inherit from HPCPatternStatistic and similarity measures from SimilarityMeasure. The statistic classes can then be registered in the tool's main function, where they are initialised and the calculations are executed. 
 * 
 * When implementing statistics or sim. measures, the helper functions provided in the SetAlgorithm, GraphAlgorithm or PatternHelper namespaces might be convenient. Further helper functions will be added in the future, whenever it appears feasible.
 */



static llvm::cl::OptionCategory HPCPatternToolCategory("HPC pattern tool options");

static llvm::cl::extrahelp CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);

static HPCPatternStatistic* Statistics[] = { new SimplePatternCountStatistic(), new FanInFanOutStatistic(10), new LinesOfCodeStatistic(), new CyclomaticComplexityStatistic() };

/** 
 * @brief Tool entry point. The tool's entry point which calls the FrontEndAction on the code.
 * Register statistics and similarity measures here.
 */
int main (int argc, const char** argv)
{
	clang::tooling::CommonOptionsParser OptsParser(argc, argv, HPCPatternToolCategory);
	clang::tooling::ClangTool HPCPatternTool(OptsParser.getCompilations(), OptsParser.getSourcePathList());

	/* Declare vector of command line arguments */
	clang::tooling::CommandLineArguments Arguments;

	/* Add Arguments to prevent inlining */
	Arguments.push_back("-fno-inline");
	
	/* Add arguments to include system headers */
	Arguments.push_back("-resource-dir");
	Arguments.push_back(CLANG_INCLUDE_DIR);

	
	clang::tooling::ArgumentsAdjuster ArgsAdjuster = clang::tooling::getInsertArgumentAdjuster(Arguments, clang::tooling::ArgumentInsertPosition::END);
	HPCPatternTool.appendArgumentsAdjuster(ArgsAdjuster);	

	/* Run the tool with options and source files provided */
	int retcode = HPCPatternTool.run(clang::tooling::newFrontendActionFactory<HPCPatternInstrAction>().get());
	//CallTreeVisualisation::PrintCallTree(10);

	for (HPCPatternStatistic* Stat : Statistics)
	{
		std::cout << std::endl << std::endl;
		Stat->Calculate();
		Stat->Print();
	}

	Statistics[0]->CSVExport("Counts.csv");
	Statistics[1]->CSVExport("FIFO.csv");
	Statistics[2]->CSVExport("LOC.csv");

	/* Similarity Measures */
	std::vector<HPCParallelPattern*> SimPatterns;

	HPCParallelPattern* IMVI = PatternGraph::GetInstance()->GetPattern(DesignSpace::ImplementationMechanism, "VariableIncrement");
	HPCParallelPattern* FCGT = PatternGraph::GetInstance()->GetPattern(DesignSpace::FindingConcurrency, "GroupTask");
	HPCParallelPattern* IMCO = PatternGraph::GetInstance()->GetPattern(DesignSpace::ImplementationMechanism, "Communication");
	HPCParallelPattern* IMSY = PatternGraph::GetInstance()->GetPattern(DesignSpace::ImplementationMechanism, "Synchronization");	
	
	SimPatterns.push_back(IMCO);
	SimPatterns.push_back(IMSY);

	JaccardSimilarityStatistic Jaccard(SimPatterns, 2, 4, GraphSearchDirection::DIR_Parents, SimilarityCriterion::Pattern, 1000);
	
	Jaccard.Calculate();
	Jaccard.Print();

	return retcode;
}
