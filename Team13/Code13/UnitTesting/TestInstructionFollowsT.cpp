#include "stdafx.h"
#include "CppUnitTest.h"

#include <algorithm>
#include <string>

#include "../source/PKB/Follows.h"
#include "../source/PKB/FollowsT.h"
#include "../source/PKB/RS2.h"
#include "../source/QPS/PQLEvaluator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestFollowsTInstructions) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Entity::performCleanUp();
		Follows::performCleanUp();
		FollowsT::performCleanUp();
	}
public:

	TEST_METHOD(executeFollowsStarInstruction_twoConstants_evaluatedTableTrue) {
		// 1. Setup:
		// Follows*(1, 2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::integer, "1");
		rhsRef = std::make_pair(PqlReferenceType::integer, "4");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::FollowsT, lhsRef, rhsRef);

		// PKB inserts 4 statements, 3 Follows
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 4; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		}
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> uPredSucTable;
		uPredSucTable[stmts[0]] = { stmts[1] };
		uPredSucTable[stmts[1]] = { stmts[2], stmts[3] };
		FollowsT::populate(uPredSucTable);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		Assert::AreEqual(size_t(0), evTable.getNumRow());
		Assert::AreEqual(true, evTable.getEvResult()); // Follows*(1, 4) should hold

		// 3. Clean-up:
		Entity::performCleanUp();
		FollowsT::performCleanUp();
		Follows::performCleanUp();
	}

	TEST_METHOD(executeFollowsStarInstruction_lhsConstRhsStmt_evaluatedTableFormed) {
		// 1. Setup:
		// Follows*(2, s2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::integer, "2");
		rhsRef = std::make_pair(PqlReferenceType::synonym, "s2");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::FollowsT, lhsRef, rhsRef);

		// PKB inserts 4 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 4; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		}
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> uPredSucTable;
		uPredSucTable[stmts[0]] = { stmts[1] }; // 1 follows 2 (ignored)
		uPredSucTable[stmts[1]] = { stmts[2], stmts[3] }; // 2 follows 3 and 4
		FollowsT::populate(uPredSucTable);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(2), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s2") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s3") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 3, 4 };
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

		// 3. Clean-up:
		Entity::performCleanUp();
		FollowsT::performCleanUp();
		Follows::performCleanUp();
	}

	TEST_METHOD(executeFollowsStarInstruction_lhsStmtRhsConst_evaluatedTableFormed) {
		// 1. Setup:
		// Follows*(s1, 4) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
		rhsRef = std::make_pair(PqlReferenceType::integer, "4");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::FollowsT, lhsRef, rhsRef);

		// PKB inserts 4 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 4; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		}
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> uPredSucTable;
		uPredSucTable[stmts[0]] = { stmts[1] }; // 1 follows 2
		uPredSucTable[stmts[1]] = { stmts[2], stmts[3] }; // 2 follows 3 and 4
		uPredSucTable[stmts[2]] = { stmts[3] }; //  3 follows 4
		FollowsT::populate(uPredSucTable);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(3), evTable.getNumRow());

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s5") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 1, 2, 3 };
		auto actualValues = tableRef.at("s1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);
		auto actualEntities = evTable.getEntities();
		Assert::AreEqual(true, actualEntities.find("s1") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("s5") != actualEntities.end());
		bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("s1");
		Assert::AreEqual(true, isPqlEntityType);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 3. Clean-up:
		Entity::performCleanUp();
		FollowsT::performCleanUp();
		Follows::performCleanUp();
	}

	TEST_METHOD(executeFollowsStarInstruction_twoStmts_evaluatedTableFormed) {
		// 1. Setup:
		// Follows'*(s1, s2) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
		rhsRef = std::make_pair(PqlReferenceType::synonym, "s2");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::FollowsT, lhsRef, rhsRef);

		// PKB inserts 4 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 4; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		}
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> uPredSucTable;
		uPredSucTable[stmts[0]] = { stmts[1], stmts[2] }; // 1 follows 2 and 3
		uPredSucTable[stmts[1]] = { stmts[2], stmts[3] }; // 2 follows 3 and 4
		uPredSucTable[stmts[2]] = { stmts[3] }; //  3 follows 4
		FollowsT::populate(uPredSucTable);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(6), evTable.getNumRow()); // {1 2, 1 3, 1 4, 2 3, 2 4, 3 4}

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s12") != tableRef.end());

		// Test Entities: std::unordered_map<std::string, PqlEntityType>
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

		auto actualEntities = evTable.getEntities();
		Assert::AreEqual(true, actualEntities.find("s1") != actualEntities.end());
		Assert::AreEqual(true, actualEntities.find("s2") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("s5") != actualEntities.end());
		bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("s1");
		bool isPqlEntityType2 = PqlEntityType::Stmt == actualEntities.at("s2");
		Assert::AreEqual(true, isPqlEntityType);
		Assert::AreEqual(true, isPqlEntityType2);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 3. Clean-up:
		Entity::performCleanUp();
		Follows::performCleanUp();
		FollowsT::performCleanUp();
	}

	TEST_METHOD(executeFollowsStarInstruction_twoStmtsStress_evaluatedTableFormed) {
		// 1. Setup:
		// Follows*(s1, _) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
		rhsRef = std::make_pair(PqlReferenceType::synonym, "s2");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::FollowsT, lhsRef, rhsRef);

		// PKB inserts 19 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 19; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		}
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> uPredSucTable;
		for (int i = 0; i < 18; i++) {
			uPredSucTable[stmts[i]] = { stmts[i + 1] }; // i follows i + 1
		}
		FollowsT::populate(uPredSucTable);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(171), evTable.getNumRow());
		// {1 2, 1 3, ..., 1 19, 2 3, 2 4, ..., 2 19, ... , 18 19}. Hence, (19*18)/2 = 171

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s207") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(2), tableRef.size()); // 2 columns: s1 and s2

		// Test Entities: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> s1values, wildcardValues;
		for (int i = 0; i < 18; i++) {
			for (int j = 0; j < (18 - i); j++) {
				s1values.emplace_back(i + 1);
			}
		}
		for (int i = 0; i < 18; i++) {
			for (int j = i; j < 18; j++) {
				wildcardValues.emplace_back(j + 2);
			}
		}

		auto actuals1Values = tableRef.at("s1");
		std::sort(actuals1Values.begin(), actuals1Values.end());
		std::sort(s1values.begin(), s1values.end());
		bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
		Assert::AreEqual(true, areVecEqual); // s1values == {19 1s, 18 2s, ... 1 19}
		auto actuals2Values = tableRef.at("s2");
		std::sort(actuals2Values.begin(), actuals2Values.end());
		std::sort(wildcardValues.begin(), wildcardValues.end());
		bool areVecEqual2 = std::equal(wildcardValues.begin(), wildcardValues.end(), actuals2Values.begin());
		Assert::AreEqual(true, areVecEqual2); // wildcardValues == {2-19, 3-19, ..., 18, 19, 19}

		auto actualEntities = evTable.getEntities();
		Assert::AreEqual(true, actualEntities.find("s1") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("_") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("s207") != actualEntities.end());
		bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("s2");
		Assert::AreEqual(true, isPqlEntityType);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 3. Clean-up:
		Entity::performCleanUp();
		FollowsT::performCleanUp();
		Follows::performCleanUp();
	}

	TEST_METHOD(executeFollowsStarInstruction_lhsStmtRhsWildcardStress_evaluatedTableFormed) {
		// 1. Setup:
		// Follows*(s1, _) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
		rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::FollowsT, lhsRef, rhsRef);

		// PKB inserts 19 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 19; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		}
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> uPredSucTable;
		for (int i = 0; i < 18; i++) {
			uPredSucTable[stmts[i]] = { stmts[i + 1] }; // i follows i + 1
		}
		FollowsT::populate(uPredSucTable);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(171), evTable.getNumRow());
		// {1 2, 1 3, ..., 1 19, 2 3, 2 4, ..., 2 19, ... , 18 19}. Hence, (19*18)/2 = 171

		// Test Table: std::unordered_map<std::string, std::vector<int>>
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(false, tableRef.find("s207") != tableRef.end());

		// Test Table size:
		Assert::AreEqual(size_t(1), tableRef.size()); // RHS wildcard will have no column (not of concern)

		// Test Entities: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> s1values, wildcardValues;
		for (int i = 0; i < 18; i++) {
			for (int j = 0; j < (18 - i); j++) {
				s1values.emplace_back(i + 1);
			}
		}

		auto actuals1Values = tableRef.at("s1");
		std::sort(actuals1Values.begin(), actuals1Values.end());
		std::sort(s1values.begin(), s1values.end());
		bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
		Assert::AreEqual(true, areVecEqual); // s1values == {19 1s, 18 2s, ... 1 19}

		auto actualEntities = evTable.getEntities();
		Assert::AreEqual(true, actualEntities.find("s1") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("_") != actualEntities.end());
		Assert::AreEqual(false, actualEntities.find("s207") != actualEntities.end());
		bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("s1");
		Assert::AreEqual(true, isPqlEntityType);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 3. Clean-up:
		Entity::performCleanUp();
		FollowsT::performCleanUp();
		Follows::performCleanUp();
	}

	TEST_METHOD(executeFollowsStarInstruction_twoWildcards_evaluatedTableFormed) {
		// 1. Setup:
		// Follows*(_, _) RelationshipInstruction
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
		rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::FollowsT, lhsRef, rhsRef);

		// PKB inserts 3 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 4; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		}
		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex>> uPredSucTable;
		uPredSucTable[stmts[0]] = { stmts[1], stmts[2] }; // 1 follows 2 and 3
		uPredSucTable[stmts[1]] = { stmts[2], stmts[3] }; // 2 follows 3 and 4
		uPredSucTable[stmts[2]] = { stmts[3] }; //  3 follows 4
		FollowsT::populate(uPredSucTable);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();

		// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow()); // Evaluating 2 wildcards will return a boolean with no table.

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
		Assert::AreEqual(true, actualEvResult); // because Follows* rs exist

		// 3. Clean-up:
		Entity::performCleanUp();
		FollowsT::performCleanUp();
		Follows::performCleanUp();
	}
	};
}
