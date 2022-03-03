#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS/PQLEvaluator.h"
#include "../source/QPS/PQLParser.h"
#include "../source/PKB/Modifies.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	// Uses (p/p1, v) or Uses(p/p1, "x") or Uses (p/p1, _ )	proc
	TEST_CLASS(TestUsesPInstruction) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Entity::performCleanUp();
		Uses::performCleanUp();
	}
public:
	TEST_METHOD(execute_lhsSynonymRhsSynonymProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::synonym, "p");
		rhsRef = std::make_pair(PqlReferenceType::synonym, "v");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesP, lhsRef, rhsRef);

		// PKB inserts uses
		Entity::insertProc("randomProc");
		ProcIndex procIndex = Entity::insertProc("p");
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		Uses::insert(procIndex, varIndex);
		Assert::AreEqual(Uses::contains(procIndex, varIndex), true);
		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: p Values: 2 \nSynonym: v Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsSynonymRhsIdentProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::synonym, "p");
		rhsRef = std::make_pair(PqlReferenceType::ident, "x");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesP, lhsRef, rhsRef);

		// PKB inserts uses
		Entity::insertProc("randomProc");
		ProcIndex procIndex = Entity::insertProc("p");
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		Uses::insert(procIndex, varIndex);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: p Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsSynonymRhsWildCardProc) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::synonym, "p");
		rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesP, lhsRef, rhsRef);

		// PKB inserts uses
		Entity::insertProc("randomProc");
		ProcIndex procIndex = Entity::insertProc("p");
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		Uses::insert(procIndex, varIndex);
		Uses::insert(procIndex, varIndex2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(2), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: p Values: 2 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}
	};
}
