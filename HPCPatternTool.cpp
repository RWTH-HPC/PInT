#include <iostream>
#include "clang/Tooling/Tooling.h"
#include "FindHPCPatternAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"
#include "clang/Tooling/ArgumentsAdjusters.h"


static llvm::cl::OptionCategory HPCPatternToolCategory("HPC pattern tool options");

static llvm::cl::extrahelp CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);

/*! \brief Tool entry point.
 *
 * The tool's entry point which calls the FrontEndAction on the code
 */
int main (int argc, const char** argv)
{
	clang::tooling::CommonOptionsParser OptsParser(argc, argv, HPCPatternToolCategory);

	clang::tooling::ClangTool HPCPatternTool(OptsParser.getCompilations(), OptsParser.getSourcePathList());
	
 	/* Construct the ArgumentAdjuster that includes the resource dir needed for compilation to the compiler frontend call´*/
	clang::tooling::ArgumentsAdjuster ClangInclAdj;
	clang::tooling::CommandLineArguments InclArgs;
	InclArgs.push_back("-fparse-all-comments");
	InclArgs.push_back("-resource-dir");
	InclArgs.push_back(CLANG_INCLUDE_DIR);
	ClangInclAdj = clang::tooling::getInsertArgumentAdjuster(InclArgs, clang::tooling::ArgumentInsertPosition::END); 

	/* Add the arguments adjuster to the adjuster chain */
	HPCPatternTool.appendArgumentsAdjuster(ClangInclAdj);

	return HPCPatternTool.run(clang::tooling::newFrontendActionFactory<FindHPCPatternAction>().get());
}
