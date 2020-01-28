#include "HPCPatternInstrASTTraversal.h"
#include "Debug.h"
#include "HPCParallelPattern.h"
#include <string>

#include <exception>

#include <iostream>
#include "clang/AST/RawCommentList.h"
#include "llvm/ADT/StringRef.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/SourceLocation.h"
#include <string>
#include "llvm/ADT/SmallVector.h"
#include "HPCRunningStats.h"

#ifndef HPCERROR_H
#include "HPCError.h"
#endif

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
	clang::SourceManager& SourceMan = Context->getSourceManager();
	if(SourceMan.isInMainFile(Decl->getBeginLoc()))
	{
		CurrentFn = Decl;
		clang::SourceLocation beginLoc = Decl->getBeginLoc();
		clang::SourceManager& SourceMan = Context->getSourceManager();
		clang::FullSourceLoc SourceLoc(beginLoc, SourceMan);
		CallTreeNode* Node;

		if ((CurrentFnEntry = PatternGraph::GetInstance()->GetFunctionNode(Decl)) == NULL)
		{
			PatternGraph::GetInstance()->RegisterFunction(Decl);
			CurrentFnEntry = PatternGraph::GetInstance()->GetFunctionNode(Decl);
		}
		if(Decl->isMain()){
			Node = ClTre->registerNode(Root, CurrentFnEntry, LastNodeType, GetTopPatternStack(), CurrentFnEntry);
			ClTre->setRootNode(Node);
		}
		else
			Node = ClTre->registerNode(Function_Decl, CurrentFnEntry, LastNodeType, GetTopPatternStack(), CurrentFnEntry);

		Node->SetLineNumber(SourceLoc.getLineNumber());
		#ifdef LOCDEBUG
			std::cout << "setted LineNumber of: "<< *Node->GetID()<<" to "<< SourceLoc.getLineNumber()<<" verification: "<<Node->getLineNumber()<< '\n';
		#endif
	#ifdef PRINT_DEBUG
		std::cout << CurrentFnEntry->GetFnName() << " (" << CurrentFnEntry->GetHash() << ")" << std::endl;
	#endif

		PatternBeginHandler.SetCurrentFnEntry(CurrentFnEntry);
		PatternEndHandler.SetCurrentFnEntry(CurrentFnEntry);
	}
	LastNodeType = Function_Decl;
	return true;
}


/**
 * @brief When we encounter a call expression, we look up the declaration of the function called.
 * If it is one of our instrumentation functions, we extract the information from the string argument with ASTMatchers.
 * A PatternCodeRegion object is created if this is the start of a region or the current region is closed.
 * For a non-instrumentation function, the function is added to the pattern which surraunds it as a child.
 * If there is no pattern, the function is a direct child of the calling function.
 *
 * @param CallExpr The clang object containing information about the call expression.
 *
 * @return True to signal continuing the traversal.
 **/
