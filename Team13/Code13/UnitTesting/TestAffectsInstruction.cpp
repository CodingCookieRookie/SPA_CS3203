#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS/PQLEvaluator.h"
#include "../source/QPS/PQLParser.h"
#include "../source/QPS/AffectsInstruction.h"
#include "../source/QPS/CacheStorage.h"
#include "../source/PKB/PKBInserter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	// Affects (a1, a2) or Affects(a1, 1) or Affects (a1, _ )
	// Affects (_, a2) or Affects(_, 1) or Affects (_, _ )
	TEST_CLASS(TestAffectsInstruction) {
private:
	PKB pkb;
	PKBGetter* pkbGetter;
	PKBInserter* pkbInserter;
	CacheStorage* cacheStorage;
	AffectsProcessor* affectsProcessor;

	TEST_METHOD_INITIALIZE(init) {
		cacheStorage = new CacheStorage();
		affectsProcessor = new AffectsProcessor(cacheStorage);
		pkb = PKB();
		pkbGetter = new PKBGetter(pkb);
		pkbInserter = new PKBInserter(pkb);
	}
public:
	TEST_METHOD(execute_lhsSynonymRhsSynonym1) {
		/*	x = 1;
			y = x;
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a2");
		Instruction* instruction = new AffectsInstruction(lhsRef, rhsRef, affectsProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "a2" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 \nSynonym: a2 Values: 3 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsSynonymRhsSynonym2) {
		/*	x = 1;
			y = x;
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		Instruction* instruction = new AffectsInstruction(lhsRef, rhsRef, affectsProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsSynonymRhsSynonym3) {
		/*
			while (true) {
				x = x + 1;
			}
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		Instruction* instruction = new AffectsInstruction(lhsRef, rhsRef, affectsProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt2, stmt);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::IsTrue(evTable.getEvResult());
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 3 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsSynonymRhsInteger) {
		/*	x = 1;
			y = x;
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		rhsRef = std::make_pair(PqlReferenceType::INTEGER, "3");
		Instruction* instruction = new AffectsInstruction(lhsRef, rhsRef, affectsProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsSynonymRhsWildCard) {
		/*	x = 1;
			y = x;
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		Instruction* instruction = new AffectsInstruction(lhsRef, rhsRef, affectsProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsWildCardRhsSynonym) {
		/*	x = 1;
			y = x;
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		Instruction* instruction = new AffectsInstruction(lhsRef, rhsRef, affectsProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 3 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsWildCardRhsInteger) {
		/*	x = 1;
			y = x;
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		rhsRef = std::make_pair(PqlReferenceType::INTEGER, "3");
		Instruction* instruction = new AffectsInstruction(lhsRef, rhsRef, affectsProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::IsTrue(evTable.getEvResult());
		std::string expected = "Table String: size: 0\n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsWildCardRhsWildCard) {
		/*	x = 1;
			y = x;
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		Instruction* instruction = new AffectsInstruction(lhsRef, rhsRef, affectsProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(true, evTable.getEvResult());
		std::string expected = "Table String: size: 0\n";
		Assert::AreEqual(expected, evTable.toString());
	}
	};
}
