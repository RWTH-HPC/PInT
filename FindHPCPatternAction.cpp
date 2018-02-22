#include "FindHPCPatternAction.h"

#include "Debug.h"

#include <iostream>
#include "FindHPCPatternConsumer.h"



std::unique_ptr<clang::ASTConsumer> FindHPCPatternAction::CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile)
{
	DEBUG_MESSAGE("Creating consumer object!")
	
	return std::unique_ptr<clang::ASTConsumer>(new FindHPCPatternConsumer(&Compiler.getASTContext()));	
}
