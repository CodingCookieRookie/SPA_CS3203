#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS/PQLEvaluator.h"
#include "../source/QPS/PQLParser.h"
#include "../source/PKB/ModifiesP.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	// Modifies (p/p1, v) or Modifies(p/p1, "x") or Modifies (p/p1, _ )	proc
	// Modifies ("p", v) or Modifies(, "x") or Modifies (p/p1, _ )	proc
	TEST_CLASS(TestModifiesPInstruction) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Attribute::performCleanUp();
		Entity::performCleanUp();
		ModifiesP::performCleanUp();
	}
public:
	TEST_METHOD(execute_lhsSynonymRhsSynonymProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "p");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		Instruction* instruction = new ModifiesPInstruction(lhsRef, rhsRef);

		std::unordered_set<std::string> expectedSynonyms{ "p", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		Entity::insertProc("randomProc");
		ProcIndex procIndex = Entity::insertProc("p");
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		ModifiesP::insert(procIndex, varIndex);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: p Values: 2 \nSynonym: v Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsSynonymRhsIdentProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "p");
		rhsRef = std::make_pair(PqlReferenceType::IDENT, "x");
		Instruction* instruction = new ModifiesPInstruction(lhsRef, rhsRef);

		std::unordered_set<std::string> expectedSynonyms{ "p" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		Entity::insertProc("randomProc");
		ProcIndex procIndex = Entity::insertProc("p");
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		ModifiesP::insert(procIndex, varIndex);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: p Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsSynonymRhsWildCardProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "p");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		Instruction* instruction = new ModifiesPInstruction(lhsRef, rhsRef);

		std::unordered_set<std::string> expectedSynonyms{ "p" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		Entity::insertProc("randomProc");
		ProcIndex procIndex = Entity::insertProc("p");
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		ModifiesP::insert(procIndex, varIndex);
		ModifiesP::insert(procIndex, varIndex2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(2), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: p Values: 2 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}
	TEST_METHOD(execute_lhsIdentRhsSynonymProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::IDENT, "p");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		Instruction* instruction = new ModifiesPInstruction(lhsRef, rhsRef);

		std::unordered_set<std::string> expectedSynonyms{ "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		Entity::insertProc("randomProc");
		ProcIndex procIndex = Entity::insertProc("p");
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		ModifiesP::insert(procIndex, varIndex);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: v Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsIdentRhsIdentProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::IDENT, "p");
		rhsRef = std::make_pair(PqlReferenceType::IDENT, "x");
		Instruction* instruction = new ModifiesPInstruction(lhsRef, rhsRef);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		Entity::insertProc("randomProc");
		ProcIndex procIndex = Entity::insertProc("p");
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		ModifiesP::insert(procIndex, varIndex);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		Assert::AreEqual(true, evTable.getEvResult());
		std::string expected = "Table String: size: 0\n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsIdentRhsWildCardProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::IDENT, "p");
		rhsRef = std::make_pair(PqlReferenceType::WILDCARD, "_");
		Instruction* instruction = new ModifiesPInstruction(lhsRef, rhsRef);

		std::unordered_set<std::string> expectedSynonyms{};
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		Entity::insertProc("randomProc");
		ProcIndex procIndex = Entity::insertProc("p");
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		ModifiesP::insert(procIndex, varIndex);
		ModifiesP::insert(procIndex, varIndex2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		Assert::AreEqual(true, evTable.getEvResult());
		std::string expected = "Table String: size: 0\n";
		Assert::AreEqual(expected, evTable.getTableString());
	}
	};
}
