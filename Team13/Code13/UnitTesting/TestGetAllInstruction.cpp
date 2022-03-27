#include "stdafx.h"
#include "CppUnitTest.h"

#include <algorithm>
#include <string>

#include "../source/PKB/Entity.h"
#include "../source/PKB/RS2.h"
#include "../source/QPS/PQLEvaluator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestGetAllInstructions) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Attribute::performCleanUp();
		Entity::performCleanUp();
	}
public:

	TEST_METHOD(executeGetAllInstruction_getAllStmt_evaluatedTableFormed) {
		// 1. Setup:
		// The 'Select s1' portion of the query
		Instruction* instruction = new GetAllInstruction(EntityType::STMT, "s1");

		// PKB inserts 5 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 5; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::ASSIGN_TYPE));
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(5), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 1, 2, 3, 4, 5 };
		auto actualValues = tableRef.at("s1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeGetAllInstruction_getAllPrintsStress_evaluatedTableFormed) {
		// 1. Setup:
		// The 'Select pn1' portion of the query
		Instruction* instruction = new GetAllInstruction(EntityType::PRINT, "pn1");

		// PKB inserts 99 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 49; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::PRINT_TYPE));
		}
		for (int i = 0; i < 50; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::ASSIGN_TYPE)); // extra
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(49), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("pn1") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, EntityType>
		std::vector<int> pn1values;
		for (int i = 0; i < 49; i++) {
			pn1values.emplace_back(i + 1);
		}
		auto actualValues = tableRef.at("pn1");
		// Cannot check equality due to unordering (actuals1Values is not {1, 2, ..., 49}), this is PKB's optimisation.
		// Instead sort, and check for equality
		std::sort(actualValues.begin(), actualValues.end());
		bool areVecEqual = std::equal(pn1values.begin(), pn1values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeGetAllInstruction_getAllVarStress_evaluatedTableFormed) {
		// 1. Setup:
		// The 'Select pn1' portion of the query
		Instruction* instruction = new GetAllInstruction(EntityType::VARIABLE, "v1");

		// PKB inserts 99 statements
		std::vector<VarIndex> vars;
		std::vector<std::string> varNames;
		for (int i = 0; i < 99; i++) {
			std::string varName = "var" + std::to_string(i);
			VarIndex var = Entity::insertVar(varName);
			vars.emplace_back(var);
			varNames.emplace_back(varName);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(99), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("v1") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, EntityType>
		std::vector<int> v1values;
		for (int i = 0; i < 99; i++) {
			v1values.emplace_back(i + 1);
		}
		auto actualValues = tableRef.at("v1");
		// Cannot check equality due to unordering (actuals1Values is not {1, 2, ..., 99}), this is PKB's optimisation.
		// Instead sort, and check for equality
		std::sort(actualValues.begin(), actualValues.end());
		bool areVecEqual = std::equal(v1values.begin(), v1values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeGetAllInstruction_getAllConstStress_evaluatedTableFormed) {
		// 1. Setup:
		// The 'Select pn1' portion of the query
		Instruction* instruction = new GetAllInstruction(EntityType::CONSTANT, "c1");

		// PKB inserts 99 statements
		for (int i = 0; i < 99; i++) {
			Entity::insertConst(i + 1);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(99), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("c1") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, EntityType>
		std::vector<int> c1values;
		for (int i = 0; i < 99; i++) {
			c1values.emplace_back(i + 1);
		}
		auto actualValues = tableRef.at("c1");
		// Cannot check equality due to unordering (actuals1Values is not {1, 2, ..., 99}), this is PKB's optimisation.
		// Instead sort, and check for equality
		std::sort(actualValues.begin(), actualValues.end());
		bool areVecEqual = std::equal(c1values.begin(), c1values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}
	};
}
