#include <iostream>
#include "clang/Tooling/Tooling.h"
#include "FindHPCPatternAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"


static llvm::cl::OptionCategory HPCPatternToolCategory("HPC pattern tool options");

static llvm::cl::extrahelp CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);

/*! \brief Tool entry point.
 *
 * The tool's entry point which calls the FrontEndAction on the code
 */
int main (int argc, const char** argv)
{
	std::cout << "HPC Pattern Tool" << std::endl;

	clang::tooling::CommonOptionsParser OptionsParser(argc, argv, HPCPatternToolCategory);

	clang::tooling::ClangTool HPCPatternTool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
	return HPCPatternTool.run(clang::tooling::newFrontendActionFactory<FindHPCPatternAction>().get());
}