bool HPCPatternInstrVisitor::VisitCallExpr(clang::CallExpr *CallExpr)
{
	clang::SourceManager& SourceMan = Context->getSourceManager();

	/*If Clause is used to make shure that only the code in compile_commands.json is traversed
	  and no used libraries*/
	if(SourceMan.isInMainFile(CallExpr->getBeginLoc()))
	{
		if (!CallExpr->getBuiltinCallee() && CallExpr->getDirectCallee() && !CallExpr->getDirectCallee()->isInStdNamespace())
		{
			clang::FunctionDecl* Callee = CallExpr->getDirectCallee();

	#ifdef PRINT_DEBUG
			std::cout << Callee->getNameInfo().getName().getAsString() << std::endl;
	#endif

			std::string FnName = Callee->getNameInfo().getName().getAsString();

			// If the CallExpr is a pattern-begin expression
			if (!FnName.compare(PATTERN_BEGIN_CXX_FNNAME) || !FnName.compare(PATTERN_BEGIN_C_FNNAME))
			{
				/*Delivers the children of the current node*/
				clang::Expr** Args = CallExpr->getArgs();
	#ifdef PRINT_DEBUG
				Args[0]->dump();
	#endif
				/*calls all registered callbacks on all matches on the given node.
				  this call is pretty stong. It creates the patternCodeRegion and if there is no mathing PatternOccurrence it
					is creating one. Also are the Child parent relations set with this call*/
				//PatternBeginFinder is a  clang::ast_matchers::MatchFinder which uses a HPCPatternBeginInstrHandler to set everything up and the Pattern is also registered in the patternStack.
				PatternCodeRegion* PatBeforethisPat = PatternBeginHandler.GetLastPattern();

				PatternBeginFinder.match(*Args[0], *Context);

				PatternCodeRegion* PatternCodeReg = PatternBeginHandler.GetLastPattern();

				/* Store this PatternCodeRegion Begin in the CallTree (ClTre)*/

				CallTreeNode* BeginNode = ClTre->registerNode(Pattern_Begin, PatternCodeReg, LastNodeType, PatBeforethisPat, CurrentFnEntry);


				/* Get the location of the fn call which denotes the beginning of this pattern */

				clang::SourceLocation LocStart = CallExpr->getBeginLoc();

				clang::FullSourceLoc SourceLoc(LocStart, SourceMan);
				BeginNode->SetLineNumber(SourceLoc.getLineNumber());
				#ifdef LOCDEBUG
					std::cout << "setted LineNumber of: "<< *BeginNode->GetID()<<" to "<< SourceLoc.getLineNumber()<<" verification: "<<BeginNode->getLineNumber()<< '\n';
				#endif
				PatternCodeReg->SetFirstLine(SourceLoc.getLineNumber());
				PatternCodeReg->SetStartSourceLoc(LocStart);


				PatternCodeReg->isInMain = SourceMan.isInMainFile(LocStart);

				LastNodeType = Pattern_Begin;
			}
			else if (!FnName.compare(PATTERN_END_CXX_FNNAME) || !FnName.compare(PATTERN_END_C_FNNAME))
			{
				clang::Expr** Args = CallExpr->getArgs();
				#ifdef PRINT_DEBUG
							std::cout << "Degub dump of Args before matching" << '\n';
							Args[0]->dump();
				#endif
				PatternCodeRegion* PatternCodeReg;
				try{
					PatternEndFinder.match(*Args[0], *Context);
					PatternCodeReg = PatternEndHandler.GetLastPattern();
				}
				catch(TooManyEndsException& e){
					e.what();
					throw TerminateEarlyException();
				}
				#ifdef PRINT_DEBUG
				std::cout << "Degub dump of Args after matching" << '\n';
							Args[0]->dump();
				#endif

				/* Get the location of the fn call which denotes the end of this pattern */
				clang::SourceManager& SourceMan = Context->getSourceManager();
				clang::SourceLocation LocEnd = CallExpr->getEndLoc();
				clang::FullSourceLoc SourceLoc(LocEnd, SourceMan);

				CallTreeNode* EndNode = ClTre->registerEndNode(Pattern_End, PatternEndHandler.GetLastPatternID(), LastNodeType, PatternCodeReg, CurrentFnEntry);
				EndNode->SetLineNumber(SourceLoc.getLineNumber());
				#ifdef LOCDEBUG
					std::cout << "setted LineNumber of: "<< *EndNode->GetID()<<" to "<< SourceLoc.getLineNumber()<<" verification: "<<EndNode->getLineNumber()<< '\n';
				#endif
			}
			// If no: search the called function for patterns
			else
			{
				/* Look up the database entry for the function in which the current callExpr is within*/
				FunctionNode* Func;

				/*if the function is not registered register*/
				if ((Func = PatternGraph::GetInstance()->GetFunctionNode(Callee)) == NULL)
				{
					PatternGraph::GetInstance()->RegisterFunction(Callee);
				}
				Func = PatternGraph::GetInstance()->GetFunctionNode(Callee);

	#ifdef PRINT_DEBUG
				std::cout << Func->GetFnName() << " (" << Func->GetHash() << ")" << std::endl;
	#endif

				/* Store this function call in the CallTree (ClTre)*/
				CallTreeNode* FuncNode = ClTre->registerNode(Function, Func, LastNodeType, GetTopPatternStack(), CurrentFnEntry);

				clang::SourceManager& SourceMan = Context->getSourceManager();
				clang::SourceLocation LocStart = CallExpr->getBeginLoc();
				clang::FullSourceLoc SourceLoc(LocStart, SourceMan);
				FuncNode->SetLineNumber(SourceLoc.getLineNumber());

				PatternCodeRegion* Top;
				/* if we are within a Pattern -> register this Functon as a child of the pattern etc. */
				if ((Top = GetTopPatternStack()) != NULL)
				{
					Top->AddChild(Func);
					Func->AddParent(Top);

					Func->AddPatternParent(Top);
					#ifdef DEBUG_J
					std::cout << Func->GetFnName()<< " hat als PatternParent: " << Top->GetID()<< '\n';
					#endif
				}
				else
				{/*if not register this function as a child for the function in which we currenty are
					 (because we are always inside a function this is possible)
					 */
					CurrentFnEntry->AddChild(Func);
					Func->AddParent(CurrentFnEntry);

					/*if the parent of this function has a PatternParent, the function inherits it to its child (Func) */
					if(!CurrentFnEntry->HasNoPatternParents()){
						//function has PatternParents too
						Func->AddPatternParents(CurrentFnEntry->GetPatternParents());
						/*If the function has PatternParents AND PatternChildre, we register the the GetPatternChildren
							as Children of the PatternParents vice versa*/
						if(!Func->HasNoPatternChildren()){
							Func->registerPatChildrenToPatParents();
						}
					}
				}
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
}

bool HalsteadVisitor::VisitBinaryOperator(clang::BinaryOperator *BinarOp){
	std::vector<HPCParallelPattern*> isInPatterns;
	IsStmtInAPatt(BinarOp, &isInPatterns);

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

bool HalsteadVisitor::VisitDeclStmt(clang::DeclStmt *DclStmt){
	std::vector<HPCParallelPattern*> isInPatterns;
	IsStmtInAPatt(DclStmt, &isInPatterns);

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

bool HalsteadVisitor::VisitCallExpr(clang::CallExpr *CallExpr){

	std::vector<HPCParallelPattern*> isInPatterns;
	IsStmtInAPatt(CallExpr, &isInPatterns);
	clang::Decl* CalleeDecl = CallExpr->getCalleeDecl();
	clang::SourceManager& SurMan = Context->getSourceManager();
	if(SurMan.isInMainFile(CallExpr->getExprLoc())){
		/*	CalleeDecl->dump();
			std::cout << "STATEMENT DUMP: " << '\n';
			clang::Stmt* CalleeStmt = CalleeDecl->getBody();
			CalleeStmt->dump();*/
	}


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
		/*
		countQual return the number of TypeQualifiers excluding the default qualifier */
		for(HPCParallelPattern* Pat : isInPatterns){
			for (size_t i = 0; i < numTypeQual; i++) {
					Pat->incrementNumOfOperators();
			}
			/*is the variable mot only declared but also initialized, than we have one operator more
			  in the patterns  */

			if(VrDcl->hasInit()){
				Pat->incrementNumOfOperators();
			}
			/*count the storage class specifiers*/
			if(VrDcl->getStorageClass()!= clang::StorageClass::SC_None){
				Pat->incrementNumOfOperators();
			}
		}
	}
	return true;
}

bool HalsteadVisitor::VisitFunctionDecl(clang::FunctionDecl *FctDecl){
	std::vector<HPCParallelPattern*> isInPatterns;
	IsDeclInAPatt(FctDecl, &isInPatterns);

	if(isInPatterns.empty()){
			return true;
	}
	else{
		for(HPCParallelPattern* Pat : isInPatterns){
			if(FctDecl->getStorageClass() != clang::StorageClass::SC_None ) Pat->incrementNumOfOperators();
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
			bool ExprAfterBegStmt = SourceMan.isPointWithin (Stm->getEndLoc(),CodeReg->GetStartLoc(), CodeReg->GetEndLoc());
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
			llvm::SmallVector<clang::Attr*, 4> Attributes;

			clang::QualType type = VDecl->getType();
			std::string typ = type.getAsString();

			int wc = 0;
			for(int i =0 ; i < typ.length(); i++){
				char actchar = typ[i];
				if(!((actchar <= 'z' && actchar >= 'a') || (actchar <= 'Z' && actchar >= 'A') || (actchar == '_'))){
					if(actchar == ' '){
						/*we only count one more if the next character is a letter or _*/
						if( ((i+1) < typ.length()) && ( (typ[i+1] <= 'z' && typ[i+1] >= 'a') || (typ[i+1] <= 'Z' && typ[i+1] >= 'A') || (typ[i+1] == '_') ) ){
							wc++;
						}
						else{
							return wc;
							}
						}
					}
				}
			return wc;

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
