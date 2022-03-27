#include "stdafx.h"
#include "CppUnitTest.h"

#include <algorithm>
#include <string>

#include "../source/PKB/Entity.h"
#include "../source/PKB/Calls.h"
#include "../source/PKB/RS2.h"
#include "../source/QPS/PQLEvaluator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestCallsInstructions) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Attribute::performCleanUp();
		Entity::performCleanUp();
		Calls::performCleanUp();
	}

public:

	TEST_METHOD(executeCallsInstruction_twoIdents_evaluatedTableFormed) {
		// 1. Setup:
		// Calls("first", "second") RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::IDENT, "first");
		rhsRef = std::make_pair(PqlReferenceType::IDENT, "second");
		Instruction* instruction = new CallsInstruction(lhsRef, rhsRef);

		// PKB inserts statements
		ProcIndex proc1 = Entity::insertProc("first");
		ProcIndex proc2 = Entity::insertProc("second");
		Calls::insert(proc1, proc2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		Assert::AreEqual(size_t(0), evTable.getNumRow());
		Assert::AreEqual(true, evTable.getEvResult());
	}

	TEST_METHOD(executeCallsInstruction_lhsIdentRhsProc_evaluatedTableFormed) {
		// 1. Setup:
		// Calls("first", q) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::IDENT, "first");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "q");
		Instruction* instruction = new CallsInstruction(lhsRef, rhsRef);

		// PKB inserts statements
		ProcIndex proc1 = Entity::insertProc("first");
		ProcIndex proc2 = Entity::insertProc("second");
		Calls::insert(proc1, proc2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("q") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("first") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 2 };
		auto actualValues = tableRef.at("q");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);
		auto actualEntities = evTable.getEntities();
		Assert::AreEqual(true, actualEntities.find("q") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("m") != actualEntities.end());
		bool isEntityType = EntityType::PROCEDURE == actualEntities.at("q");
		Assert::AreEqual(true, isEntityType);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeCallsInstruction_lhsProcRhsIdent_evaluatedTableFormed) {
		// 1. Setup:
		// Calls(p, "second") RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "p");
		rhsRef = std::make_pair(PqlReferenceType::IDENT, "second");
		Instruction* instruction = new CallsInstruction(lhsRef, rhsRef);

		// PKB inserts statements
		ProcIndex proc1 = Entity::insertProc("first");
		ProcIndex proc2 = Entity::insertProc("second");
		ProcIndex proc3 = Entity::insertProc("third");
		Calls::insert(proc1, proc2);
		Calls::insert(proc3, proc2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(2), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("q") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 1, 3 };
		auto actualValues = tableRef.at("p");
		std::sort(actualValues.begin(), actualValues.end());
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);
		auto actualEntities = evTable.getEntities();
		Assert::AreEqual(true, actualEntities.find("p") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("m") != actualEntities.end());
		bool isEntityType = EntityType::PROCEDURE == actualEntities.at("p");
		Assert::AreEqual(true, isEntityType);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeCallsInstruction_twoProcs_evaluatedTableFormed) {
		// 1. Setup:
		// Calls(p, q) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "p");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "q");
		Instruction* instruction = new CallsInstruction(lhsRef, rhsRef);

		// PKB inserts 4 procs
		std::vector<ProcIndex> procs;
		std::string procName = "procedure";
		for (int i = 0; i < 4; i++) {
			procName += std::to_string(i + 1);
			procs.emplace_back(Entity::insertProc(procName));
		}
		for (int i = 0; i < 3; i++) {
			Calls::insert(procs[i], procs[i + 1]);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(3), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("r") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, EntityType>
		std::vector<int> pvalues{ 1, 2, 3 };
		auto actualpValues = tableRef.at("p");
		bool areVecEqual = std::equal(pvalues.begin(), pvalues.end(), actualpValues.begin());
		Assert::AreEqual(true, areVecEqual);
		std::vector<int> qvalues{ 2, 3, 4 };
		auto actualqValues = tableRef.at("q");
		bool areVecEqual2 = std::equal(qvalues.begin(), qvalues.end(), actualqValues.begin());
		Assert::AreEqual(true, areVecEqual2);

		auto actualEntities = evTable.getEntities();
		Assert::AreEqual(true, actualEntities.find("p") != actualEntities.end());
		Assert::AreEqual(true, actualEntities.find("q") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("r") != actualEntities.end());
		bool isEntityType = EntityType::PROCEDURE == actualEntities.at("p");
		bool isEntityType2 = EntityType::PROCEDURE == actualEntities.at("q");
		Assert::AreEqual(true, isEntityType);
		Assert::AreEqual(true, isEntityType2);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeCallsInstruction_lhsProcRhsWildcardStress_evaluatedTableFormed) {
		// 1. Setup:
		// Calls(p, _) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "p");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		Instruction* instruction = new CallsInstruction(lhsRef, rhsRef);

		// PKB inserts 19 procedures
		std::vector<ProcIndex> procs;
		std::string procName = "procedure";
		for (int i = 0; i < 19; i++) {
			procName += std::to_string(i + 1);
			procs.emplace_back(Entity::insertProc(procName));
		}
		for (int i = 0; i < 18; i++) {
			Calls::insert(procs[i], procs[i + 1]);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(18), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("r") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(1), tableRef.size()); // RHS wildcard will not have column (not of concern)

		// Test Entities: std::unordered_map<std::string, EntityType>
		std::vector<int> pvalues, wildcardValues;
		for (int i = 0; i < 18; i++) {
			pvalues.emplace_back(i + 1);
		}
		auto actualpValues = tableRef.at("p");
		std::sort(actualpValues.begin(), actualpValues.end());
		bool areVecEqual = std::equal(pvalues.begin(), pvalues.end(), actualpValues.begin());
		Assert::AreEqual(true, areVecEqual); // pvalues == {1, 2, ... 18}

		auto actualEntities = evTable.getEntities();
		Assert::AreEqual(true, actualEntities.find("p") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("_") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("r") != actualEntities.end());
		bool isEntityType = EntityType::PROCEDURE == actualEntities.at("p");
		Assert::AreEqual(true, isEntityType);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeCallsInstruction_twoWildcards_evaluatedTableFormed) {
		// 1. Setup:
		// Calls(_, _) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		Instruction* instruction = new CallsInstruction(lhsRef, rhsRef);

		// PKB inserts 19 procedures
		std::vector<ProcIndex> procs;
		std::string procName = "procedure";
		for (int i = 0; i < 19; i++) {
			procName += std::to_string(i + 1);
			procs.emplace_back(Entity::insertProc(procName));
		}
		for (int i = 0; i < 18; i++) {
			Calls::insert(procs[i], procs[i + 1]);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(false, tableRef.find("_") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("p") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(0), tableRef.size()); // Two wildcards will have no columns => only have boolean

		// Test Entities: std::unordered_map<std::string, EntityType>
		auto actualEntities = evTable.getEntities();
		Assert::AreEqual(false, actualEntities.find("_") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("q") != actualEntities.end());

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult); // because Calls rs exist
	}
	};
}
