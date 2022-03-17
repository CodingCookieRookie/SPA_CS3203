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
	};
}
