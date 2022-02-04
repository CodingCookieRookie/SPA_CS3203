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
			const char* source = "   procedure proc \n "
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

<<<<<<< HEAD
			ReadNode* readNode = (ReadNode*)statements[0];
			PrintNode* printNode = (PrintNode*)statements[1];
			Assert::AreEqual(std::string("x1"), readNode->getVarName());
=======
			ReadNode* readNode1 = (ReadNode*) statements[0];
			Assert::AreEqual(std::string("x1"), readNode1->getVarName());
			Assert::AreEqual(size_t(1), readNode1->getStmtNum());

			PrintNode* printNode = (PrintNode*)statements[1];
>>>>>>> 27f72e7... Update unit test
			Assert::AreEqual(std::string("y123"), printNode->getVarName());
			Assert::AreEqual(size_t(2), printNode->getStmtNum());

			ReadNode* readNode2 = (ReadNode*)statements[2];
			Assert::AreEqual(std::string("Y1Yy"), readNode2->getVarName());
			Assert::AreEqual(size_t(3), readNode2->getStmtNum());
		}

	};
}
