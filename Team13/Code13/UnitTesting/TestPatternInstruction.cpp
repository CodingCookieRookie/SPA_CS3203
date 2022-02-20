#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS/PQLEvaluator.h"
#include "../source/QPS/PQLParser.h"
#include "../source/PKB/Pattern.h"
#include <PKB/ExpressionProcessor.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPatternInstruction) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Entity::performCleanUp();
		Pattern::performCleanUp();
	}
public:
	// Pattern a(v, "_x_") or Pattern a(v, "_123_") or Pattern a("x", "_x_") or Pattern a("x", "__")
	// Pattern a(v, *) or Pattern a("x", *)
	// Pattern a(*, "_x_")
	// Pattern a(*, *)
	TEST_METHOD(execute_lhsSynonymRhsConst_ConstOutOfBoundsEvTableFalse) {
		// assign1 = assign1 + 234
		// Pattern a1(v, "_12345_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::synonym, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::partial, " 12345 ");
		Instruction* instruction = new PatternInstruction(synonym, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+234");
		Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getStmtsFromVarPattern(varIndex, expressionSpec.second, true);
		Assert::AreEqual(size_t(0), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: \nSynonym: v Values: \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsIdentRhsIdent_LeftIdentOutOfBoundsEvTableFalse) {
		// assign1 = assign1 + x
		// Pattern a1("v", "_xx_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::ident, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::partial, " xx ");
		Instruction* instruction = new PatternInstruction(synonym, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getStmtsFromVarPattern(varIndex, expressionSpec.second, true);
		Assert::AreEqual(size_t(0), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.getTableString());
	}
	TEST_METHOD(execute_lhsSynonymRhsIdentVarPartialPqlinsertPostFixInfoTest1) {
		// assign1 = assign1 + x
		// Pattern a1(v, "_xx_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::synonym, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::partial, " xx ");
		Instruction* instruction = new PatternInstruction(synonym, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getStmtsFromVarPattern(varIndex, expressionSpec.second, true);
		Assert::AreEqual(size_t(0), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: \nSynonym: v Values: \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsSynonymRhsIdentVarPartialPqlinsertPostFixInfoTest2) {
		// assign1 = assign1 + xx
		// Pattern a1(v, "_x_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::synonym, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::partial, " x ");
		Instruction* instruction = new PatternInstruction(synonym, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+xx");
		Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated

		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern(" x ", true);
		Assert::AreEqual(size_t(0), std::get<0>(allPatternStmtInfo).size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: \nSynonym: v Values: \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsSynonymRhsIdentVar) {
		// assign1 = assign1 + x
		// Pattern a1(v, "_x_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::synonym, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::partial, " x ");
		Instruction* instruction = new PatternInstruction(synonym, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getStmtsFromVarPattern(varIndex, expressionSpec.second, true);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 \nSynonym: v Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsSynonymRhsIdentConstant) {   // assign1 = assign1 + 123
		// Pattern a1(v, "_123_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::synonym, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::partial, " 123 ");
		Instruction* instruction = new PatternInstruction(synonym, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		VarIndex varIndex = Entity::insertVar("assign1");
		Entity::insertConst(123);
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+123");
		Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getStmtsFromVarPattern(varIndex, expressionSpec.second, true);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 \nSynonym: v Values: 1 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsIdentRhsIdentSynonym) {
		// assign1 = assign1 + x
		// Pattern a1("assign1", "_x_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::ident, "assign1");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::partial, " x ");
		Instruction* instruction = new PatternInstruction(synonym, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("x");
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getStmtsFromVarPattern(varIndex, expressionSpec.second, true);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsIdentRhsIdentConstant) {
		// assign1 = assign1 + 123
		// Pattern a1("assign1", "_123_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::ident, "assign1");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::partial, " 123 ");
		Instruction* instruction = new PatternInstruction(synonym, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("x");
		Entity::insertConst(123);
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+123");
		Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getStmtsFromVarPattern(varIndex, expressionSpec.second, true);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsSynonymRhsWildCard) {
		// assign1 = assign1 + x
		// Pattern a1(v, *)
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::synonym, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::wildcard, "");
		Instruction* instruction = new PatternInstruction(synonym, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix(expressionSpec.second), true);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 \nSynonym: v Values: 1 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsIdentRhsWildCard) {
		// assign1 = assign1 + x
		// Pattern a1("x", *)
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::ident, "assign1");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::wildcard, "");
		Instruction* instruction = new PatternInstruction(synonym, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix(expressionSpec.second), true);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsWildCardRhsIdent) {
		// assign1 = assign1 + x
		// Pattern a1(*, "_x_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::wildcard, "");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::partial, " x ");
		Instruction* instruction = new PatternInstruction(synonym, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern(expressionSpec.second, true);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsWildCardRhsWildCard) {
		// assign1 = assign1 + x
		// Pattern a1(*, *)
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::wildcard, "");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::wildcard, "");
		Instruction* instruction = new PatternInstruction(synonym, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix(expressionSpec.second), true);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}
	};
}
