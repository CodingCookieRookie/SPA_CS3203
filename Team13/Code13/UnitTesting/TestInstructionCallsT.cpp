#include "stdafx.h"
#include "CppUnitTest.h"

#include <algorithm>
#include <string>

#include "../source/QPS/PQLEvaluator.h"
#include "../source/PKB/PKBInserter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestCallsTInstructions) {
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

	TEST_METHOD(executeCallsTInstruction_twoIdents_evaluatedTableFormed) {
		// 1. Setup:
		// Calls("first", "second") RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::IDENT, "procedure2");
		rhsRef = std::make_pair(PqlReferenceType::IDENT, "procedure3");
		Instruction* instruction = new CallsTInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 4 procs, establishes 3 calls relationships
		std::vector<ProcIndex> procs;
		std::string procName;
		for (int i = 0; i < 4; i++) {
			procName = "procedure" + std::to_string(i + 1);
			procs.emplace_back(pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, procName));
		}

		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[0], procs[1]);
		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[1], procs[2]);
		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[1], procs[3]);
		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[2], procs[3]);
		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[0], procs[3]);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		Assert::AreEqual(size_t(0), evTable.getNumRow());
		Assert::AreEqual(true, evTable.getEvResult());
	}

	TEST_METHOD(executeCallsTInstruction_lhsIdentRhsProc_evaluatedTableFormed) {
		// 1. Setup:
		// Calls*("procedure1", q) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::IDENT, "procedure1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "q");
		Instruction* instruction = new CallsTInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "q" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 4 procs, establishes 3 calls relationships
		std::vector<ProcIndex> procs;
		std::string procName;
		for (int i = 0; i < 4; i++) {
			procName = "procedure" + std::to_string(i + 1);
			procs.emplace_back(pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, procName));
		}

		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[0], procs[1]);
		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[1], procs[2]);
		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[0], procs[2]);
		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[1], procs[2]);
		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[0], procs[3]);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(3), evTable.getNumRow()); // 1 calls 2, which calls 3 and 4.

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("q") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("r") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 2, 3, 4 };
		auto actualValues = tableRef.at("q");
		std::sort(actualValues.begin(), actualValues.end());
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeCallsTInstruction_lhsProcRhsIdent_evaluatedTableFormed) {
		// 1. Setup:
		// Calls*(p, "procedure4") RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "p");
		rhsRef = std::make_pair(PqlReferenceType::IDENT, "procedure4");
		Instruction* instruction = new CallsTInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "p" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 4 procs, establishes 4 calls relationships
		std::vector<ProcIndex> procs;
		std::string procName;
		for (int i = 0; i < 4; i++) {
			procName = "procedure" + std::to_string(i + 1);
			procs.emplace_back(pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, procName));
		}

		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[0], procs[1]);
		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[1], procs[2]);

		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[0], procs[3]);
		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[1], procs[3]);
		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[2], procs[3]);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(3), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("t") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 1, 2, 3 };
		auto actualValues = tableRef.at("p");
		std::sort(actualValues.begin(), actualValues.end());
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeCallsTInstruction_twoProcs_evaluatedTableFormed) {
		// 1. Setup:
		// Calls*(p, q) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "p");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "q");
		Instruction* instruction = new CallsTInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "p", "q" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 4 procs, establishes 3 calls relationships
		std::vector<ProcIndex> procs;
		std::string procName;
		for (int i = 0; i < 4; i++) {
			procName = "procedure" + std::to_string(i + 1);
			procs.emplace_back(pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, procName));
		}

		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[0], procs[1]);
		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[0], procs[2]);
		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[0], procs[3]);
		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[1], procs[2]);
		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[1], procs[3]);
		pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[2], procs[3]);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(6), evTable.getNumRow()); // {1 2, 1 3, 1 4, 2 3, 2 4, 3 4}

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p") != tableRef.end());
		Assert::AreEqual(true, tableRef.find("q") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("r") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("t") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, EntityType>
		std::vector<int> s1values{ 1, 1, 1, 2, 2, 3 };
		auto actuals1Values = tableRef.at("p");
		std::sort(actuals1Values.begin(), actuals1Values.end());
		bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
		Assert::AreEqual(true, areVecEqual);
		std::vector<int> s2values{ 2, 3, 3, 4, 4, 4 };
		auto actuals2Values = tableRef.at("q");
		std::sort(actuals2Values.begin(), actuals2Values.end());
		bool areVecEqual2 = std::equal(s2values.begin(), s2values.end(), actuals2Values.begin());
		Assert::AreEqual(true, areVecEqual2);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeCallsTInstruction_twoProcStress_evaluatedTableFormed) {
		// 1. Setup:
		// Calls*(s1, s2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "p");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "q");
		Instruction* instruction = new CallsTInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "p", "q" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 19 procs, establishes 18 Calls relationships
		std::vector<ProcIndex> procs;
		std::string procName;
		for (int i = 0; i < 19; i++) {
			procName = "procedure" + std::to_string(i + 1);
			procs.emplace_back(pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, procName));
		}

		for (int i = 0; i < 19; i++) {
			for (int j = i + 1; j < 19; j++) {
				// i Calls i + 1, simulate 19 Calls relationships
				pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[i], procs[j]);
			}
		}
		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(171), evTable.getNumRow());
		// {1 2, 1 3, ..., 1 19, 2 3, 2 4, ..., 2 19, ... , 18 19}. Hence, (19*18)/2 = 171

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p") != tableRef.end());
		Assert::AreEqual(true, tableRef.find("q") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("_") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("r") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(2), tableRef.size()); // 2 columns: p and q

		// Test Entities: std::unordered_map<std::string, EntityType>
		std::vector<int> pvalues, qValues;
		for (int i = 0; i < 18; i++) {
			for (int j = 0; j < (18 - i); j++) {
				pvalues.emplace_back(i + 1);
			}
		}
		for (int i = 0; i < 18; i++) {
			for (int j = i; j < 18; j++) {
				qValues.emplace_back(j + 2);
			}
		}

		auto actualpValues = tableRef.at("p");
		std::sort(actualpValues.begin(), actualpValues.end());
		bool areVecEqual = std::equal(pvalues.begin(), pvalues.end(), actualpValues.begin());
		Assert::AreEqual(true, areVecEqual); // pValues == {19 1s, 18 2s, ... 1 19}
		auto actualqValues = tableRef.at("q");
		std::sort(actualqValues.begin(), actualqValues.end());
		std::sort(qValues.begin(), qValues.end());
		bool areVecEqual2 = std::equal(qValues.begin(), qValues.end(), actualqValues.begin());
		Assert::AreEqual(true, areVecEqual2); // qValues == {2-19, 3-19, ..., 18, 19, 19}

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeCallsTInstruction_lhsProcRhsWildcardStress_evaluatedTableFormed) {
		// 1. Setup:
		// Calls*(p, _) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "p");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		Instruction* instruction = new CallsTInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "p" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 99 procs, establishes 98 Calls relationships
		std::vector<ProcIndex> procs;
		std::string procName;
		for (int i = 0; i < 99; i++) {
			procName = "procedure" + std::to_string(i + 1);
			procs.emplace_back(pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, procName));
		}

		for (int i = 0; i < 99; i++) {
			for (int j = i + 1; j < 99; j++) {
				// i Calls i + 1, simulate 98 Calls relationships
				pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[i], procs[j]);
			}
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t((99 * 98) / 2), evTable.getNumRow());
		// {1 2, 1 3, ..., 1 99, 2 3, 2 4, ..., 2 99, ... , 98 99}. (99*98)/2

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("r") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("_") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(1), tableRef.size()); // RHS wildcard will have no column (not of concern)

		// Test Entities: std::unordered_map<std::string, EntityType>
		std::vector<int> pValues, wildcardValues;
		for (int i = 0; i < 99 - 1; i++) {
			for (int j = 0; j < (99 - 1 - i); j++) {
				pValues.emplace_back(i + 1);
			}
		}

		auto actualspValues = tableRef.at("p");
		std::sort(actualspValues.begin(), actualspValues.end());
		std::sort(pValues.begin(), pValues.end());
		bool areVecEqual = std::equal(pValues.begin(), pValues.end(), actualspValues.begin());
		Assert::AreEqual(true, areVecEqual); // s1values == {99 1s, 98 2s, ... 1 99}

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeCallsTInstruction_lhsWildcardRhsIdentStress_evaluatedTableFormed) {
		// 1. Setup:
		// Calls*(_, "procedure87") RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		rhsRef = std::make_pair(PqlReferenceType::IDENT, "procedure87");
		Instruction* instruction = new CallsTInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 99 procs, establishes 98 Calls relationships
		std::vector<ProcIndex> procs;
		std::string procName;
		for (int i = 0; i < 99; i++) {
			procName = "procedure" + std::to_string(i + 1);
			procs.emplace_back(pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, procName));
		}

		for (int i = 0; i < 99 - 1; i++) {
			// i Calls i + 1, simulate 98 Calls relationships
			pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[i], procs[i + 1]);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow()); // not 86, wildcard has no column and row

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(false, tableRef.find("_") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("procedure87") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(0), tableRef.size()); // LHS wildcard will still have column (innerJoinMerge() will drop it during merge)

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult); // there exists a procedure which calls* of 87
	}

	TEST_METHOD(executeCallsTInstruction_twoWildcardsStress_evaluatedTableFormed) {
		// 1. Setup:
		// Calls*(_, _) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		Instruction* instruction = new CallsTInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts 99 procs, establishes 98 Calls relationships
		std::vector<ProcIndex> procs;
		std::string procName;
		for (int i = 0; i < 99; i++) {
			procName = "procedure" + std::to_string(i + 1);
			procs.emplace_back(pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, procName));
		}

		for (int i = 0; i < 99 - 1; i++) {
			// i Calls i + 1, simulate 98 Calls relationships
			pkbInserter->insertRSInfo(RelationshipType::CALLS_T, procs[i], procs[i + 1]);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow()); // Evaluating 2 wildcards will return a boolean with no table.

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(false, tableRef.find("_") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("p") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("q") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(0), tableRef.size()); // Two wildcards will have no columns => only have boolean

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult); // because Follows* rs exist
	}
	};
}
