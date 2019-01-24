#include "HPCPatternInstrASTTraversal.h"
#include "Debug.h"
#include "HPCParallelPattern.h"

#include <iostream>
#include "clang/AST/RawCommentList.h"
#include "llvm/ADT/StringRef.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/SourceLocation.h"
#include <string>



/**
 * @brief If a function declaration is encountered, look up the corresponding database entry.
 * We set helper variables in the PatternBeginHandler and PatternEndHandler to the FunctionNode for correct parent-child-relations.
 *
 * @param Decl The clang object encountered by the visitor.
 *
 * @return Always true to signal that the tree traversal should be continued.
 **/


bool HPCPatternInstrVisitor::VisitFunctionDecl(clang::FunctionDecl *Decl)
{
	CurrentFn = Decl;

	if ((CurrentFnEntry = PatternGraph::GetInstance()->GetFunctionNode(Decl)) == NULL)
	{
		PatternGraph::GetInstance()->RegisterFunction(Decl);
		CurrentFnEntry = PatternGraph::GetInstance()->GetFunctionNode(Decl);
	}

#ifdef PRINT_DEBUG
	std::cout << CurrentFnEntry->GetFnName() << " (" << CurrentFnEntry->GetHash() << ")" << std::endl;
#endif
	PatternBeginHandler.SetCurrentFnEntry(CurrentFnEntry);
	PatternEndHandler.SetCurrentFnEntry(CurrentFnEntry);

	return true;
}


/**
 * @brief When we encounter a call expression, we look up the declaration of the function called.
 * If it is one of our instrumentation functions, we extract the information from the string argument with ASTMatchers.
 * A PatternCodeRegion object is created if this is the start of a region or the current region is closed.
 * For a non-instrumentation function, the function is added to top-most pattern as a child.
 * If the pattern stack is empty, the function is a direct child of the calling function.
 *
 * @param CallExpr The clang object containing information about the call expression.
 *
 * @return True to signal continuing the traversal.
 **/
bool HPCPatternInstrVisitor::VisitCallExpr(clang::CallExpr *CallExpr)
{
	if (!CallExpr->getBuiltinCallee() && CallExpr->getDirectCallee() && !CallExpr->getDirectCallee()->isInStdNamespace())
	{
		clang::FunctionDecl* Callee = CallExpr->getDirectCallee();

#ifdef PRINT_DEBUG
		std::cout << Callee->getNameInfo().getName().getAsString() << std::endl;
#endif

		std::string FnName = Callee->getNameInfo().getName().getAsString();

		// Is this a call to our pattern functions?
		if (!FnName.compare(PATTERN_BEGIN_CXX_FNNAME) || !FnName.compare(PATTERN_BEGIN_C_FNNAME))
		{
			/*Delivers the children of the current node*/
			clang::Expr** Args = CallExpr->getArgs();
#ifdef PRINT_DEBUG
			Args[0]->dump();
#endif
			/*calls all registered callbacks on all matches on the given node */
			PatternBeginFinder.match(*Args[0], *Context);
			PatternCodeRegion* PatternCodeReg = PatternBeginHandler.GetLastPattern();

			/* Get the location of the fn call which denotes the beginning of this pattern */
			clang::SourceManager& SourceMan = Context->getSourceManager();
			clang::SourceLocation LocStart = CallExpr->getLocStart();
			//std::cout << "Sourcelocation vom begindes Patterns"<<LocStart.printToString(SourceMan);

			clang::FullSourceLoc SourceLoc(LocStart, SourceMan);
			PatternCodeReg->SetFirstLine(SourceLoc.getLineNumber());
			PatternCodeReg->SetStartSourceLoc(LocStart);

		}
		else if (!FnName.compare(PATTERN_END_CXX_FNNAME) || !FnName.compare(PATTERN_END_C_FNNAME))
		{
			clang::Expr** Args = CallExpr->getArgs();
#ifdef PRINT_DEBUG
			Args[0]->dump();
#endif
			PatternEndFinder.match(*Args[0], *Context);
			PatternCodeRegion* PatternCodeReg = PatternEndHandler.GetLastPattern();


			/* Get the location of the fn call which denotes the end of this pattern */
			clang::SourceManager& SourceMan = Context->getSourceManager();
			clang::SourceLocation LocEnd = CallExpr->getLocEnd();
			clang::FullSourceLoc SourceLoc(LocEnd, SourceMan);
			PatternCodeReg->SetLastLine(SourceLoc.getLineNumber());
			PatternCodeReg->SetEndSourceLoc(LocEnd);
		}
		// If no: search the called function for patterns
		else
		{
			/* Look up the database entry for this function */
			FunctionNode* Func;

			if ((Func = PatternGraph::GetInstance()->GetFunctionNode(Callee)) == NULL)
			{
				PatternGraph::GetInstance()->RegisterFunction(Callee);
				Func = PatternGraph::GetInstance()->GetFunctionNode(Callee);
			}

#ifdef PRINT_DEBUG
			std::cout << Func->GetFnName() << " (" << Func->GetHash() << ")" << std::endl;
#endif
			PatternCodeRegion* Top;
			if ((Top = GetTopPatternStack()) != NULL)
			{
				Top->AddChild(Func);
				Func->AddParent(Top);
			}
			else
			{
				CurrentFnEntry->AddChild(Func);
				Func->AddParent(CurrentFnEntry);
			}
		}
	}

	return true;
}

