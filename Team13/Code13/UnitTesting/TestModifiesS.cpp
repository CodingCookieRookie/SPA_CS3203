#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "../source/PKB/ModifiesS.h"
#include "../source/PKB/UsesS.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestModifiesS) {
private:
	VarIndex varIndex1 = { 1 };
	VarIndex varIndex2 = { 2 };

	StmtIndex stmtIndex1 = { 3 };
	StmtIndex stmtIndex2 = { 4 };

	TEST_METHOD_CLEANUP(cleanUpModifiesS) {
		ModifiesS::performCleanUp();
	}

public:
	TEST_METHOD(insert_getFromVariable) {
		std::vector<int> expectedAns{ stmtIndex1.index };

		ModifiesS::insert(stmtIndex1, varIndex1);
		auto procedures = ModifiesS::getFromVariable(varIndex1);
		Assert::IsTrue(expectedAns == procedures);

		/* Check if other relationship gets affected */
		auto procedures2 = UsesS::getFromVariable(varIndex1);
		Assert::IsTrue(0 == procedures2.size());
		UsesS::performCleanUp();
	};

	TEST_METHOD(contains) {
		ModifiesS::insert(stmtIndex1, varIndex1);
		Assert::AreEqual(true, ModifiesS::contains(stmtIndex1, varIndex1));
		Assert::AreEqual(false, ModifiesS::contains(stmtIndex1, varIndex2));
		Assert::AreEqual(false, ModifiesS::contains(stmtIndex2, varIndex1));
		Assert::AreEqual(false, ModifiesS::contains(stmtIndex2, varIndex2));
	};

	TEST_METHOD(insert_getVariables) {
		std::vector<int> expectedAns{ varIndex1.index, varIndex2.index };

		ModifiesS::insert(stmtIndex1, varIndex1);
		ModifiesS::insert(stmtIndex1, varIndex2);
		auto variables = ModifiesS::getVariables(stmtIndex1);
		Assert::IsTrue(expectedAns == variables);
	};

	TEST_METHOD(getAllSynonymVarInfo) {
		std::vector<int> procedures{ stmtIndex1.index, stmtIndex1.index, stmtIndex2.index, stmtIndex2.index };
		std::vector<int> variables{ varIndex1.index, varIndex2.index, varIndex1.index, varIndex2.index };
		std::tuple<std::vector<int>, std::vector<int>> expectedAns = std::make_tuple(procedures, variables);

		ModifiesS::insert(stmtIndex1, varIndex1);
		ModifiesS::insert(stmtIndex1, varIndex2);
		ModifiesS::insert(stmtIndex2, varIndex1);
		ModifiesS::insert(stmtIndex2, varIndex2);

		auto procVarInfo = ModifiesS::getAllSynonymVarInfo();
		Assert::IsTrue(expectedAns == procVarInfo);
	};

	TEST_METHOD(populateFromSubSynonyms) {
		std::vector<int> expectedAns{ varIndex1.index, varIndex2.index };

		std::unordered_set<StmtIndex> subStmts{ stmtIndex1 };
		ModifiesS::insert(stmtIndex1, varIndex1);
		ModifiesS::insert(stmtIndex1, varIndex2);
		ModifiesS::populateFromSubSynonyms(stmtIndex2, subStmts);

		auto variables = ModifiesS::getVariables(stmtIndex2);
		Assert::IsTrue(expectedAns == variables);
	};
	};
}
