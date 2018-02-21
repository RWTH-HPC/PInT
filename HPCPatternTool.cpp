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
	clang::tooling::CommonOptionsParser OptionsParser(argc, argv, HPCPatternToolCategory);

	clang::tooling::ClangTool HPCPatternTool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
	
	/* Construct the ArgumentAdjuster that includes the resource dir needed for compilation to the compiler frontend call */
	clang::tooling::ArgumentsAdjuster ClangIncludeAdjuster;
	clang::tooling::CommandLineArguments IncludeArguments;
	IncludeArguments.push_back("-resource-dir");
	IncludeArguments.push_back(CLANG_INCLUDE_DIR);
	ClangIncludeAdjuster = clang::tooling::getInsertArgumentAdjuster(IncludeArguments, clang::tooling::ArgumentInsertPosition::END); 

	HPCPatternTool.appendArgumentsAdjuster(ClangIncludeAdjuster);

	return HPCPatternTool.run(clang::tooling::newFrontendActionFactory<FindHPCPatternAction>().get());
}
