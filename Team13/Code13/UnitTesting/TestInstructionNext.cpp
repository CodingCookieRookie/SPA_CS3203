#include "stdafx.h"
#include "CppUnitTest.h"

#include <algorithm>
#include <string>

#include "../source/PKB/Entity.h"
#include "../source/PKB/Next.h"
#include "../source/PKB/RS2.h"
#include "../source/QPS/PQLEvaluator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestNextInstructions) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Attribute::performCleanUp();
		Entity::performCleanUp();
		Next::performCleanUp();
	}
public:

	TEST_METHOD(executeNextInstruction_twoConstants_evaluatedTableFormed) {
		// 1. Setup:
		// Next (1, 2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::integer, "1");
		rhsRef = std::make_pair(PqlReferenceType::integer, "2");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Next, lhsRef, rhsRef);

		// PKB inserts statements
		StmtIndex stmt1 = Entity::insertStmt(StatementType::assignType);
		StmtIndex stmt2 = Entity::insertStmt(StatementType::assignType);
		Next::insert(stmt1, stmt2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		Assert::AreEqual(size_t(0), evTable.getNumRow());
		Assert::AreEqual(true, evTable.getEvResult());
	}

	TEST_METHOD(executeNextInstruction_lhsConstRhsStmt_evaluatedTableFormed) {
		// 1. Setup:
		// Next(1, s2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::integer, "1");
		rhsRef = std::make_pair(PqlReferenceType::synonym, "s2");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Next, lhsRef, rhsRef);

		// PKB inserts statements
		StmtIndex stmt1 = Entity::insertStmt(StatementType::assignType);
		StmtIndex stmt2 = Entity::insertStmt(StatementType::assignType);
		Next::insert(stmt1, stmt2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s2") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s3") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 2 };
		auto actualValues = tableRef.at("s2");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);
		auto actualEntities = evTable.getEntities();
		Assert::AreEqual(true, actualEntities.find("s2") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("s3") != actualEntities.end());
		bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("s2");
		Assert::AreEqual(true, isPqlEntityType);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeNextInstruction_lhsStmtRhsConst_evaluatedTableFormed) {
		// 1. Setup:
		// Next(s1, 2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
		rhsRef = std::make_pair(PqlReferenceType::integer, "2");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Next, lhsRef, rhsRef);

		// PKB inserts statements
		StmtIndex stmt1 = Entity::insertStmt(StatementType::assignType);
		StmtIndex stmt2 = Entity::insertStmt(StatementType::assignType);
		Next::insert(stmt1, stmt2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s7") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 1 };
		auto actualValues = tableRef.at("s1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);
		auto actualEntities = evTable.getEntities();
		Assert::AreEqual(true, actualEntities.find("s1") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("s7") != actualEntities.end());
		bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("s1");
		Assert::AreEqual(true, isPqlEntityType);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeNextInstruction_twoStmts_evaluatedTableFormed) {
		// 1. Setup:
		// Next(s1, s2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
		rhsRef = std::make_pair(PqlReferenceType::synonym, "s2");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Next, lhsRef, rhsRef);

		// PKB inserts 5 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 5; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		}
		for (int i = 0; i < 4; i++) {
			Next::insert(stmts[i], stmts[i + 1]);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(4), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s6") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> s1values{ 1, 2, 3, 4 };
		auto actuals1Values = tableRef.at("s1");
		bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
		Assert::AreEqual(true, areVecEqual);
		std::vector<int> s2values{ 2, 3, 4, 5 };
		auto actuals2Values = tableRef.at("s2");
		bool areVecEqual2 = std::equal(s2values.begin(), s2values.end(), actuals2Values.begin());
		Assert::AreEqual(true, areVecEqual2);

		auto actualEntities = evTable.getEntities();
		Assert::AreEqual(true, actualEntities.find("s1") != actualEntities.end());
		Assert::AreEqual(true, actualEntities.find("s2") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("s6") != actualEntities.end());
		bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("s1");
		bool isPqlEntityType2 = PqlEntityType::Stmt == actualEntities.at("s2");
		Assert::AreEqual(true, isPqlEntityType);
		Assert::AreEqual(true, isPqlEntityType2);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeNextInstruction_twoStmtsSameSyn_evaluatedTableFormed) {
		// 1. Setup:
		// Next(s1, s1) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
		rhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Next, lhsRef, rhsRef);

		// PKB inserts 5 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 6; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		}
		for (int i = 0; i < 4; i++) {
			Next::insert(stmts[i], stmts[i + 1]);
		}
		Next::insert(stmts[5], stmts[5]); // 5 is a while loop to itself

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s6") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> s1values{ 6 };
		auto actuals1Values = tableRef.at("s1");
		bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
		Assert::AreEqual(true, areVecEqual);

		auto actualEntities = evTable.getEntities();
		Assert::AreEqual(true, actualEntities.find("s1") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("s2") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("s6") != actualEntities.end());
		bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("s1");
		Assert::AreEqual(true, isPqlEntityType);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeNextInstruction_lhsStmtRhsWildcardStress_evaluatedTableFormed) {
		// 1. Setup:
		// Next(s1, _) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
		rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Next, lhsRef, rhsRef);

		// PKB inserts 37213 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 37213; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		}
		for (int i = 0; i < 37213 - 1; i++) {
			Next::insert(stmts[i], stmts[i + 1]);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(37213 - 1), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("stress") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(1), tableRef.size()); // RHS wildcard will still have column (innerJoinMerge() will drop it during merge)

		// Test Entities: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> s1values;
		for (int i = 0; i < 37213 - 1; i++) {
			s1values.emplace_back(i + 1);
		}
		auto actuals1Values = tableRef.at("s1");
		// Cannot check equality due to misordering (actuals1Values is not {1, 2, ... 37212}), this is PKB's optimisation.
		// Instead sort, and check for equality
		std::sort(actuals1Values.begin(), actuals1Values.end());
		bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
		Assert::AreEqual(true, areVecEqual); // s1values == {1, 2, ... 37212}

		auto actualEntities = evTable.getEntities();
		Assert::AreEqual(true, actualEntities.find("s1") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("_") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("stress") != actualEntities.end());
		bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("s1");
		Assert::AreEqual(true, isPqlEntityType);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);
	}

	TEST_METHOD(executeNextInstruction_lhsWildcardRhsConst_evaluatedTableFormed) {
		// 1. Setup:
		// Next(_, 3) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
		rhsRef = std::make_pair(PqlReferenceType::integer, "3");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Next, lhsRef, rhsRef);

		// PKB inserts 3 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 3; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		}
		for (int i = 0; i < 2; i++) {
			Next::insert(stmts[i], stmts[i + 1]);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow()); // Only 1 Next of 3

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(false, tableRef.find("_") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("23") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(0), tableRef.size()); // RHS wildcard will still have column (innerJoinMerge() will drop it during merge)

		// Test Entities: std::unordered_map<std::string, PqlEntityType>
		// No entities to test

		auto actualEntities = evTable.getEntities();
		Assert::AreEqual(false, actualEntities.find("_") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("2") != actualEntities.end());

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult); // there exists some statement whose Next is 3
	}

	TEST_METHOD(executeNextInstruction_lhsWildcardRhsConstStress_evaluatedTableFormed) {
		// 1. Setup:
		// Next(_, 28) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
		rhsRef = std::make_pair(PqlReferenceType::integer, "28");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Next, lhsRef, rhsRef);

		// PKB inserts 976 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 976; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		}
		for (int i = 0; i < 976 - 1; i++) {
			Next::insert(stmts[i], stmts[i + 1]);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(false, tableRef.find("_") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("stress") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(0), tableRef.size()); // RHS wildcard will still have column (innerJoinMerge() will drop it during merge)

		// Test Entities: std::unordered_map<std::string, PqlEntityType>
		// No Entities populated

		auto actualEntities = evTable.getEntities();
		Assert::AreEqual(false, actualEntities.find("_") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("28") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("stress") != actualEntities.end());

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult); // there exists a statment whose Next is 28
	}

	TEST_METHOD(executeNextInstruction_twoWildcards_evaluatedTableFormed) {
		// 1. Setup:
		// Next(_, _) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
		rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Next, lhsRef, rhsRef);

		// PKB inserts 3 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 3; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		}
		for (int i = 0; i < 2; i++) {
			Next::insert(stmts[i], stmts[i + 1]);
		}

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow()); //

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(false, tableRef.find("_") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s1") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(0), tableRef.size()); // Two wildcards will have no columns => only have boolean

		// Test Entities: std::unordered_map<std::string, PqlEntityType>
		auto actualEntities = evTable.getEntities();
		Assert::AreEqual(false, actualEntities.find("_") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("s2") != actualEntities.end());

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult); // because Next rs exist
	}
	};
}