HPCPatternInstrVisitor::HPCPatternInstrVisitor (clang::ASTContext* Context) : Context(Context)
{
	using namespace clang::ast_matchers;
	StatementMatcher StringArgumentMatcher = hasDescendant(stringLiteral().bind("patternstr"));

	PatternBeginFinder.addMatcher(StringArgumentMatcher, &PatternBeginHandler);
	PatternEndFinder.addMatcher(StringArgumentMatcher, &PatternEndHandler);
}

Halstead* currentHlst;

HalsteadVisitor::HalsteadVisitor(clang::ASTContext *Context) : Context(Context)
{
	actHalstead = getActualHalstead();
}

 /* Consumer function implementations
 */
void HPCPatternInstrConsumer::HandleTranslationUnit(clang::ASTContext &Context)
{
	/* Traverse the AST for comments and parse them */
	DEBUG_MESSAGE("Using Visitor to traverse from top translation declaration unit");
	Visitor.TraverseDecl(Context.getTranslationUnitDecl());
	//	EsthersVisitor.TraverseDecl(Context.getTranslationUnitDecl());
}


bool HalsteadVisitor::VisitBinaryOperator(clang::BinaryOperator *BinarOp){
	std::vector<HPCParallelPattern*> isInPatterns;
	IsStmtInAPatt(BinarOp, &isInPatterns);
 	//std::cout << "groesse isInPatterns: "<<isInPatterns.size() << '\n';
	if(isInPatterns.empty()){
			return true;
	}
	else{
		for(HPCParallelPattern* Pat : isInPatterns){
			Pat->incrementNumOfOperators();
		}
	}
	return true;
}

bool HalsteadVisitor::VisitCXXOperatorCallExpr(clang::CXXOperatorCallExpr *CXXOperatorCallExpr){

	std::vector<HPCParallelPattern*> isInPatterns;
	IsStmtInAPatt(CXXOperatorCallExpr, &isInPatterns);

	if(isInPatterns.empty()){
			return true;
	}
	else{
		for(HPCParallelPattern* Pat : isInPatterns){
			Pat->incrementNumOfOperators();
		}
	}
	return true;
}

bool HalsteadVisitor::VisitUnaryOperator(clang::UnaryOperator *UnaryOp){

	std::vector<HPCParallelPattern*> isInPatterns;
	IsStmtInAPatt(UnaryOp, &isInPatterns);

	if(isInPatterns.empty()){
			return true;
	}
	else{
		for(HPCParallelPattern* Pat : isInPatterns){
			Pat->incrementNumOfOperators();
		}
	}
	return true;
}

bool HalsteadVisitor::VisitDeclStmt(clang::DeclStmt *DeclStmt){

	std::vector<HPCParallelPattern*> isInPatterns;
	IsStmtInAPatt(DeclStmt, &isInPatterns);

	if(isInPatterns.empty()){
			return true;
	}
	else{
		for(HPCParallelPattern* Pat : isInPatterns){
			Pat->incrementNumOfOperators();
		}
	}
	return true;
}

bool HalsteadVisitor::VisitCompoundAssignOperator(clang::CompoundAssignOperator *CompAsOp){

		std::vector<HPCParallelPattern*> isInPatterns;
		IsStmtInAPatt(CompAsOp, &isInPatterns);

		if(isInPatterns.empty()){
				return true;
		}
		else{
			for(HPCParallelPattern* Pat : isInPatterns){
				Pat->incrementNumOfOperators();
			}
		}
		return true;
}

bool HalsteadVisitor::VisitMemberExpr(clang::MemberExpr *MemExpr){
	std::vector<HPCParallelPattern*> isInPatterns;
	IsStmtInAPatt(MemExpr, &isInPatterns);

	if(isInPatterns.empty()){
			return true;
	}
	else{
		for(HPCParallelPattern* Pat : isInPatterns){
			Pat->incrementNumOfOperators();
		}
	}
	return true;
}

