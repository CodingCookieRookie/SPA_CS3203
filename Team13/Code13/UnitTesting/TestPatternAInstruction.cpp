#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/PKB/PKBGetter.h"
#include "../source/PKB/PKBInserter.h"
#include "../source/QPS/PQLEvaluator.h"
#include "../source/QPS/PQLParser.h"
#include "PKB/ExpressionProcessor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPatternAInstruction) {
private:
	PKB pkb;
	PKBGetter* pkbGetter;
	PKBInserter* pkbInserter;

	TEST_METHOD_INITIALIZE(init) {
		pkb = PKB();
		pkbGetter = new PKBGetter(pkb);
		pkbInserter = new PKBInserter(pkb);
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
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+234");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<Index> allStmts = pkbGetter->getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
		Assert::AreEqual(size_t(0), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: \nSynonym: v Values: \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsIdentRhsIdent_LeftIdentOutOfBoundsEvTableFalse) {
		// assign1 = assign1 + x
		// Pattern a1("v", "_xx_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::IDENT, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::PARTIAL, " xx ");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<Index> allStmts = pkbGetter->getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
		Assert::AreEqual(size_t(0), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.toString());
	}
	TEST_METHOD(execute_lhsSynonymRhsIdentVarPartialPqlinsertAssignInfoTest1) {
		// assign1 = assign1 + x
		// Pattern a1(v, "_xx_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::PARTIAL, " xx ");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<Index> allStmts = pkbGetter->getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
		Assert::AreEqual(size_t(0), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: \nSynonym: v Values: \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsSynonymRhsIdentVarPartialPqlinsertAssignInfoTest2) {
		// assign1 = assign1 + xx
		// Pattern a1(v, "_x_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::PARTIAL, " x ");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+xx");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<Index>, std::vector<Index>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(0), std::get<0>(allPatternStmtInfo).size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: \nSynonym: v Values: \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsSynonymRhsIdentVar) {
		// assign1 = assign1 + x
		// Pattern a1(v, "_x_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::PARTIAL, " x ");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<Index> allStmts = pkbGetter->getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 \nSynonym: v Values: 2 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsSynonymRhsIdentConstant) {   // assign1 = assign1 + 123
		// Pattern a1(v, "_123_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::PARTIAL, " 123 ");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "assign1");
		pkbInserter->insertConst(123);
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+123");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<Index> allStmts = pkbGetter->getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 \nSynonym: v Values: 1 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsIdentRhsIdentSynonym) {
		// assign1 = assign1 + x
		// Pattern a1("assign1", "_x_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::IDENT, "assign1");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::PARTIAL, " x ");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<Index> allStmts = pkbGetter->getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsIdentRhsIdentConstant) {
		// assign1 = assign1 + 123
		// Pattern a1("assign1", "_123_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::IDENT, "assign1");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::PARTIAL, " 123 ");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertConst(123);
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+123");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<Index> allStmts = pkbGetter->getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsSynonymRhsWildCard) {
		// assign1 = assign1 + x
		// Pattern a1(v, *)
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::WILDCARD, "");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::tuple<std::vector<Index>, std::vector<Index>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(expressionSpec.second));
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 \nSynonym: v Values: 1 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsIdentRhsWildCard) {
		// assign1 = assign1 + x
		// Pattern a1("x", *)
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::IDENT, "assign1");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::WILDCARD, "");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::tuple<std::vector<Index>, std::vector<Index>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(expressionSpec.second));
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsWildCardRhsIdent) {
		// assign1 = assign1 + x
		// Pattern a1(*, "_x_")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::PARTIAL, " x ");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::tuple<std::vector<Index>, std::vector<Index>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(expressionSpec.second);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsWildCardRhsWildCard) {
		// assign1 = assign1 + x
		// Pattern a1(*, *)
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::WILDCARD, "");
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "assign1");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::tuple<std::vector<Index>, std::vector<Index>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(expressionSpec.second));
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
		Assert::AreEqual(expected, evTable.toString());
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
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "assign1");

		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<Index> allPatternStmtInfo = pkbGetter->getAssignStmtsFromVarExprFullMatch(varIndex, postFixExpression);
		Assert::AreEqual(size_t(1), allPatternStmtInfo.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 \nSynonym: v Values: 1 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsSynonymRhsIdentExactMatch2) {
		// assign1 = assign1 + x + y
		// Pattern a1(v, "assign1 + x + y")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x+y");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::FULL, postFixExpression);
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "assign1");

		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<Index> allPatternStmtInfo = pkbGetter->getAssignStmtsFromVarExprFullMatch(varIndex, postFixExpression);
		Assert::AreEqual(size_t(1), allPatternStmtInfo.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 \nSynonym: v Values: 1 \n";
		Assert::AreEqual(expected, evTable.toString());
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
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "assign1");

		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::vector<Index> allPatternStmtInfo = pkbGetter->getAssignStmtsFromVarExprFullMatch(varIndex, postFixExpression);
		Assert::AreEqual(size_t(1), allPatternStmtInfo.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsWildCardRhsIdentExactMatch) {
		// assign1 = assign1 + x
		// Pattern a1(_, "assign1 + x + y")
		// 1. Setup:
		std::string synonym = "a1";
		PqlReference entRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("assign1+x+y");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::FULL, postFixExpression);
		Instruction* instruction = new PatternAInstruction(synonym, entRef, expressionSpec, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "assign1");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "assign2");

		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);
		pkbInserter->insertAssignInfo(varIndex2, postFixExpression, stmt2);

		// Check PKB populated
		std::tuple< std::vector<Index>, std::vector<Index>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprFullMatch(postFixExpression);
		Assert::AreEqual(size_t(2), std::get<0>(allPatternStmtInfo).size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(2), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 3 \n";
		Assert::AreEqual(expected, evTable.toString());
	}
	};
}
