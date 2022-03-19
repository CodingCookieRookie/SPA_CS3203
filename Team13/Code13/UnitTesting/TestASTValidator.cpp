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
				   print a; }
			procedure main {
				   read a; }
			}
		*/
		PrintNode* printNode = new PrintNode("a");
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(printNode);
		ProcedureNode* procedureNode1 = new ProcedureNode("main");
		procedureNode1->addStmtLst(stmtLstNode1);

		ReadNode* readNode = new ReadNode("a");
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

	TEST_METHOD(validateAST_callNonExistentFunction_noContainerStmt_ASTExceptionThrown) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
				   call proc1; }
			procedure proc {
				   read a; }
			}
		*/
		CallNode* callNode = new CallNode("proc1");
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(callNode);
		ProcedureNode* procedureNode1 = new ProcedureNode("main");
		procedureNode1->addStmtLst(stmtLstNode1);

		ReadNode* readNode = new ReadNode("a");
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

	TEST_METHOD(validateAST_callNonExistentFunction_insideOneContainerStmt_ASTExceptionThrown) {
		/* AST is equivalent to the SIMPLE program
		   procedure main {
			while (x==y) {
				   call proc1; }
			}
			procedure proc {
				   read a; }
			}
		*/
		CallNode* callNode = new CallNode("proc1");
		StmtLstNode* innerStmtLstNode = new StmtLstNode();
		innerStmtLstNode->addStmtNode(callNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, innerStmtLstNode);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(whileNode);
		ProcedureNode* procedureNode1 = new ProcedureNode("main");
		procedureNode1->addStmtLst(stmtLstNode1);

		ReadNode* readNode = new ReadNode("a");
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

	TEST_METHOD(validateAST_callNonExistentFunction_insideNestedContainerStmts_ASTExceptionThrown) {
		/* AST is equivalent to the SIMPLE program
			procedure proc {
				   read a; }
			}
			procedure main {
			while (x==y) {
				if (x==y) then { read x; } else {
				   call proc1; }}
			}
		*/

		/* procedure proc {
				   read a; }
			}
		*/
		ReadNode* readNode = new ReadNode("a");
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(readNode);
		ProcedureNode* procedureNode1 = new ProcedureNode("proc");
		procedureNode1->addStmtLst(stmtLstNode1);

		/* procedure main {
			while (x==y) {
				call proc;
				if (x==y) then { read a; } else {
				   call proc1; }}
			}
		*/
		CallNode* callNode1 = new CallNode("proc1");
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(callNode1);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(readNode);
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode);
		CallNode* callNode2 = new CallNode("proc");
		StmtLstNode* whileStmtLstNode = new StmtLstNode();
		whileStmtLstNode->addStmtNode(callNode2);
		whileStmtLstNode->addStmtNode(ifNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, whileStmtLstNode);
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

		CallNode* callNode = new CallNode(procName);
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
			while (x==y) {
				if (x==y) then { read x; } else {
				   call main; }}
			}
		*/

		std::string procName = "main";

		ReadNode* readNode = new ReadNode("x");
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(readNode);

		CallNode* callNode1 = new CallNode(procName);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(callNode1);

		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode);
		StmtLstNode* whileStmtLstNode = new StmtLstNode();
		whileStmtLstNode->addStmtNode(ifNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, whileStmtLstNode);
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(whileNode);
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
				   call main; }
			}
			procedure main {
			while (x==y) {
				if (x==y) then { read x; } else {
				   call proc; }}
			}
		*/

		std::string procName1 = "proc";
		std::string procName2 = "main";

		/* proc1 -> proc2 */
		/* procedure proc {
				  call main; }
			}
		*/
		CallNode* callNode1 = new CallNode(procName2);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(callNode1);
		ProcedureNode* procedureNode1 = new ProcedureNode(procName1);
		procedureNode1->addStmtLst(stmtLstNode1);

		/* proc2 -> proc1 */
		/* procedure main {
			while (x==y) {
				if (x==y) then { read x; } else {
				   call proc; }}
			}
		*/
		CallNode* callNode2 = new CallNode(procName1);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(callNode2);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(new ReadNode("x"));
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode);
		StmtLstNode* whileStmtLstNode = new StmtLstNode();
		whileStmtLstNode->addStmtNode(ifNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, whileStmtLstNode);
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
			while (x==y) {
				if (x==y) then { read x; } else {
				   call proc2; }}
			}
			procedure proc2 {
				   call proc3; call proc3; }
			}
			procedure proc3 {
				if ((x == y) && (z > 0)) then {
				   print a; } else {
				   read y; }
				if (x != z) then {
				   call proc1; } else {
				   read a; }
			}
		*/

		std::string procName1 = "proc1";
		std::string procName2 = "proc2";
		std::string procName3 = "proc3";

		/* proc1 -> proc2 */
		/* procedure proc1 {
			while (x==y) {
				if (x==y) then { read x; } else {
				   call proc2; }}
			}
		*/
		CallNode* callNode1 = new CallNode(procName2);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(callNode1);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(new ReadNode("x"));
		ExprNode* rootExprNode1 = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* leftExprNode1 = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode1 = new ExprNode(ExprNodeValueType::varName, "y");
		rootExprNode1->addChild(leftExprNode1);
		rootExprNode1->addChild(rightExprNode1);
		IfNode* ifNode1 = new IfNode(rootExprNode1, thenStmtLstNode, elseStmtLstNode);
		StmtLstNode* whileStmtLstNode = new StmtLstNode();
		whileStmtLstNode->addStmtNode(ifNode1);
		WhileNode* whileNode = new WhileNode(rootExprNode1, whileStmtLstNode);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(whileNode);
		ProcedureNode* procedureNode1 = new ProcedureNode(procName1);
		procedureNode1->addStmtLst(stmtLstNode1);

		/* proc2 -> proc3 */
		/* procedure proc2 {
				   call proc3; call proc3; }
			}
		*/
		CallNode* callNode2 = new CallNode(procName3);
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(callNode2);
		stmtLstNode2->addStmtNode(callNode2);
		ProcedureNode* procedureNode2 = new ProcedureNode(procName2);
		procedureNode2->addStmtLst(stmtLstNode2);

		/* proc3 -> proc1 */
		/* procedure proc3 {
				if ((x == y) && (z > 0)) then {
				   print a; } else {
				   read y; }
				if (x != z) then {
				   call proc1; } else {
				   read a; }
			}
		 */
		PrintNode* printNode1 = new PrintNode("a");
		StmtLstNode* thenStmtLstNode1 = new StmtLstNode();
		thenStmtLstNode1->addStmtNode(printNode1);
		ReadNode* readNode1 = new ReadNode("y");
		StmtLstNode* elseStmtLstNode1 = new StmtLstNode();
		elseStmtLstNode1->addStmtNode(readNode1);
		ExprNode* rootExprNode2 = new ExprNode(ExprNodeValueType::logicalOperator, "&&");
		ExprNode* leftExprNode2 = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* rightExprNode2 = new ExprNode(ExprNodeValueType::relOperator, ">");
		rootExprNode2->addChild(leftExprNode2);
		rootExprNode2->addChild(rightExprNode2);

		leftExprNode2->addChild(new ExprNode(ExprNodeValueType::varName, "x"));
		leftExprNode2->addChild(new ExprNode(ExprNodeValueType::varName, "y"));
		rightExprNode2->addChild(new ExprNode(ExprNodeValueType::varName, "z"));
		rightExprNode2->addChild(new ExprNode(ExprNodeValueType::constValue, "0"));
		IfNode* ifNode2 = new IfNode(rootExprNode2, thenStmtLstNode1, elseStmtLstNode1);

		CallNode* callNode3 = new CallNode(procName1);
		StmtLstNode* thenStmtLstNode2 = new StmtLstNode();
		thenStmtLstNode2->addStmtNode(callNode3);
		ReadNode* readNode2 = new ReadNode("a");
		StmtLstNode* elseStmtLstNode2 = new StmtLstNode();
		elseStmtLstNode2->addStmtNode(readNode2);
		ExprNode* rootExprNode3 = new ExprNode(ExprNodeValueType::relOperator, "!=");
		ExprNode* leftExprNode3 = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode3 = new ExprNode(ExprNodeValueType::varName, "z");
		rootExprNode3->addChild(leftExprNode3);
		rootExprNode3->addChild(rightExprNode3);
		IfNode* ifNode3 = new IfNode(rootExprNode3, thenStmtLstNode2, elseStmtLstNode2);

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
			while (x==y) {
				if (x==y) then { read x; } else {
				   call proc2; }}
			}
			procedure proc2 {
				   call proc3; call proc3; }
			}
			procedure proc3 { call proc1; }
			procedure proc4 { read proc1; }
			procedure proc5 {
				if ((x == y) && (z > 0)) then {
				   print a; } else {
				   read y; }
				if (x != z) then {
				   call proc4; } else {
				   read a; }
			}
		*/

		std::string procName1 = "proc1";
		std::string procName2 = "proc2";
		std::string procName3 = "proc3";
		std::string procName4 = "proc4";
		std::string procName5 = "proc5";

		/* proc1 -> proc2 */
		/* procedure proc1 {
			while (x==y) {
				if (x==y) then { read x; } else {
				   call proc2; }}
			}
		*/
		CallNode* callNode1 = new CallNode(procName2);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(callNode1);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(new ReadNode("x"));
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode1 = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode);
		StmtLstNode* whileStmtLstNode = new StmtLstNode();
		whileStmtLstNode->addStmtNode(ifNode1);
		WhileNode* whileNode = new WhileNode(rootExprNode, whileStmtLstNode);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(whileNode);
		ProcedureNode* procedureNode1 = new ProcedureNode(procName1);
		procedureNode1->addStmtLst(stmtLstNode1);

		/* proc2 -> proc3 */
		/* procedure proc2 {
				   call proc3; call proc3; }
			}
		*/
		CallNode* callNode2 = new CallNode(procName3);
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(callNode2);
		stmtLstNode2->addStmtNode(callNode2);
		ProcedureNode* procedureNode2 = new ProcedureNode(procName2);
		procedureNode2->addStmtLst(stmtLstNode2);

		/* proc3 -> proc1 */
		/* procedure proc3 { call proc1; } */
		StmtLstNode* stmtLstNode3 = new StmtLstNode();
		stmtLstNode3->addStmtNode(new CallNode(procName1));
		ProcedureNode* procedureNode3 = new ProcedureNode(procName3);
		procedureNode3->addStmtLst(stmtLstNode3);

		/* procedure proc4 { read proc1; } */
		StmtLstNode* stmtLstNode4 = new StmtLstNode();
		stmtLstNode4->addStmtNode(new ReadNode(procName1));
		ProcedureNode* procedureNode4 = new ProcedureNode(procName4);
		procedureNode4->addStmtLst(stmtLstNode4);

		/* proc5 -> proc4 */
		/* procedure proc5 {
				if ((x == y) && (z > 0)) then {
				   print a; } else {
				   read y; }
				if (x != z) then {
				   call proc4; } else {
				   read a; }
			}
		*/
		PrintNode* printNode1 = new PrintNode("a");
		StmtLstNode* thenStmtLstNode1 = new StmtLstNode();
		thenStmtLstNode1->addStmtNode(printNode1);
		ReadNode* readNode1 = new ReadNode("y");
		StmtLstNode* elseStmtLstNode1 = new StmtLstNode();
		elseStmtLstNode1->addStmtNode(readNode1);
		ExprNode* rootExprNode2 = new ExprNode(ExprNodeValueType::logicalOperator, "&&");
		ExprNode* leftExprNode2 = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* rightExprNode2 = new ExprNode(ExprNodeValueType::relOperator, ">");
		rootExprNode2->addChild(leftExprNode2);
		rootExprNode2->addChild(rightExprNode2);

		leftExprNode2->addChild(new ExprNode(ExprNodeValueType::varName, "x"));
		leftExprNode2->addChild(new ExprNode(ExprNodeValueType::varName, "y"));
		rightExprNode2->addChild(new ExprNode(ExprNodeValueType::varName, "z"));
		rightExprNode2->addChild(new ExprNode(ExprNodeValueType::constValue, "0"));
		IfNode* ifNode2 = new IfNode(rootExprNode2, thenStmtLstNode1, elseStmtLstNode1);

		CallNode* callNode3 = new CallNode(procName4);
		StmtLstNode* thenStmtLstNode2 = new StmtLstNode();
		thenStmtLstNode2->addStmtNode(callNode3);
		ReadNode* readNode2 = new ReadNode("a");
		StmtLstNode* elseStmtLstNode2 = new StmtLstNode();
		elseStmtLstNode2->addStmtNode(readNode2);
		ExprNode* rootExprNode3 = new ExprNode(ExprNodeValueType::relOperator, "!=");
		ExprNode* leftExprNode3 = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode3 = new ExprNode(ExprNodeValueType::varName, "z");
		rootExprNode3->addChild(leftExprNode3);
		rootExprNode3->addChild(rightExprNode3);
		IfNode* ifNode3 = new IfNode(rootExprNode3, thenStmtLstNode2, elseStmtLstNode2);

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
			while (x==y) {
				if (x==y) then { read x; } else {
				   call proc2; }}
			}
			procedure proc2 {
				   call proc3; call proc3; call proc4; }
			}
			procedure proc3 { read proc1; }
			procedure proc4 { read proc1; }
		*/

		std::string procName1 = "proc1";
		std::string procName2 = "proc2";
		std::string procName3 = "proc3";
		std::string procName4 = "proc4";

		/* proc1 -> proc2 */
		/* procedure proc1 {
			while (x==y) {
				if (x==y) then { read x; } else {
				   call proc2; }}
			}
		*/
		CallNode* callNode1 = new CallNode(procName2);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(callNode1);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(new ReadNode("x"));
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode);
		StmtLstNode* whileStmtLstNode = new StmtLstNode();
		whileStmtLstNode->addStmtNode(ifNode);
		WhileNode* whileNode = new WhileNode(rootExprNode, whileStmtLstNode);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(whileNode);
		ProcedureNode* procedureNode1 = new ProcedureNode(procName1);
		procedureNode1->addStmtLst(stmtLstNode1);

		/* proc2 -> proc3, proc4 */
		/* procedure proc2 {
				   call proc3; call proc3; call proc4; }
			}
		*/
		CallNode* callNode2 = new CallNode(procName3);
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(callNode2);
		stmtLstNode2->addStmtNode(callNode2);
		stmtLstNode2->addStmtNode(new CallNode(procName4));
		ProcedureNode* procedureNode2 = new ProcedureNode(procName2);
		procedureNode2->addStmtLst(stmtLstNode2);

		/* procedure proc3 { read proc1; } */
		StmtLstNode* stmtLstNode3 = new StmtLstNode();
		stmtLstNode3->addStmtNode(new ReadNode(procName1));
		ProcedureNode* procedureNode3 = new ProcedureNode(procName3);
		procedureNode3->addStmtLst(stmtLstNode3);

		/* procedure proc4 { read proc1; } */
		StmtLstNode* stmtLstNode4 = new StmtLstNode();
		stmtLstNode4->addStmtNode(new ReadNode(procName1));
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
			while (x==y) {
				call proc3;
				if (x==y) then { read x; } else {
				   call proc2; }}
			}
			procedure proc2 {
				   call proc3; call proc3; }
			}
			procedure proc3 { read proc1; }
		*/

		std::string procName1 = "proc1";
		std::string procName2 = "proc2";
		std::string procName3 = "proc3";

		/* proc1 -> proc2, proc3 */
		/* procedure proc1 {
			while (x==y) {
				call proc3;
				if (x==y) then { read x; } else {
				   call proc2; }}
			}
		*/
		CallNode* callNode1 = new CallNode(procName3);
		CallNode* callNode2 = new CallNode(procName2);
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(callNode1);
		elseStmtLstNode->addStmtNode(callNode2);
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(new ReadNode("x"));
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode1 = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode);
		StmtLstNode* whileStmtLstNode = new StmtLstNode();
		whileStmtLstNode->addStmtNode(callNode1);
		whileStmtLstNode->addStmtNode(ifNode1);
		WhileNode* whileNode = new WhileNode(rootExprNode, whileStmtLstNode);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(whileNode);
		ProcedureNode* procedureNode1 = new ProcedureNode(procName1);
		procedureNode1->addStmtLst(stmtLstNode1);

		/* proc2 -> proc3 */
		/* procedure proc2 {
				   call proc3; call proc3; }
			}
		*/
		CallNode* callNode3 = new CallNode(procName3);
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(callNode3);
		stmtLstNode2->addStmtNode(callNode3);
		ProcedureNode* procedureNode2 = new ProcedureNode(procName2);
		procedureNode2->addStmtLst(stmtLstNode2);

		/* procedure proc3 { read proc1; } */
		StmtLstNode* stmtLstNode3 = new StmtLstNode();
		stmtLstNode3->addStmtNode(new ReadNode(procName1));
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
			while (x==y) {
				if (x==y) then { read x; } else {
				   call proc2; }}
			}
			procedure proc2 {
				   call proc3; call proc3; }
			}
			procedure proc3 { read proc1; }
			procedure proc4 { read proc1; }
			procedure proc5 {
				if ((x == y) && (z > 0)) then {
				   print a; } else {
				   read y; }
				if (x != z) then {
				   call proc4; } else {
				   read a; }
			}
		*/

		std::string procName1 = "proc1";
		std::string procName2 = "proc2";
		std::string procName3 = "proc3";
		std::string procName4 = "proc4";
		std::string procName5 = "proc5";

		/* proc1 -> proc2 */
		/* procedure proc1 {
			while (x==y) {
				if (x==y) then { read x; } else {
				   call proc2; }}
			}
		*/
		StmtLstNode* elseStmtLstNode = new StmtLstNode();
		elseStmtLstNode->addStmtNode(new CallNode(procName2));
		StmtLstNode* thenStmtLstNode = new StmtLstNode();
		thenStmtLstNode->addStmtNode(new ReadNode("x"));
		ExprNode* rootExprNode = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* leftExprNode = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode = new ExprNode(ExprNodeValueType::varName, "y");
		rootExprNode->addChild(leftExprNode);
		rootExprNode->addChild(rightExprNode);
		IfNode* ifNode1 = new IfNode(rootExprNode, thenStmtLstNode, elseStmtLstNode);
		StmtLstNode* whileStmtLstNode = new StmtLstNode();
		whileStmtLstNode->addStmtNode(ifNode1);
		WhileNode* whileNode = new WhileNode(rootExprNode, whileStmtLstNode);
		StmtLstNode* stmtLstNode1 = new StmtLstNode();
		stmtLstNode1->addStmtNode(whileNode);
		ProcedureNode* procedureNode1 = new ProcedureNode(procName1);
		procedureNode1->addStmtLst(stmtLstNode1);

		/* proc2 -> proc3 */
		/* procedure proc2 {
				   call proc3; call proc3; }
			}
		*/
		CallNode* callNode = new CallNode(procName3);
		StmtLstNode* stmtLstNode2 = new StmtLstNode();
		stmtLstNode2->addStmtNode(callNode);
		stmtLstNode2->addStmtNode(callNode);
		ProcedureNode* procedureNode2 = new ProcedureNode(procName2);
		procedureNode2->addStmtLst(stmtLstNode2);

		/* procedure proc3 { read proc1; } */
		StmtLstNode* stmtLstNode3 = new StmtLstNode();
		stmtLstNode3->addStmtNode(new ReadNode(procName1));
		ProcedureNode* procedureNode3 = new ProcedureNode(procName3);
		procedureNode3->addStmtLst(stmtLstNode3);

		/* procedure proc4 { read proc1; } */
		StmtLstNode* stmtLstNode4 = new StmtLstNode();
		stmtLstNode4->addStmtNode(new ReadNode(procName1));
		ProcedureNode* procedureNode4 = new ProcedureNode(procName4);
		procedureNode4->addStmtLst(stmtLstNode4);

		/* proc5 -> proc4 */
		/* procedure proc5 {
				if ((x == y) && (z > 0)) then {
				   print a; } else {
				   read y; }
				if (x != z) then {
				   call proc4; } else {
				   read a; }
			}
		*/
		PrintNode* printNode1 = new PrintNode("a");
		StmtLstNode* thenStmtLstNode1 = new StmtLstNode();
		thenStmtLstNode1->addStmtNode(printNode1);
		ReadNode* readNode1 = new ReadNode("y");
		StmtLstNode* elseStmtLstNode1 = new StmtLstNode();
		elseStmtLstNode1->addStmtNode(readNode1);
		ExprNode* rootExprNode2 = new ExprNode(ExprNodeValueType::logicalOperator, "&&");
		ExprNode* leftExprNode2 = new ExprNode(ExprNodeValueType::relOperator, "==");
		ExprNode* rightExprNode2 = new ExprNode(ExprNodeValueType::relOperator, ">");
		rootExprNode2->addChild(leftExprNode2);
		rootExprNode2->addChild(rightExprNode2);

		leftExprNode2->addChild(new ExprNode(ExprNodeValueType::varName, "x"));
		leftExprNode2->addChild(new ExprNode(ExprNodeValueType::varName, "y"));
		rightExprNode2->addChild(new ExprNode(ExprNodeValueType::varName, "z"));
		rightExprNode2->addChild(new ExprNode(ExprNodeValueType::constValue, "0"));
		IfNode* ifNode2 = new IfNode(rootExprNode2, thenStmtLstNode1, elseStmtLstNode1);

		StmtLstNode* thenStmtLstNode2 = new StmtLstNode();
		thenStmtLstNode2->addStmtNode(new CallNode(procName4));
		ReadNode* readNode2 = new ReadNode("a");
		StmtLstNode* elseStmtLstNode2 = new StmtLstNode();
		elseStmtLstNode2->addStmtNode(readNode2);
		ExprNode* rootExprNode3 = new ExprNode(ExprNodeValueType::relOperator, "!=");
		ExprNode* leftExprNode3 = new ExprNode(ExprNodeValueType::varName, "x");
		ExprNode* rightExprNode3 = new ExprNode(ExprNodeValueType::varName, "z");
		rootExprNode3->addChild(leftExprNode3);
		rootExprNode3->addChild(rightExprNode3);
		IfNode* ifNode3 = new IfNode(rootExprNode3, thenStmtLstNode2, elseStmtLstNode2);

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
