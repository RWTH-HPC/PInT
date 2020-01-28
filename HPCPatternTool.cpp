
#include "HPCPatternInstrASTTraversal.h"
#include "TreeVisualisation.h"
#include "HPCPatternStatistics.h"
#include "HPCParallelPattern.h"
#include "Helpers.h"
#include "SimilarityMetrics.h"

#include "ToolInformation.h"
#ifndef HPCRUNNINGSTATS_H
  #include "HPCRunningStats.h"
#endif
//#include "HPCRunningStats.h"

#include <iostream>
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"
#include "clang/Tooling/ArgumentsAdjusters.h"

#ifndef HPCERROR_H
#include "HPCError.h"
#endif

//#define DEBUG
int  MAX_DEPTH = 8;

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

static llvm::cl::OptionCategory onlyPattern("Patterntree without function calls");
static llvm::cl::extrahelp Help("-onlyPattern Use this flag, if you want to see the Patterntree without function calls\n \n");
static llvm::cl::opt<bool> OnlyPatterns("onlyPattern", llvm::cl::cat(onlyPattern));

static llvm::cl::OptionCategory noTree("Output without the relation tree");
static llvm::cl::extrahelp HelpNoTree("-noTree Use this flag, if you don't want to see tree\n \n");
static llvm::cl::opt<bool> NoTree("noTree", llvm::cl::cat(noTree));

static llvm::cl::OptionCategory useSpecFiles("Only traverse certain Files");
static llvm::cl::extrahelp HelpUseSpecFiles("-useSpecFiles Use this Flag, if you want to traverse certain files instead of a all Files within the compilation data base.");
static llvm::cl::opt<bool> UseSpecFiles("useSpecFiles", llvm::cl::cat(useSpecFiles));

static llvm::cl::OptionCategory maxTreeDisplayDepth("Sets maximal depth to display the tree");
static llvm::cl::extrahelp HelpMaxTreeDisplayDepth("This only changes the depth with which the tree is displayed. The other statistics are still using the whole tree.");
static llvm::cl::opt<unsigned int> MaxTreeDisplayDepth("maxTreeDisplayDepth", llvm::cl::cat(maxTreeDisplayDepth));

static llvm::cl::OptionCategory displayCompilationsList("Displays every File in the compilation database");
static llvm::cl::extrahelp HelpDisplayCompilationsList("Use this option to be shure that every file which you want to analyze is in the compilation database. If not ur file is not analyzed by the tool and you should add this file in your compile_commands.json file");
static llvm::cl::opt<bool> DisplayCompilationsList("displayCompilationsList", llvm::cl::cat(displayCompilationsList));

static llvm::cl::OptionCategory pintVersion("Shows the currently used version of the tool");
static llvm::cl::extrahelp HelpPintVersion("Use this option if you want to know which version you builded");
static llvm::cl::opt<bool> PintVersion("pintVersion", llvm::cl::cat(pintVersion));

static llvm::cl::OptionCategory relationTree("Output the relation tree");
static llvm::cl::extrahelp HelpRelationTree("-relationTree Use this flag, if you want to see the relation tree\n \n");
static llvm::cl::opt<bool> RelationTree("relationTree", llvm::cl::cat(noTree));

Halstead* actHalstead = new Halstead();

static HPCPatternStatistic* Statistics[] = { new SimplePatternCountStatistic(), new FanInFanOutStatistic(20), new LinesOfCodeStatistic(), new CyclomaticComplexityStatistic(), actHalstead };

/**
 * @brief Tool entry point. The tool's entry point which calls the FrontEndAction on the code.
 * Register statistics and similarity measures here.
 */

