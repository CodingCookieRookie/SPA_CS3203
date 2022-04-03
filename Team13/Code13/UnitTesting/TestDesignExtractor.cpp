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
		Next::performCleanUp();
		Pattern::performCleanUp();
		Parent::performCleanUp();
		ParentT::performCleanUp();
		Follows::performCleanUp();
		FollowsT::performCleanUp();
		Calls::performCleanUp();
		CallsT::performCleanUp();
		Container::performCleanUp();
	}

public:
	TEST_METHOD(extract_readStatementOnly_success) {
		std::string varName = "x";
		std::string procName = "main";

		ReadNode* readNode = new ReadNode(varName, 1);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(readNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);
		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));
		Assert::AreEqual(size_t(1), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(1), Entity::getAllVars().size());
		Assert::AreEqual(varName, Entity::getVarName(Entity::getAllVars()[0]));

		Assert::AreEqual(size_t(1), std::get<0>(ModifiesS::getAllInfo()).size());
	}

	TEST_METHOD(extract_printStatementOnly_success) {
		std::string varName = "x";
		std::string procName = "main";

		PrintNode* printNode = new PrintNode(varName, 1);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(printNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);
		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));
		Assert::AreEqual(size_t(1), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(1), Entity::getAllVars().size());
		Assert::AreEqual(varName, Entity::getVarName(Entity::getAllVars()[0]));

		Assert::AreEqual(size_t(1), std::get<0>(UsesS::getAllInfo()).size());
	}

	TEST_METHOD(extract_readandPrintStatement_success) {
		std::string varNameX = "x";
		std::string varNameY = "y";
		std::string procName = "main";

		ReadNode* readNode = new ReadNode(varNameX, 1);
		PrintNode* printNode = new PrintNode(varNameY, 2);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(readNode);
		stmtLstNode->addStmtNode(printNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);
		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));
		Assert::AreEqual(size_t(2), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(2), Entity::getAllVars().size());
		Assert::AreEqual(varNameX, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varNameY, Entity::getVarName(Entity::getAllVars()[1]));

		Assert::AreEqual(size_t(1), std::get<0>(ModifiesS::getAllInfo()).size());
		Assert::AreEqual(size_t(1), std::get<0>(UsesS::getAllInfo()).size());

		Assert::AreEqual(size_t(1), std::get<0>(Follows::getAllInfo()).size());
		Assert::AreEqual(size_t(1), std::get<0>(FollowsT::getAllInfo()).size());
	}

	TEST_METHOD(extract_multipleStatements_allFollowsFollowsTCaptured) {
		std::string varNameX = "x";
		std::string varNameY = "y";
		std::string procName = "main";

		ReadNode* readNode = new ReadNode(varNameX, 1);
		PrintNode* printNode = new PrintNode(varNameY, 2);
		ExprNode* exprNode = new ExprNode(ExprNodeValueType::VAR_NAME, varNameX);
		AssignNode* assignNode = new AssignNode(varNameY, exprNode, 3);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(readNode);
		stmtLstNode->addStmtNode(printNode);
		stmtLstNode->addStmtNode(assignNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		Assert::AreEqual(size_t(2), std::get<0>(Follows::getAllInfo()).size());
		/* We expect (3 choose 2) = 3 relationships in Follows T */
		Assert::AreEqual(size_t(3), std::get<0>(FollowsT::getAllInfo()).size());
	}

	TEST_METHOD(extract_singleIfStatement_parentCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			  1. if (x == y) then {
				2.    print x; } else {
				  3. read y; } }
		*/
		PrintNode* printNode = new PrintNode("x", 2);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(printNode);
		ReadNode* readNode = new ReadNode("y", 3);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(readNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		/* We expect two Parent relationships to be captured, one from the print statement in the then-block,
		   and one from the read statement in the else-block. */
		Assert::AreEqual(size_t(2), std::get<0>(Parent::getAllInfo()).size());
	}

	TEST_METHOD(extract_singleWhileStatement_parentCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				1. while (x == y) {
					2. print x; } }
		*/
		PrintNode* printNode = new PrintNode("x", 2);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(printNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, stmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		/* We expect one Parent relationships to be captured, from the read statement in the while-block */
		Assert::AreEqual(size_t(1), std::get<0>(Parent::getAllInfo()).size());
	}

	TEST_METHOD(extract_whileInWhile_parentAndParentTCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			  1.  while (a <= b) {
				 2.  while (x == y) {
					3.   print x; } } }
		*/

		PrintNode* printNode = new PrintNode("x", 3);
		StmtLstNode* innerStmtLstNode = new StmtLstNode();
		innerStmtLstNode->addStmtNode(printNode);
		ExprNode* innerRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* innerLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* innerRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		innerRootExprNode->addChild(innerLeftExprNode);
		innerRootExprNode->addChild(innerRightExprNode);
		WhileNode* innerWhileNode = new WhileNode(innerRootExprNode, innerStmtLstNode, 2);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(innerWhileNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "<=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "a");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, stmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		/* We expect two Parent relationships to be captured,
		   one from the outer while to inner while, and one from the inner while to read x; */
		Assert::AreEqual(size_t(2), std::get<0>(Parent::getAllInfo()).size());
		/* We expect three Parent relationships to be captured,
		   one from the outer while to inner while, one from the inner while to read x;,
		   and one from the outer while to read x; */
		Assert::AreEqual(size_t(3), std::get<0>(ParentT::getAllInfo()).size());
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
		PrintNode* printNode = new PrintNode("x", 3);
		StmtLstNode* innerWhileStmtLstNode = new StmtLstNode();
		innerWhileStmtLstNode->addStmtNode(printNode);
		ExprNode* whileRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* whileLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* whileRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		whileRootExprNode->addChild(whileLeftExprNode);
		whileRootExprNode->addChild(whileRightExprNode);
		WhileNode* innerWhileNode = new WhileNode(whileRootExprNode, innerWhileStmtLstNode, 2);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(innerWhileNode);

		/* Handle else-block */
		ReadNode* readNode = new ReadNode("y", 5);
		StmtLstNode* innerThenStmtLstNode = new StmtLstNode();
		innerThenStmtLstNode->addStmtNode(readNode);
		ExprNode* innerElseRootExprNode = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, "+");
		ExprNode* innerElseLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		ExprNode* innerElseRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "c");
		innerElseRootExprNode->addChild(innerElseLeftExprNode);
		innerElseRootExprNode->addChild(innerElseRightExprNode);
		AssignNode* assignNode = new AssignNode("a", innerElseRootExprNode, 6);
		StmtLstNode* innerElseStmtLstNode = new StmtLstNode();
		innerElseStmtLstNode->addStmtNode(assignNode);
		ExprNode* innerIfRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "!=");
		ExprNode* innerIfLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "hello");
		ExprNode* innerIfRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "world");
		innerIfRootExprNode->addChild(innerIfLeftExprNode);
		innerIfRootExprNode->addChild(innerIfRightExprNode);
		IfNode* innerIfNode = new IfNode(innerIfRootExprNode, innerThenStmtLstNode, innerElseStmtLstNode, 4);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(innerIfNode);

		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "<=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "a");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		/* We expect five Parent relationships to be captured: (1, 2), (1, 4), (2, 3), (4, 5) and (4, 6). */
		Assert::AreEqual(size_t(5), std::get<0>(Parent::getAllInfo()).size());

		/* We expect eight ParentT relationships to be captured:
		   (1, 2), (1, 3), (1, 4), (1, 5), (1, 6), (2, 3), (4, 5) and (4, 6). */
		Assert::AreEqual(size_t(8), std::get<0>(ParentT::getAllInfo()).size());
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
		PrintNode* printNode = new PrintNode("x", 3);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(printNode);

		/* Handle else-block */
		ReadNode* readNode = new ReadNode("y", 4);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(readNode);

		ExprNode* ifRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "<=");
		ExprNode* ifLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "a");
		ExprNode* ifRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		ifRootExprNode->addChild(ifLeftExprNode);
		ifRootExprNode->addChild(ifRightExprNode);
		IfNode* ifNode = new IfNode(ifRootExprNode, thenStmtLstNode, elseStmtLstNode, 2);
		StmtLstNode* innerStmtLstNode = new StmtLstNode();
		innerStmtLstNode->addStmtNode(ifNode);

		ExprNode* whileRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* whileLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* whileRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		whileRootExprNode->addChild(whileLeftExprNode);
		whileRootExprNode->addChild(whileRightExprNode);
		WhileNode* whileNode = new WhileNode(whileRootExprNode, innerStmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		/* We expect three Parent relationships to be captured: (1, 2), (2, 3), and (2, 4). */
		Assert::AreEqual(size_t(3), std::get<0>(Parent::getAllInfo()).size());

		/* We expect five ParentT relationships to be captured:
		   (1, 2), (1, 3), (1, 4), (2, 3), and (2, 4). */
		Assert::AreEqual(size_t(5), std::get<0>(ParentT::getAllInfo()).size());
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

		ExprNode* exprNodeX = new ExprNode(ExprNodeValueType::VAR_NAME, varNameX);
		ExprNode* exprNodeY = new ExprNode(ExprNodeValueType::VAR_NAME, varNameY);
		ExprNode* exprNodePlus = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, plusName);
		exprNodePlus->addChild(exprNodeX);
		exprNodePlus->addChild(exprNodeY);
		ExprNode* exprNode1 = new ExprNode(ExprNodeValueType::CONST_VALUE, constName1);
		ExprNode* exprNode2 = new ExprNode(ExprNodeValueType::CONST_VALUE, constName2);
		ExprNode* exprNodeMultiply = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, multiplyName);
		exprNodeMultiply->addChild(exprNode1);
		exprNodeMultiply->addChild(exprNode2);
		ExprNode* exprNodeDivide = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, divideName);
		exprNodeDivide->addChild(exprNodePlus);
		exprNodeDivide->addChild(exprNodeMultiply);
		ExprNode* exprNode3 = new ExprNode(ExprNodeValueType::CONST_VALUE, constName3);
		ExprNode* exprNodeMod = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, modName);
		exprNodeMod->addChild(exprNodeDivide);
		exprNodeMod->addChild(exprNode3);
		AssignNode* assignNode = new AssignNode(varNameX, exprNodeMod, 1);

		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(assignNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

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
			  1. if (x == x) then {
				2.   print x; } else {
				  3. read y; } }
		*/
		PrintNode* printNode = new PrintNode("x", 2);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(printNode);
		ReadNode* readNode = new ReadNode("y", 3);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(readNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		VarIndex x = Entity::getVarIdx("x");
		VarIndex y = Entity::getVarIdx("y");
		Assert::AreEqual(size_t(1), Pattern::getIfStmtsFromVar(x).size());
		Assert::AreEqual(size_t(0), Pattern::getIfStmtsFromVar(y).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAllIfPatternInfo()).size());
	}

	TEST_METHOD(extract_singleIfStatement_multVars_patternCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			 1.  if ((x == y) && (z > 0)) then {
				2.   print a; } else {
				  3. read y; } }
		*/
		PrintNode* printNode = new PrintNode("a", 2);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(printNode);
		ReadNode* readNode = new ReadNode("y", 3);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(readNode);
		ExprNode* andOp = new ExprNode(ExprNodeValueType::LOGICAL_OPERATOR, "&&");
		ExprNode* eqOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* gtOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, ">");
		andOp->addChild(eqOp);
		andOp->addChild(gtOp);

		eqOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "x"));
		eqOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "y"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "z"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::CONST_VALUE, "0"));

		IfNode* ifNode = new IfNode(andOp, thenStmtLstNode, elseStmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

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
			   1. if ((x == y) && (z > 0)) then {
				  2. print a; } else {
				   3. read y; }
				4. if (x != z) then {
				   5. print x; } else {
				   6. read a; }
			}
		*/
		PrintNode* printNode1 = new PrintNode("a", 2);
		StmtLstNode* thenStmtLstNode1 = new StmtLstNode();
		thenStmtLstNode1->addStmtNode(printNode1);
		ReadNode* readNode1 = new ReadNode("y", 3);
		StmtLstNode* elseStmtLstNode1 = new StmtLstNode();
		elseStmtLstNode1->addStmtNode(readNode1);
		ExprNode* andOp = new ExprNode(ExprNodeValueType::LOGICAL_OPERATOR, "&&");
		ExprNode* eqOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* gtOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, ">");
		andOp->addChild(eqOp);
		andOp->addChild(gtOp);

		eqOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "x"));
		eqOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "y"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "z"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::CONST_VALUE, "0"));
		IfNode* ifNode1 = new IfNode(andOp, thenStmtLstNode1, elseStmtLstNode1, 1);

		PrintNode* printNode2 = new PrintNode("x", 5);
		StmtLstNode* thenStmtLstNode2 = new StmtLstNode();
		thenStmtLstNode2->addStmtNode(printNode2);
		ReadNode* readNode2 = new ReadNode("a", 6);
		StmtLstNode* elseStmtLstNode2 = new StmtLstNode();
		elseStmtLstNode2->addStmtNode(readNode2);
		ExprNode* neqOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, "!=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "z");
		neqOp->addChild(leftExprNode);
		neqOp->addChild(rightExprNode);
		IfNode* ifNode2 = new IfNode(neqOp, thenStmtLstNode2, elseStmtLstNode2, 4);

		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode1);
		outerStmtLstNode->addStmtNode(ifNode2);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

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
				1. while (x == 0) {
					2. read y; } }
		*/
		PrintNode* printNode = new PrintNode("y", 2);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(printNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::CONST_VALUE, "0");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, stmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		VarIndex x = Entity::getVarIdx("x");
		VarIndex y = Entity::getVarIdx("y");
		Assert::AreEqual(size_t(1), Pattern::getWhileStmtsFromVar(x).size());
		Assert::AreEqual(size_t(0), Pattern::getWhileStmtsFromVar(y).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
	}

	TEST_METHOD(extract_singleWhileStatement_multVars_patternCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
		   1. while ((x == y) && (z > 0)) {
				2.	print a; } }
		*/
		PrintNode* printNode = new PrintNode("a", 2);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(printNode);
		ExprNode* andOp = new ExprNode(ExprNodeValueType::LOGICAL_OPERATOR, "&&");
		ExprNode* eqOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* gtOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, ">");
		andOp->addChild(eqOp);
		andOp->addChild(gtOp);

		eqOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "x"));
		eqOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "y"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "z"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::CONST_VALUE, "0"));

		WhileNode* whileNode = new WhileNode(andOp, stmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

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
			   1. while ((x == y) && (z > 0)) {
				  2. print a; }
				3. while (x != z)  {
				   4. print a; }
			}
		*/
		PrintNode* printNode1 = new PrintNode("a", 2);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(printNode1);
		ExprNode* andOp = new ExprNode(ExprNodeValueType::LOGICAL_OPERATOR, "&&");
		ExprNode* eqOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* gtOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, ">");
		andOp->addChild(eqOp);
		andOp->addChild(gtOp);

		eqOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "x"));
		eqOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "y"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "z"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::CONST_VALUE, "0"));
		WhileNode* whileNode1 = new WhileNode(andOp, stmtLstNode1, 1);

		PrintNode* printNode2 = new PrintNode("a", 4);
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(printNode2);
		ExprNode* neqOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, "!=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "z");
		neqOp->addChild(leftExprNode);
		neqOp->addChild(rightExprNode);
		WhileNode* whileNode2 = new WhileNode(neqOp, stmtLstNode2, 3);

		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode1);
		outerStmtLstNode->addStmtNode(whileNode2);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

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

		std::string varName = "x";
		std::string procName = "main";
		ReadNode* readNode = new ReadNode(varName, 1);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(readNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> cfgTable = designExtractor->getCFG();
		Assert::IsTrue(size_t(0) == cfgTable.size());
	}

	TEST_METHOD(generateCFG_printStatementOnly) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				print x; } }
		*/

		std::string varName = "x";
		std::string procName = "main";
		PrintNode* printNode = new PrintNode(varName, 1);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(printNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> cfgTable = designExtractor->getCFG();
		Assert::IsTrue(size_t(0) == cfgTable.size());
	}

	TEST_METHOD(generateCFG_readAndPrintStatementOnly) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				read x;
				print y;  } }
		*/
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> expectedCfg;
		expectedCfg[stmtIdx1].insert(stmtIdx2);

		std::string varNameX = "x";
		std::string varNameY = "y";
		std::string procName = "main";

		ReadNode* readNode = new ReadNode(varNameX, 1);
		PrintNode* printNode = new PrintNode(varNameY, 2);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(readNode);
		stmtLstNode->addStmtNode(printNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> cfgTable = designExtractor->getCFG();
		Assert::IsTrue(size_t(1) == cfgTable.size());

		Assert::IsTrue(expectedCfg == cfgTable);
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
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> expectedCfg;
		expectedCfg[stmtIdx1].insert(stmtIdx2);
		expectedCfg[stmtIdx2].insert(stmtIdx3);

		std::string varNameX = "x";
		std::string varNameY = "y";
		std::string procName = "main";

		ReadNode* readNode = new ReadNode(varNameX, 1);
		PrintNode* printNode = new PrintNode(varNameY, 2);
		ExprNode* exprNode = new ExprNode(ExprNodeValueType::VAR_NAME, varNameX);
		AssignNode* assignNode = new AssignNode(varNameY, exprNode, 3);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(readNode);
		stmtLstNode->addStmtNode(printNode);
		stmtLstNode->addStmtNode(assignNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> cfgTable = designExtractor->getCFG();
		Assert::IsTrue(size_t(2) == cfgTable.size());

		Assert::IsTrue(expectedCfg == cfgTable);
	}

	TEST_METHOD(generateCFG_singleIfStatement) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   1. if (x == y) then {
				  2. print x; } else {
				   3. read y; } }
		*/
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> expectedCfg;
		expectedCfg[stmtIdx1].insert(stmtIdx2);
		expectedCfg[stmtIdx1].insert(stmtIdx3);

		PrintNode* printNode = new PrintNode("x", 2);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(printNode);
		ReadNode* readNode = new ReadNode("y", 3);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(readNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> cfgTable = designExtractor->getCFG();
		Assert::IsTrue(size_t(1) == cfgTable.size());

		Assert::IsTrue(expectedCfg == cfgTable);
	}

	TEST_METHOD(generateCFG_singleWhileStatement) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				1. while (x == y) {
					2. print x; } }
		*/
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> expectedCfg;
		expectedCfg[stmtIdx1].insert(stmtIdx2);
		expectedCfg[stmtIdx2].insert(stmtIdx1);

		PrintNode* printNode = new PrintNode("x", 2);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(printNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, stmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> cfgTable = designExtractor->getCFG();
		Assert::IsTrue(size_t(2) == cfgTable.size());

		Assert::IsTrue(expectedCfg == cfgTable);
	}

	TEST_METHOD(generateCFG_whileInWhile) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			  1. while (a <= b) {
				2.   while (x == y) {
					3.   print x; } } }
		*/
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> expectedCfg;
		expectedCfg[stmtIdx1].insert(stmtIdx2);
		expectedCfg[stmtIdx2].insert(stmtIdx3);
		expectedCfg[stmtIdx3].insert(stmtIdx2);
		expectedCfg[stmtIdx2].insert(stmtIdx1);

		PrintNode* printNode = new PrintNode("x", 3);
		StmtLstNode* innerStmtLstNode = new StmtLstNode();
		innerStmtLstNode->addStmtNode(printNode);
		ExprNode* innerRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* innerLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* innerRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		innerRootExprNode->addChild(innerLeftExprNode);
		innerRootExprNode->addChild(innerRightExprNode);
		WhileNode* innerWhileNode = new WhileNode(innerRootExprNode, innerStmtLstNode, 2);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(innerWhileNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "<=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "a");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, stmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> cfgTable = designExtractor->getCFG();
		Assert::IsTrue(size_t(3) == cfgTable.size());

		Assert::IsTrue(expectedCfg == cfgTable);
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
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		StmtIndex stmtIdx6 = StmtIndex(6);
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> expectedCfg;
		expectedCfg[stmtIdx1].insert(stmtIdx2);
		expectedCfg[stmtIdx1].insert(stmtIdx4);
		expectedCfg[stmtIdx2].insert(stmtIdx3);
		expectedCfg[stmtIdx3].insert(stmtIdx2);
		expectedCfg[stmtIdx4].insert(stmtIdx5);
		expectedCfg[stmtIdx4].insert(stmtIdx6);

		/* Handle then-block */
		PrintNode* printNode = new PrintNode("x", 3);
		StmtLstNode* innerWhileStmtLstNode = new StmtLstNode();
		innerWhileStmtLstNode->addStmtNode(printNode);
		ExprNode* whileRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* whileLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* whileRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		whileRootExprNode->addChild(whileLeftExprNode);
		whileRootExprNode->addChild(whileRightExprNode);
		WhileNode* innerWhileNode = new WhileNode(whileRootExprNode, innerWhileStmtLstNode, 2);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(innerWhileNode);

		/* Handle else-block */
		ReadNode* readNode = new ReadNode("y", 5);
		StmtLstNode* innerThenStmtLstNode = new StmtLstNode();
		innerThenStmtLstNode->addStmtNode(readNode);
		ExprNode* innerElseRootExprNode = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, "+");
		ExprNode* innerElseLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		ExprNode* innerElseRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "c");
		innerElseRootExprNode->addChild(innerElseLeftExprNode);
		innerElseRootExprNode->addChild(innerElseRightExprNode);
		AssignNode* assignNode = new AssignNode("a", innerElseRootExprNode, 6);
		StmtLstNode* innerElseStmtLstNode = new StmtLstNode();
		innerElseStmtLstNode->addStmtNode(assignNode);
		ExprNode* innerIfRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "!=");
		ExprNode* innerIfLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "hello");
		ExprNode* innerIfRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "world");
		innerIfRootExprNode->addChild(innerIfLeftExprNode);
		innerIfRootExprNode->addChild(innerIfRightExprNode);
		IfNode* innerIfNode = new IfNode(innerIfRootExprNode, innerThenStmtLstNode, innerElseStmtLstNode, 4);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(innerIfNode);

		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "<=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "a");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> cfgTable = designExtractor->getCFG();
		Assert::IsTrue(size_t(4) == cfgTable.size());

		Assert::IsTrue(expectedCfg == cfgTable);
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
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> expectedCfg;
		expectedCfg[stmtIdx1].insert(stmtIdx2);
		expectedCfg[stmtIdx2].insert(stmtIdx3);
		expectedCfg[stmtIdx2].insert(stmtIdx4);
		expectedCfg[stmtIdx3].insert(stmtIdx1);
		expectedCfg[stmtIdx4].insert(stmtIdx1);

		/* Handle then-block */
		PrintNode* printNode = new PrintNode("x", 3);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(printNode);

		/* Handle else-block */
		ReadNode* readNode = new ReadNode("y", 4);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(readNode);

		ExprNode* ifRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "<=");
		ExprNode* ifLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "a");
		ExprNode* ifRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		ifRootExprNode->addChild(ifLeftExprNode);
		ifRootExprNode->addChild(ifRightExprNode);
		IfNode* ifNode = new IfNode(ifRootExprNode, thenStmtLstNode, elseStmtLstNode, 2);
		StmtLstNode* innerStmtLstNode = new StmtLstNode();
		innerStmtLstNode->addStmtNode(ifNode);

		ExprNode* whileRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* whileLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* whileRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		whileRootExprNode->addChild(whileLeftExprNode);
		whileRootExprNode->addChild(whileRightExprNode);
		WhileNode* whileNode = new WhileNode(whileRootExprNode, innerStmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> cfgTable = designExtractor->getCFG();
		Assert::IsTrue(size_t(4) == cfgTable.size());

		Assert::IsTrue(expectedCfg == cfgTable);
	}

	TEST_METHOD(generateCFG_multIfStatements) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   1. if ((x == y) && (z > 0)) then {
			   2.    print a; } else {
			   3.   read y; }
			   4. if (x != z) then {
			   5.    print x; } else {
			   6.    read a; }
			}
		*/
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		StmtIndex stmtIdx6 = StmtIndex(6);
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> expectedCfg;
		expectedCfg[stmtIdx1].insert(stmtIdx2);
		expectedCfg[stmtIdx1].insert(stmtIdx3);
		expectedCfg[stmtIdx2].insert(stmtIdx4);
		expectedCfg[stmtIdx3].insert(stmtIdx4);
		expectedCfg[stmtIdx4].insert(stmtIdx5);
		expectedCfg[stmtIdx4].insert(stmtIdx6);

		PrintNode* printNode1 = new PrintNode("a", 2);
		StmtLstNode* thenStmtLstNode1 = new StmtLstNode();
		thenStmtLstNode1->addStmtNode(printNode1);
		ReadNode* readNode1 = new ReadNode("y", 3);
		StmtLstNode* elseStmtLstNode1 = new StmtLstNode();
		elseStmtLstNode1->addStmtNode(readNode1);
		ExprNode* andOp = new ExprNode(ExprNodeValueType::LOGICAL_OPERATOR, "&&");
		ExprNode* eqOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* gtOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, ">");
		andOp->addChild(eqOp);
		andOp->addChild(gtOp);

		eqOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "x"));
		eqOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "y"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "z"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::CONST_VALUE, "0"));
		IfNode* ifNode1 = new IfNode(andOp, thenStmtLstNode1, elseStmtLstNode1, 1);

		PrintNode* printNode2 = new PrintNode("x", 5);
		StmtLstNode* thenStmtLstNode2 = new StmtLstNode();
		thenStmtLstNode2->addStmtNode(printNode2);
		ReadNode* readNode2 = new ReadNode("a", 6);
		StmtLstNode* elseStmtLstNode2 = new StmtLstNode();
		elseStmtLstNode2->addStmtNode(readNode2);
		ExprNode* neqOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, "!=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "z");
		neqOp->addChild(leftExprNode);
		neqOp->addChild(rightExprNode);
		IfNode* ifNode2 = new IfNode(neqOp, thenStmtLstNode2, elseStmtLstNode2, 4);

		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode1);
		outerStmtLstNode->addStmtNode(ifNode2);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> cfgTable = designExtractor->getCFG();
		Assert::IsTrue(size_t(4) == cfgTable.size());

		Assert::IsTrue(expectedCfg == cfgTable);
	}

	TEST_METHOD(generateCFG_multWhileStatements) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			1.   while ((x == y) && (z > 0)) {
			2.	   print a; }
			3.	while (x != z)  {
			4.	   print a; }
			}
		*/
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> expectedCfg;
		expectedCfg[stmtIdx1].insert(stmtIdx2);
		expectedCfg[stmtIdx2].insert(stmtIdx1);
		expectedCfg[stmtIdx1].insert(stmtIdx3);
		expectedCfg[stmtIdx3].insert(stmtIdx4);
		expectedCfg[stmtIdx4].insert(stmtIdx3);

		PrintNode* printNode1 = new PrintNode("a", 2);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(printNode1);
		ExprNode* andOp = new ExprNode(ExprNodeValueType::LOGICAL_OPERATOR, "&&");
		ExprNode* eqOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* gtOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, ">");
		andOp->addChild(eqOp);
		andOp->addChild(gtOp);

		eqOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "x"));
		eqOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "y"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "z"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::CONST_VALUE, "0"));
		WhileNode* whileNode1 = new WhileNode(andOp, stmtLstNode1, 1);

		PrintNode* printNode2 = new PrintNode("a", 4);
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(printNode2);
		ExprNode* neqOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, "!=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "z");
		neqOp->addChild(leftExprNode);
		neqOp->addChild(rightExprNode);
		WhileNode* whileNode2 = new WhileNode(neqOp, stmtLstNode2, 3);

		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode1);
		outerStmtLstNode->addStmtNode(whileNode2);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> cfgTable = designExtractor->getCFG();
		Assert::IsTrue(size_t(4) == cfgTable.size());

		Assert::IsTrue(expectedCfg == cfgTable);
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
		innerWhileStmtLstNode->addStmtNode(new PrintNode(varNameX, 3));
		innerWhileStmtLstNode->addStmtNode(new ReadNode(varNameX, 4));
		ExprNode* whileRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* whileLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, varNameX);
		ExprNode* whileRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, varNameY);
		whileRootExprNode->addChild(whileLeftExprNode);
		whileRootExprNode->addChild(whileRightExprNode);
		WhileNode* innerWhileNode = new WhileNode(whileRootExprNode, innerWhileStmtLstNode, 2);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(innerWhileNode);

		/* Handle else-block */
		ReadNode* readNode = new ReadNode(varNameY, 6);
		StmtLstNode* innerThenStmtLstNode = new StmtLstNode();
		innerThenStmtLstNode->addStmtNode(readNode);
		ExprNode* innerElseRootExprNode = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, "+");
		ExprNode* innerElseLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		ExprNode* innerElseRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "c");
		innerElseRootExprNode->addChild(innerElseLeftExprNode);
		innerElseRootExprNode->addChild(innerElseRightExprNode);
		AssignNode* assignNode = new AssignNode("a", innerElseRootExprNode, 7);
		StmtLstNode* innerElseStmtLstNode = new StmtLstNode();
		innerElseStmtLstNode->addStmtNode(assignNode);
		innerElseStmtLstNode->addStmtNode(new CallNode(procName2, 8));
		ExprNode* innerIfRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "!=");
		ExprNode* innerIfLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "hello");
		ExprNode* innerIfRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "world");
		innerIfRootExprNode->addChild(innerIfLeftExprNode);
		innerIfRootExprNode->addChild(innerIfRightExprNode);
		IfNode* innerIfNode = new IfNode(innerIfRootExprNode, innerThenStmtLstNode, innerElseStmtLstNode, 5);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(innerIfNode);

		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "<=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "a");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode1 = new ProcedureNode(procName1);
		procedureNode1->addStmtLst(outerStmtLstNode);

		/* 2nd proc */
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(new ReadNode(varNameX, 9));
		ProcedureNode* procedureNode2 = new ProcedureNode(procName2);
		procedureNode2->addStmtLst(stmtLstNode);

		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode1);
		programNode->addProcedure(procedureNode2);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		/* Check stmts and attributes population */
		Assert::AreEqual(size_t(9), Entity::getAllStmts().size());

		/* read x */
		std::vector<EntityAttributeRef> expectedResultReadVarNameX{ 4, 9 };
		std::vector<EntityAttributeRef> resultReadVarNameX = Attribute::getEqualNameAttributesFromName(EntityType::READ, varNameX);
		Assert::IsTrue(expectedResultReadVarNameX == resultReadVarNameX);

		/* read y */
		std::vector<EntityAttributeRef> expectedResultReadVarNameY{ 6 };
		std::vector<EntityAttributeRef> resultReadVarNameY = Attribute::getEqualNameAttributesFromName(EntityType::READ, varNameY);
		Assert::IsTrue(expectedResultReadVarNameY == resultReadVarNameY);

		/* print x */
		std::vector<EntityAttributeRef> expectedResultPrintVarNameX{ 3 };
		std::vector<EntityAttributeRef> resultPrintVarNameX = Attribute::getEqualNameAttributesFromName(EntityType::PRINT, varNameX);
		Assert::IsTrue(expectedResultPrintVarNameX == resultPrintVarNameX);

		/* print y (does not exist) */
		std::vector<EntityAttributeRef> expectedResultPrintVarNameY{ };
		std::vector<EntityAttributeRef> resultPrintVarNameY = Attribute::getEqualNameAttributesFromName(EntityType::PRINT, varNameY);
		Assert::IsTrue(expectedResultPrintVarNameY == resultPrintVarNameY);

		/* call proc2 */
		std::vector<EntityAttributeRef> expectedResultCallProcName2{ 8 };
		std::vector<EntityAttributeRef> resultCallProcName2 = Attribute::getEqualNameAttributesFromName(EntityType::CALL, procName2);
		Assert::IsTrue(expectedResultCallProcName2 == resultCallProcName2);
	}

	TEST_METHOD(extract_readAndPrintStatementOnly_nextCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				read x;
				print y;  } }
		*/
		std::string varNameX = "x";
		std::string varNameY = "y";
		std::string procName = "main";

		ReadNode* readNode = new ReadNode(varNameX, 1);
		PrintNode* printNode = new PrintNode(varNameY, 2);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(readNode);
		stmtLstNode->addStmtNode(printNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx2));
		Assert::AreEqual(size_t(1), std::get<0>(Next::getAllInfo()).size());
	}

	TEST_METHOD(extract_multipleStatements_nextCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				read x;
				print y;
				y = x  } }
		*/
		std::string varNameX = "x";
		std::string varNameY = "y";
		std::string procName = "main";

		ReadNode* readNode = new ReadNode(varNameX, 1);
		PrintNode* printNode = new PrintNode(varNameY, 2);
		ExprNode* exprNode = new ExprNode(ExprNodeValueType::VAR_NAME, varNameX);
		AssignNode* assignNode = new AssignNode(varNameY, exprNode, 3);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(readNode);
		stmtLstNode->addStmtNode(printNode);
		stmtLstNode->addStmtNode(assignNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::contains(stmtIdx2, stmtIdx3));
		Assert::AreEqual(size_t(2), std::get<0>(Next::getAllInfo()).size());
	}

	TEST_METHOD(extract_singleIfStatement_nextCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			  1. if (x == y) then {
				2.   print x; } else {
				  3. read y; } }
		*/
		PrintNode* printNode = new PrintNode("x", 2);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(printNode);
		ReadNode* readNode = new ReadNode("y", 3);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(readNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx3));
		Assert::AreEqual(size_t(2), std::get<0>(Next::getAllInfo()).size());
	}

	TEST_METHOD(extract_singleWhileStatement_nextCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				1. while (x == y) {
					2. print x; } }
		*/
		PrintNode* printNode = new PrintNode("x", 2);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(printNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, stmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::contains(stmtIdx2, stmtIdx1));
		Assert::AreEqual(size_t(2), std::get<0>(Next::getAllInfo()).size());
	}

	TEST_METHOD(extract_whileInWhile_nextCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   1. while (a <= b) {
				  2. while (x == y) {
					3.   print x; } } }
		*/

		PrintNode* printNode = new PrintNode("x", 3);
		StmtLstNode* innerStmtLstNode = new StmtLstNode();
		innerStmtLstNode->addStmtNode(printNode);
		ExprNode* innerRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* innerLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* innerRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		innerRootExprNode->addChild(innerLeftExprNode);
		innerRootExprNode->addChild(innerRightExprNode);
		WhileNode* innerWhileNode = new WhileNode(innerRootExprNode, innerStmtLstNode, 2);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(innerWhileNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "<=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "a");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, stmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::contains(stmtIdx2, stmtIdx1));
		Assert::IsTrue(Next::contains(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Next::contains(stmtIdx3, stmtIdx2));
		Assert::AreEqual(size_t(4), std::get<0>(Next::getAllInfo()).size());
	}

	TEST_METHOD(extract_whileAndIfInIf_nextCaptured) {
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
		PrintNode* printNode = new PrintNode("x", 3);
		StmtLstNode* innerWhileStmtLstNode = new StmtLstNode();
		innerWhileStmtLstNode->addStmtNode(printNode);
		ExprNode* whileRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* whileLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* whileRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		whileRootExprNode->addChild(whileLeftExprNode);
		whileRootExprNode->addChild(whileRightExprNode);
		WhileNode* innerWhileNode = new WhileNode(whileRootExprNode, innerWhileStmtLstNode, 2);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(innerWhileNode);

		/* Handle else-block */
		ReadNode* readNode = new ReadNode("y", 5);
		StmtLstNode* innerThenStmtLstNode = new StmtLstNode();
		innerThenStmtLstNode->addStmtNode(readNode);
		ExprNode* innerElseRootExprNode = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, "+");
		ExprNode* innerElseLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		ExprNode* innerElseRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "c");
		innerElseRootExprNode->addChild(innerElseLeftExprNode);
		innerElseRootExprNode->addChild(innerElseRightExprNode);
		AssignNode* assignNode = new AssignNode("a", innerElseRootExprNode, 6);
		StmtLstNode* innerElseStmtLstNode = new StmtLstNode();
		innerElseStmtLstNode->addStmtNode(assignNode);
		ExprNode* innerIfRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "!=");
		ExprNode* innerIfLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "hello");
		ExprNode* innerIfRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "world");
		innerIfRootExprNode->addChild(innerIfLeftExprNode);
		innerIfRootExprNode->addChild(innerIfRightExprNode);
		IfNode* innerIfNode = new IfNode(innerIfRootExprNode, innerThenStmtLstNode, innerElseStmtLstNode, 4);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(innerIfNode);

		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "<=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "a");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		StmtIndex stmtIdx6 = StmtIndex(6);
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx4));
		Assert::IsTrue(Next::contains(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Next::contains(stmtIdx3, stmtIdx2));
		Assert::IsTrue(Next::contains(stmtIdx4, stmtIdx5));
		Assert::IsTrue(Next::contains(stmtIdx4, stmtIdx6));
		Assert::AreEqual(size_t(6), std::get<0>(Next::getAllInfo()).size());
	}

	TEST_METHOD(extract_ifInWhile_nextCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   1. while (x == y) {
				   2. if (a <= b) then {
					   3. print x; } else {
					   4. read y; } } }
		*/

		/* Handle then-block */
		PrintNode* printNode = new PrintNode("x", 3);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(printNode);

		/* Handle else-block */
		ReadNode* readNode = new ReadNode("y", 4);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(readNode);

		ExprNode* ifRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "<=");
		ExprNode* ifLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "a");
		ExprNode* ifRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		ifRootExprNode->addChild(ifLeftExprNode);
		ifRootExprNode->addChild(ifRightExprNode);
		IfNode* ifNode = new IfNode(ifRootExprNode, thenStmtLstNode, elseStmtLstNode, 2);
		StmtLstNode* innerStmtLstNode = new StmtLstNode();
		innerStmtLstNode->addStmtNode(ifNode);

		ExprNode* whileRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* whileLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* whileRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		whileRootExprNode->addChild(whileLeftExprNode);
		whileRootExprNode->addChild(whileRightExprNode);
		WhileNode* whileNode = new WhileNode(whileRootExprNode, innerStmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::contains(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Next::contains(stmtIdx2, stmtIdx4));
		Assert::IsTrue(Next::contains(stmtIdx3, stmtIdx1));
		Assert::IsTrue(Next::contains(stmtIdx4, stmtIdx1));
		Assert::AreEqual(size_t(5), std::get<0>(Next::getAllInfo()).size());
	}

	TEST_METHOD(extract_multIfStatements_nextCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   1. if ((x == y) && (z > 0)) then {
				  2. print a; } else {
				   3. read y; }
				4. if (x != z) then {
				   5. print x; } else {
				   6. read a; }
			}
		*/

		PrintNode* printNode1 = new PrintNode("a", 2);
		StmtLstNode* thenStmtLstNode1 = new StmtLstNode();
		thenStmtLstNode1->addStmtNode(printNode1);
		ReadNode* readNode1 = new ReadNode("y", 3);
		StmtLstNode* elseStmtLstNode1 = new StmtLstNode();
		elseStmtLstNode1->addStmtNode(readNode1);
		ExprNode* andOp = new ExprNode(ExprNodeValueType::LOGICAL_OPERATOR, "&&");
		ExprNode* eqOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* gtOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, ">");
		andOp->addChild(eqOp);
		andOp->addChild(gtOp);

		eqOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "x"));
		eqOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "y"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "z"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::CONST_VALUE, "0"));
		IfNode* ifNode1 = new IfNode(andOp, thenStmtLstNode1, elseStmtLstNode1, 1);

		PrintNode* printNode2 = new PrintNode("x", 5);
		StmtLstNode* thenStmtLstNode2 = new StmtLstNode();
		thenStmtLstNode2->addStmtNode(printNode2);
		ReadNode* readNode2 = new ReadNode("a", 6);
		StmtLstNode* elseStmtLstNode2 = new StmtLstNode();
		elseStmtLstNode2->addStmtNode(readNode2);
		ExprNode* neqOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, "!=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "z");
		neqOp->addChild(leftExprNode);
		neqOp->addChild(rightExprNode);
		IfNode* ifNode2 = new IfNode(neqOp, thenStmtLstNode2, elseStmtLstNode2, 4);

		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode1);
		outerStmtLstNode->addStmtNode(ifNode2);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		StmtIndex stmtIdx6 = StmtIndex(6);
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx3));
		Assert::IsTrue(Next::contains(stmtIdx2, stmtIdx4));
		Assert::IsTrue(Next::contains(stmtIdx3, stmtIdx4));
		Assert::IsTrue(Next::contains(stmtIdx4, stmtIdx5));
		Assert::IsTrue(Next::contains(stmtIdx4, stmtIdx6));
		Assert::AreEqual(size_t(6), std::get<0>(Next::getAllInfo()).size());
	}

	TEST_METHOD(extract_multWhileStatements_nextCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			  1. while ((x == y) && (z > 0)) {
				2.   print a; }
				3. while (x != z)  {
				   4. print a; }
			}
		*/

		PrintNode* printNode1 = new PrintNode("a", 2);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(printNode1);
		ExprNode* andOp = new ExprNode(ExprNodeValueType::LOGICAL_OPERATOR, "&&");
		ExprNode* eqOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* gtOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, ">");
		andOp->addChild(eqOp);
		andOp->addChild(gtOp);

		eqOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "x"));
		eqOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "y"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "z"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::CONST_VALUE, "0"));
		WhileNode* whileNode1 = new WhileNode(andOp, stmtLstNode1, 1);

		PrintNode* printNode2 = new PrintNode("a", 4);
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(printNode2);
		ExprNode* neqOp = new ExprNode(ExprNodeValueType::REL_OPERATOR, "!=");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "z");
		neqOp->addChild(leftExprNode);
		neqOp->addChild(rightExprNode);
		WhileNode* whileNode2 = new WhileNode(neqOp, stmtLstNode2, 3);

		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode1);
		outerStmtLstNode->addStmtNode(whileNode2);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::contains(stmtIdx2, stmtIdx1));
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx3));
		Assert::IsTrue(Next::contains(stmtIdx3, stmtIdx4));
		Assert::IsTrue(Next::contains(stmtIdx4, stmtIdx3));
		Assert::AreEqual(size_t(5), std::get<0>(Next::getAllInfo()).size());
	}

	TEST_METHOD(extract_ifStatementThenNonContainerStatments_nextCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   1. if (x == y) then {
				  2. print x; } else {
				   3. read y; }
				4. read x;
				5. print y;}
		*/

		PrintNode* ifPrintNode = new PrintNode("x", 2);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(ifPrintNode);
		ReadNode* ifReadNode = new ReadNode("y", 3);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(ifReadNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();

		ReadNode* readNode = new ReadNode("x", 4);
		PrintNode* printNode = new PrintNode("y", 5);

		outerStmtLstNode->addStmtNode(ifNode);
		outerStmtLstNode->addStmtNode(readNode);
		outerStmtLstNode->addStmtNode(printNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx3));
		Assert::IsTrue(Next::contains(stmtIdx2, stmtIdx4));
		Assert::IsTrue(Next::contains(stmtIdx3, stmtIdx4));
		Assert::IsTrue(Next::contains(stmtIdx4, stmtIdx5));

		Assert::AreEqual(size_t(5), std::get<0>(Next::getAllInfo()).size());
	}

	TEST_METHOD(extract_whileStatementThenNonContainerStatments_nextCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				1. while (x == y) {
					2. print x; }
				3. read a;
				4. a = b + c}
		*/
		PrintNode* printNode = new PrintNode("x", 2);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(printNode);
		ExprNode* whileRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* whileLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* whileRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		whileRootExprNode->addChild(whileLeftExprNode);
		whileRootExprNode->addChild(whileRightExprNode);
		WhileNode* whileNode = new WhileNode(whileRootExprNode, stmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();

		ReadNode* readNode = new ReadNode("a", 3);

		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, "+");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "c");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		AssignNode* assignNode = new AssignNode("a", rootExprNode, 4);

		outerStmtLstNode->addStmtNode(whileNode);
		outerStmtLstNode->addStmtNode(readNode);
		outerStmtLstNode->addStmtNode(assignNode);

		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::contains(stmtIdx2, stmtIdx1));
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx3));
		Assert::IsTrue(Next::contains(stmtIdx3, stmtIdx4));
		Assert::AreEqual(size_t(4), std::get<0>(Next::getAllInfo()).size());
	}

	TEST_METHOD(extract_ifStatementMultipleStatementsInStmtLst_nextCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   1. if (x == y) then {
				  2. print x;
				   3. a = b + c;} else {
				  4. read y;
				   5. print a; } }
		*/
		PrintNode* ifPrintNode = new PrintNode("x", 2);

		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, "+");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "c");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		AssignNode* assignNode = new AssignNode("a", rootExprNode, 3);

		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(ifPrintNode);
		thenStmtLstNode->addStmtNode(assignNode);

		ReadNode* readNode = new ReadNode("y", 5);
		PrintNode* thenPrintNode = new PrintNode("a", 6);

		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(readNode);
		elseStmtLstNode->addStmtNode(thenPrintNode);

		ExprNode* ifRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* ifLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* ifRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		ifRootExprNode->addChild(ifLeftExprNode);
		ifRootExprNode->addChild(ifRightExprNode);
		IfNode* ifNode = new IfNode(ifRootExprNode, thenStmtLstNode, elseStmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::contains(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx4));
		Assert::IsTrue(Next::contains(stmtIdx4, stmtIdx5));
		Assert::AreEqual(size_t(4), std::get<0>(Next::getAllInfo()).size());
	}

	TEST_METHOD(extract_whileStatementMultipleStatementsInStmtLst_nextCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				1. while (x == y) {
					2. read x;
					3. a = b + c;
					4. print y;} }
		*/
		ReadNode* readNode = new ReadNode("x", 2);

		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, "+");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "c");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		AssignNode* assignNode = new AssignNode("a", rootExprNode, 3);

		PrintNode* printNode = new PrintNode("y", 4);

		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(readNode);
		stmtLstNode->addStmtNode(assignNode);
		stmtLstNode->addStmtNode(printNode);

		ExprNode* whileRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* whileLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* whileRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		whileRootExprNode->addChild(whileLeftExprNode);
		whileRootExprNode->addChild(whileRightExprNode);
		WhileNode* whileNode = new WhileNode(whileRootExprNode, stmtLstNode, 1);
		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::contains(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Next::contains(stmtIdx3, stmtIdx4));
		Assert::IsTrue(Next::contains(stmtIdx4, stmtIdx1));
		Assert::AreEqual(size_t(4), std::get<0>(Next::getAllInfo()).size());
	}

	TEST_METHOD(extract_multipleProc_nextCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				1. read x;
				2. print y;  } }

		   procedure helper {
				3. a = b + c;
				4. print x;  } }
		*/
		ReadNode* readNode = new ReadNode("x", 1);
		PrintNode* firstPrintNode = new PrintNode("y", 2);

		StmtLstNode* firstStmtLstNode = new StmtLstNode();
		firstStmtLstNode->addStmtNode(readNode);
		firstStmtLstNode->addStmtNode(firstPrintNode);
		ProcedureNode* firstProcedureNode = new ProcedureNode("main");
		firstProcedureNode->addStmtLst(firstStmtLstNode);

		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, "+");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "c");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		AssignNode* assignNode = new AssignNode("a", rootExprNode, 3);
		PrintNode* secondPrintNode = new PrintNode("x", 4);

		StmtLstNode* secondStmtLstNode = new StmtLstNode();
		secondStmtLstNode->addStmtNode(assignNode);
		secondStmtLstNode->addStmtNode(secondPrintNode);
		ProcedureNode* secondProcedureNode = new ProcedureNode("helper");
		secondProcedureNode->addStmtLst(secondStmtLstNode);

		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(firstProcedureNode);
		programNode->addProcedure(secondProcedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::contains(stmtIdx3, stmtIdx4));
		Assert::AreEqual(size_t(2), std::get<0>(Next::getAllInfo()).size());
	}

	TEST_METHOD(extract_mixedCode_nextCaptured) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			   1. while (x == y) {
				   2. if (a <= b) then {
					   3. print x; } else {
					   4. read y;
					   5. a = b + c; }
				   6. print y; }
			   7. a = b + c; }
		*/

		/* Handle then-block */
		PrintNode* ifPrintNode = new PrintNode("x", 3);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(ifPrintNode);

		/* Handle else-block */
		ReadNode* ifReadNode = new ReadNode("y", 4);
		ExprNode* ifRootExprNode = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, "+");
		ExprNode* ifLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		ExprNode* ifRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "c");
		ifRootExprNode->addChild(ifLeftExprNode);
		ifRootExprNode->addChild(ifRightExprNode);
		AssignNode* ifAssignNode = new AssignNode("a", ifRootExprNode, 5);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(ifReadNode);
		elseStmtLstNode->addStmtNode(ifAssignNode);

		ExprNode* ifRootCondExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "<=");
		ExprNode* ifLeftCondExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "a");
		ExprNode* ifRightCondExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		ifRootCondExprNode->addChild(ifLeftCondExprNode);
		ifRootCondExprNode->addChild(ifRightCondExprNode);
		IfNode* ifNode = new IfNode(ifRootCondExprNode, thenStmtLstNode, elseStmtLstNode, 2);

		PrintNode* whilePrintNode = new PrintNode("y", 6);

		StmtLstNode* innerStmtLstNode = new StmtLstNode();
		innerStmtLstNode->addStmtNode(ifNode);
		innerStmtLstNode->addStmtNode(whilePrintNode);

		ExprNode* whileRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* whileLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* whileRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		whileRootExprNode->addChild(whileLeftExprNode);
		whileRootExprNode->addChild(whileRightExprNode);
		WhileNode* whileNode = new WhileNode(whileRootExprNode, innerStmtLstNode, 1);

		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, "+");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "b");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "c");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		AssignNode* assignNode = new AssignNode("a", rootExprNode, 7);

		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(whileNode);
		outerStmtLstNode->addStmtNode(assignNode);

		ProcedureNode* procedureNode = new ProcedureNode("main");
		procedureNode->addStmtLst(outerStmtLstNode);
		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		StmtIndex stmtIdx6 = StmtIndex(6);
		StmtIndex stmtIdx7 = StmtIndex(7);
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::contains(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Next::contains(stmtIdx2, stmtIdx4));
		Assert::IsTrue(Next::contains(stmtIdx3, stmtIdx6));
		Assert::IsTrue(Next::contains(stmtIdx4, stmtIdx5));
		Assert::IsTrue(Next::contains(stmtIdx5, stmtIdx6));
		Assert::IsTrue(Next::contains(stmtIdx6, stmtIdx1));
		Assert::IsTrue(Next::contains(stmtIdx1, stmtIdx7));
		Assert::AreEqual(size_t(8), std::get<0>(Next::getAllInfo()).size());
	}

	TEST_METHOD(extract_insertCalls_callsAndCallsTCaptured) {
		/* AST is equivalent to the SIMPLE program
			procedure proc1 {
			1. while (x==y) {
				2. if (x==y) then {
					3. read x; } else {
				  4. call proc2; }}
			}
			procedure proc2 {
				   5. call proc3;
				   6. call proc3; }
			}
			procedure proc3 {
				7. read proc1; }
			procedure proc4 {
				8. read proc1; }
			procedure proc5 {
				9. if ((x == y) && (z > 0)) then {
				   10. print a; } else {
				   11. read y; }
				12. if (x != z) then {
				   13. call proc4; } else {
				   14. read a; }
			}
		*/

		std::string procName1 = "proc1";
		std::string procName2 = "proc2";
		std::string procName3 = "proc3";
		std::string procName4 = "proc4";
		std::string procName5 = "proc5";

		/* proc1 -> proc2 */
		/* procedure proc1 {
			1. while (x==y) {
				2. if (x==y) then {
					3. read x; } else {
				  4. call proc2; }}
			}
		*/
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(new CallNode(procName2, 4));
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(new ReadNode("x", 3));
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode1 = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode, 2);
		StmtLstNode* whileStmtLstNode = new StmtLstNode();
		whileStmtLstNode->addStmtNode(ifNode1);
		WhileNode* whileNode = new WhileNode(rootExprNode, whileStmtLstNode, 1);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(whileNode);
		ProcedureNode* procedureNode1 = new ProcedureNode(procName1);
		procedureNode1->addStmtLst(stmtLstNode1);

		/* proc2 -> proc3 */
		/* procedure proc2 {
				   5. call proc3;
				   6. call proc3; }
			}
		*/
		CallNode* callNode = new CallNode(procName3, 5);
		CallNode* callNode1 = new CallNode(procName3, 6);
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(callNode);
		stmtLstNode2->addStmtNode(callNode1);
		ProcedureNode* procedureNode2 = new ProcedureNode(procName2);
		procedureNode2->addStmtLst(stmtLstNode2);

		/* procedure proc3 {
				7. read proc1; }
		*/
		StmtLstNode* stmtLstNode3 = new StmtLstNode();
		stmtLstNode3->addStmtNode(new ReadNode(procName1, 7));
		ProcedureNode* procedureNode3 = new ProcedureNode(procName3);
		procedureNode3->addStmtLst(stmtLstNode3);

		/* procedure proc4 {
				8. read proc1; }
		*/
		StmtLstNode* stmtLstNode4 = new StmtLstNode();
		stmtLstNode4->addStmtNode(new ReadNode(procName1, 8));
		ProcedureNode* procedureNode4 = new ProcedureNode(procName4);
		procedureNode4->addStmtLst(stmtLstNode4);

		/* proc5 -> proc4 */
		/* procedure proc5 {
				9. if ((x == y) && (z > 0)) then {
				   10. print a; } else {
				   11. read y; }
				12. if (x != z) then {
				   13. call proc4; } else {
				   14. read a; }
			}
		*/
		PrintNode* printNode1 = new PrintNode("a", 10);
		StmtLstNode* thenStmtLstNode1 = new StmtLstNode();
		thenStmtLstNode1->addStmtNode(printNode1);
		ReadNode* readNode1 = new ReadNode("y", 11);
		StmtLstNode* elseStmtLstNode1 = new StmtLstNode();
		elseStmtLstNode1->addStmtNode(readNode1);
		ExprNode* rootExprNode2 = new ExprNode(ExprNodeValueType::LOGICAL_OPERATOR, "&&");
		ExprNode* leftExprNode2 = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* rightExprNode2 = new ExprNode(ExprNodeValueType::REL_OPERATOR, ">");
		rootExprNode2->addChild(leftExprNode2);
		rootExprNode2->addChild(rightExprNode2);

		leftExprNode2->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "x"));
		leftExprNode2->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "y"));
		rightExprNode2->addChild(new ExprNode(ExprNodeValueType::VAR_NAME, "z"));
		rightExprNode2->addChild(new ExprNode(ExprNodeValueType::CONST_VALUE, "0"));
		IfNode* ifNode2 = new IfNode(rootExprNode2, thenStmtLstNode1, elseStmtLstNode1, 9);

		StmtLstNode* thenStmtLstNode2 = new StmtLstNode();
		thenStmtLstNode2->addStmtNode(new CallNode(procName4, 13));
		ReadNode* readNode2 = new ReadNode("a", 14);
		StmtLstNode* elseStmtLstNode2 = new StmtLstNode();
		elseStmtLstNode2->addStmtNode(readNode2);
		ExprNode* rootExprNode3 = new ExprNode(ExprNodeValueType::REL_OPERATOR, "!=");
		ExprNode* leftExprNode3 = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode3 = new ExprNode(ExprNodeValueType::VAR_NAME, "z");
		rootExprNode3->addChild(leftExprNode3);
		rootExprNode3->addChild(rightExprNode3);
		IfNode* ifNode3 = new IfNode(rootExprNode3, thenStmtLstNode2, elseStmtLstNode2, 12);

		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode2);
		outerStmtLstNode->addStmtNode(ifNode3);
		ProcedureNode* procedureNode5 = new ProcedureNode(procName5);
		procedureNode5->addStmtLst(outerStmtLstNode);

		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode1);
		programNode->addProcedure(procedureNode2);
		programNode->addProcedure(procedureNode3);
		programNode->addProcedure(procedureNode4);
		programNode->addProcedure(procedureNode5);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		/* Check Calls population */
		/* We expect three Calls relationships to be captured: (1, 2), (2, 3), and (5, 4). */
		Assert::AreEqual(size_t(3), std::get<0>(Calls::getAllInfo()).size());

		/* We expect four CallsT relationships to be captured:
		   (1, 2), (1, 3), (2, 3), and (5, 4). */
		Assert::AreEqual(size_t(4), std::get<0>(CallsT::getAllInfo()).size());
	}

	TEST_METHOD(extract_singleProc_insertStmtFromProc_usesPAndModifiesPCaptured) {
		/* AST is equivalent to the SIMPLE program
			procedure proc1 {
			1. while (u==v) {
				2. a = b + c;
				3. if (m==n) then {
					4. read x; } else {
				   5. print y; }}
			6. read y;
			}
		*/

		std::string procName = "proc1";
		std::string varNameU = "u";
		std::string varNameV = "v";
		std::string varNameA = "a";
		std::string varNameB = "b";
		std::string varNameC = "c";
		std::string varNameM = "m";
		std::string varNameN = "n";
		std::string varNameX = "x";
		std::string varNameY = "y";

		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(new PrintNode(varNameY, 5));
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(new ReadNode(varNameX, 4));

		ExprNode* ifRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* ifLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, varNameM);
		ExprNode* ifRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, varNameN);
		ifRootExprNode->addChild(ifLeftExprNode);
		ifRootExprNode->addChild(ifRightExprNode);
		IfNode* ifNode = new IfNode(ifRootExprNode, thenStmtLstNode, elseStmtLstNode, 3);

		ExprNode* assignRootExprNode = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, "+");
		ExprNode* assignLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, varNameB);
		ExprNode* assignRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, varNameC);
		assignRootExprNode->addChild(assignLeftExprNode);
		assignRootExprNode->addChild(assignRightExprNode);
		AssignNode* assignNode = new AssignNode("a", assignRootExprNode, 2);

		StmtLstNode* whileStmtLstNode = new StmtLstNode();
		whileStmtLstNode->addStmtNode(assignNode);
		whileStmtLstNode->addStmtNode(ifNode);

		ExprNode* whileRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* whileLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, varNameU);
		ExprNode* whileRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, varNameV);
		whileRootExprNode->addChild(whileLeftExprNode);
		whileRootExprNode->addChild(whileRightExprNode);
		WhileNode* whileNode = new WhileNode(whileRootExprNode, whileStmtLstNode, 1);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(whileNode);
		stmtLstNode->addStmtNode(new ReadNode(varNameY, 6));

		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);

		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		ProcIndex procIndex = Entity::getProcIdx(procName);

		VarIndex varIndexU = Entity::getVarIdx(varNameU);
		VarIndex varIndexV = Entity::getVarIdx(varNameV);
		VarIndex varIndexA = Entity::getVarIdx(varNameA);
		VarIndex varIndexB = Entity::getVarIdx(varNameB);
		VarIndex varIndexC = Entity::getVarIdx(varNameC);
		VarIndex varIndexM = Entity::getVarIdx(varNameM);
		VarIndex varIndexN = Entity::getVarIdx(varNameN);
		VarIndex varIndexX = Entity::getVarIdx(varNameX);
		VarIndex varIndexY = Entity::getVarIdx(varNameY);

		/* Check UsesP (vars might not be in order) */
		/* Vars used: u, v, b, c, m, n, y */
		std::vector<VarIndex> expectedResultUsesP{ varIndexU, varIndexV, varIndexB, varIndexC, varIndexM, varIndexN, varIndexY };
		std::vector<EntityAttributeRef> resultUsesP = UsesP::getFromLeftArg(procIndex);
		Assert::AreEqual(size_t(7), resultUsesP.size());

		for (VarIndex varIndex : resultUsesP) {
			Assert::IsTrue(std::find(expectedResultUsesP.begin(), expectedResultUsesP.end(), varIndex) != expectedResultUsesP.end());
		}

		/* Check ModifiesP (vars might not be in order) */
		/* Vars modified: a, x, y */
		std::vector<VarIndex> expectedResultModifiesP{ varIndexA, varIndexX, varIndexY };
		std::vector<EntityAttributeRef> resultModifiesP = ModifiesP::getFromLeftArg(procIndex);
		Assert::AreEqual(size_t(3), resultModifiesP.size());

		for (VarIndex varIndex : resultModifiesP) {
			Assert::IsTrue(std::find(expectedResultModifiesP.begin(), expectedResultModifiesP.end(), varIndex) != expectedResultModifiesP.end());
		}
	}

	TEST_METHOD(extract_multProcs_insertStmtFromProc_usesPAndModifiesPCaptured) {
		/* AST is equivalent to the SIMPLE program
			procedure proc1 {
			1. while (u==v) {
				2. a = b + c;
				3. if (m==n) then {
					4. read x; } else {
				   5. print y; }}
			6. read y;
			}

			procedure proc2 {
				7. read u;
				8. a = b + c;
			}
		*/

		std::string procName1 = "proc1";
		std::string procName2 = "proc2";
		std::string varNameU = "u";
		std::string varNameV = "v";
		std::string varNameA = "a";
		std::string varNameB = "b";
		std::string varNameC = "c";
		std::string varNameM = "m";
		std::string varNameN = "n";
		std::string varNameX = "x";
		std::string varNameY = "y";

		/* 1st proc */
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(new PrintNode(varNameY, 5));
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(new ReadNode(varNameX, 4));

		ExprNode* ifRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* ifLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, varNameM);
		ExprNode* ifRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, varNameN);
		ifRootExprNode->addChild(ifLeftExprNode);
		ifRootExprNode->addChild(ifRightExprNode);
		IfNode* ifNode = new IfNode(ifRootExprNode, thenStmtLstNode, elseStmtLstNode, 3);

		ExprNode* assignRootExprNode = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, "+");
		ExprNode* assignLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, varNameB);
		ExprNode* assignRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, varNameC);
		assignRootExprNode->addChild(assignLeftExprNode);
		assignRootExprNode->addChild(assignRightExprNode);
		AssignNode* assignNode = new AssignNode("a", assignRootExprNode, 2);

		StmtLstNode* whileStmtLstNode = new StmtLstNode();
		whileStmtLstNode->addStmtNode(assignNode);
		whileStmtLstNode->addStmtNode(ifNode);

		ExprNode* whileRootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* whileLeftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, varNameU);
		ExprNode* whileRightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, varNameV);
		whileRootExprNode->addChild(whileLeftExprNode);
		whileRootExprNode->addChild(whileRightExprNode);
		WhileNode* whileNode = new WhileNode(whileRootExprNode, whileStmtLstNode, 1);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(whileNode);
		stmtLstNode1->addStmtNode(new ReadNode(varNameY, 6));

		ProcedureNode* procedureNode1 = new ProcedureNode(procName1);
		procedureNode1->addStmtLst(stmtLstNode1);

		/* 2nd proc */
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(new ReadNode(varNameU, 7));
		stmtLstNode2->addStmtNode(new AssignNode("a", assignRootExprNode, 8));

		ProcedureNode* procedureNode2 = new ProcedureNode(procName2);
		procedureNode2->addStmtLst(stmtLstNode2);

		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode1);
		programNode->addProcedure(procedureNode2);
		SourceAST ast(programNode);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor* designExtractor = new DesignExtractor();
		designExtractor->extract(ast, pkbInserter);

		ProcIndex procIndex1 = Entity::getProcIdx(procName1);
		ProcIndex procIndex2 = Entity::getProcIdx(procName2);

		VarIndex varIndexU = Entity::getVarIdx(varNameU);
		VarIndex varIndexV = Entity::getVarIdx(varNameV);
		VarIndex varIndexA = Entity::getVarIdx(varNameA);
		VarIndex varIndexB = Entity::getVarIdx(varNameB);
		VarIndex varIndexC = Entity::getVarIdx(varNameC);
		VarIndex varIndexM = Entity::getVarIdx(varNameM);
		VarIndex varIndexN = Entity::getVarIdx(varNameN);
		VarIndex varIndexX = Entity::getVarIdx(varNameX);
		VarIndex varIndexY = Entity::getVarIdx(varNameY);

		/* Check UsesP for 1st proc (vars might not be in order) */
		/* Vars used: u, v, b, c, m, n, y */
		std::vector<VarIndex> expectedResultUsesP1{ varIndexU, varIndexV, varIndexB, varIndexC, varIndexM, varIndexN, varIndexY };
		std::vector<EntityAttributeRef> resultUsesP1 = UsesP::getFromLeftArg(procIndex1);
		Assert::AreEqual(size_t(7), resultUsesP1.size());
		for (VarIndex varIndex : resultUsesP1) {
			Assert::IsTrue(std::find(expectedResultUsesP1.begin(), expectedResultUsesP1.end(), varIndex) != expectedResultUsesP1.end());
		}

		/* Check ModifiesP for 1st proc (vars might not be in order) */
		/* Vars modified: a, x, y */
		std::vector<VarIndex> expectedResultModifiesP1{ varIndexA, varIndexX, varIndexY };
		std::vector<EntityAttributeRef> resultModifiesP1 = ModifiesP::getFromLeftArg(procIndex1);
		Assert::AreEqual(size_t(3), resultModifiesP1.size());
		for (VarIndex varIndex : resultModifiesP1) {
			Assert::IsTrue(std::find(expectedResultModifiesP1.begin(), expectedResultModifiesP1.end(), varIndex) != expectedResultModifiesP1.end());
		}

		/* Check UsesP for 2nd proc (vars might not be in order) */
		/* Vars used: b, c */
		std::vector<VarIndex> expectedResultUsesP2{ varIndexB, varIndexC };
		std::vector<EntityAttributeRef> resultUsesP2 = UsesP::getFromLeftArg(procIndex2);
		Assert::AreEqual(size_t(2), resultUsesP2.size());
		for (VarIndex varIndex : resultUsesP2) {
			Assert::IsTrue(std::find(expectedResultUsesP2.begin(), expectedResultUsesP2.end(), varIndex) != expectedResultUsesP2.end());
		}

		/* Check ModifiesP for 2nd proc (vars might not be in order) */
		/* Vars modified: u, a */
		std::vector<VarIndex> expectedResultModifiesP2{ varIndexU, varIndexA };
		std::vector<EntityAttributeRef> resultModifiesP2 = ModifiesP::getFromLeftArg(procIndex2);
		Assert::AreEqual(size_t(2), resultModifiesP2.size());
		for (VarIndex varIndex : resultModifiesP2) {
			Assert::IsTrue(std::find(expectedResultModifiesP2.begin(), expectedResultModifiesP2.end(), varIndex) != expectedResultModifiesP2.end());
		}
	}
	};
}
