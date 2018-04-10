#include "HPCPatternInstrHandler.h"
#include "HPCParallelPattern.h"

#include <iostream>

void HPCPatternBeginInstrHandler::run(const clang::ast_matchers::MatchFinder::MatchResult &Result)
{
	const clang::StringLiteral* patternstr = Result.Nodes.getNodeAs<clang::StringLiteral>("patternstr");	
	std::cout << patternstr->getString().str() << std::endl;

	HPCParallelPattern pattern(patternstr->getString().str());
#if PRINT_DEBUG
	pattern.Print();	
#endif
}

void HPCPatternEndInstrHandler::run(const clang::ast_matchers::MatchFinder::MatchResult &Result)
{
	const clang::StringLiteral* patternstr = Result.Nodes.getNodeAs<clang::StringLiteral>("patternstr");	
	std::cout << patternstr->getString().str() << std::endl;

	RemoveFromPatternStack(patternstr->getString().str());
}
