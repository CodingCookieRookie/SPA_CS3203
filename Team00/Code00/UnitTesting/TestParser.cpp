#include "stdafx.h"
#include "CppUnitTest.h"
#include "SP/Parser.h"
#include <vector>
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestParser)
	{
	public:
		TEST_METHOD(Parse_OneProcedureReadPrint_Successful)
		{
			const char* source = "   procedure proc  "
				"{ read x1; print y123;  \n "
				" read Y1Yy ; } ";
			Parser parser(source);
			SourceAST ast = parser.parse();
			std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

			/* Test procedureNodes */
			Assert::AreEqual(size_t(1), procNodes.size());
			Assert::AreEqual(std::string("proc"), procNodes[0]->getProcName());

			/* Test statements */
			StmtListNode* stmtListNode = procNodes[0]->getStmtListNode();
			std::vector<StmtNode*> statements = stmtListNode->getStmtNodes();
			Assert::AreEqual(size_t(3), statements.size());

			ReadNode* readNode1 = (ReadNode*) statements[0];
			Assert::AreEqual(std::string("x1"), readNode1->getVarName());
			Assert::AreEqual(size_t(1), readNode1->getStmtNum());

			PrintNode* printNode = (PrintNode*)statements[1];
			Assert::AreEqual(std::string("y123"), printNode->getVarName());
			Assert::AreEqual(size_t(2), printNode->getStmtNum());

			ReadNode* readNode2 = (ReadNode*)statements[2];
			Assert::AreEqual(std::string("Y1Yy"), readNode2->getVarName());
			Assert::AreEqual(size_t(3), readNode2->getStmtNum());
		}

		TEST_METHOD(Parse_MultipleProceduresReadPrint_Successful)
		{
			const char* source = "   procedure proc1  "
				"{ read x1; } "
				" \n procedure proc2 { \n"
				" print y123;  \n "
				" read Y1Yy ; } ";
			Parser parser(source);
			SourceAST ast = parser.parse();
			std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

			/* Test procedureNodes */
			Assert::AreEqual(size_t(2), procNodes.size());
			Assert::AreEqual(std::string("proc1"), procNodes[0]->getProcName());
			Assert::AreEqual(std::string("proc2"), procNodes[1]->getProcName());

			/* Test statements in stmtListNode1 */
			StmtListNode* stmtListNode1 = procNodes[0]->getStmtListNode();
			std::vector<StmtNode*> statements1 = stmtListNode1->getStmtNodes();
			Assert::AreEqual(size_t(1), statements1.size());

			ReadNode* readNode1 = (ReadNode*)statements1[0];
			Assert::AreEqual(std::string("x1"), readNode1->getVarName());
			Assert::AreEqual(size_t(1), readNode1->getStmtNum());

			/* Test statements in stmtListNode2 */
			StmtListNode* stmtListNode2 = procNodes[1]->getStmtListNode();
			std::vector<StmtNode*> statements2 = stmtListNode2->getStmtNodes();
			Assert::AreEqual(size_t(2), statements2.size());

			PrintNode* printNode = (PrintNode*)statements2[0];
			Assert::AreEqual(std::string("y123"), printNode->getVarName());
			Assert::AreEqual(size_t(2), printNode->getStmtNum());

			ReadNode* readNode2 = (ReadNode*)statements2[1];
			Assert::AreEqual(std::string("Y1Yy"), readNode2->getVarName());
			Assert::AreEqual(size_t(3), readNode2->getStmtNum());
		}

	};
}