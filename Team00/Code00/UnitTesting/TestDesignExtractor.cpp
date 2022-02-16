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
		Pattern::performCleanUp();
		Parent::performCleanUp();
		ParentT::performCleanUp();
		Follows::performCleanUp();
		FollowsT::performCleanUp();
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

		Assert::AreEqual(size_t(1), std::get<0>(Modifies::getAllStmtVarInfo()).size());
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

		Assert::AreEqual(size_t(1), std::get<0>(Uses::getAllStmtVarInfo()).size());
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

		Assert::AreEqual(size_t(1), std::get<0>(Modifies::getAllStmtVarInfo()).size());
		Assert::AreEqual(size_t(1), std::get<0>(Uses::getAllStmtVarInfo()).size());

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
		DesignExtractor::Extract(ast);

		Assert::AreEqual(size_t(2), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		/* We expect (3 choose 2) = 3 relationships in Follows T */
		Assert::AreEqual(size_t(3), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
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
		DesignExtractor::Extract(ast);

		Assert::AreEqual(size_t(2), Entity::getAllVars().size());
		Assert::AreEqual(size_t(3), Entity::getAllConsts().size());
		std::tuple<std::vector<int>, std::vector<int>> patternVarResult = Pattern::getStmtsFromPattern(std::string(" x "), true);
		Assert::AreEqual(size_t(1), std::get<0>(patternVarResult).size());

		std::tuple<std::vector<int>, std::vector<int>> patternConstResult = Pattern::getStmtsFromPattern(std::string(" 1 "), true);
		Assert::AreEqual(size_t(1), std::get<0>(patternConstResult).size());
	}
	};
}
