#include "stdafx.h"
#include "CppUnitTest.h"

#include "SP/DesignExtractor.h"

#include "PKB/Entity.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestDesignExtractor) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Entity::performCleanUp();
		Uses::performCleanUp();
		Modifies::performCleanUp();
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
		DesignExtractor::Extract(ast);
		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));
		Assert::AreEqual(size_t(1), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(1), Entity::getAllVars().size());
		Assert::AreEqual(varName, Entity::getVarName(Entity::getAllVars()[0]));

		std::unordered_map<ProcIndex, std::vector<StmtIndex>, ProcIndex::HashFunction>
			procStmtMap = DesignExtractor::getProcStmtMap();
		Assert::AreEqual(size_t(1), procStmtMap.size());
		ProcIndex procIndex = Entity::getProcIdx(procName);
		Assert::AreEqual(size_t(1), procStmtMap.at(procIndex).size());

		Assert::AreEqual(size_t(1), Modifies::getAllStmtVarInfo().size());
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
		DesignExtractor::Extract(ast);
		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));
		Assert::AreEqual(size_t(1), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(1), Entity::getAllVars().size());
		Assert::AreEqual(varName, Entity::getVarName(Entity::getAllVars()[0]));

		std::unordered_map<ProcIndex, std::vector<StmtIndex>, ProcIndex::HashFunction>
			procStmtMap = DesignExtractor::getProcStmtMap();
		Assert::AreEqual(size_t(1), procStmtMap.size());
		ProcIndex procIndex = Entity::getProcIdx(procName);
		Assert::AreEqual(size_t(1), procStmtMap.at(procIndex).size());

		Assert::AreEqual(size_t(1), Uses::getAllStmtVarInfo().size());
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
		DesignExtractor::Extract(ast);
		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));
		Assert::AreEqual(size_t(2), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(2), Entity::getAllVars().size());
		Assert::AreEqual(varNameX, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varNameY, Entity::getVarName(Entity::getAllVars()[1]));

		std::unordered_map<ProcIndex, std::vector<StmtIndex>, ProcIndex::HashFunction>
			procStmtMap = DesignExtractor::getProcStmtMap();
		Assert::AreEqual(size_t(1), procStmtMap.size());
		ProcIndex procIndex = Entity::getProcIdx(procName);
		Assert::AreEqual(size_t(2), procStmtMap.at(procIndex).size());

		std::unordered_map<StmtIndex, StmtIndex, StmtIndex::HashFunction>
			stmtFollowsMap = DesignExtractor::getStmtFollowsMap();
		Assert::AreEqual(size_t(1), stmtFollowsMap.size());
		Assert::IsTrue(StmtIndex(2) == stmtFollowsMap.at(StmtIndex(1)));

		Assert::AreEqual(size_t(1), Modifies::getAllStmtVarInfo().size());
		Assert::AreEqual(size_t(1), Uses::getAllStmtVarInfo().size());
	}
	};
}
