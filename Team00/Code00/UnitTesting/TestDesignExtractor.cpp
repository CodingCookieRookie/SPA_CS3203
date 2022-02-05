#include "stdafx.h"
#include "CppUnitTest.h"

#include "SP/DesignExtractor.h"
#include "PKB/Entity.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestDesignExtractor) {
	public:
		TEST_METHOD(TestMethod1) {
			std::string varName = "x";
			std::string procName = "main";

			ReadNode* readNode = new ReadNode(varName);
			StmtListNode* stmtLstNode = new StmtListNode();
			stmtLstNode->addStmtNode(readNode);
			ProcedureNode* procedureNode = new ProcedureNode(procName);
			procedureNode->addStmtList(stmtLstNode);
			ProgramNode* programNode = new ProgramNode();
			programNode->addProcedure(procedureNode);
			SourceAST ast(programNode);
			DesignExtractor::Extract(ast);
			Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
			Assert::AreEqual(procName, Entity::getAllProcs()[0]);
			Assert::AreEqual(size_t(1), Entity::getAllStmts().size());
			Assert::AreEqual(size_t(1), Entity::getAllVars().size());
			Assert::AreEqual(varName, Entity::getAllVars()[0]);
		}
	};
}
