#include "stdafx.h"
#include "CppUnitTest.h"

#include <algorithm>
#include <string>

#include "../source/QPS/PQLEvaluator.h"
#include "../source/PKB/PKBInserter.h"
#include "../source/QPS/NextTProcessor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestNextTInstructions) {
private:
	PKB* pkb;
	PKBGetter* pkbGetter;
	PKBInserter* pkbInserter;
	NextTProcessor* nextTProcessor;

	TEST_METHOD_INITIALIZE(init) {
		pkb = new PKB();
		pkbGetter = new PKBGetter(pkb);
		pkbInserter = new PKBInserter(pkb);
		nextTProcessor = new NextTProcessor(new NextTCache());
	}

public:

	TEST_METHOD(executeNextTInstruction_twoConstants_evaluatedTableFormed) {
		// 1. Setup:
		// Next (1, 2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::INTEGER, "1");
		rhsRef = std::make_pair(PqlReferenceType::INTEGER, "2");
		Instruction* instruction = new NextTInstruction(lhsRef, rhsRef, nextTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{};
		std::unordered_set<std::string> synonyms = instruction->getSynonyms();

		Assert::IsTrue(synonyms == expectedSynonyms);

		// PKB inserts statements
		StmtIndex stmt1 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt1, stmt2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		Assert::AreEqual(size_t(0), evTable.getNumRow());
		Assert::AreEqual(true, evTable.getEvResult());
	}

	TEST_METHOD(executeNextTInstruction_lhsConstRhsStmt_evaluatedTableFormed) {
		// 1. Setup:
		// Next(1, s2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::INTEGER, "1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s2");
		Instruction* instruction = new NextTInstruction(lhsRef, rhsRef, nextTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "s2" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts statements
		StmtIndex stmt1 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt1, stmt2);

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

	TEST_METHOD(executeNextTInstruction_lhsStmtRhsConst_evaluatedTableFormed) {
		// 1. Setup:
		// Next(s1, 2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s1");
		rhsRef = std::make_pair(PqlReferenceType::INTEGER, "2");
		Instruction* instruction = new NextTInstruction(lhsRef, rhsRef, nextTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "s1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts statements
		StmtIndex stmt1 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt1, stmt2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s7") != tableRef.end());

		// Test Entities:
		std::vector<Index> values{ 1 };
		auto actualValues = tableRef.at("s1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeNextTInstruction_twoStmts_evaluatedTableFormed) {
		// 1. Setup:
		// Next(s1, s2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s2");
		Instruction* instruction = new NextTInstruction(lhsRef, rhsRef, nextTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "s1", "s2" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 5 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 5; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}
		for (int i = 0; i < 4; i++) {
			pkbInserter->insertRSInfo(RelationshipType::NEXT, stmts[i], stmts[i + 1]);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(10), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(true, tableRef.find("s2") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s6") != tableRef.end());

		// Test Entities:
		std::vector<Index> s1values{ 1, 1, 1, 1, 2, 2, 2, 3, 3, 4 };
		auto actuals1Values = tableRef.at("s1");
		bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
		Assert::AreEqual(true, areVecEqual);
		std::vector<Index> s2values{ 2, 3, 4, 5, 3, 4, 5, 4, 5, 5 };
		auto actuals2Values = tableRef.at("s2");
		bool areVecEqual2 = std::equal(s2values.begin(), s2values.end(), actuals2Values.begin());
		Assert::AreEqual(true, areVecEqual2);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeNextTInstruction_twoStmtsSameSyn_evaluatedTableFormed) {
		// 1. Setup:
		// Next(s1, s1) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s1");
		Instruction* instruction = new NextTInstruction(lhsRef, rhsRef, nextTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "s1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 5 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 6; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}
		for (int i = 0; i < 4; i++) {
			pkbInserter->insertRSInfo(RelationshipType::NEXT, stmts[i], stmts[i + 1]);
		}
		// 5 is a while loop to itself
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmts[5], stmts[5]);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s2") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s6") != tableRef.end());

		// Test Entities:
		std::vector<Index> s1values{ 6 };
		auto actuals1Values = tableRef.at("s1");
		bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeNextTInstruction_lhsStmtRhsWildcardStress_evaluatedTableFormed) {
		// 1. Setup:
		// Next(s1, _) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "s1");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		Instruction* instruction = new NextTInstruction(lhsRef, rhsRef, nextTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "s1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 500 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 500; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}
		for (int i = 0; i < 500 - 1; i++) {
			pkbInserter->insertRSInfo(RelationshipType::NEXT, stmts[i], stmts[i + 1]);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(124750), evTable.getNumRow());
		// (499 * 500) / 2 = 124750

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("_") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("stress") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(1), tableRef.size()); // RHS wildcard will still have column (innerJoinMerge() will drop it during merge)

		// Test Entities:
		std::vector<Index> s1values;
		for (int i = 0; i < 500; i++) {
			for (int j = 0; j < 499 - i; j++) {
				s1values.emplace_back(i + 1);
			}
		}
		auto actuals1Values = tableRef.at("s1");
		// Cannot check equality due to misordering (actuals1Values is not {1, 2, ... 500}), this is PKB's optimisation.
		// Instead sort, and check for equality
		std::sort(actuals1Values.begin(), actuals1Values.end());
		bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
		Assert::AreEqual(true, areVecEqual); // s1values == {1, 2, ... 500}

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeNextTInstruction_lhsWildcardRhsConst_evaluatedTableFormed) {
		// 1. Setup:
		// Next(_, 3) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		rhsRef = std::make_pair(PqlReferenceType::INTEGER, "3");
		Instruction* instruction = new NextTInstruction(lhsRef, rhsRef, nextTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 3 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 3; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}
		for (int i = 0; i < 2; i++) {
			pkbInserter->insertRSInfo(RelationshipType::NEXT, stmts[i], stmts[i + 1]);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow()); // Only 1 Next of 3

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(false, tableRef.find("_") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("2") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("23") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(0), tableRef.size()); // RHS wildcard will still have column (innerJoinMerge() will drop it during merge)

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult); // there exists some statement whose Next is 3
	}

	TEST_METHOD(executeNextTInstruction_lhsWildcardRhsConstStress_evaluatedTableFormed) {
		// 1. Setup:
		// Next(_, 28) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		rhsRef = std::make_pair(PqlReferenceType::INTEGER, "28");
		Instruction* instruction = new NextTInstruction(lhsRef, rhsRef, nextTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 976 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 976; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}
		for (int i = 0; i < 976 - 1; i++) {
			pkbInserter->insertRSInfo(RelationshipType::NEXT, stmts[i], stmts[i + 1]);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(false, tableRef.find("_") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("28") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("stress") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(0), tableRef.size()); // RHS wildcard will still have column (innerJoinMerge() will drop it during merge)

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult); // there exists a statment whose Next is 28
	}

	TEST_METHOD(executeNextTInstruction_twoWildcards_evaluatedTableFormed) {
		// 1. Setup:
		// Next(_, _) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		Instruction* instruction = new NextTInstruction(lhsRef, rhsRef, nextTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 3 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 3; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}
		for (int i = 0; i < 2; i++) {
			pkbInserter->insertRSInfo(RelationshipType::NEXT, stmts[i], stmts[i + 1]);
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
		Assert::AreEqual(true, actualEvResult); // because Next rs exist
	}
	};
}
