#include "HPCPatternHandler.h"
#include <iostream>

void PatternStringHandler::run(const clang::ast_matchers::MatchFinder::MatchResult &Result)
{
	const clang::StringLiteral* patternstr = Result.Nodes.getNodeAs<clang::StringLiteral>("patternstr");	
	std::cout << patternstr->getString().str() << std::endl;
}
