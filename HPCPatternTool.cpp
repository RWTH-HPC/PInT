#include <iostream>
#include "clang/Tooling/Tooling.h"
#include "FindHPCPatternAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

/*! \brief Tool entry point.
 *
 * The tool's entry point which calls the FrontEndAction on the code
 */
int main (int argc, char** argv)
{
	std::cout << "HPC Pattern Tool" << std::endl;

	/* TODO
	 * Parse Command Line Options
	 * Create ClangTool
	 * Define Tool Category
	 */

	/* TODO add argument check */ 
	if (argc > 1)
	{
		clang::tooling::runToolOnCode(new FindHPCPatternAction, argv[1]);
	}
}
