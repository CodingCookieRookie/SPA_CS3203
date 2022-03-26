#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS/PQLEvaluator.h"
#include "../source/QPS/PQLParser.h"
#include "../source/PKB/UsesP.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	// Uses (p/p1, v) or Uses(p/p1, "x") or Uses (p/p1, _ )	proc
	// Uses ("p", v) or Uses(, "x") or Uses (p/p1, _ )	proc
	TEST_CLASS(TestUsesPInstruction) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Attribute::performCleanUp();
		Entity::performCleanUp();
		UsesP::performCleanUp();
	}
public:
	TEST_METHOD(execute_lhsSynonymRhsSynonymProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::Synonym, "p");
		rhsRef = std::make_pair(PqlReferenceType::Synonym, "v");
		Instruction* instruction = new UsesPInstruction(lhsRef, rhsRef);

		// PKB inserts uses
		Entity::insertProc("randomProc");
		ProcIndex procIndex = Entity::insertProc("p");
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		UsesP::insert(procIndex, varIndex);
		Assert::AreEqual(UsesP::contains(procIndex, varIndex), true);
		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: p Values: 2 \nSynonym: v Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsSynonymRhsIdentProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::Synonym, "p");
		rhsRef = std::make_pair(PqlReferenceType::Ident, "x");
		Instruction* instruction = new UsesPInstruction(lhsRef, rhsRef);

		// PKB inserts uses
		Entity::insertProc("randomProc");
		ProcIndex procIndex = Entity::insertProc("p");
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		UsesP::insert(procIndex, varIndex);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: p Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsSynonymRhsWildCardProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::Synonym, "p");
		rhsRef = std::make_pair(PqlReferenceType::Wildcard, "_");
		Instruction* instruction = new UsesPInstruction(lhsRef, rhsRef);

		// PKB inserts uses
		Entity::insertProc("randomProc");
		ProcIndex procIndex = Entity::insertProc("p");
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		UsesP::insert(procIndex, varIndex);
		UsesP::insert(procIndex, varIndex2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(2), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: p Values: 2 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}
	TEST_METHOD(execute_lhsIdentRhsSynonymProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::Ident, "p");
		rhsRef = std::make_pair(PqlReferenceType::Synonym, "v");
		Instruction* instruction = new UsesPInstruction(lhsRef, rhsRef);

		// PKB inserts uses
		Entity::insertProc("randomProc");
		ProcIndex procIndex = Entity::insertProc("p");
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		UsesP::insert(procIndex, varIndex);
		Assert::AreEqual(UsesP::contains(procIndex, varIndex), true);
		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: v Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsIdentRhsIdentProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::Ident, "p");
		rhsRef = std::make_pair(PqlReferenceType::Ident, "x");
		Instruction* instruction = new UsesPInstruction(lhsRef, rhsRef);

		// PKB inserts uses
		Entity::insertProc("randomProc");
		ProcIndex procIndex = Entity::insertProc("p");
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		UsesP::insert(procIndex, varIndex);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 0\n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsIdentRhsWildCardProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::Ident, "p");
		rhsRef = std::make_pair(PqlReferenceType::Wildcard, "_");
		Instruction* instruction = new UsesPInstruction(lhsRef, rhsRef);

		// PKB inserts uses
		Entity::insertProc("randomProc");
		ProcIndex procIndex = Entity::insertProc("p");
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		UsesP::insert(procIndex, varIndex);
		UsesP::insert(procIndex, varIndex2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 0\n";
		Assert::AreEqual(expected, evTable.getTableString());
	}
	};
}
