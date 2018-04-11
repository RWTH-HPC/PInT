#include "HPCPatternInstrHandler.h"
#include "HPCParallelPattern.h"

#include <iostream>



void HPCPatternBeginInstrHandler::SetCurrentFnEntry(FunctionDeclDatabaseEntry* FnEntry) 
{
	CurrentFnEntry = FnEntry;
}

void HPCPatternBeginInstrHandler::run(const clang::ast_matchers::MatchFinder::MatchResult &Result)
{
	const clang::StringLiteral* patternstr = Result.Nodes.getNodeAs<clang::StringLiteral>("patternstr");	
	std::cout << patternstr->getString().str() << std::endl;

	HPCParallelPattern* Pattern = new HPCParallelPattern(patternstr->getString().str());
	CurrentFnEntry->Patterns.push_back(Pattern);

#if PRINT_DEBUG
	Pattern->Print();	
#endif
}



void HPCPatternEndInstrHandler::run(const clang::ast_matchers::MatchFinder::MatchResult &Result)
{
	const clang::StringLiteral* patternstr = Result.Nodes.getNodeAs<clang::StringLiteral>("patternstr");	
	std::cout << patternstr->getString().str() << std::endl;

	RemoveFromPatternStack(patternstr->getString().str());
}

void HPCPatternEndInstrHandler::SetCurrentFnEntry(FunctionDeclDatabaseEntry* FnEntry) 
{
	CurrentFnEntry = FnEntry;
}
