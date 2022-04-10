#include "stdafx.h"
#include "CppUnitTest.h"

#include <algorithm>
#include <string>

#include "../source/QPS/PQLEvaluator.h"
#include "../source/PKB/PKBInserter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestFollowsInstructions) {
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
	TEST_METHOD(executeFollowsInstruction_twoConstants_evaluatedTableFormed) {
		// 1. Setup:
		// Follows(1, 2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::INTEGER, "1");
		rhsRef = std::make_pair(PqlReferenceType::INTEGER, "2");
		Instruction* instruction = new FollowsInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts statements
		StmtIndex stmt1 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::FOLLOWS, stmt1, stmt2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		Assert::AreEqual(size_t(0), evTable.getNumRow());
		Assert::AreEqual(true, evTable.getEvResult());
	}

	TEST_METHOD(executeFollowsInstruction_lhsConstantRhsStmt_evaluatedTableFormed) {
		// 1. Setup:
		// Follows(1, s2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::INTEGER, "1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s2");
		Instruction* instruction = new FollowsInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "s2" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts statements
		StmtIndex stmt1 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::FOLLOWS, stmt1, stmt2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s2") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s3") != tableRef.end());

		// Test Entities:
		std::vector<Index> values{ 2 };
		auto actualValues = tableRef.at("s2");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeFollowsInstruction_lhsStmrRhsConst_evaluatedTableFormed) {
		// 1. Setup:
		// Follows(s1, 2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s1");
		rhsRef = std::make_pair(PqlReferenceType::INTEGER, "2");
		Instruction* instruction = new FollowsInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "s1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts statements
		StmtIndex stmt1 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::FOLLOWS, stmt1, stmt2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s5") != tableRef.end());

		// Test Entities:
		std::vector<Index> values{ 1 };
		auto actualValues = tableRef.at("s1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeFollowsInstruction_twoStmts_evaluatedTableFormed) {
		// 1. Setup:
		// Follows(s1, s2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s2");
		Instruction* instruction = new FollowsInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "s1", "s2" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 4 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 4; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}
		for (int i = 0; i < 3; i++) {
			pkbInserter->insertRSInfo(RelationshipType::FOLLOWS, stmts[i], stmts[i + 1]);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(3), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(true, tableRef.find("s2") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s5") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s12") != tableRef.end());

		// Test Entities:
		std::vector<Index> s1values{ 1, 2, 3 };
		auto actuals1Values = tableRef.at("s1");
		bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
		Assert::AreEqual(true, areVecEqual);
		std::vector<Index> s2values{ 2, 3, 4 };
		auto actuals2Values = tableRef.at("s2");
		bool areVecEqual2 = std::equal(s2values.begin(), s2values.end(), actuals2Values.begin());
		Assert::AreEqual(true, areVecEqual2);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeFollowsInstruction_lhsStmtRhsWildcardStress_evaluatedTableFormed) {
		// 1. Setup:
		// Follows(s1, _) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s1");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		Instruction* instruction = new FollowsInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "s1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 19 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 19; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}
		for (int i = 0; i < 18; i++) {
			pkbInserter->insertRSInfo(RelationshipType::FOLLOWS, stmts[i], stmts[i + 1]);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(18), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("_") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s207") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(1), tableRef.size()); // RHS wildcard will not have column (not of concern)

		// Test Entities:
		std::vector<Index> s1values, wildcardValues;
		for (int i = 0; i < 18; i++) {
			s1values.emplace_back(i + 1);
		}
		auto actuals1Values = tableRef.at("s1");
		bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
		Assert::AreEqual(true, areVecEqual); // s1values == {1, 2, ... 18}

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeFollowsInstruction_twoWildcards_evaluatedTableFormed) {
		// 1. Setup:
		// Parent(_, _) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		Instruction* instruction = new FollowsInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 3 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 3; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}
		for (int i = 0; i < 2; i++) {
			pkbInserter->insertRSInfo(RelationshipType::FOLLOWS, stmts[i], stmts[i + 1]);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow()); //

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(false, tableRef.find("_") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s2") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(0), tableRef.size()); // Two wildcards will have no columns => only have boolean

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult); // because Follows rs exist
	}
	};
}
