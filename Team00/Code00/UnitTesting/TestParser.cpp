#include "CppUnitTest.h"
#include "stdafx.h"

#include "SP/Parser.h"

#include <iostream>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestParser) {
public:
	TEST_METHOD(parse_oneProcedureReadPrint_success) {
		const char* source = "   procedure proc \n "
			"{ read x1; print y123;  \n "
			" read Y1Yy ; } ";
		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(1), procNodes.size());
		Assert::AreEqual(std::string("proc"), procNodes[0]->getProcName());

		/* Test statements */
		StmtLstNode* stmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> statements = stmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(3), statements.size());

		ReadNode* readNode1 = (ReadNode*)statements[0];
		Assert::AreEqual(std::string("x1"), readNode1->getVarName());

		PrintNode* printNode = (PrintNode*)statements[1];
		Assert::AreEqual(std::string("y123"), printNode->getVarName());

		ReadNode* readNode2 = (ReadNode*)statements[2];
		Assert::AreEqual(std::string("Y1Yy"), readNode2->getVarName());
	}

	TEST_METHOD(parse_multipleProceduresReadPrint_success) {
		const char* source = "   procedure proc1  "
			"{ read x1; } "
			" \n procedure proc2 { \n"
			" print y123;  \n "
			" read Y1Yy ; } ";
		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(2), procNodes.size());
		Assert::AreEqual(std::string("proc1"), procNodes[0]->getProcName());
		Assert::AreEqual(std::string("proc2"), procNodes[1]->getProcName());

		/* Test statements in stmtLstNode1 */
		StmtLstNode* stmtLstNode1 = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> statements1 = stmtLstNode1->getStmtNodes();
		Assert::AreEqual(size_t(1), statements1.size());

		ReadNode* readNode1 = (ReadNode*)statements1[0];
		Assert::AreEqual(std::string("x1"), readNode1->getVarName());

		/* Test statements in stmtLstNode2 */
		StmtLstNode* stmtLstNode2 = procNodes[1]->getStmtLstNode();
		std::vector<StmtNode*> statements2 = stmtLstNode2->getStmtNodes();
		Assert::AreEqual(size_t(2), statements2.size());

		PrintNode* printNode = (PrintNode*)statements2[0];
		Assert::AreEqual(std::string("y123"), printNode->getVarName());

		ReadNode* readNode2 = (ReadNode*)statements2[1];
		Assert::AreEqual(std::string("Y1Yy"), readNode2->getVarName());
	}
	};
}
