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
	
	HPCParallelPattern* Pattern = new HPCParallelPattern(patternstr->getString().str());
	HPCParallelPattern* Top = GetTopPatternStack();
	
	/* Connect the child and parent links between the objects */
	if (Top != NULL)
	{
		Top->AddChild(Pattern);
		Pattern->AddParent(Top);
	}
	else
	{	
		CurrentFnEntry->AddChild(Pattern);
		Pattern->AddParent(CurrentFnEntry);
	}

	AddToPatternStack(Pattern);

#if PRINT_DEBUG
	Pattern->Print();	
#endif
}



void HPCPatternEndInstrHandler::run(const clang::ast_matchers::MatchFinder::MatchResult &Result)
{
	const clang::StringLiteral* patternstr = Result.Nodes.getNodeAs<clang::StringLiteral>("patternstr");	

	RemoveFromPatternStack(patternstr->getString().str());
}

void HPCPatternEndInstrHandler::SetCurrentFnEntry(FunctionDeclDatabaseEntry* FnEntry) 
{
	CurrentFnEntry = FnEntry;
}
