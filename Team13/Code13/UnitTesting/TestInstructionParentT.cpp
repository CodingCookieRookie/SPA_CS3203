#include "stdafx.h"
#include "CppUnitTest.h"

#include <algorithm>
#include <string>

#include "../source/QPS/PQLEvaluator.h"
#include "../source/PKB/PKBInserter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestParentTInstructions) {
private:
	PKB* pkb;
	PKBGetter* pkbGetter;
	PKBInserter* pkbInserter;

	TEST_METHOD_INITIALIZE(init) {
		pkb = new PKB();
		pkbGetter = new PKBGetter(pkb);
		pkbInserter = new PKBInserter(pkb);
	}

public:

	TEST_METHOD(executeParentStarInstruction_twoConstants_evaluatedTableFormed) {
		// 1. Setup:
		// Parent*(1, 2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::INTEGER, "1");
		rhsRef = std::make_pair(PqlReferenceType::INTEGER, "2");
		Instruction* instruction = new ParentStarInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts statements
		StmtIndex stmt1 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmt1, stmt2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		Assert::AreEqual(size_t(0), evTable.getNumRow());
		Assert::AreEqual(true, evTable.getEvResult());
	}

	TEST_METHOD(executeParentStarInstruction_lhsConstRhsStmt_evaluatedTableFormed) {
		// 1. Setup:
		// Parent*(1, s2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::INTEGER, "1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s2");
		Instruction* instruction = new ParentStarInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "s2" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 4 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 4; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}

		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[0], stmts[1]);
		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[1], stmts[2]);
		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[0], stmts[2]);
		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[1], stmts[2]);
		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[0], stmts[3]);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(3), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s2") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s3") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 2, 3, 4 };
		auto actualValues = tableRef.at("s2");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeParentStarInstruction_lhsStmtRhsConst_evaluatedTableFormed) {
		// 1. Setup:
		// Parent*(s1, 4) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s1");
		rhsRef = std::make_pair(PqlReferenceType::INTEGER, "4");
		Instruction* instruction = new ParentStarInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "s1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 4 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 4; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}

		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[0], stmts[1]);
		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[1], stmts[2]);

		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[0], stmts[3]);
		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[1], stmts[3]);
		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[2], stmts[3]);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(3), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s5") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 1, 2, 3 };
		auto actualValues = tableRef.at("s1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeParentStarInstruction_twoStmts_evaluatedTableFormed) {
		// 1. Setup:
		// Follows'*(s1, s2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s2");
		Instruction* instruction = new ParentStarInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "s1", "s2" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 4 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 4; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}

		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[0], stmts[1]);
		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[0], stmts[2]);
		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[0], stmts[3]);
		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[1], stmts[2]);
		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[1], stmts[3]);
		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[2], stmts[3]);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(6), evTable.getNumRow()); // {1 2, 1 3, 1 4, 2 3, 2 4, 3 4}

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(true, tableRef.find("s2") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s5") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s12") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, EntityType>
		std::vector<int> s1values{ 1, 1, 1, 2, 2, 3 };
		auto actuals1Values = tableRef.at("s1");
		std::sort(actuals1Values.begin(), actuals1Values.end());
		bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
		Assert::AreEqual(true, areVecEqual);
		std::vector<int> s2values{ 2, 3, 3, 4, 4, 4 };
		auto actuals2Values = tableRef.at("s2");
		std::sort(actuals2Values.begin(), actuals2Values.end());
		bool areVecEqual2 = std::equal(s2values.begin(), s2values.end(), actuals2Values.begin());
		Assert::AreEqual(true, areVecEqual2);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeParentStarInstruction_twoStmtsStress_evaluatedTableFormed) {
		// 1. Setup:
		// Parent*(s1, s2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s2");
		Instruction* instruction = new ParentStarInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "s1", "s2" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 19 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 19; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}

		for (int i = 0; i < 19; i++) {
			for (int j = i + 1; j < 19; j++) {
				pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[i], stmts[j]); // i parents i + 1, simulate 19 nesting levels
			}
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(171), evTable.getNumRow());
		// {1 2, 1 3, ..., 1 19, 2 3, 2 4, ..., 2 19, ... , 18 19}. Hence, (19*18)/2 = 171

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("_") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s207") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(2), tableRef.size()); // 2 columns: s1 and s2

		// Test Entities: std::unordered_map<std::string, EntityType>Instruction* instruction = new ParentInstruction(lhsRef, rhsRef);
		std::vector<int> s1values, s2values;
		for (int i = 0; i < 18; i++) {
			for (int j = 0; j < (18 - i); j++) {
				s1values.emplace_back(i + 1);
			}
		}
		for (int i = 0; i < 18; i++) {
			for (int j = i; j < 18; j++) {
				s2values.emplace_back(j + 2);
			}
		}

		auto actuals1Values = tableRef.at("s1");
		std::sort(actuals1Values.begin(), actuals1Values.end());
		bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
		Assert::AreEqual(true, areVecEqual); // s1values == {19 1s, 18 2s, ... 1 19}
		auto actuals2Values = tableRef.at("s2");
		std::sort(actuals2Values.begin(), actuals2Values.end());
		std::sort(s2values.begin(), s2values.end());
		bool areVecEqual2 = std::equal(s2values.begin(), s2values.end(), actuals2Values.begin());
		Assert::AreEqual(true, areVecEqual2); // wildcardValues == {2-19, 3-19, ..., 18, 19, 19}

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeParentStarInstruction_lhsStmtRhsWildcardStress_evaluatedTableFormed) {
		// 1. Setup:
		// Parent*(s1, _) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s1");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		Instruction* instruction = new ParentStarInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "s1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 99 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 99; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}

		for (int i = 0; i < 99; i++) {
			for (int j = i + 1; j < 99; j++) {
				pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[i], stmts[j]); // i parents i + 1, simulate 99 nesting levels
			}
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t((99 * 98) / 2), evTable.getNumRow());
		// {1 2, 1 3, ..., 1 99, 2 3, 2 4, ..., 2 99, ... , 98 99}. (99*98)/2

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("_") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s207") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(1), tableRef.size()); // RHS wildcard will have no column (not of concern)

		// Test Entities: std::unordered_map<std::string, EntityType>
		std::vector<int> s1values, wildcardValues;
		for (int i = 0; i < 99 - 1; i++) {
			for (int j = 0; j < (99 - 1 - i); j++) {
				s1values.emplace_back(i + 1);
			}
		}

		auto actuals1Values = tableRef.at("s1");
		std::sort(actuals1Values.begin(), actuals1Values.end());
		std::sort(s1values.begin(), s1values.end());
		bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
		Assert::AreEqual(true, areVecEqual); // s1values == {99 1s, 98 2s, ... 1 99}

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeParentStarInstruction_lhsWildcardRhsConstStress_evaluatedTableFormed) {
		// 1. Setup:
		// Parent*(_, 87) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		rhsRef = std::make_pair(PqlReferenceType::INTEGER, "87");
		Instruction* instruction = new ParentStarInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 99 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 99; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}

		for (int i = 0; i < 99 - 1; i++) {
			pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[i], stmts[i + 1]); // i parents i + 1
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow()); // not 86, wildcard has no column and row

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(false, tableRef.find("_") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("87") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("stress") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(0), tableRef.size()); // LHS wildcard will still have column (innerJoinMerge() will drop it during merge)

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult); // there exists a parent* of 87
	}

	TEST_METHOD(executeParentStarInstruction_twoWildcards_evaluatedTableFormed) {
		// 1. Setup:
		// Parent*(_, _) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		Instruction* instruction = new ParentStarInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 3 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 4; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}

		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[0], stmts[1]);
		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[0], stmts[2]);
		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[1], stmts[2]);
		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[1], stmts[3]);
		pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[2], stmts[3]);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow()); // Evaluating 2 wildcards will return a boolean with no table.

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(false, tableRef.find("_") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s2") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(0), tableRef.size()); // Two wildcards will have no columns => only have boolean

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult); // because Follows* rs exist
	}
	};
}
