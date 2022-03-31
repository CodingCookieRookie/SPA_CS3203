#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/PKB/Pattern.h"
#include "../source/QPS/PQLEvaluator.h"
#include "../source/QPS/PQLParser.h"
#include "PKB/ExpressionProcessor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPatternAInstruction) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Attribute::performCleanUp();
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
		PqlReference entRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::PARTIAL, " 12345 ");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+234");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
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
		PqlReference entRef = std::make_pair(PqlReferenceType::IDENT, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::PARTIAL, " xx ");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
		Assert::AreEqual(size_t(0), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.getTableString());
	}
	TEST_METHOD(execute_lhsSynonymRhsIdentVarPartialPqlinsertAssignInfoTest1) {
		// assign1 = assign1 + x
		// Pattern a1(v, "_xx_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::PARTIAL, " xx ");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "v"};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
		Assert::AreEqual(size_t(0), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: \nSynonym: v Values: \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsSynonymRhsIdentVarPartialPqlinsertAssignInfoTest2) {
		// assign1 = assign1 + xx
		// Pattern a1(v, "_x_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::PARTIAL, " x ");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "v"};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+xx");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
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
		PqlReference entRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::PARTIAL, " x ");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
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
		PqlReference entRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::PARTIAL, " 123 ");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = Entity::insertVar("assign1");
		Entity::insertConst(123);
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+123");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
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
		PqlReference entRef = std::make_pair(PqlReferenceType::IDENT, "assign1");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::PARTIAL, " x ");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		Entity::insertVar("x");
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
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
		PqlReference entRef = std::make_pair(PqlReferenceType::IDENT, "assign1");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::PARTIAL, " 123 ");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		Entity::insertVar("x");
		Entity::insertConst(123);
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+123");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
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
		PqlReference entRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::WILDCARD, "");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(expressionSpec.second));
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
		PqlReference entRef = std::make_pair(PqlReferenceType::IDENT, "assign1");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::WILDCARD, "");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(expressionSpec.second));
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
		PqlReference entRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::PARTIAL, " x ");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(expressionSpec.second);
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
		PqlReference entRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::WILDCARD, "");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = Entity::insertVar("assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(expressionSpec.second));
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	/*FULL pattern matching*/
	// Pattern a(v, "x") or Pattern a(v, "x + y + 123") or Pattern a("x", "x + y + 123")
	// Pattern a(_, "x + y + 123")
	/*expressionSpec is converted to post fix before passed into PatternInstruction*/
	TEST_METHOD(execute_lhsSynonymRhsIdentExactMatch1) {
		// assign1 = assign1 + x
		// Pattern a1(v, "assign1")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::FULL, postFixExpression);
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = Entity::insertVar("assign1");

		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<int> allPatternStmtInfo = Pattern::getAssignStmtsFromVarExprFullMatch(varIndex, postFixExpression);
		Assert::AreEqual(size_t(1), allPatternStmtInfo.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 \nSynonym: v Values: 1 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsSynonymRhsIdentExactMatch2) {
		// assign1 = assign1 + x + y
		// Pattern a1(v, "assign1 + x + y")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x+y");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::FULL, postFixExpression);
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = Entity::insertVar("assign1");

		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<int> allPatternStmtInfo = Pattern::getAssignStmtsFromVarExprFullMatch(varIndex, postFixExpression);
		Assert::AreEqual(size_t(1), allPatternStmtInfo.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 \nSynonym: v Values: 1 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}
	/*expressionSpec is converted to post fix before passed into PatternInstruction*/
	TEST_METHOD(execute_lhsIdentRhsIdentExactMatch) {
		// assign1 = assign1 + x
		// Pattern a1("assign1", "assign1 + x + y")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::IDENT, "assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x+y");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::FULL, postFixExpression);
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = Entity::insertVar("assign1");

		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<int> allPatternStmtInfo = Pattern::getAssignStmtsFromVarExprFullMatch(varIndex, postFixExpression);
		Assert::AreEqual(size_t(1), allPatternStmtInfo.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsWildCardRhsIdentExactMatch) {
		// assign1 = assign1 + x
		// Pattern a1(_, "assign1 + x + y")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x+y");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::FULL, postFixExpression);
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = Entity::insertVar("assign1");
		VarIndex varIndex2 = Entity::insertVar("assign2");

		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);
		Pattern::insertAssignInfo(varIndex2, postFixExpression, stmt2);

		// Check PKB populated
		std::tuple< std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprFullMatch(postFixExpression);
		Assert::AreEqual(size_t(2), std::get<0>(allPatternStmtInfo).size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(2), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 3 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}
	};
}
