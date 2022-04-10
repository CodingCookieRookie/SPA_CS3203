#include "stdafx.h"
#include "CppUnitTest.h"

#include "SP/ASTValidator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestASTValidator) {
public:
	TEST_METHOD(validateAST_duplicateProc_ASTExceptionThrown) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				   1. print a; }
			procedure main {
				   2. read a; }
			}
		*/
		PrintNode* printNode = new PrintNode("a", 1);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(printNode);
		ProcedureNode* procedureNode1 = new ProcedureNode("main");
		procedureNode1->addStmtLst(stmtLstNode1);

		ReadNode* readNode = new ReadNode("a", 2);
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(readNode);
		ProcedureNode* procedureNode2 = new ProcedureNode("main");
		procedureNode2->addStmtLst(stmtLstNode2);

		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode1);
		programNode->addProcedure(procedureNode2);
		SourceAST ast(programNode);

		auto wrapperFunc = [&ast] { ASTValidator::validateAST(ast); };
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			ASTValidator::validateAST(ast);
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::DUPLICATE_PROC_NAMES_ERROR.c_str(), ex.what());
		}
	}

	TEST_METHOD(validateAST_callNonExistentProcedure_noContainerStmt_ASTExceptionThrown) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				   1. call proc1; }
			procedure proc {
				   2. read a; }
			}
		*/
		CallNode* callNode = new CallNode("proc1", 1);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(callNode);
		ProcedureNode* procedureNode1 = new ProcedureNode("main");
		procedureNode1->addStmtLst(stmtLstNode1);

		ReadNode* readNode = new ReadNode("a", 2);
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(readNode);
		ProcedureNode* procedureNode2 = new ProcedureNode("proc");
		procedureNode2->addStmtLst(stmtLstNode2);

		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode1);
		programNode->addProcedure(procedureNode2);
		SourceAST ast(programNode);

		auto wrapperFunc = [&ast] { ASTValidator::validateAST(ast); };
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			ASTValidator::validateAST(ast);
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CALL_NON_EXISTENT_PROC_NAME_ERROR.c_str(), ex.what());
		}
	}

	TEST_METHOD(validateAST_callNonExistentProcedure_insideOneContainerStmt_ASTExceptionThrown) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			1. while (x==y) {
				2.   call proc1; }
			}
			procedure proc {
				  3. read a; }
			}
		*/
		CallNode* callNode = new CallNode("proc1", 2);
		StmtLstNode* innerStmtLstNode = new StmtLstNode();
		innerStmtLstNode->addStmtNode(callNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, innerStmtLstNode, 1);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(whileNode);
		ProcedureNode* procedureNode1 = new ProcedureNode("main");
		procedureNode1->addStmtLst(stmtLstNode1);

		ReadNode* readNode = new ReadNode("a", 3);
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(readNode);
		ProcedureNode* procedureNode2 = new ProcedureNode("proc");
		procedureNode2->addStmtLst(stmtLstNode2);

		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode1);
		programNode->addProcedure(procedureNode2);
		SourceAST ast(programNode);

		auto wrapperFunc = [&ast] { ASTValidator::validateAST(ast); };
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			ASTValidator::validateAST(ast);
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CALL_NON_EXISTENT_PROC_NAME_ERROR.c_str(), ex.what());
		}
	}

	TEST_METHOD(validateAST_callNonExistentProcedure_insideNestedContainerStmts_ASTExceptionThrown) {
		/* AST is equivalent to the SIMPLE program
			procedure proc {
				   1. read a; }
			}
			procedure main {
			2. while (x==y) {
				3. if (x==y) then {
					4. read x; } else {
				   5. call proc1; }}
			}
		*/

		/* procedure proc {
				   1. read a; }
			}
		*/
		ReadNode* readNode = new ReadNode("a", 1);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(readNode);
		ProcedureNode* procedureNode1 = new ProcedureNode("proc");
		procedureNode1->addStmtLst(stmtLstNode1);

		/* procedure main {
			2. while (x==y) {
				3. if (x==y) then {
					4. read x; } else {
				   5. call proc1; }}
			}
		*/
		CallNode* callNode1 = new CallNode("proc1", 5);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(callNode1);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(readNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode, 3);
		StmtLstNode* whileStmtLstNode = new StmtLstNode();
		whileStmtLstNode->addStmtNode(ifNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, whileStmtLstNode, 2);
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(whileNode);
		ProcedureNode* procedureNode2 = new ProcedureNode("main");
		procedureNode2->addStmtLst(stmtLstNode2);

		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode1);
		programNode->addProcedure(procedureNode2);
		SourceAST ast(programNode);

		auto wrapperFunc = [&ast] { ASTValidator::validateAST(ast); };
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			ASTValidator::validateAST(ast);
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CALL_NON_EXISTENT_PROC_NAME_ERROR.c_str(), ex.what());
		}
	}

	TEST_METHOD(validateAST_selfCall_noContainerStmts_ASTExceptionThrown) {
		/* AST is equivalent to the SIMPLE program
			procedure proc {
				   call proc; }
			}
		*/
		std::string procName = "proc";

		CallNode* callNode = new CallNode(procName, 1);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(callNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);

		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);

		auto wrapperFunc = [&ast] { ASTValidator::validateAST(ast); };
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			ASTValidator::validateAST(ast);
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CYCLIC_CALL_ERROR.c_str(), ex.what());
		}
	}

	TEST_METHOD(validateAST_selfCall_insideNestedContainerStmts_ASTExceptionThrown) {
		/* AST is equivalent to the SIMPLE program
			procedure main {
			1. while (x==y) {
				2. if (x==y) then {
					3. read x; } else {
				   4. call main; }}
			}
		*/

		std::string procName = "main";

		ReadNode* readNode = new ReadNode("x", 3);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(readNode);

		CallNode* callNode1 = new CallNode(procName, 4);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(callNode1);

		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode, 2);
		StmtLstNode* whileStmtLstNode = new StmtLstNode();
		whileStmtLstNode->addStmtNode(readNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, whileStmtLstNode, 1);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(ifNode);
		ProcedureNode* procedureNode = new ProcedureNode(procName);
		procedureNode->addStmtLst(stmtLstNode);

		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode);
		SourceAST ast(programNode);

		auto wrapperFunc = [&ast] { ASTValidator::validateAST(ast); };
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			ASTValidator::validateAST(ast);
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CYCLIC_CALL_ERROR.c_str(), ex.what());
		}
	}

	TEST_METHOD(validateAST_cyclicCall_twoProcs_ASTExceptionThrown) {
		/* AST is equivalent to the SIMPLE program
			procedure proc {
				   1. call main;
			}
			procedure main {
			2. while (x==y) {
				3. if (x==y) then {
				4. read x; } else {
				   5. call proc; }}
			}
		*/

		std::string procName1 = "proc";
		std::string procName2 = "main";

		/* proc1 -> proc2 */
		/* procedure proc {
				   1. call main;
			}
		*/
		CallNode* callNode1 = new CallNode(procName2, 1);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(callNode1);
		ProcedureNode* procedureNode1 = new ProcedureNode(procName1);
		procedureNode1->addStmtLst(stmtLstNode1);

		/* proc2 -> proc1 */
		/* procedure main {
			2. while (x==y) {
				3. if (x==y) then {
				4. read x; } else {
				   5. call proc; }}
			}
		*/
		CallNode* callNode2 = new CallNode(procName1, 5);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(callNode2);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(new ReadNode("x", 4));
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode, 3);
		StmtLstNode* whileStmtLstNode = new StmtLstNode();
		whileStmtLstNode->addStmtNode(ifNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, whileStmtLstNode, 2);
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(whileNode);
		ProcedureNode* procedureNode2 = new ProcedureNode(procName2);
		procedureNode2->addStmtLst(stmtLstNode2);

		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode1);
		programNode->addProcedure(procedureNode2);
		SourceAST ast(programNode);

		auto wrapperFunc = [&ast] { ASTValidator::validateAST(ast); };
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			ASTValidator::validateAST(ast);
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CYCLIC_CALL_ERROR.c_str(), ex.what());
		}
	}

	TEST_METHOD(validateAST_cyclicCall_transitiveCalls_ASTExceptionThrown) {
		/* AST is equivalent to the SIMPLE program
			procedure proc1 {
			1. while (x==y) {
				2. if (x==y) then {
				3. read x; } else {
				   4. call proc2; }}
			}
			procedure proc2 {
				   5. call proc3;
				   6. call proc3;
			}
			procedure proc3 {
				7. if ((x == y) && (z > 0)) then {
				   8. print a; } else {
				   9. read y; }
				10. if (x != z) then {
				   11. call proc1; } else {
				   12. read a; }
			}
		*/

		std::string procName1 = "proc1";
		std::string procName2 = "proc2";
		std::string procName3 = "proc3";

		/* proc1 -> proc2 */
		/* procedure proc1 {
			1. while (x==y) {
				2. if (x==y) then {
				3. read x; } else {
				   4. call proc2; }}
			}
		*/
		CallNode* callNode1 = new CallNode(procName2, 4);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(callNode1);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(new ReadNode("x", 3));
		ExprNode* rootExprNode1 = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* leftExprNode1 = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode1 = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		rootExprNode1->addChild(leftExprNode1);
		rootExprNode1->addChild(rightExprNode1);
		IfNode* ifNode1 = new IfNode(rootExprNode1, thenStmtLstNode, elseStmtLstNode, 2);
		StmtLstNode* whileStmtLstNode = new StmtLstNode();
		whileStmtLstNode->addStmtNode(ifNode1);
		WhileNode* whileNode = new WhileNode(rootExprNode1, whileStmtLstNode, 1);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(whileNode);
		ProcedureNode* procedureNode1 = new ProcedureNode(procName1);
		procedureNode1->addStmtLst(stmtLstNode1);

		/* proc2 -> proc3 */
		/* procedure proc2 {
				   5. call proc3;
				   6. call proc3;
			}
		*/
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(new CallNode(procName3, 5));
		stmtLstNode2->addStmtNode(new CallNode(procName3, 6));
		ProcedureNode* procedureNode2 = new ProcedureNode(procName2);
		procedureNode2->addStmtLst(stmtLstNode2);

		/* proc3 -> proc1 */
		/* procedure proc3 {
				7. if ((x == y) && (z > 0)) then {
				   8. print a; } else {
				   9. read y; }
				10. if (x != z) then {
				   11. call proc1; } else {
				   12. read a; }
			}
		 */
		PrintNode* printNode1 = new PrintNode("a", 8);
		StmtLstNode* thenStmtLstNode1 = new StmtLstNode();
		thenStmtLstNode1->addStmtNode(printNode1);
		ReadNode* readNode1 = new ReadNode("y", 9);
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
		IfNode* ifNode2 = new IfNode(rootExprNode2, thenStmtLstNode1, elseStmtLstNode1, 7);

		CallNode* callNode3 = new CallNode(procName1, 11);
		StmtLstNode* thenStmtLstNode2 = new StmtLstNode();
		thenStmtLstNode2->addStmtNode(callNode3);
		ReadNode* readNode2 = new ReadNode("a", 12);
		StmtLstNode* elseStmtLstNode2 = new StmtLstNode();
		elseStmtLstNode2->addStmtNode(readNode2);
		ExprNode* rootExprNode3 = new ExprNode(ExprNodeValueType::REL_OPERATOR, "!=");
		ExprNode* leftExprNode3 = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode3 = new ExprNode(ExprNodeValueType::VAR_NAME, "z");
		rootExprNode3->addChild(leftExprNode3);
		rootExprNode3->addChild(rightExprNode3);
		IfNode* ifNode3 = new IfNode(rootExprNode3, thenStmtLstNode2, elseStmtLstNode2, 10);

		StmtLstNode* outerStmtLstNode = new StmtLstNode();
		outerStmtLstNode->addStmtNode(ifNode2);
		outerStmtLstNode->addStmtNode(ifNode3);
		ProcedureNode* procedureNode3 = new ProcedureNode(procName3);
		procedureNode3->addStmtLst(outerStmtLstNode);

		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode1);
		programNode->addProcedure(procedureNode2);
		programNode->addProcedure(procedureNode3);
		SourceAST ast(programNode);
		auto wrapperFunc = [&ast] { ASTValidator::validateAST(ast); };
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			ASTValidator::validateAST(ast);
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CYCLIC_CALL_ERROR.c_str(), ex.what());
		}
	}

	TEST_METHOD(validateAST_cyclicCall_transitiveCalls_disjointCallsGraphs_ASTExceptionThrown) {
		/* AST is equivalent to the SIMPLE program
			procedure proc1 {
			1. while (x==y) {
				2. if (x==y) then {
					3. read x; } else {
				   4. call proc2; }}
			}
			procedure proc2 {
				   5. call proc3;
				   6. call proc3;
			}
			procedure proc3 {
				7. call proc1; }
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
		CallNode* callNode1 = new CallNode(procName2, 4);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(callNode1);
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
				   6. call proc3;
			}
		*/
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(new CallNode(procName3, 5));
		stmtLstNode2->addStmtNode(new CallNode(procName3, 6));
		ProcedureNode* procedureNode2 = new ProcedureNode(procName2);
		procedureNode2->addStmtLst(stmtLstNode2);

		/* proc3 -> proc1 */
		/* procedure proc3 {
				7. call proc1; }
		*/
		StmtLstNode* stmtLstNode3 = new StmtLstNode();
		stmtLstNode3->addStmtNode(new CallNode(procName1, 7));
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

		CallNode* callNode3 = new CallNode(procName4, 13);
		StmtLstNode* thenStmtLstNode2 = new StmtLstNode();
		thenStmtLstNode2->addStmtNode(callNode3);
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
		auto wrapperFunc = [&ast] { ASTValidator::validateAST(ast); };
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			ASTValidator::validateAST(ast);
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CYCLIC_CALL_ERROR.c_str(), ex.what());
		}
	}

	TEST_METHOD(validateAST_noCyclicCall_transitiveCalls_success) {
		/* AST is equivalent to the SIMPLE program
			procedure proc1 {
			1. while (x==y) {
				2. if (x==y) then {
					3. read x; } else {
				   4. call proc2; }}
			}
			procedure proc2 {
				   5. call proc3;
				   6. call proc3;
				   7. call proc4; }
			}
			procedure proc3 {
				8. read proc1; }
			procedure proc4 {
				9. read proc1; }
		*/

		std::string procName1 = "proc1";
		std::string procName2 = "proc2";
		std::string procName3 = "proc3";
		std::string procName4 = "proc4";

		/* proc1 -> proc2 */
		/* procedure proc1 {
			1. while (x==y) {
				2. if (x==y) then {
					3. read x; } else {
				   4. call proc2; }}
			}
		*/
		CallNode* callNode1 = new CallNode(procName2, 4);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(callNode1);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(new ReadNode("x", 3));
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode, 2);
		StmtLstNode* whileStmtLstNode = new StmtLstNode();
		whileStmtLstNode->addStmtNode(ifNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, whileStmtLstNode, 1);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(whileNode);
		ProcedureNode* procedureNode1 = new ProcedureNode(procName1);
		procedureNode1->addStmtLst(stmtLstNode1);

		/* proc2 -> proc3, proc4 */
		/* procedure proc2 {
				   5. call proc3;
				   6. call proc3;
				   7. call proc4; }
			}
		*/
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(new CallNode(procName3, 5));
		stmtLstNode2->addStmtNode(new CallNode(procName3, 6));
		stmtLstNode2->addStmtNode(new CallNode(procName4, 7));
		ProcedureNode* procedureNode2 = new ProcedureNode(procName2);
		procedureNode2->addStmtLst(stmtLstNode2);

		/* procedure proc3 {
				8. read proc1; }
		*/
		StmtLstNode* stmtLstNode3 = new StmtLstNode();
		stmtLstNode3->addStmtNode(new ReadNode(procName1, 8));
		ProcedureNode* procedureNode3 = new ProcedureNode(procName3);
		procedureNode3->addStmtLst(stmtLstNode3);

		/* procedure proc4 {
				9. read proc1; }
		*/
		StmtLstNode* stmtLstNode4 = new StmtLstNode();
		stmtLstNode4->addStmtNode(new ReadNode(procName1, 9));
		ProcedureNode* procedureNode4 = new ProcedureNode(procName4);
		procedureNode4->addStmtLst(stmtLstNode4);

		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode1);
		programNode->addProcedure(procedureNode2);
		programNode->addProcedure(procedureNode3);
		programNode->addProcedure(procedureNode4);
		SourceAST ast(programNode);
		ASTValidator::validateAST(ast);

		/* If an exception is raised, the test automatically fails */
		Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(true);
	}

	TEST_METHOD(validateAST_noCyclicCall_transitiveCalls_forwardEdge_success) {
		/* AST is equivalent to the SIMPLE program
			procedure proc1 {
			1. while (x==y) {
				2. call proc3;
				3. if (x==y) then {
					4. read x; } else {
				   5. call proc2; }}
			}
			procedure proc2 {
				   6. call proc3;
				   7. call proc3; }
			}
			procedure proc3 {
				8. read proc1; }
		*/

		std::string procName1 = "proc1";
		std::string procName2 = "proc2";
		std::string procName3 = "proc3";

		/* proc1 -> proc2, proc3 */
		/* procedure proc1 {
			1. while (x==y) {
				2. call proc3;
				3. if (x==y) then {
					4. read x; } else {
				   5. call proc2; }}
			}
		*/
		CallNode* callNode1 = new CallNode(procName3, 2);
		CallNode* callNode2 = new CallNode(procName2, 5);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(callNode1);
		elseStmtLstNode->addStmtNode(callNode2);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(new ReadNode("x", 4));
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::VAR_NAME, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode1 = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode, 3);
		StmtLstNode* whileStmtLstNode = new StmtLstNode();
		whileStmtLstNode->addStmtNode(callNode1);
		whileStmtLstNode->addStmtNode(ifNode1);
		WhileNode* whileNode = new WhileNode(rootExprNode, whileStmtLstNode, 1);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(whileNode);
		ProcedureNode* procedureNode1 = new ProcedureNode(procName1);
		procedureNode1->addStmtLst(stmtLstNode1);

		/* proc2 -> proc3 */
		/* procedure proc2 {
				   6. call proc3;
				   7. call proc3; }
			}
		*/
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(new CallNode(procName3, 6));
		stmtLstNode2->addStmtNode(new CallNode(procName3, 7));
		ProcedureNode* procedureNode2 = new ProcedureNode(procName2);
		procedureNode2->addStmtLst(stmtLstNode2);

		/* procedure proc3 {
				8. read proc1; }
		*/
		StmtLstNode* stmtLstNode3 = new StmtLstNode();
		stmtLstNode3->addStmtNode(new ReadNode(procName1, 8));
		ProcedureNode* procedureNode3 = new ProcedureNode(procName3);
		procedureNode3->addStmtLst(stmtLstNode3);

		ProgramNode* programNode = new ProgramNode();
		programNode->addProcedure(procedureNode1);
		programNode->addProcedure(procedureNode2);
		programNode->addProcedure(procedureNode3);
		SourceAST ast(programNode);
		ASTValidator::validateAST(ast);

		/* If an exception is raised, the test automatically fails */
		Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(true);
	}

	TEST_METHOD(validateAST_noCyclicCall_transitiveCalls_disjointCallsGraphs_success) {
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
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(new CallNode(procName3, 5));
		stmtLstNode2->addStmtNode(new CallNode(procName3, 6));
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
		ASTValidator::validateAST(ast);

		/* If an exception is raised, the test automatically fails */
		Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(true);
	}
	};
}
