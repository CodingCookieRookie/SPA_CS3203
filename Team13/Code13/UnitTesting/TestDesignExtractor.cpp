#include "stdafx.h"
#include "CppUnitTest.h"

#include "PKB/Entity.h"
#include "SP/DesignExtractor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestDesignExtractor) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Attribute::performCleanUp();
		Entity::performCleanUp();
		UsesS::performCleanUp();
		UsesP::performCleanUp();
		ModifiesS::performCleanUp();
		ModifiesP::performCleanUp();
		Pattern::performCleanUp();
		Parent::performCleanUp();
		ParentT::performCleanUp();
		Follows::performCleanUp();
		FollowsT::performCleanUp();
		Container::performCleanUp();
	}

public:
	TEST_METHOD(extract_readStatementOnly_success) {
		std::string varName = "x";
		std::string procName = "main";

		ReadNode* readNode = new ReadNode(varName);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(readNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);
		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));
		Assert::AreEqual(size_t(1), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(1), Entity::getAllVars().size());
		Assert::AreEqual(varName, Entity::getVarName(Entity::getAllVars()[0]));

		Assert::AreEqual(size_t(1), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
	}

	TEST_METHOD(extract_printStatementOnly_success) {
		std::string varName = "x";
		std::string procName = "main";

		PrintNode* printNode = new PrintNode(varName);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(printNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);
		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));
		Assert::AreEqual(size_t(1), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(1), Entity::getAllVars().size());
		Assert::AreEqual(varName, Entity::getVarName(Entity::getAllVars()[0]));

		Assert::AreEqual(size_t(1), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
	}

	TEST_METHOD(extract_readandPrintStatement_success) {
		std::string varNameX = "x";
		std::string varNameY = "y";
		std::string procName = "main";

		ReadNode* readNode = new ReadNode(varNameX);
		PrintNode* printNode = new PrintNode(varNameY);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(readNode);
		stmtLstNode->addStmtNode(printNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);
		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));
		Assert::AreEqual(size_t(2), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(2), Entity::getAllVars().size());
		Assert::AreEqual(varNameX, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varNameY, Entity::getVarName(Entity::getAllVars()[1]));

		std::unordered_map<StmtIndex, StmtIndex>
			stmtFollowsMap = DesignExtractor::getStmtFollowsMap();
		Assert::AreEqual(size_t(1), stmtFollowsMap.size());
		Assert::IsTrue(StmtIndex(2) == stmtFollowsMap.at(StmtIndex(1)));

		Assert::AreEqual(size_t(1), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(1), std::get<0>(UsesS::getAllSynonymVarInfo()).size());

		Assert::AreEqual(size_t(1), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(1), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
	}

	TEST_METHOD(extract_multipleStatements_allFollowsFollowsTCaptured) {
		std::string varNameX = "x";
		std::string varNameY = "y";
		std::string procName = "main";

		ReadNode* readNode = new ReadNode(varNameX);
		PrintNode* printNode = new PrintNode(varNameY);
		ExprNode* exprNode = new ExprNode(ExprNodeValueType::varName, varNameX);
		AssignNode* assignNode = new AssignNode(varNameY, exprNode);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(readNode);
		stmtLstNode->addStmtNode(printNode);
		stmtLstNode->addStmtNode(assignNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(2), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		/* We expect (3 choose 2) = 3 relationships in Follows T */
		Assert::AreEqual(size_t(3), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
	}

	TEST_METHOD(extract_singleIfStatement_parentCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   if (x == y) then {
				   print x; } else {
				   read y; } }
		*/
		PrintNode* printNode = new PrintNode("x");
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(printNode);
		ReadNode* readNode = new ReadNode("y");
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(readNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);

		/* We expect two Parent relationships to be captured, one from the print statement in the then-block,
		   and one from the read statement in the else-block. */
		Assert::AreEqual(size_t(2), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
	}

	TEST_METHOD(extract_singleWhileStatement_parentCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				while (x == y) {
					read x; } }
		*/
		PrintNode* printNode = new PrintNode("x");
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(printNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, stmtLstNode);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);

		/* We expect one Parent relationships to be captured, from the read statement in the while-block */
		Assert::AreEqual(size_t(1), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
	}

	TEST_METHOD(extract_whileInWhile_parentAndParentTCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   while (a <= b) {
				   while (x == y) {
					   print x; } } }
		*/

		PrintNode* printNode = new PrintNode("x");
		StmtLstNode* innerStmtLstNode = new StmtLstNode();
		innerStmtLstNode->addStmtNode(printNode);
		ExprNode* innerRootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* innerLeftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* innerRightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		innerRootExprNode->addChild(innerLeftExprNode);
		innerRootExprNode->addChild(innerRightExprNode);
		WhileNode* innerWhileNode = new WhileNode(innerRootExprNode, innerStmtLstNode);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(innerWhileNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "<=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "a");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "b");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, stmtLstNode);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);

		/* We expect two Parent relationships to be captured,
		   one from the outer while to inner while, and one from the inner while to read x; */
		Assert::AreEqual(size_t(2), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		/* We expect three Parent relationships to be captured,
		   one from the outer while to inner while, one from the inner while to read x;,
		   and one from the outer while to read x; */
		Assert::AreEqual(size_t(3), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
	}

	TEST_METHOD(extract_whileAndIfInIf_parentAndParentTCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   1. if (a <= b) then {
				   2. while (x == y) {
					   3. print x; } } else {
				   4. if (hello != world) then {
					   5. read y; } else {
					   6. a = b + c; } } }
		*/

		/* Handle then-block */
		PrintNode* printNode = new PrintNode("x");
		StmtLstNode* innerWhileStmtLstNode = new StmtLstNode();
		innerWhileStmtLstNode->addStmtNode(printNode);
		ExprNode* whileRootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* whileLeftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* whileRightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		whileRootExprNode->addChild(whileLeftExprNode);
		whileRootExprNode->addChild(whileRightExprNode);
		WhileNode* innerWhileNode = new WhileNode(whileRootExprNode, innerWhileStmtLstNode);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(innerWhileNode);

		/* Handle else-block */
		ReadNode* readNode = new ReadNode("y");
		StmtLstNode* innerThenStmtLstNode = new StmtLstNode();
		innerThenStmtLstNode->addStmtNode(readNode);
		ExprNode* innerElseRootExprNode = new ExprNode(ExprNodeValueType::arithmeticOperator, "+");
		ExprNode* innerElseLeftExprNode = new ExprNode(ExprNodeValueType::varName, "b");
		ExprNode* innerElseRightExprNode = new ExprNode(ExprNodeValueType::varName, "c");
		innerElseRootExprNode->addChild(innerElseLeftExprNode);
		innerElseRootExprNode->addChild(innerElseRightExprNode);
		AssignNode* assignNode = new AssignNode("a", innerElseRootExprNode);
		StmtLstNode* innerElseStmtLstNode = new StmtLstNode();
		innerElseStmtLstNode->addStmtNode(assignNode);
		ExprNode* innerIfRootExprNode = new ExprNode(ExprNodeValueType::relOperator, "!=");
		ExprNode* innerIfLeftExprNode = new ExprNode(ExprNodeValueType::varName, "hello");
		ExprNode* innerIfRightExprNode = new ExprNode(ExprNodeValueType::varName, "world");
		innerIfRootExprNode->addChild(innerIfLeftExprNode);
		innerIfRootExprNode->addChild(innerIfRightExprNode);
		IfNode* innerIfNode = new IfNode(innerIfRootExprNode, innerThenStmtLstNode, innerElseStmtLstNode);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(innerIfNode);

		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "<=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "a");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "b");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);

		/* We expect five Parent relationships to be captured: (1, 2), (1, 4), (2, 3), (4, 5) and (4, 6). */
		Assert::AreEqual(size_t(5), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());

		/* We expect eight ParentT relationships to be captured:
		   (1, 2), (1, 3), (1, 4), (1, 5), (1, 6), (2, 3), (4, 5) and (4, 6). */
		Assert::AreEqual(size_t(8), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
	}

	TEST_METHOD(extract_ifInWhile_parentAndParentTCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   1. while (x == y) {
				   2. if (a <= b) then {
					   3. print x; } else {
					   4. read y; } } }
		*/

		/* Handle then-block */
		PrintNode* printNode = new PrintNode("x");
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(printNode);

		/* Handle else-block */
		ReadNode* readNode = new ReadNode("y");
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(readNode);

		ExprNode* ifRootExprNode = new ExprNode(ExprNodeValueType::relOperator, "<=");
		ExprNode* ifLeftExprNode = new ExprNode(ExprNodeValueType::varName, "a");
		ExprNode* ifRightExprNode = new ExprNode(ExprNodeValueType::varName, "b");
		ifRootExprNode->addChild(ifLeftExprNode);
		ifRootExprNode->addChild(ifRightExprNode);
		IfNode* ifNode = new IfNode(ifRootExprNode, thenStmtLstNode, elseStmtLstNode);
		StmtLstNode* innerStmtLstNode = new StmtLstNode();
		innerStmtLstNode->addStmtNode(ifNode);

		ExprNode* whileRootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* whileLeftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* whileRightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		whileRootExprNode->addChild(whileLeftExprNode);
		whileRootExprNode->addChild(whileRightExprNode);
		WhileNode* whileNode = new WhileNode(whileRootExprNode, innerStmtLstNode);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);

		/* We expect three Parent relationships to be captured: (1, 2), (2, 3), and (2, 4). */
		Assert::AreEqual(size_t(3), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());

		/* We expect five ParentT relationships to be captured:
		   (1, 2), (1, 3), (1, 4), (2, 3), and (2, 4). */
		Assert::AreEqual(size_t(5), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
	}

	TEST_METHOD(extract_assign_postfixExpressionExtracted) {
		std::string varNameX = "x";
		std::string varNameY = "y";
		std::string constName1 = "1";
		std::string constName2 = "2";
		std::string constName3 = "3";
		std::string plusName = "+";
		std::string multiplyName = "*";
		std::string divideName = "/";
		std::string modName = "%";
		std::string procName = "main";
		std::string postfixString = " x y + 1 2 * / 3 % ";

		ExprNode* exprNodeX = new ExprNode(ExprNodeValueType::varName, varNameX);
		ExprNode* exprNodeY = new ExprNode(ExprNodeValueType::varName, varNameY);
		ExprNode* exprNodePlus = new ExprNode(ExprNodeValueType::arithmeticOperator, plusName);
		exprNodePlus->addChild(exprNodeX);
		exprNodePlus->addChild(exprNodeY);
		ExprNode* exprNode1 = new ExprNode(ExprNodeValueType::constValue, constName1);
		ExprNode* exprNode2 = new ExprNode(ExprNodeValueType::constValue, constName2);
		ExprNode* exprNodeMultiply = new ExprNode(ExprNodeValueType::arithmeticOperator, multiplyName);
		exprNodeMultiply->addChild(exprNode1);
		exprNodeMultiply->addChild(exprNode2);
		ExprNode* exprNodeDivide = new ExprNode(ExprNodeValueType::arithmeticOperator, divideName);
		exprNodeDivide->addChild(exprNodePlus);
		exprNodeDivide->addChild(exprNodeMultiply);
		ExprNode* exprNode3 = new ExprNode(ExprNodeValueType::constValue, constName3);
		ExprNode* exprNodeMod = new ExprNode(ExprNodeValueType::arithmeticOperator, modName);
		exprNodeMod->addChild(exprNodeDivide);
		exprNodeMod->addChild(exprNode3);
		AssignNode* assignNode = new AssignNode(varNameX, exprNodeMod);

		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(assignNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(2), Entity::getAllVars().size());
		Assert::AreEqual(size_t(3), Entity::getAllConsts().size());
		std::tuple<std::vector<int>, std::vector<int>> patternVarResult = Pattern::getAssignStmtsFromExprPartialMatch(std::string(" x "));
		Assert::AreEqual(size_t(1), std::get<0>(patternVarResult).size());

		std::tuple<std::vector<int>, std::vector<int>> patternConstResult = Pattern::getAssignStmtsFromExprPartialMatch(std::string(" 1 "));
		Assert::AreEqual(size_t(1), std::get<0>(patternConstResult).size());
	}

	TEST_METHOD(extract_singleIfStatement_singleVar_patternCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   if (x == x) then {
				   print x; } else {
				   read y; } }
		*/
		PrintNode* printNode = new PrintNode("x");
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(printNode);
		ReadNode* readNode = new ReadNode("y");
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(readNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);

		VarIndex x = Entity::getVarIdx("x");
		VarIndex y = Entity::getVarIdx("y");
		Assert::AreEqual(size_t(1), Pattern::getIfStmtsFromVar(x).size());
		Assert::AreEqual(size_t(0), Pattern::getIfStmtsFromVar(y).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAllIfPatternInfo()).size());
	}

	TEST_METHOD(extract_singleIfStatement_multVars_patternCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   if ((x == y) && (z > 0)) then {
				   print a; } else {
				   read y; } }
		*/
		PrintNode* printNode = new PrintNode("a");
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(printNode);
		ReadNode* readNode = new ReadNode("y");
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(readNode);
		ExprNode* andOp = new ExprNode(ExprNodeValueType::logicalOperator, "&&");
		ExprNode* eqOp = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* gtOp = new ExprNode(ExprNodeValueType::relOperator, ">");
		andOp->addChild(eqOp);
		andOp->addChild(gtOp);

		eqOp->addChild(new ExprNode(ExprNodeValueType::varName, "x"));
		eqOp->addChild(new ExprNode(ExprNodeValueType::varName, "y"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::varName, "z"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::constValue, "0"));

		IfNode* ifNode = new IfNode(andOp, thenStmtLstNode, elseStmtLstNode);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);

		VarIndex x = Entity::getVarIdx("x");
		VarIndex y = Entity::getVarIdx("y");
		VarIndex z = Entity::getVarIdx("z");
		VarIndex a = Entity::getVarIdx("a");
		Assert::AreEqual(size_t(1), Pattern::getIfStmtsFromVar(x).size());
		Assert::AreEqual(size_t(1), Pattern::getIfStmtsFromVar(y).size());
		Assert::AreEqual(size_t(1), Pattern::getIfStmtsFromVar(z).size());
		Assert::AreEqual(size_t(0), Pattern::getIfStmtsFromVar(a).size());

		Assert::AreEqual(size_t(3), std::get<0>(Pattern::getAllIfPatternInfo()).size());
	}

	TEST_METHOD(extract_multIfStatements_multVars_patternCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   if ((x == y) && (z > 0)) then {
				   print a; } else {
				   read y; }
				if (x != z) then {
				   print x; } else {
				   read a; }
			}
		*/
		PrintNode* printNode1 = new PrintNode("a");
		StmtLstNode* thenStmtLstNode1 = new StmtLstNode();
		thenStmtLstNode1->addStmtNode(printNode1);
		ReadNode* readNode1 = new ReadNode("y");
		StmtLstNode* elseStmtLstNode1 = new StmtLstNode();
		elseStmtLstNode1->addStmtNode(readNode1);
		ExprNode* andOp = new ExprNode(ExprNodeValueType::logicalOperator, "&&");
		ExprNode* eqOp = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* gtOp = new ExprNode(ExprNodeValueType::relOperator, ">");
		andOp->addChild(eqOp);
		andOp->addChild(gtOp);

		eqOp->addChild(new ExprNode(ExprNodeValueType::varName, "x"));
		eqOp->addChild(new ExprNode(ExprNodeValueType::varName, "y"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::varName, "z"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::constValue, "0"));
		IfNode* ifNode1 = new IfNode(andOp, thenStmtLstNode1, elseStmtLstNode1);

		PrintNode* printNode2 = new PrintNode("x");
		StmtLstNode* thenStmtLstNode2 = new StmtLstNode();
		thenStmtLstNode2->addStmtNode(printNode2);
		ReadNode* readNode2 = new ReadNode("a");
		StmtLstNode* elseStmtLstNode2 = new StmtLstNode();
		elseStmtLstNode2->addStmtNode(readNode2);
		ExprNode* neqOp = new ExprNode(ExprNodeValueType::relOperator, "!=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "z");
		neqOp->addChild(leftExprNode);
		neqOp->addChild(rightExprNode);
		IfNode* ifNode2 = new IfNode(neqOp, thenStmtLstNode2, elseStmtLstNode2);

		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode1);
		outerStmtLstNode->addStmtNode(ifNode2);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);

		VarIndex x = Entity::getVarIdx("x");
		VarIndex y = Entity::getVarIdx("y");
		VarIndex z = Entity::getVarIdx("z");
		VarIndex a = Entity::getVarIdx("a");
		Assert::AreEqual(size_t(2), Pattern::getIfStmtsFromVar(x).size());
		Assert::AreEqual(size_t(1), Pattern::getIfStmtsFromVar(y).size());
		Assert::AreEqual(size_t(2), Pattern::getIfStmtsFromVar(z).size());
		Assert::AreEqual(size_t(0), Pattern::getIfStmtsFromVar(a).size());

		Assert::AreEqual(size_t(5), std::get<0>(Pattern::getAllIfPatternInfo()).size());
	}

	TEST_METHOD(extract_singleWhileStatement_singleVar_patternCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				while (x == 0) {
					read y; } }
		*/
		PrintNode* printNode = new PrintNode("y");
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(printNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::constValue, "0");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, stmtLstNode);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);

		VarIndex x = Entity::getVarIdx("x");
		VarIndex y = Entity::getVarIdx("y");
		Assert::AreEqual(size_t(1), Pattern::getWhileStmtsFromVar(x).size());
		Assert::AreEqual(size_t(0), Pattern::getWhileStmtsFromVar(y).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
	}

	TEST_METHOD(extract_singleWhileStatement_multVars_patternCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
		   while ((x == y) && (z > 0)) {
					print a; } }
		*/
		PrintNode* printNode = new PrintNode("a");
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(printNode);
		ExprNode* andOp = new ExprNode(ExprNodeValueType::logicalOperator, "&&");
		ExprNode* eqOp = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* gtOp = new ExprNode(ExprNodeValueType::relOperator, ">");
		andOp->addChild(eqOp);
		andOp->addChild(gtOp);

		eqOp->addChild(new ExprNode(ExprNodeValueType::varName, "x"));
		eqOp->addChild(new ExprNode(ExprNodeValueType::varName, "y"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::varName, "z"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::constValue, "0"));

		WhileNode* whileNode = new WhileNode(andOp, stmtLstNode);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);

		VarIndex x = Entity::getVarIdx("x");
		VarIndex y = Entity::getVarIdx("y");
		VarIndex z = Entity::getVarIdx("z");
		VarIndex a = Entity::getVarIdx("a");
		Assert::AreEqual(size_t(1), Pattern::getWhileStmtsFromVar(x).size());
		Assert::AreEqual(size_t(1), Pattern::getWhileStmtsFromVar(y).size());
		Assert::AreEqual(size_t(1), Pattern::getWhileStmtsFromVar(z).size());
		Assert::AreEqual(size_t(0), Pattern::getWhileStmtsFromVar(a).size());

		Assert::AreEqual(size_t(3), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
	}

	TEST_METHOD(extract_multWhileStatements_multVars_patternCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   while ((x == y) && (z > 0)) {
				   print a; }
				while (x != z)  {
				   print a; }
			}
		*/
		PrintNode* printNode1 = new PrintNode("a");
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(printNode1);
		ExprNode* andOp = new ExprNode(ExprNodeValueType::logicalOperator, "&&");
		ExprNode* eqOp = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* gtOp = new ExprNode(ExprNodeValueType::relOperator, ">");
		andOp->addChild(eqOp);
		andOp->addChild(gtOp);

		eqOp->addChild(new ExprNode(ExprNodeValueType::varName, "x"));
		eqOp->addChild(new ExprNode(ExprNodeValueType::varName, "y"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::varName, "z"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::constValue, "0"));
		WhileNode* whileNode1 = new WhileNode(andOp, stmtLstNode1);

		PrintNode* printNode2 = new PrintNode("a");
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(printNode2);
		ExprNode* neqOp = new ExprNode(ExprNodeValueType::relOperator, "!=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "z");
		neqOp->addChild(leftExprNode);
		neqOp->addChild(rightExprNode);
		WhileNode* whileNode2 = new WhileNode(neqOp, stmtLstNode2);

		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode1);
		outerStmtLstNode->addStmtNode(whileNode2);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);

		VarIndex x = Entity::getVarIdx("x");
		VarIndex y = Entity::getVarIdx("y");
		VarIndex z = Entity::getVarIdx("z");
		VarIndex a = Entity::getVarIdx("a");
		Assert::AreEqual(size_t(2), Pattern::getWhileStmtsFromVar(x).size());
		Assert::AreEqual(size_t(1), Pattern::getWhileStmtsFromVar(y).size());
		Assert::AreEqual(size_t(2), Pattern::getWhileStmtsFromVar(z).size());
		Assert::AreEqual(size_t(0), Pattern::getWhileStmtsFromVar(a).size());

		Assert::AreEqual(size_t(5), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
	}

	TEST_METHOD(generateCFG_readStatementOnly) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				read x; } }
		*/
		StmtIndex stmtIdx = StmtIndex(1);
		std::unordered_set<StmtIndex> expectedStmtIndices = { stmtIdx };

		std::string varName = "x";
		std::string procName = "main";
		ReadNode* readNode = new ReadNode(varName);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(readNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);
		CFG cfg = DesignExtractor::generateCFG(stmtLstNode);

		CFGNode* headNode = cfg.getHead();
		std::unordered_set<StmtIndex> headNodeStmtIndices = headNode->getStmtIndices();
		Assert::IsTrue(expectedStmtIndices == headNodeStmtIndices);

		CFGNode* tailNode = cfg.getTail();
		std::unordered_set<StmtIndex> tailNodeStmtIndices = tailNode->getStmtIndices();
		Assert::IsTrue(expectedStmtIndices == tailNodeStmtIndices);

		Assert::AreEqual(size_t(1), cfg.size());
	}

	TEST_METHOD(generateCFG_printStatementOnly) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				print x; } }
		*/
		StmtIndex stmtIdx = StmtIndex(1);
		std::unordered_set<StmtIndex> expectedStmtIndices = { stmtIdx };

		std::string varName = "x";
		std::string procName = "main";
		PrintNode* printNode = new PrintNode(varName);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(printNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);
		CFG cfg = DesignExtractor::generateCFG(stmtLstNode);

		CFGNode* headNode = cfg.getHead();
		std::unordered_set<StmtIndex> headNodeStmtIndices = headNode->getStmtIndices();
		Assert::IsTrue(expectedStmtIndices == headNodeStmtIndices);

		CFGNode* tailNode = cfg.getTail();
		std::unordered_set<StmtIndex> tailNodeStmtIndices = tailNode->getStmtIndices();
		Assert::IsTrue(expectedStmtIndices == tailNodeStmtIndices);

		Assert::AreEqual(size_t(1), cfg.size());
	}

	TEST_METHOD(generateCFG_readAndPrintStatementOnly) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				read x;
				print y;  } }
		*/
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		std::unordered_set<StmtIndex> expectedStmtIndices = { stmtIdx1, stmtIdx2 };

		std::string varNameX = "x";
		std::string varNameY = "y";
		std::string procName = "main";

		ReadNode* readNode = new ReadNode(varNameX);
		PrintNode* printNode = new PrintNode(varNameY);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(readNode);
		stmtLstNode->addStmtNode(printNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);
		CFG cfg = DesignExtractor::generateCFG(stmtLstNode);

		CFGNode* headNode = cfg.getHead();
		std::unordered_set<StmtIndex> headNodeStmtIndices = headNode->getStmtIndices();
		Assert::IsTrue(expectedStmtIndices == headNodeStmtIndices);

		CFGNode* tailNode = cfg.getTail();
		std::unordered_set<StmtIndex> tailNodeStmtIndices = tailNode->getStmtIndices();
		Assert::IsTrue(expectedStmtIndices == tailNodeStmtIndices);

		Assert::AreEqual(size_t(1), cfg.size());
	}

	TEST_METHOD(generateCFG_multipleStatements) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				read x;
				print y;
				y = x  } }
		*/
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		std::unordered_set<StmtIndex> expectedStmtIndices = { stmtIdx1, stmtIdx2, stmtIdx3 };

		std::string varNameX = "x";
		std::string varNameY = "y";
		std::string procName = "main";

		ReadNode* readNode = new ReadNode(varNameX);
		PrintNode* printNode = new PrintNode(varNameY);
		ExprNode* exprNode = new ExprNode(ExprNodeValueType::varName, varNameX);
		AssignNode* assignNode = new AssignNode(varNameY, exprNode);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(readNode);
		stmtLstNode->addStmtNode(printNode);
		stmtLstNode->addStmtNode(assignNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);
		CFG cfg = DesignExtractor::generateCFG(stmtLstNode);

		CFGNode* headNode = cfg.getHead();
		std::unordered_set<StmtIndex> headNodeStmtIndices = headNode->getStmtIndices();
		Assert::IsTrue(expectedStmtIndices == headNodeStmtIndices);

		CFGNode* tailNode = cfg.getTail();
		std::unordered_set<StmtIndex> tailNodeStmtIndices = tailNode->getStmtIndices();
		Assert::IsTrue(expectedStmtIndices == tailNodeStmtIndices);

		Assert::AreEqual(size_t(1), cfg.size());
	}

	TEST_METHOD(generateCFG_singleIfStatement) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   if (x == y) then {
				   print x; } else {
				   read y; } }
		*/
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		std::unordered_set<StmtIndex> expectedHeadNodeStmtIndices = { stmtIdx1 };
		std::unordered_set<StmtIndex> expectedMidNodeStmtIndices = { stmtIdx2, stmtIdx3 };

		PrintNode* printNode = new PrintNode("x");
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(printNode);
		ReadNode* readNode = new ReadNode("y");
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(readNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);
		CFG cfg = DesignExtractor::generateCFG(outerStmtLstNode);

		CFGNode* headNode = cfg.getHead();
		std::unordered_set<StmtIndex> headNodeStmtIndices = headNode->getStmtIndices();
		std::unordered_set<CFGNode*> headNodeNextNodes = headNode->getNextNodes();
		Assert::IsTrue(expectedHeadNodeStmtIndices == headNodeStmtIndices);
		Assert::IsTrue(2 == headNodeNextNodes.size());

		CFGNode* tailNode = cfg.getTail();
		std::unordered_set<StmtIndex> tailNodeStmtIndices = tailNode->getStmtIndices();
		std::unordered_set<CFGNode*> tailNodeNextNodes = tailNode->getNextNodes();
		Assert::IsTrue(0 == tailNodeStmtIndices.size());
		Assert::IsTrue(0 == tailNodeNextNodes.size());

		Assert::AreEqual(size_t(4), cfg.size());
	}

	TEST_METHOD(generateCFG_singleWhileStatement) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				while (x == y) {
					read x; } }
		*/
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		std::unordered_set<StmtIndex> expectedHeadNodeStmtIndices = { stmtIdx1 };
		std::unordered_set<StmtIndex> expectedMidNodeStmtIndices = { stmtIdx2 };

		PrintNode* printNode = new PrintNode("x");
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(printNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, stmtLstNode);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);
		CFG cfg = DesignExtractor::generateCFG(outerStmtLstNode);

		CFGNode* headNode = cfg.getHead();
		CFGNode* tailNode = cfg.getTail();
		/* We expect the head node to be the tail node, since the code terminates at evaluation of the while statement condition. */
		Assert::IsTrue(headNode == tailNode);

		std::unordered_set<StmtIndex> headNodeStmtIndices = headNode->getStmtIndices();
		std::unordered_set<CFGNode*> headNodeNextNodes = headNode->getNextNodes();
		Assert::IsTrue(expectedHeadNodeStmtIndices == headNodeStmtIndices);
		Assert::IsTrue(1 == headNodeNextNodes.size());

		CFGNode* midNode = *headNodeNextNodes.begin();
		std::unordered_set<StmtIndex> midNodeStmtIndices = midNode->getStmtIndices();
		std::unordered_set<CFGNode*> midNodeNextNodes = midNode->getNextNodes();
		Assert::IsTrue(expectedMidNodeStmtIndices == midNodeStmtIndices);
		Assert::IsTrue(1 == midNodeNextNodes.size());
		Assert::IsTrue(headNode == *midNodeNextNodes.begin());

		Assert::AreEqual(size_t(2), cfg.size());
	}

	TEST_METHOD(generateCFG_whileInWhile) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   while (a <= b) {
				   while (x == y) {
					   print x; } } }
		*/
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		std::unordered_set<StmtIndex> expectedHeadNodeStmtIndices = { stmtIdx1 };
		std::unordered_set<StmtIndex> expectedSecondNodeStmtIndices = { stmtIdx2 };
		std::unordered_set<StmtIndex> expectedThirdNodeStmtIndices = { stmtIdx3 };

		PrintNode* printNode = new PrintNode("x");
		StmtLstNode* innerStmtLstNode = new StmtLstNode();
		innerStmtLstNode->addStmtNode(printNode);
		ExprNode* innerRootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* innerLeftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* innerRightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		innerRootExprNode->addChild(innerLeftExprNode);
		innerRootExprNode->addChild(innerRightExprNode);
		WhileNode* innerWhileNode = new WhileNode(innerRootExprNode, innerStmtLstNode);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(innerWhileNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "<=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "a");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "b");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, stmtLstNode);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);
		CFG cfg = DesignExtractor::generateCFG(outerStmtLstNode);

		CFGNode* headNode = cfg.getHead();
		CFGNode* tailNode = cfg.getTail();
		/* We expect the head node to be the tail node, since the code terminates at evaluation of the while statement condition. */
		Assert::IsTrue(headNode == tailNode);

		std::unordered_set<StmtIndex> headNodeStmtIndices = headNode->getStmtIndices();
		std::unordered_set<CFGNode*> headNodeNextNodes = headNode->getNextNodes();
		Assert::IsTrue(expectedHeadNodeStmtIndices == headNodeStmtIndices);
		Assert::IsTrue(1 == headNodeNextNodes.size());

		CFGNode* secondNode = *headNodeNextNodes.begin();
		std::unordered_set<StmtIndex> secondNodeStmtIndices = secondNode->getStmtIndices();
		std::unordered_set<CFGNode*> secondNodeNextNodes = secondNode->getNextNodes();
		Assert::IsTrue(expectedSecondNodeStmtIndices == secondNodeStmtIndices);
		Assert::IsTrue(2 == secondNodeNextNodes.size());

		CFGNode* thirdNode = *secondNodeNextNodes.begin();
		std::unordered_set<StmtIndex> thirdNodeStmtIndices = thirdNode->getStmtIndices();
		std::unordered_set<CFGNode*> thirdNodeNextNodes = thirdNode->getNextNodes();
		Assert::IsTrue(expectedThirdNodeStmtIndices == thirdNodeStmtIndices);
		Assert::IsTrue(1 == thirdNodeNextNodes.size());
		Assert::IsTrue(secondNode == *thirdNodeNextNodes.begin());

		Assert::IsTrue(headNode == *std::next(secondNodeNextNodes.begin()));

		Assert::AreEqual(size_t(3), cfg.size());
	}

	TEST_METHOD(generateCFG_whileAndIfInIf) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   1. if (a <= b) then {
				   2. while (x == y) {
					   3. print x; } } else {
				   4. if (hello != world) then {
					   5. read y; } else {
					   6. a = b + c; } } }
		*/
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		StmtIndex stmtIdx6 = StmtIndex(6);
		std::unordered_set<StmtIndex> expectedHeadNodeStmtIndices = { stmtIdx1 };
		std::unordered_set<StmtIndex> expectedSecondNodeStmtIndices = { stmtIdx2 };
		std::unordered_set<StmtIndex> expectedFourthNodeStmtIndices = { stmtIdx4 };
		std::unordered_set<StmtIndex> expectedFifthNodeStmtIndices = { stmtIdx5 };
		std::unordered_set<StmtIndex> expectedSixthNodeStmtIndices = { stmtIdx6 };

		/* Handle then-block */
		PrintNode* printNode = new PrintNode("x");
		StmtLstNode* innerWhileStmtLstNode = new StmtLstNode();
		innerWhileStmtLstNode->addStmtNode(printNode);
		ExprNode* whileRootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* whileLeftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* whileRightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		whileRootExprNode->addChild(whileLeftExprNode);
		whileRootExprNode->addChild(whileRightExprNode);
		WhileNode* innerWhileNode = new WhileNode(whileRootExprNode, innerWhileStmtLstNode);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(innerWhileNode);

		/* Handle else-block */
		ReadNode* readNode = new ReadNode("y");
		StmtLstNode* innerThenStmtLstNode = new StmtLstNode();
		innerThenStmtLstNode->addStmtNode(readNode);
		ExprNode* innerElseRootExprNode = new ExprNode(ExprNodeValueType::arithmeticOperator, "+");
		ExprNode* innerElseLeftExprNode = new ExprNode(ExprNodeValueType::varName, "b");
		ExprNode* innerElseRightExprNode = new ExprNode(ExprNodeValueType::varName, "c");
		innerElseRootExprNode->addChild(innerElseLeftExprNode);
		innerElseRootExprNode->addChild(innerElseRightExprNode);
		AssignNode* assignNode = new AssignNode("a", innerElseRootExprNode);
		StmtLstNode* innerElseStmtLstNode = new StmtLstNode();
		innerElseStmtLstNode->addStmtNode(assignNode);
		ExprNode* innerIfRootExprNode = new ExprNode(ExprNodeValueType::relOperator, "!=");
		ExprNode* innerIfLeftExprNode = new ExprNode(ExprNodeValueType::varName, "hello");
		ExprNode* innerIfRightExprNode = new ExprNode(ExprNodeValueType::varName, "world");
		innerIfRootExprNode->addChild(innerIfLeftExprNode);
		innerIfRootExprNode->addChild(innerIfRightExprNode);
		IfNode* innerIfNode = new IfNode(innerIfRootExprNode, innerThenStmtLstNode, innerElseStmtLstNode);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(innerIfNode);

		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "<=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "a");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "b");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);
		CFG cfg = DesignExtractor::generateCFG(outerStmtLstNode);

		CFGNode* headNode = cfg.getHead();
		CFGNode* tailNode = cfg.getTail();

		std::unordered_set<StmtIndex> headNodeStmtIndices = headNode->getStmtIndices();
		std::unordered_set<CFGNode*> headNodeNextNodes = headNode->getNextNodes();
		Assert::IsTrue(expectedHeadNodeStmtIndices == headNodeStmtIndices);
		Assert::IsTrue(2 == headNodeNextNodes.size());

		CFGNode* secondNode = *headNodeNextNodes.begin();
		std::unordered_set<StmtIndex> secondNodeStmtIndices = secondNode->getStmtIndices();
		std::unordered_set<CFGNode*> secondNodeNextNodes = secondNode->getNextNodes();
		Assert::IsTrue(expectedSecondNodeStmtIndices == secondNodeStmtIndices);
		Assert::IsTrue(2 == secondNodeNextNodes.size());
		Assert::IsTrue(tailNode == *std::next(secondNodeNextNodes.begin()));

		CFGNode* fourthNode = *std::next(headNodeNextNodes.begin());
		std::unordered_set<StmtIndex> fourthNodeStmtIndices = fourthNode->getStmtIndices();
		std::unordered_set<CFGNode*> fourthNodeNextNodes = fourthNode->getNextNodes();
		Assert::IsTrue(expectedFourthNodeStmtIndices == fourthNodeStmtIndices);
		Assert::IsTrue(2 == fourthNodeNextNodes.size());

		CFGNode* fifthNode = *fourthNodeNextNodes.begin();
		std::unordered_set<StmtIndex> fifthNodeStmtIndices = fifthNode->getStmtIndices();
		std::unordered_set<CFGNode*> fifthNodeNextNodes = fifthNode->getNextNodes();
		Assert::IsTrue(expectedFifthNodeStmtIndices == fifthNodeStmtIndices);
		Assert::IsTrue(1 == fifthNodeNextNodes.size());

		CFGNode* sixthNode = *std::next(fourthNodeNextNodes.begin());
		std::unordered_set<StmtIndex> sixthNodeStmtIndices = sixthNode->getStmtIndices();
		std::unordered_set<CFGNode*> sixthNodeNextNodes = sixthNode->getNextNodes();
		Assert::IsTrue(expectedSixthNodeStmtIndices == sixthNodeStmtIndices);
		Assert::IsTrue(1 == sixthNodeNextNodes.size());

		CFGNode* innerIfLoopDummyNode = *fifthNodeNextNodes.begin();
		Assert::IsTrue(innerIfLoopDummyNode == *sixthNodeNextNodes.begin());

		Assert::IsTrue(tailNode == *innerIfLoopDummyNode->getNextNodes().begin());

		Assert::AreEqual(size_t(8), cfg.size());
	}

	TEST_METHOD(generateCFG_ifInWhile) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   1. while (x == y) {
				   2. if (a <= b) then {
					   3. print x; } else {
					   4. read y; } } }
		*/
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		std::unordered_set<StmtIndex> expectedHeadNodeStmtIndices = { stmtIdx1 };
		std::unordered_set<StmtIndex> expectedSecondNodeStmtIndices = { stmtIdx2 };
		std::unordered_set<StmtIndex> expectedThirdNodeStmtIndices = { stmtIdx3 };
		std::unordered_set<StmtIndex> expectedFourthNodeStmtIndices = { stmtIdx4 };

		/* Handle then-block */
		PrintNode* printNode = new PrintNode("x");
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(printNode);

		/* Handle else-block */
		ReadNode* readNode = new ReadNode("y");
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(readNode);

		ExprNode* ifRootExprNode = new ExprNode(ExprNodeValueType::relOperator, "<=");
		ExprNode* ifLeftExprNode = new ExprNode(ExprNodeValueType::varName, "a");
		ExprNode* ifRightExprNode = new ExprNode(ExprNodeValueType::varName, "b");
		ifRootExprNode->addChild(ifLeftExprNode);
		ifRootExprNode->addChild(ifRightExprNode);
		IfNode* ifNode = new IfNode(ifRootExprNode, thenStmtLstNode, elseStmtLstNode);
		StmtLstNode* innerStmtLstNode = new StmtLstNode();
		innerStmtLstNode->addStmtNode(ifNode);

		ExprNode* whileRootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* whileLeftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* whileRightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		whileRootExprNode->addChild(whileLeftExprNode);
		whileRootExprNode->addChild(whileRightExprNode);
		WhileNode* whileNode = new WhileNode(whileRootExprNode, innerStmtLstNode);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);
		CFG cfg = DesignExtractor::generateCFG(outerStmtLstNode);

		CFGNode* headNode = cfg.getHead();
		CFGNode* tailNode = cfg.getTail();
		/* We expect the head node to be the tail node, since the code terminates at evaluation of the while statement condition. */
		Assert::IsTrue(headNode == tailNode);

		std::unordered_set<StmtIndex> headNodeStmtIndices = headNode->getStmtIndices();
		std::unordered_set<CFGNode*> headNodeNextNodes = headNode->getNextNodes();
		Assert::IsTrue(expectedHeadNodeStmtIndices == headNodeStmtIndices);
		Assert::IsTrue(1 == headNodeNextNodes.size());

		CFGNode* secondNode = *headNodeNextNodes.begin();
		std::unordered_set<StmtIndex> secondNodeStmtIndices = secondNode->getStmtIndices();
		std::unordered_set<CFGNode*> secondNodeNextNodes = secondNode->getNextNodes();
		Assert::IsTrue(expectedSecondNodeStmtIndices == secondNodeStmtIndices);
		Assert::IsTrue(2 == secondNodeNextNodes.size());

		CFGNode* thirdNode = *secondNodeNextNodes.begin();
		std::unordered_set<StmtIndex> thirdNodeStmtIndices = thirdNode->getStmtIndices();
		std::unordered_set<CFGNode*> thirdNodeNextNodes = thirdNode->getNextNodes();
		Assert::IsTrue(expectedThirdNodeStmtIndices == thirdNodeStmtIndices);
		Assert::IsTrue(1 == thirdNodeNextNodes.size());

		CFGNode* ifLoopDummyNode = *thirdNodeNextNodes.begin();
		Assert::IsTrue(headNode == *ifLoopDummyNode->getNextNodes().begin());

		CFGNode* fourthNode = *std::next(secondNodeNextNodes.begin());
		std::unordered_set<StmtIndex> fourthNodeStmtIndices = fourthNode->getStmtIndices();
		std::unordered_set<CFGNode*> fourthNodeNextNodes = fourthNode->getNextNodes();
		Assert::IsTrue(expectedFourthNodeStmtIndices == fourthNodeStmtIndices);
		Assert::IsTrue(1 == fourthNodeNextNodes.size());
		Assert::IsTrue(ifLoopDummyNode == *fourthNodeNextNodes.begin());

		Assert::AreEqual(size_t(5), cfg.size());
	}

	TEST_METHOD(generateCFG_multIfStatements) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   if ((x == y) && (z > 0)) then {
				   print a; } else {
				   read y; }
				if (x != z) then {
				   print x; } else {
				   read a; }
			}
		*/
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		StmtIndex stmtIdx6 = StmtIndex(6);
		std::unordered_set<StmtIndex> expectedHeadNodeStmtIndices = { stmtIdx1 };
		std::unordered_set<StmtIndex> expectedSecondNodeStmtIndices = { stmtIdx2 };
		std::unordered_set<StmtIndex> expectedThirdNodeStmtIndices = { stmtIdx3 };
		std::unordered_set<StmtIndex> expectedFourthNodeStmtIndices = { stmtIdx4 };
		std::unordered_set<StmtIndex> expectedFifthNodeStmtIndices = { stmtIdx5 };
		std::unordered_set<StmtIndex> expectedSixthNodeStmtIndices = { stmtIdx6 };

		PrintNode* printNode1 = new PrintNode("a");
		StmtLstNode* thenStmtLstNode1 = new StmtLstNode();
		thenStmtLstNode1->addStmtNode(printNode1);
		ReadNode* readNode1 = new ReadNode("y");
		StmtLstNode* elseStmtLstNode1 = new StmtLstNode();
		elseStmtLstNode1->addStmtNode(readNode1);
		ExprNode* andOp = new ExprNode(ExprNodeValueType::logicalOperator, "&&");
		ExprNode* eqOp = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* gtOp = new ExprNode(ExprNodeValueType::relOperator, ">");
		andOp->addChild(eqOp);
		andOp->addChild(gtOp);

		eqOp->addChild(new ExprNode(ExprNodeValueType::varName, "x"));
		eqOp->addChild(new ExprNode(ExprNodeValueType::varName, "y"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::varName, "z"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::constValue, "0"));
		IfNode* ifNode1 = new IfNode(andOp, thenStmtLstNode1, elseStmtLstNode1);

		PrintNode* printNode2 = new PrintNode("x");
		StmtLstNode* thenStmtLstNode2 = new StmtLstNode();
		thenStmtLstNode2->addStmtNode(printNode2);
		ReadNode* readNode2 = new ReadNode("a");
		StmtLstNode* elseStmtLstNode2 = new StmtLstNode();
		elseStmtLstNode2->addStmtNode(readNode2);
		ExprNode* neqOp = new ExprNode(ExprNodeValueType::relOperator, "!=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "z");
		neqOp->addChild(leftExprNode);
		neqOp->addChild(rightExprNode);
		IfNode* ifNode2 = new IfNode(neqOp, thenStmtLstNode2, elseStmtLstNode2);

		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode1);
		outerStmtLstNode->addStmtNode(ifNode2);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);
		CFG cfg = DesignExtractor::generateCFG(outerStmtLstNode);

		CFGNode* headNode = cfg.getHead();
		CFGNode* tailNode = cfg.getTail();

		std::unordered_set<StmtIndex> headNodeStmtIndices = headNode->getStmtIndices();
		std::unordered_set<CFGNode*> headNodeNextNodes = headNode->getNextNodes();
		Assert::IsTrue(expectedHeadNodeStmtIndices == headNodeStmtIndices);
		Assert::IsTrue(2 == headNodeNextNodes.size());

		CFGNode* secondNode = *headNodeNextNodes.begin();
		std::unordered_set<StmtIndex> secondNodeStmtIndices = secondNode->getStmtIndices();
		std::unordered_set<CFGNode*> secondNodeNextNodes = secondNode->getNextNodes();
		Assert::IsTrue(expectedSecondNodeStmtIndices == secondNodeStmtIndices);
		Assert::IsTrue(1 == secondNodeNextNodes.size());

		CFGNode* thirdNode = *std::next(headNodeNextNodes.begin());
		std::unordered_set<StmtIndex> thirdNodeStmtIndices = thirdNode->getStmtIndices();
		std::unordered_set<CFGNode*> thirdNodeNextNodes = thirdNode->getNextNodes();
		Assert::IsTrue(expectedThirdNodeStmtIndices == thirdNodeStmtIndices);
		Assert::IsTrue(1 == thirdNodeNextNodes.size());

		CFGNode* firstIfLoopDummyNode = *secondNodeNextNodes.begin();
		Assert::IsTrue(firstIfLoopDummyNode == *thirdNodeNextNodes.begin());

		CFGNode* fourthNode = *firstIfLoopDummyNode->getNextNodes().begin();
		std::unordered_set<StmtIndex> fourthNodeStmtIndices = fourthNode->getStmtIndices();
		std::unordered_set<CFGNode*> fourthNodeNextNodes = fourthNode->getNextNodes();
		Assert::IsTrue(expectedFourthNodeStmtIndices == fourthNodeStmtIndices);
		Assert::IsTrue(2 == fourthNodeNextNodes.size());

		CFGNode* fifthNode = *fourthNodeNextNodes.begin();
		std::unordered_set<StmtIndex> fifthNodeStmtIndices = fifthNode->getStmtIndices();
		std::unordered_set<CFGNode*> fifthNodeNextNodes = fifthNode->getNextNodes();
		Assert::IsTrue(expectedFifthNodeStmtIndices == fifthNodeStmtIndices);
		Assert::IsTrue(1 == fifthNodeNextNodes.size());

		CFGNode* sixthNode = *std::next(fourthNodeNextNodes.begin());
		std::unordered_set<StmtIndex> sixthNodeStmtIndices = sixthNode->getStmtIndices();
		std::unordered_set<CFGNode*> sixthNodeNextNodes = sixthNode->getNextNodes();
		Assert::IsTrue(expectedSixthNodeStmtIndices == sixthNodeStmtIndices);
		Assert::IsTrue(1 == sixthNodeNextNodes.size());

		CFGNode* secondIfLoopDummyNode = *fifthNodeNextNodes.begin();
		Assert::IsTrue(secondIfLoopDummyNode == *sixthNodeNextNodes.begin());
		Assert::IsTrue(tailNode == secondIfLoopDummyNode);

		Assert::AreEqual(size_t(8), cfg.size());
	}

	TEST_METHOD(generateCFG_multWhileStatements) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   while ((x == y) && (z > 0)) {
				   print a; }
				while (x != z)  {
				   print a; }
			}
		*/
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		std::unordered_set<StmtIndex> expectedHeadNodeStmtIndices = { stmtIdx1 };
		std::unordered_set<StmtIndex> expectedSecondNodeStmtIndices = { stmtIdx2 };
		std::unordered_set<StmtIndex> expectedThirdNodeStmtIndices = { stmtIdx3 };
		std::unordered_set<StmtIndex> expectedFourthNodeStmtIndices = { stmtIdx4 };

		PrintNode* printNode1 = new PrintNode("a");
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(printNode1);
		ExprNode* andOp = new ExprNode(ExprNodeValueType::logicalOperator, "&&");
		ExprNode* eqOp = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* gtOp = new ExprNode(ExprNodeValueType::relOperator, ">");
		andOp->addChild(eqOp);
		andOp->addChild(gtOp);

		eqOp->addChild(new ExprNode(ExprNodeValueType::varName, "x"));
		eqOp->addChild(new ExprNode(ExprNodeValueType::varName, "y"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::varName, "z"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::constValue, "0"));
		WhileNode* whileNode1 = new WhileNode(andOp, stmtLstNode1);

		PrintNode* printNode2 = new PrintNode("a");
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(printNode2);
		ExprNode* neqOp = new ExprNode(ExprNodeValueType::relOperator, "!=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "z");
		neqOp->addChild(leftExprNode);
		neqOp->addChild(rightExprNode);
		WhileNode* whileNode2 = new WhileNode(neqOp, stmtLstNode2);

		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode1);
		outerStmtLstNode->addStmtNode(whileNode2);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);
		CFG cfg = DesignExtractor::generateCFG(outerStmtLstNode);

		CFGNode* headNode = cfg.getHead();
		CFGNode* tailNode = cfg.getTail();

		std::unordered_set<StmtIndex> headNodeStmtIndices = headNode->getStmtIndices();
		std::unordered_set<CFGNode*> headNodeNextNodes = headNode->getNextNodes();
		Assert::IsTrue(expectedHeadNodeStmtIndices == headNodeStmtIndices);
		Assert::IsTrue(2 == headNodeNextNodes.size());

		CFGNode* secondNode = *headNodeNextNodes.begin();
		std::unordered_set<StmtIndex> secondNodeStmtIndices = secondNode->getStmtIndices();
		std::unordered_set<CFGNode*> secondNodeNextNodes = secondNode->getNextNodes();
		Assert::IsTrue(expectedSecondNodeStmtIndices == secondNodeStmtIndices);
		Assert::IsTrue(1 == secondNodeNextNodes.size());
		Assert::IsTrue(headNode == *secondNodeNextNodes.begin());

		CFGNode* thirdNode = *std::next(headNodeNextNodes.begin());
		std::unordered_set<StmtIndex> thirdNodeStmtIndices = thirdNode->getStmtIndices();
		std::unordered_set<CFGNode*> thirdNodeNextNodes = thirdNode->getNextNodes();
		Assert::IsTrue(expectedThirdNodeStmtIndices == thirdNodeStmtIndices);
		Assert::IsTrue(1 == thirdNodeNextNodes.size());

		CFGNode* fourthNode = *thirdNodeNextNodes.begin();
		std::unordered_set<StmtIndex> fourthNodeStmtIndices = fourthNode->getStmtIndices();
		std::unordered_set<CFGNode*> fourthNodeNextNodes = fourthNode->getNextNodes();
		Assert::IsTrue(expectedFourthNodeStmtIndices == fourthNodeStmtIndices);
		Assert::IsTrue(1 == fourthNodeNextNodes.size());
		Assert::IsTrue(thirdNode == *fourthNodeNextNodes.begin());

		Assert::AreEqual(size_t(4), cfg.size());
	}

	TEST_METHOD(extract_multProcs_insertStmt_attributesPopulated) {
		/* AST is equivalent to the SIMPLE program
		   procedure proc1 {
			   1. if (a <= b) then {
				   2. while (x == y) {
					   3. print x;
					   4. read x; } } else {
				   5. if (hello != world) then {
					   6. read y; } else {
					   7. a = b + c;
					   8. call proc2 } } }
			procedure proc2 {
				9. read x; }
		*/

		std::string procName1 = "proc1";
		std::string procName2 = "proc2";
		std::string varNameX = "x";
		std::string varNameY = "y";

		/* 1st proc */
		/* Handle then-block */
		StmtLstNode* innerWhileStmtLstNode = new StmtLstNode();
		innerWhileStmtLstNode->addStmtNode(new PrintNode(varNameX));
		innerWhileStmtLstNode->addStmtNode(new ReadNode(varNameX));
		ExprNode* whileRootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* whileLeftExprNode = new ExprNode(ExprNodeValueType::varName, varNameX);
		ExprNode* whileRightExprNode = new ExprNode(ExprNodeValueType::varName, varNameY);
		whileRootExprNode->addChild(whileLeftExprNode);
		whileRootExprNode->addChild(whileRightExprNode);
		WhileNode* innerWhileNode = new WhileNode(whileRootExprNode, innerWhileStmtLstNode);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(innerWhileNode);

		/* Handle else-block */
		ReadNode* readNode = new ReadNode(varNameY);
		StmtLstNode* innerThenStmtLstNode = new StmtLstNode();
		innerThenStmtLstNode->addStmtNode(readNode);
		ExprNode* innerElseRootExprNode = new ExprNode(ExprNodeValueType::arithmeticOperator, "+");
		ExprNode* innerElseLeftExprNode = new ExprNode(ExprNodeValueType::varName, "b");
		ExprNode* innerElseRightExprNode = new ExprNode(ExprNodeValueType::varName, "c");
		innerElseRootExprNode->addChild(innerElseLeftExprNode);
		innerElseRootExprNode->addChild(innerElseRightExprNode);
		AssignNode* assignNode = new AssignNode("a", innerElseRootExprNode);
		StmtLstNode* innerElseStmtLstNode = new StmtLstNode();
		innerElseStmtLstNode->addStmtNode(assignNode);
		innerElseStmtLstNode->addStmtNode(new CallNode(procName2));
		ExprNode* innerIfRootExprNode = new ExprNode(ExprNodeValueType::relOperator, "!=");
		ExprNode* innerIfLeftExprNode = new ExprNode(ExprNodeValueType::varName, "hello");
		ExprNode* innerIfRightExprNode = new ExprNode(ExprNodeValueType::varName, "world");
		innerIfRootExprNode->addChild(innerIfLeftExprNode);
		innerIfRootExprNode->addChild(innerIfRightExprNode);
		IfNode* innerIfNode = new IfNode(innerIfRootExprNode, innerThenStmtLstNode, innerElseStmtLstNode);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(innerIfNode);

		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "<=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "a");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "b");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode1 = new ProcedureNode(procName1);
		procedureNode1->addStmtLst(outerStmtLstNode);

		/* 2nd proc */
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(new ReadNode(varNameX));
		ProcedureNode* procedureNode2 = new ProcedureNode(procName2);
		procedureNode2->addStmtLst(stmtLstNode);

		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode1);
		programNode->addProcedure(procedureNode2);
		SourceAST ast(programNode);
		DesignExtractor::extract(ast);

		/* Check stmts and attributes population */
		Assert::AreEqual(size_t(9), Entity::getAllStmts().size());

		/* read x */
		std::vector<EntityAttributeRef> expectedResultReadVarNameX{ 4, 9 };
		std::vector<EntityAttributeRef> resultReadVarNameX = Attribute::getEqualNameAttributesFromName(PqlEntityType::Read, varNameX);
		Assert::IsTrue(expectedResultReadVarNameX == resultReadVarNameX);

		/* read y */
		std::vector<EntityAttributeRef> expectedResultReadVarNameY{ 6 };
		std::vector<EntityAttributeRef> resultReadVarNameY = Attribute::getEqualNameAttributesFromName(PqlEntityType::Read, varNameY);
		Assert::IsTrue(expectedResultReadVarNameY == resultReadVarNameY);

		/* print x */
		std::vector<EntityAttributeRef> expectedResultPrintVarNameX{ 3 };
		std::vector<EntityAttributeRef> resultPrintVarNameX = Attribute::getEqualNameAttributesFromName(PqlEntityType::Print, varNameX);
		Assert::IsTrue(expectedResultPrintVarNameX == resultPrintVarNameX);

		/* print y (does not exist) */
		std::vector<EntityAttributeRef> expectedResultPrintVarNameY{ };
		std::vector<EntityAttributeRef> resultPrintVarNameY = Attribute::getEqualNameAttributesFromName(PqlEntityType::Print, varNameY);
		Assert::IsTrue(expectedResultPrintVarNameY == resultPrintVarNameY);

		/* call proc2 */
		std::vector<EntityAttributeRef> expectedResultCallProcName2{ 8 };
		std::vector<EntityAttributeRef> resultCallProcName2 = Attribute::getEqualNameAttributesFromName(PqlEntityType::Call, procName2);
		Assert::IsTrue(expectedResultCallProcName2 == resultCallProcName2);
	}
	};
}
