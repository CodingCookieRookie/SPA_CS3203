#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/common/Types.h"
#include "../source/PKB/Uses.h"
#include "../source/PKB/Modifies.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestUses) {
private:
	VarIndex varIndex1 = { 1 };
	VarIndex varIndex2 = { 2 };

	StmtIndex  stmtIndex1 = { 3 };
	StmtIndex  stmtIndex2 = { 4 };

	ProcIndex procIndex1 = { 5 };
	ProcIndex procIndex2 = { 6 };

	TEST_METHOD_CLEANUP(cleanUpEntities) {
		Uses::performCleanUp();
	}

public:
	TEST_METHOD(insert_getStatements_stmtIndex) {
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> expectedAns;
		expectedAns.insert(stmtIndex1);

		Uses::insert(stmtIndex1, varIndex1);
		auto statements = Uses::getStatements(varIndex1);
		Assert::IsTrue(expectedAns == statements);

		// Check if other relationship gets affected
		auto statements2 = Modifies::getStatements(varIndex1);
		Assert::IsTrue(0 == statements2.size());
		Modifies::performCleanUp();
	};

	TEST_METHOD(insert_getProcedures_procIndex) {
		std::unordered_set<ProcIndex, ProcIndex::HashFunction> expectedAns;
		expectedAns.insert(procIndex1);

		Uses::insert(procIndex1, varIndex1);
		auto procedures = Uses::getProcedures(varIndex1);
		Assert::IsTrue(expectedAns == procedures);

		// Check if other relationship gets affected
		auto procedures2 = Modifies::getProcedures(varIndex1);
		Assert::IsTrue(0 == procedures2.size());
		Modifies::performCleanUp();
	};

	TEST_METHOD(contains_stmtIndex) {
		Uses::insert(stmtIndex1, varIndex1);
		Assert::AreEqual(true, Uses::contains(stmtIndex1, varIndex1));
		Assert::AreEqual(false, Uses::contains(stmtIndex1, varIndex2));
		Assert::AreEqual(false, Uses::contains(stmtIndex2, varIndex1));
		Assert::AreEqual(false, Uses::contains(stmtIndex2, varIndex2));
	};

	TEST_METHOD(contains_procIndex) {
		Uses::insert(procIndex1, varIndex1);
		Assert::AreEqual(true, Uses::contains(procIndex1, varIndex1));
		Assert::AreEqual(false, Uses::contains(procIndex1, varIndex2));
		Assert::AreEqual(false, Uses::contains(procIndex2, varIndex1));
		Assert::AreEqual(false, Uses::contains(procIndex2, varIndex2));
	};

	TEST_METHOD(insert_getVariables_procIndex) {
		std::unordered_set<VarIndex, VarIndex::HashFunction> expectedAns;
		expectedAns.insert(varIndex1);
		expectedAns.insert(varIndex2);

		Uses::insert(procIndex1, varIndex1);
		Uses::insert(procIndex1, varIndex2);
		auto variables = Uses::getVariables(procIndex1);
		Assert::IsTrue(expectedAns == variables);
	};

	TEST_METHOD(insert_getVariables_stmtIndex) {
		std::unordered_set<VarIndex, VarIndex::HashFunction> expectedAns;
		expectedAns.insert(varIndex1);
		expectedAns.insert(varIndex2);

		Uses::insert(stmtIndex1, varIndex1);
		Uses::insert(stmtIndex1, varIndex2);
		auto variables = Uses::getVariables(stmtIndex1);
		Assert::IsTrue(expectedAns == variables);
	};

	TEST_METHOD(getAllProcVarInfo) {
		std::vector<std::tuple<ProcIndex, VarIndex>> expectedAns;
		expectedAns.push_back(std::make_tuple(procIndex1, varIndex1));
		expectedAns.push_back(std::make_tuple(procIndex1, varIndex2));
		expectedAns.push_back(std::make_tuple(procIndex2, varIndex1));
		expectedAns.push_back(std::make_tuple(procIndex2, varIndex2));

		Uses::insert(procIndex1, varIndex1);
		Uses::insert(procIndex1, varIndex2);
		Uses::insert(procIndex2, varIndex1);
		Uses::insert(procIndex2, varIndex2);

		auto procVarInfo = Uses::getAllProcVarInfo();
		Assert::IsTrue(expectedAns == procVarInfo);
	};

	TEST_METHOD(getAllStmtVarInfo) {
		std::vector<std::tuple<StmtIndex, VarIndex>> expectedAns;
		expectedAns.push_back(std::make_tuple(stmtIndex1, varIndex1));
		expectedAns.push_back(std::make_tuple(stmtIndex1, varIndex2));
		expectedAns.push_back(std::make_tuple(stmtIndex2, varIndex1));
		expectedAns.push_back(std::make_tuple(stmtIndex2, varIndex2));

		Uses::insert(stmtIndex1, varIndex1);
		Uses::insert(stmtIndex1, varIndex2);
		Uses::insert(stmtIndex2, varIndex1);
		Uses::insert(stmtIndex2, varIndex2);

		auto stmtVarInfo = Uses::getAllStmtVarInfo();
		Assert::IsTrue(expectedAns == stmtVarInfo);
	};
	};
}