int main (int argc, const char** argv)
{
  setCommandArguments(OnlyPatterns.getValue(), NoTree.getValue(), UseSpecFiles.getValue(), MaxTreeDisplayDepth.getValue(), DisplayCompilationsList.getValue(), PintVersion.getValue(), RelationTree.getValue());
	MaxTreeDisplayDepth.setInitialValue(MAX_DEPTH);

		clang::tooling::CommonOptionsParser OptsParserVersion(argc, argv, pintVersion);

	if(PintVersion.getValue()){
			std::cout << "You are currently using the Version: " << PInTVersion <<'\n';
	}
	else{
		clang::tooling::CommonOptionsParser OptsParser(argc, argv, HPCPatternToolCategory);
		std::vector<std::string> analyseList;
		if(UseSpecFiles.getValue()){
			analyseList = OptsParser.getSourcePathList();
			std::cout << "ANALYZE LIST: " << '\n';

			for(int i = 0; i  < analyseList.size(); i++){
				std::cout << analyseList[i] << std::endl;
			}

		}
		else{
			analyseList = (OptsParser.getCompilations()).getAllFiles();
		}

		clang::tooling::ClangTool HPCPatternTool(OptsParser.getCompilations(),analyseList);
		if(DisplayCompilationsList.getValue()){
			std::cout << "COMPILATIONS LIST: "<<std::endl;
			std::vector<std::string> d = (OptsParser.getCompilations()).getAllFiles();

			for(int i = 0; i< d.size(); i++){
				std::cout << d[i] << std::endl;
			}

		}

		/* Declare vector of command line arguments */
		clang::tooling::CommandLineArguments Arguments;

		/* Add Arguments to prevent inlining */
		Arguments.push_back("-fno-inline");

		/* Add arguments to include system headers */
		Arguments.push_back("-resource-dir");
		Arguments.push_back(CLANG_INCLUDE_DIR);


		clang::tooling::ArgumentsAdjuster ArgsAdjuster = clang::tooling::getInsertArgumentAdjuster(Arguments, clang::tooling::ArgumentInsertPosition::END);
		HPCPatternTool.appendArgumentsAdjuster(ArgsAdjuster);
		setActualHalstead(actHalstead);

		/* Run the tool with options and source files provided */
		int retcode = 0;
		try{
			retcode = HPCPatternTool.run(clang::tooling::newFrontendActionFactory<HPCPatternInstrAction>().get());

      #ifdef DEBUG
        std::cout << "\nPrinting out DeclarationVector: " << std::endl;
        for(CallTreeNode* Node : *ClTre->GetDeclarationVector())
        {
          std::cout << *Node->GetID() << " " << Node->GetNodeType()<< std::endl;
          for(auto CalleeEntry = (Node->GetCallees())->begin() ; CalleeEntry != (Node->GetCallees())->end();){
            std::cout << "--> " << *(CalleeEntry->second)->GetID() << " " << (CalleeEntry->second)->GetNodeType()<< std::endl;
            CalleeEntry++;
          }
        }
      #endif
      if(!NoTree.getValue()){
        ClTre->appendAllDeclToCallTree(ClTre->getRoot(), MAX_DEPTH);
        ClTre->setUpTree();
      }
		}
		catch(std::exception& terminate){
			std::cout << terminate.what();
      return 0;
		}
    try{
      ClTre->lookIfTreeIsCorrect();
    }
    catch(TooManyBeginsException& begins){
      begins.what();
      return 0;
    }
		//int halstead = HPCPatternTool.run(clang::tooling::newFrontendActionFactory<HalsteadClassAction>().get());
	  if(!NoTree.getValue()){
			int mxdspldpth = MaxTreeDisplayDepth.getValue();
      if(RelationTree.getValue())
      {
		    CallTreeVisualisation::PrintRelationTree(mxdspldpth, OnlyPatterns.getValue());
        CallTreeVisualisation::PrintCallTree(mxdspldpth, ClTre, OnlyPatterns.getValue());
      }
      else
        CallTreeVisualisation::PrintCallTree(mxdspldpth, ClTre, OnlyPatterns.getValue());
	  }

		for (HPCPatternStatistic* Stat : Statistics)
		{
			std::cout << std::endl << std::endl;
			Stat->Calculate();
			Stat->Print();
		}

		Statistics[0]->CSVExport("Counts.csv");
		Statistics[1]->CSVExport("FIFO.csv");
		Statistics[2]->CSVExport("LOC.csv");


		/* Similarity Measures
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
	*/
		return retcode; //&& halstead;
	}
	return 1;
}
