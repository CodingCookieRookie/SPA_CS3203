#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/common/Types.h"
#include "../source/PKB/Uses.h"
#include "../source/PKB/Modifies.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestModifies) {
private:
	VarIndex varIndex1 = { 1 };
	VarIndex varIndex2 = { 2 };

	StmtIndex  stmtIndex1 = { 3 };
	StmtIndex  stmtIndex2 = { 4 };

	ProcIndex procIndex1 = { 5 };
	ProcIndex procIndex2 = { 6 };

	TEST_METHOD_CLEANUP(cleanUpModifies) {
		Modifies::performCleanUp();
	}

public:
	TEST_METHOD(insert_getStatements_stmtIndex) {
		std::vector<int> expectedAns{ stmtIndex1.index };

		Modifies::insert(stmtIndex1, varIndex1);
		auto statements = Modifies::getStatements(varIndex1);
		Assert::IsTrue(expectedAns == statements);

		/* Check if other relationship gets affected */
		auto statements2 = Uses::getStatements(varIndex1);
		Assert::IsTrue(0 == statements2.size());
		Uses::performCleanUp();
	};

	TEST_METHOD(insert_getProcedures_procIndex) {
		std::vector<int> expectedAns{ procIndex1.index };

		Modifies::insert(procIndex1, varIndex1);
		auto procedures = Modifies::getProcedures(varIndex1);
		Assert::IsTrue(expectedAns == procedures);

		/* Check if other relationship gets affected */
		auto procedures2 = Uses::getProcedures(varIndex1);
		Assert::IsTrue(0 == procedures2.size());
		Uses::performCleanUp();
	};

	TEST_METHOD(contains_stmtIndex) {
		Modifies::insert(stmtIndex1, varIndex1);
		Assert::AreEqual(true, Modifies::contains(stmtIndex1, varIndex1));
		Assert::AreEqual(false, Modifies::contains(stmtIndex1, varIndex2));
		Assert::AreEqual(false, Modifies::contains(stmtIndex2, varIndex1));
		Assert::AreEqual(false, Modifies::contains(stmtIndex2, varIndex2));
	};

	TEST_METHOD(contains_procIndex) {
		Modifies::insert(procIndex1, varIndex1);
		Assert::AreEqual(true, Modifies::contains(procIndex1, varIndex1));
		Assert::AreEqual(false, Modifies::contains(procIndex1, varIndex2));
		Assert::AreEqual(false, Modifies::contains(procIndex2, varIndex1));
		Assert::AreEqual(false, Modifies::contains(procIndex2, varIndex2));
	};

	TEST_METHOD(insert_getVariables_procIndex) {
		std::vector<int> expectedAns{ varIndex1.index, varIndex2.index };

		Modifies::insert(procIndex1, varIndex1);
		Modifies::insert(procIndex1, varIndex2);
		auto variables = Modifies::getVariables(procIndex1);
		Assert::IsTrue(expectedAns == variables);
	};

	TEST_METHOD(insert_getVariables_stmtIndex) {
		std::vector<int> expectedAns{ varIndex1.index, varIndex2.index };

		Modifies::insert(stmtIndex1, varIndex1);
		Modifies::insert(stmtIndex1, varIndex2);
		auto variables = Modifies::getVariables(stmtIndex1);
		Assert::IsTrue(expectedAns == variables);
	};

	TEST_METHOD(getAllProcVarInfo) {
		std::vector<int> procedures{ procIndex1.index, procIndex1.index, procIndex2.index, procIndex2.index };
		std::vector<int> variables{ varIndex1.index, varIndex2.index, varIndex1.index, varIndex2.index };
		std::tuple<std::vector<int>, std::vector<int>> expectedAns = std::make_tuple(procedures, variables);

		Modifies::insert(procIndex1, varIndex1);
		Modifies::insert(procIndex1, varIndex2);
		Modifies::insert(procIndex2, varIndex1);
		Modifies::insert(procIndex2, varIndex2);

		auto procVarInfo = Modifies::getAllProcVarInfo();
		Assert::IsTrue(expectedAns == procVarInfo);
	};

	TEST_METHOD(getAllStmtVarInfo) {
		std::vector<int> statements{ stmtIndex1.index, stmtIndex1.index, stmtIndex2.index, stmtIndex2.index };
		std::vector<int> variables{ varIndex1.index, varIndex2.index, varIndex1.index, varIndex2.index };
		std::tuple<std::vector<int>, std::vector<int>> expectedAns = std::make_tuple(statements, variables);

		Modifies::insert(stmtIndex1, varIndex1);
		Modifies::insert(stmtIndex1, varIndex2);
		Modifies::insert(stmtIndex2, varIndex1);
		Modifies::insert(stmtIndex2, varIndex2);

		auto stmtVarInfo = Modifies::getAllStmtVarInfo();
		Assert::IsTrue(expectedAns == stmtVarInfo);
	};

	TEST_METHOD(populateForContainers) {
		std::vector<int> expectedAns{ varIndex1.index, varIndex2.index };

		std::unordered_set<StmtIndex, StmtIndex::HashFunction> subStmts{ stmtIndex1 };
		Modifies::insert(stmtIndex1, varIndex1);
		Modifies::insert(stmtIndex1, varIndex2);
		Modifies::populateForContainers(stmtIndex2, subStmts);

		auto variables = Modifies::getVariables(stmtIndex2);
		Assert::IsTrue(expectedAns == variables);
	};
	};
}