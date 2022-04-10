#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS/Evaluator/PQLEvaluator.h"
#include "../source/QPS/Parser/PQLParser.h"
#include "../source/PKB/PKBInserter.h"
#include "../source/PKB/PKBGetter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	// Modifies (a/r/s/a1, v) or Modifies(a/r/s/a1, "x") or Modifies (a/r/s/a1, _ )
	// Modifies (1, v)	=> true or Modifies (1, _ ) (under statement)
	// Modifies (p/p1, v) or Modifies(p/p1, "x") or Modifies (p/p1, _ )	proc
	TEST_CLASS(TestModifiesInstruction) {
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
	TEST_METHOD(execute_lhsSynonymRhsSynonymStmt) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "v");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 \nSynonym: v Values: 2 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsSynonymRhsIdentStmt) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		rhsRef = std::make_pair(PqlReferenceType::IDENT, "x");
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsSynonymRhsWildCardStmt) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(2), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 2 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsConstRhsSynonym_EvTableTrue) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::INTEGER, "2");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(2), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 3 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsConstRhsSynonym_EvTableFalse) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::INTEGER, "2");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsConstRhsSynonym_ConstOutOfBoundsEvTableFalse) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::INTEGER, "50");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "a1" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(false, evTable.getEvResult());
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 0\n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsConstRhsSynonym_VarOutOfBoundsEvTableFalse) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::INTEGER, "2");
		rhsRef = std::make_pair(PqlReferenceType::IDENT, "fhg");
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(false, evTable.getEvResult());
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 0\n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsConstRhsWildcard_EvTableTrue) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::INTEGER, "2");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(true, evTable.getEvResult());
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 0\n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsConstRhsWildcard_EvTableFalse) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::INTEGER, "2");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(false, evTable.getEvResult());
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 0\n";
		Assert::AreEqual(expected, evTable.toString());
	}
	};
}