bool HalsteadVisitor::VisitStringLiteral(clang::StringLiteral *StrgLit){
	std::vector<HPCParallelPattern*> isInPatterns;
	IsStmtInAPatt(StrgLit, &isInPatterns);

	if(isInPatterns.empty()){
			return true;
	}
	else{
		for(HPCParallelPattern* Pat : isInPatterns){
			Pat->incrementNumOfOperators();
		}
	}
	return true;
}
bool HalsteadVisitor::VisitCharacterLiteral(clang::CharacterLiteral *CharLit){
	std::vector<HPCParallelPattern*> isInPatterns;
	IsStmtInAPatt(CharLit, &isInPatterns);

	if(isInPatterns.empty()){
			return true;
	}
	else{
		for(HPCParallelPattern* Pat : isInPatterns){
			Pat->incrementNumOfOperators();
		}
	}
	return true;
}
/*bisher werden nur die TypeQualifiers gezählt. will man noch einmal die Decl selbst mit zählen muss man plus 1 rechnen*/
bool HalsteadVisitor::VisitVarDecl(clang::VarDecl *VrDcl){
	std::vector<HPCParallelPattern*> isInPatterns;
	IsDeclInAPatt(VrDcl, &isInPatterns);

	if(isInPatterns.empty()){
			return true;
	}
	else{
		int numTypeQual = HalsteadVisitor::countQual(VrDcl);
		/*countQual return the number of TypeQualifiers excluding the default qualifier
		  If numTypeQual = 0 the for loop should be computed once if numTypeQual = n the
			loop shoulb be enteren n+1 times */
		for(HPCParallelPattern* Pat : isInPatterns){
			for (size_t i = 0; i <= numTypeQual; i++) {
					Pat->incrementNumOfOperators();
			}
		}
	}
	return true;
}
	void HalsteadVisitor::IsStmtInAPatt(clang::Stmt *Stm, std::vector<HPCParallelPattern*> *isInPatterns){

	std::vector<PatternOccurrence*> WorkOccStackForHalstead(OccStackForHalstead.begin(), OccStackForHalstead.end()) ;
	// WorkOccStackForHalstead is correctly initialized
	clang::SourceManager& SourceMan = Context->getSourceManager();

	for (int i = 0; i < WorkOccStackForHalstead.size(); i++){

		PatternOccurrence* PatOcc = WorkOccStackForHalstead[i];
		std::vector<PatternCodeRegion*> CodeRegions = PatOcc->GetCodeRegions();
		getActualHalstead()->insertPattern(PatOcc->GetPattern());

		for(int i = 0; i < CodeRegions.size(); i++){

			PatternCodeRegion* CodeReg = CodeRegions[i];
			bool ExprAfterBegStmt = SourceMan.isPointWithin (Stm->getLocEnd(),CodeReg->GetStartLoc(), CodeReg->GetEndLoc());
			//std::cout << "wer von ExprAfterBegStmt: " <<ExprAfterBegStmt<< '\n';
			//bool ExprBeforEndStmt = SourceMan.isBeforeInTranslationUnit(DeclStmt->getEndLoc(), CodeReg->GetEndLoc());

			//if(ExprAfterBegStmt && ExprBeforEndStmt){
			if(ExprAfterBegStmt){

				isInPatterns->push_back(PatOcc->GetPattern());

			}
		}
	}
}

	void HalsteadVisitor::IsDeclInAPatt(clang::Decl *Dcl, std::vector<HPCParallelPattern*> *isInPatterns){

			std::vector<PatternOccurrence*> WorkOccStackForHalstead(OccStackForHalstead.begin(), OccStackForHalstead.end()) ;

			clang::SourceManager& SourceMan = Context->getSourceManager();

			for (int i = 0; i < WorkOccStackForHalstead.size(); i++){

				PatternOccurrence* PatOcc = WorkOccStackForHalstead[i];
				std::vector<PatternCodeRegion*> CodeRegions = PatOcc->GetCodeRegions();
				getActualHalstead()->insertPattern(PatOcc->GetPattern());

				for(int i = 0; i < CodeRegions.size(); i++){

					PatternCodeRegion* CodeReg = CodeRegions[i];
					bool ExprAfterBegStmt = SourceMan.isPointWithin (Dcl->getLocation(),CodeReg->GetStartLoc(), CodeReg->GetEndLoc());

					if(ExprAfterBegStmt){
						isInPatterns->push_back(PatOcc->GetPattern());
					}
				}
			}
		}

	int HalsteadVisitor::countQual(clang::VarDecl* VDecl){
			clang::QualType type = VDecl->getType();
			std::string typeStr = type.getAsString();
			int numWords = 0;
			for(int i = 0; i < (int)typeStr.length(); i++){
				if(isspace(typeStr[i])) numWords ++;
			}
			return numWords;
	}
/*
 * Frontend action function implementations
 */
std::unique_ptr<clang::ASTConsumer> HPCPatternInstrAction::CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile)
{
	DEBUG_MESSAGE("Creating consumer object!")
	return std::unique_ptr<clang::ASTConsumer>(new HPCPatternInstrConsumer(&Compiler.getASTContext()));
}

std::unique_ptr<clang::ASTConsumer> HalsteadClassAction::CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile)
{
	return std::unique_ptr<clang::ASTConsumer>(new HalsteadConsumer(&Compiler.getASTContext()));
}
