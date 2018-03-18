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

	/* Declare vector of command line arguments */
	clang::tooling::CommandLineArguments Arguments;

	/* Add Arguments for parsing of all comments */
	Arguments.push_back("-fparse-all-comments");
	Arguments.push_back("-fms-extensions");

	/* Add Arguments to prevent inlining */
	Arguments.push_back("-fno-inline");
	
	/* Add arguments to include system headers */
	Arguments.push_back("-resource-dir");
	Arguments.push_back(CLANG_INCLUDE_DIR);

	
	clang::tooling::ArgumentsAdjuster ArgsAdjuster = clang::tooling::getInsertArgumentAdjuster(Arguments, clang::tooling::ArgumentInsertPosition::END);
	HPCPatternTool.appendArgumentsAdjuster(ArgsAdjuster);	

	/* Run the tool with options and source files provided */
	return HPCPatternTool.run(clang::tooling::newFrontendActionFactory<FindHPCPatternAction>().get());
}
