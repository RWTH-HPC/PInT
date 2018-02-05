#include "FindHPCPatternAction.h"
#include <iostream>


std::unique_ptr<clang::ASTConsumer> FindHPCPatternAction::CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile)
{
	std::cout << "Hallo!" << std::endl;	
}
