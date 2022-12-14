#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS/Evaluator/PQLEvaluator.h"
#include "../source/QPS/Parser/PQLParser.h"
#include "../source/PKB/PKBInserter.h"
#include "../source/PKB/PKBGetter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	// Modifies (p/p1, v) or Modifies(p/p1, "x") or Modifies (p/p1, _ )	proc
	// Modifies ("p", v) or Modifies(, "x") or Modifies (p/p1, _ )	proc
	TEST_CLASS(TestModifiesPInstruction) {
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
	TEST_METHOD(execute_lhsSynonymRhsSynonymProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "p");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		Instruction* instruction = new ModifiesPInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "p", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "randomProc");
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "p");
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		//ModifiesP::insert(procIndex, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_P, procIndex, varIndex);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: p Values: 2 \nSynonym: v Values: 2 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsSynonymRhsIdentProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "p");
		rhsRef = std::make_pair(PqlReferenceType::IDENT, "x");
		Instruction* instruction = new ModifiesPInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "p" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "randomProc");
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "p");
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_P, procIndex, varIndex);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: p Values: 2 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsSynonymRhsWildCardProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "p");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		Instruction* instruction = new ModifiesPInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "p" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "randomProc");
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "p");
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_P, procIndex, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_P, procIndex, varIndex2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(2), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: p Values: 2 2 \n";
		Assert::AreEqual(expected, evTable.toString());
	}
	TEST_METHOD(execute_lhsIdentRhsSynonymProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::IDENT, "p");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		Instruction* instruction = new ModifiesPInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "randomProc");
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "p");
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_P, procIndex, varIndex);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: v Values: 2 \n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsIdentRhsIdentProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::IDENT, "p");
		rhsRef = std::make_pair(PqlReferenceType::IDENT, "x");
		Instruction* instruction = new ModifiesPInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "randomProc");
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "p");
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_P, procIndex, varIndex);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		Assert::AreEqual(true, evTable.getEvResult());
		std::string expected = "Table String: size: 0\n";
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsIdentRhsWildCardProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::IDENT, "p");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		Instruction* instruction = new ModifiesPInstruction(lhsRef, rhsRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "randomProc");
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "p");
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_P, procIndex, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_P, procIndex, varIndex2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		Assert::AreEqual(true, evTable.getEvResult());
		std::string expected = "Table String: size: 0\n";
		Assert::AreEqual(expected, evTable.toString());
	}
	};
}
