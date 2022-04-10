#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS/Evaluator/PQLEvaluator.h"
#include "../source/QPS/Parser/PQLParser.h"
#include "../source/QPS/Instructions/RelationshipInstruction/AffectsTInstruction.h"
#include "../source/QPS/Processors/CacheStorage.h"
#include "../source/PKB/PKBInserter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	// Affects (a1, a2) or Affects(a1, 1) or Affects (a1, _ )
	// Affects (_, a2) or Affects(_, 1) or Affects (_, _ )
	TEST_CLASS(TestAffectsTInstruction) {
private:
	PKB pkb;
	PKBInserter* pkbInserter;
	PKBGetter* pkbGetter;
	CacheStorage* cacheStorage;
	CacheStorage* cacheTStorage;
	AffectsProcessor* affectsProcessor;
	AffectsTProcessor* affectsTProcessor;

	TEST_METHOD_INITIALIZE(init) {
		cacheStorage = new CacheStorage();
		cacheTStorage = new CacheStorage();
		affectsProcessor = new AffectsProcessor(cacheStorage);
		affectsTProcessor = new AffectsTProcessor(cacheTStorage, affectsProcessor);
		pkb = PKB();
		pkbInserter = new PKBInserter(pkb);
		pkbGetter = new PKBGetter(pkb);
	}
public:
	TEST_METHOD(execute_lhsSynonymRhsSynonym1) {
		/*	x = 1;
			y = x;
			z = y;
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a2");
		Instruction* instruction = new AffectsTInstruction(lhsRef, rhsRef, affectsTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "a2" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt2, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt3, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt2, stmt3);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(3), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 2 3 \nSynonym: a2 Values: 3 4 4 \n";
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
		Instruction* instruction = new AffectsTInstruction(lhsRef, rhsRef, affectsTProcessor, pkbGetter);

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
		Instruction* instruction = new AffectsTInstruction(lhsRef, rhsRef, affectsTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		StmtIndex stmt1 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt1, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt1, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt1);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt1, stmt);
		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 3 \n";
		Assert::AreEqual(expected, evTable.toString());
	}
	TEST_METHOD(execute_lhsSynonymRhsSynonym4) {
		/*
			while (true) {
				x = x + 1;
				x = x + 1;
			}
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		Instruction* instruction = new AffectsTInstruction(lhsRef, rhsRef, affectsTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt3, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt3, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt2, stmt3);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt3, stmt);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(2), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 3 4 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsSynonymRhsSynonym5) {
		/*
			while (true) {
				x = x + 1;
				x = y + 1;			// 4 affects 3 so 3 does not affect 3
			}
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		Instruction* instruction = new AffectsTInstruction(lhsRef, rhsRef, affectsTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt3, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt3, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt2, stmt3);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt3, stmt);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsSynonymRhsSynonyn6) {
		/*
			while (true) {
				x = x + 1;
				y = x + 1;	// 3 affects 4, 3 affects 3 since 4 does not affect 3
			}
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		Instruction* instruction = new AffectsTInstruction(lhsRef, rhsRef, affectsTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt3, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt3, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt2, stmt3);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt3, stmt);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 3 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsSynonymRhsInteger) {
		/*
		x = 1;
		y = x;
		z = y;
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		rhsRef = std::make_pair(PqlReferenceType::INTEGER, "4");
		Instruction* instruction = new AffectsTInstruction(lhsRef, rhsRef, affectsTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt2, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt3, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt2, stmt3);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(2), evTable.getNumRow());
		std::vector<int> expectedRes = { 2, 3 };
		std::vector<int> vec = evTable.getTableRef()["a1"];
		for (size_t i = 0; i < vec.size(); i++) {
			Assert::IsTrue(std::find(expectedRes.begin(), expectedRes.end(), vec.at(i)) != expectedRes.end());
		}
	}

	TEST_METHOD(execute_lhsSynonymRhsWildCard) {
		/*
		x = 1;
		y = x;
		z = y;
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		Instruction* instruction = new AffectsTInstruction(lhsRef, rhsRef, affectsTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt2, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt3, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt2, stmt3);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(3), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 2 3 \n"; /* 2 affects* 3, 2 affects* 4, 3 affects* 4 */
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsWildCardRhsSynonym) {
		/*
		x = 1;
		y = x;
		z = y;
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		Instruction* instruction = new AffectsTInstruction(lhsRef, rhsRef, affectsTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt2, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt3, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt2, stmt3);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(3), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 3 4 4 \n"; /* 2 affects* 3, 2 affects* 4, 3 affects* 4 */
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsWildCardRhsInteger) {
		/*
		x = 1;
		y = x;
		z = y;
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		rhsRef = std::make_pair(PqlReferenceType::INTEGER, "4");
		Instruction* instruction = new AffectsTInstruction(lhsRef, rhsRef, affectsTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt2, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt3, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt2, stmt3);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		Assert::IsTrue(evTable.getEvResult());
		std::string expected = "Table String: size: 0\n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsWildCardRhsWildCard) {
		/*
		x = 1;
		y = x;
		z = y;
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		Instruction* instruction = new AffectsTInstruction(lhsRef, rhsRef, affectsTProcessor, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt2, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt3, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt2, stmt3);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		Assert::IsTrue(evTable.getEvResult());
		std::string expected = "Table String: size: 0\n"; /* 2 affects* 3, 2 affects* 4, 3 affects* 4 */
		Assert::AreEqual(expected, evTable.toString());
	}
	};
}
