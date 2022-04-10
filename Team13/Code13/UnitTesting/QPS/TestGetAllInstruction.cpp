#include "stdafx.h"
#include "CppUnitTest.h"

#include <algorithm>
#include <string>

#include "../source/PKB/PKBGetter.h"
#include "../source/PKB/PKBInserter.h"
#include "../source/QPS/Evaluator/PQLEvaluator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestGetAllInstructions) {
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

	TEST_METHOD(executeGetAllInstruction_getAllStmt_evaluatedTableFormed) {
		// 1. Setup:
		// The 'Select s1' portion of the query
		ParsedGetAll getAllSynonym = ParsedGetAll(EntityType::STMT, "s1");
		Instruction* instruction = getAllSynonym.toInstruction(pkbGetter);

		// PKB inserts 5 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 5; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(5), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());

		// Test Entities:
		std::vector<Index> values{ 1, 2, 3, 4, 5 };
		auto actualValues = tableRef.at("s1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeGetAllInstruction_getAllStmtsStress_evaluatedTableFormed) {
		// 1. Setup:
		// The 'Select pn1' portion of the query
		ParsedGetAll getAllSynonym = ParsedGetAll(EntityType::STMT, "stress1");
		Instruction* instruction = getAllSynonym.toInstruction(pkbGetter);

		// PKB inserts 100 statements of all kinds
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 10; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(100), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("stress1") != tableRef.end());

		// Test Entities:
		std::vector<Index> stress1values;
		for (int i = 0; i < 100; i++) {
			stress1values.emplace_back(i + 1);
		}
		auto actualValues = tableRef.at("stress1");
		// Cannot check equality due to unordering (actuals1Values is not {1, 2, ..., 100}), this is PKB's optimisation.
		// Instead sort, and check for equality
		std::sort(actualValues.begin(), actualValues.end());
		bool areVecEqual = std::equal(stress1values.begin(), stress1values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeGetAllInstruction_getAllPrintsStress_evaluatedTableFormed) {
		// 1. Setup:
		// The 'Select pn1' portion of the query
		ParsedGetAll getAllSynonym = ParsedGetAll(EntityType::PRINT, "pn1");
		Instruction* instruction = getAllSynonym.toInstruction(pkbGetter);

		// PKB inserts 99 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 49; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		}
		for (int i = 0; i < 50; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE)); // extra
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(49), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("pn1") != tableRef.end());

		// Test Entities:
		std::vector<Index> pn1values;
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

	TEST_METHOD(executeGetAllInstruction_getAllCallsStress_evaluatedTableFormed) {
		// 1. Setup:
		// The 'Select pn1' portion of the query
		ParsedGetAll getAllSynonym = ParsedGetAll(EntityType::CALL, "cl1");
		Instruction* instruction = getAllSynonym.toInstruction(pkbGetter);

		// PKB inserts 99 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 49; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		}
		for (int i = 0; i < 50; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE)); // extra
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(49), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("cl1") != tableRef.end());

		// Test Entities:
		std::vector<Index> cl1values;
		for (int i = 0; i < 49; i++) {
			cl1values.emplace_back(i + 1);
		}
		auto actualValues = tableRef.at("cl1");
		// Cannot check equality due to unordering (actuals1Values is not {1, 2, ..., 49}), this is PKB's optimisation.
		// Instead sort, and check for equality
		std::sort(actualValues.begin(), actualValues.end());
		bool areVecEqual = std::equal(cl1values.begin(), cl1values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeGetAllInstruction_getAllReadsStress_evaluatedTableFormed) {
		// 1. Setup:
		// The 'Select pn1' portion of the query
		ParsedGetAll getAllSynonym = ParsedGetAll(EntityType::READ, "r1");
		Instruction* instruction = getAllSynonym.toInstruction(pkbGetter);

		// PKB inserts 99 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 49; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));
		}
		for (int i = 0; i < 50; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE)); // extra
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(49), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("r1") != tableRef.end());

		// Test Entities:
		std::vector<Index> r1values;
		for (int i = 0; i < 49; i++) {
			r1values.emplace_back(i + 1);
		}
		auto actualValues = tableRef.at("r1");
		// Cannot check equality due to unordering (actuals1Values is not {1, 2, ..., 49}), this is PKB's optimisation.
		// Instead sort, and check for equality
		std::sort(actualValues.begin(), actualValues.end());
		bool areVecEqual = std::equal(r1values.begin(), r1values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeGetAllInstruction_getAllIfsStress_evaluatedTableFormed) {
		// 1. Setup:
		// The 'Select pn1' portion of the query
		ParsedGetAll getAllSynonym = ParsedGetAll(EntityType::IF, "if1");
		Instruction* instruction = getAllSynonym.toInstruction(pkbGetter);

		// PKB inserts 99 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 49; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		}
		for (int i = 0; i < 50; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE)); // extra
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(49), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("if1") != tableRef.end());

		// Test Entities:
		std::vector<Index> if1values;
		for (int i = 0; i < 49; i++) {
			if1values.emplace_back(i + 1);
		}
		auto actualValues = tableRef.at("if1");
		// Cannot check equality due to unordering (actuals1Values is not {1, 2, ..., 49}), this is PKB's optimisation.
		// Instead sort, and check for equality
		std::sort(actualValues.begin(), actualValues.end());
		bool areVecEqual = std::equal(if1values.begin(), if1values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeGetAllInstruction_getAllWhilesStress_evaluatedTableFormed) {
		// 1. Setup:
		// The 'Select pn1' portion of the query
		ParsedGetAll getAllSynonym = ParsedGetAll(EntityType::WHILE, "w1");
		Instruction* instruction = getAllSynonym.toInstruction(pkbGetter);

		// PKB inserts 99 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 49; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		}
		for (int i = 0; i < 50; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE)); // extra
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(49), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("w1") != tableRef.end());

		// Test Entities:
		std::vector<Index> w1values;
		for (int i = 0; i < 49; i++) {
			w1values.emplace_back(i + 1);
		}
		auto actualValues = tableRef.at("w1");
		// Cannot check equality due to unordering (actuals1Values is not {1, 2, ..., 49}), this is PKB's optimisation.
		// Instead sort, and check for equality
		std::sort(actualValues.begin(), actualValues.end());
		bool areVecEqual = std::equal(w1values.begin(), w1values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeGetAllInstruction_getAllAssignsStress_evaluatedTableFormed) {
		// 1. Setup:
		// The 'Select pn1' portion of the query
		ParsedGetAll getAllSynonym = ParsedGetAll(EntityType::ASSIGN, "a1");
		Instruction* instruction = getAllSynonym.toInstruction(pkbGetter);

		// PKB inserts 99 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 49; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}
		for (int i = 0; i < 50; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE)); // extra
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(49), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a1") != tableRef.end());

		// Test Entities:
		std::vector<Index> a1values;
		for (int i = 0; i < 49; i++) {
			a1values.emplace_back(i + 1);
		}
		auto actualValues = tableRef.at("a1");
		// Cannot check equality due to unordering (actuals1Values is not {1, 2, ..., 49}), this is PKB's optimisation.
		// Instead sort, and check for equality
		std::sort(actualValues.begin(), actualValues.end());
		bool areVecEqual = std::equal(a1values.begin(), a1values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}
	TEST_METHOD(executeGetAllInstruction_getAllVarStress_evaluatedTableFormed) {
		// 1. Setup:
		// The 'Select pn1' portion of the query
		ParsedGetAll getAllSynonym = ParsedGetAll(EntityType::VARIABLE, "v1");
		Instruction* instruction = getAllSynonym.toInstruction(pkbGetter);

		// PKB inserts 99 statements
		std::vector<VarIndex> vars;
		std::vector<std::string> varNames;
		for (int i = 0; i < 99; i++) {
			std::string varName = "var" + std::to_string(i);
			VarIndex var = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, varName);
			vars.emplace_back(var);
			varNames.emplace_back(varName);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(99), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("v1") != tableRef.end());

		// Test Entities:
		std::vector<Index> v1values;
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

	TEST_METHOD(executeGetAllInstruction_getAllProcStress_evaluatedTableFormed) {
		// 1. Setup:
		// The 'Select pn1' portion of the query
		ParsedGetAll getAllSynonym = ParsedGetAll(EntityType::PROCEDURE, "p1");
		Instruction* instruction = getAllSynonym.toInstruction(pkbGetter);

		// PKB inserts 99 statements
		std::vector<VarIndex> procs;
		std::vector<std::string> procNames;
		for (int i = 0; i < 99; i++) {
			std::string procName = "proc" + std::to_string(i);
			ProcIndex proc = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, procName);
			procs.emplace_back(proc);
			procNames.emplace_back(procName);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(99), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p1") != tableRef.end());

		// Test Entities:
		std::vector<Index> p1values;
		for (int i = 0; i < 99; i++) {
			p1values.emplace_back(i + 1);
		}
		auto actualValues = tableRef.at("p1");
		// Cannot check equality due to unordering (actuals1Values is not {1, 2, ..., 99}), this is PKB's optimisation.
		// Instead sort, and check for equality
		std::sort(actualValues.begin(), actualValues.end());
		bool areVecEqual = std::equal(p1values.begin(), p1values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeGetAllInstruction_getAllConstStress_evaluatedTableFormed) {
		// 1. Setup:
		// The 'Select pn1' portion of the query
		ParsedGetAll getAllSynonym = ParsedGetAll(EntityType::CONSTANT, "c1");
		Instruction* instruction = getAllSynonym.toInstruction(pkbGetter);

		// PKB inserts 99 statements
		for (int i = 0; i < 99; i++) {
			pkbInserter->insertConst(i + 1);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(99), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("c1") != tableRef.end());

		// Test Entities:
		std::vector<Index> c1values;
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
