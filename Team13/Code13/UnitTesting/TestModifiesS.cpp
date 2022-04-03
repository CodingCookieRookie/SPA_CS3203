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
	TEST_METHOD(insert_getFromRightArg) {
		std::vector<StmtIndex> expectedAns{ stmtIndex1 };

		ModifiesS::insert(stmtIndex1, varIndex1);
		auto statements = ModifiesS::getFromRightArg(varIndex1);
		Assert::IsTrue(expectedAns == statements);

		/* Check if other relationship gets affected */
		auto statements2 = UsesS::getFromRightArg(varIndex1);
		Assert::IsTrue(0 == statements2.size());
		UsesS::performCleanUp();
	};

	TEST_METHOD(contains) {
		ModifiesS::insert(stmtIndex1, varIndex1);
		Assert::AreEqual(true, ModifiesS::contains(stmtIndex1, varIndex1));
		Assert::AreEqual(false, ModifiesS::contains(stmtIndex1, varIndex2));
		Assert::AreEqual(false, ModifiesS::contains(stmtIndex2, varIndex1));
		Assert::AreEqual(false, ModifiesS::contains(stmtIndex2, varIndex2));
	};

	TEST_METHOD(insert_getFromLeftArg) {
		std::vector<VarIndex> expectedAns{ varIndex1, varIndex2 };

		ModifiesS::insert(stmtIndex1, varIndex1);
		ModifiesS::insert(stmtIndex1, varIndex2);
		auto variables = ModifiesS::getFromLeftArg(stmtIndex1);
		Assert::IsTrue(expectedAns == variables);
	};

	TEST_METHOD(getAllInfo) {
		std::vector<StmtIndex> statements{ stmtIndex1, stmtIndex1, stmtIndex2, stmtIndex2 };
		std::vector<VarIndex> variables{ varIndex1, varIndex2, varIndex1, varIndex2 };
		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> expectedAns = std::make_tuple(statements, variables);

		ModifiesS::insert(stmtIndex1, varIndex1);
		ModifiesS::insert(stmtIndex1, varIndex2);
		ModifiesS::insert(stmtIndex2, varIndex1);
		ModifiesS::insert(stmtIndex2, varIndex2);

		auto procVarInfo = ModifiesS::getAllInfo();
		Assert::IsTrue(expectedAns == procVarInfo);
	};

	TEST_METHOD(populateFromSubSynonyms) {
		std::vector<VarIndex> expectedAns{ varIndex1, varIndex2 };

		std::unordered_set<StmtIndex> subStmts{ stmtIndex1 };
		ModifiesS::insert(stmtIndex1, varIndex1);
		ModifiesS::insert(stmtIndex1, varIndex2);
		ModifiesS::populateFromSubSynonyms(stmtIndex2, subStmts);

		auto variables = ModifiesS::getFromLeftArg(stmtIndex2);
		Assert::IsTrue(expectedAns == variables);
	};
	};
}
