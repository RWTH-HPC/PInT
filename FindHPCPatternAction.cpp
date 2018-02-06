#include "FindHPCPatternAction.h"


std::unique_ptr<clang::ASTConsumer> FindHPCPatternAction::CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile)
{
	std::cout << "Creating consumer object!" << std::endl;
	
	return std::unique_ptr<clang::ASTConsumer>(new FindHPCPatternConsumer(&Compiler.getASTContext()));	
}
