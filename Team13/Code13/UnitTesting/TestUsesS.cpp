#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "../source/PKB/ModifiesS.h"
#include "../source/PKB/UsesS.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestUsesS) {
private:
	VarIndex varIndex1 = { 1 };
	VarIndex varIndex2 = { 2 };

	StmtIndex stmtIndex1 = { 3 };
	StmtIndex stmtIndex2 = { 4 };

	TEST_METHOD_CLEANUP(cleanUpUsesS) {
		UsesS::performCleanUp();
	}

public:
	TEST_METHOD(insert_getFromVariable) {
		std::vector<int> expectedAns{ stmtIndex1 };

		UsesS::insert(stmtIndex1, varIndex1);
		auto procedures = UsesS::getFromVariable(varIndex1);
		Assert::IsTrue(expectedAns == procedures);

		/* Check if other relationship gets affected */
		auto procedures2 = ModifiesS::getFromVariable(varIndex1);
		Assert::IsTrue(0 == procedures2.size());
		ModifiesS::performCleanUp();
	};

	TEST_METHOD(contains) {
		UsesS::insert(stmtIndex1, varIndex1);
		Assert::AreEqual(true, UsesS::contains(stmtIndex1, varIndex1));
		Assert::AreEqual(false, UsesS::contains(stmtIndex1, varIndex2));
		Assert::AreEqual(false, UsesS::contains(stmtIndex2, varIndex1));
		Assert::AreEqual(false, UsesS::contains(stmtIndex2, varIndex2));
	};

	TEST_METHOD(insert_getVariables) {
		std::vector<int> expectedAns{ varIndex1, varIndex2 };

		UsesS::insert(stmtIndex1, varIndex1);
		UsesS::insert(stmtIndex1, varIndex2);
		auto variables = UsesS::getVariables(stmtIndex1);
		Assert::IsTrue(expectedAns == variables);
	};

	TEST_METHOD(getAllSynonymVarInfo) {
		std::vector<int> procedures{ stmtIndex1, stmtIndex1, stmtIndex2, stmtIndex2 };
		std::vector<int> variables{ varIndex1, varIndex2, varIndex1, varIndex2 };
		std::tuple<std::vector<int>, std::vector<int>> expectedAns = std::make_tuple(procedures, variables);

		UsesS::insert(stmtIndex1, varIndex1);
		UsesS::insert(stmtIndex1, varIndex2);
		UsesS::insert(stmtIndex2, varIndex1);
		UsesS::insert(stmtIndex2, varIndex2);

		auto procVarInfo = UsesS::getAllSynonymVarInfo();
		Assert::IsTrue(expectedAns == procVarInfo);
	};

	TEST_METHOD(populateFromSubSynonyms) {
		std::vector<int> expectedAns{ varIndex1, varIndex2 };

		std::unordered_set<StmtIndex> subStmts{ stmtIndex1 };
		UsesS::insert(stmtIndex1, varIndex1);
		UsesS::insert(stmtIndex1, varIndex2);
		UsesS::populateFromSubSynonyms(stmtIndex2, subStmts);

		auto variables = UsesS::getVariables(stmtIndex2);
		Assert::IsTrue(expectedAns == variables);
	};
	};
}
