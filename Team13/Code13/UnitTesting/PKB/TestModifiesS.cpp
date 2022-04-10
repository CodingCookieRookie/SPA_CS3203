#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/Relationship/ModifiesS.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestModifiesS) {
private:
	VarIndex varIndex1 = { 1 };
	VarIndex varIndex2 = { 2 };

	StmtIndex stmtIndex1 = { 3 };
	StmtIndex stmtIndex2 = { 4 };

	ModifiesS* modifiesS;

	TEST_METHOD_INITIALIZE(init) {
		modifiesS = new ModifiesS();
	}

public:
	TEST_METHOD(insert_getFromRightArg) {
		std::vector<StmtIndex> expectedAns{ stmtIndex1 };

		modifiesS->insert(stmtIndex1, varIndex1);
		auto statements = modifiesS->getFromRightArg(varIndex1);
		Assert::IsTrue(expectedAns == statements);
	};

	TEST_METHOD(contains) {
		modifiesS->insert(stmtIndex1, varIndex1);
		Assert::AreEqual(true, modifiesS->contains(stmtIndex1, varIndex1));
		Assert::AreEqual(false, modifiesS->contains(stmtIndex1, varIndex2));
		Assert::AreEqual(false, modifiesS->contains(stmtIndex2, varIndex1));
		Assert::AreEqual(false, modifiesS->contains(stmtIndex2, varIndex2));
	};

	TEST_METHOD(insert_getFromLeftArg) {
		std::vector<VarIndex> expectedAns{ varIndex1, varIndex2 };

		modifiesS->insert(stmtIndex1, varIndex1);
		modifiesS->insert(stmtIndex1, varIndex2);
		auto variables = modifiesS->getFromLeftArg(stmtIndex1);
		Assert::IsTrue(expectedAns == variables);
	};

	TEST_METHOD(getAllInfo) {
		std::vector<StmtIndex> statements{ stmtIndex1, stmtIndex1, stmtIndex2, stmtIndex2 };
		std::vector<VarIndex> variables{ varIndex1, varIndex2, varIndex1, varIndex2 };
		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> expectedAns =
			std::make_tuple(statements, variables);

		modifiesS->insert(stmtIndex1, varIndex1);
		modifiesS->insert(stmtIndex1, varIndex2);
		modifiesS->insert(stmtIndex2, varIndex1);
		modifiesS->insert(stmtIndex2, varIndex2);

		auto procVarInfo = modifiesS->getAllInfo();
		Assert::IsTrue(expectedAns == procVarInfo);
	};
	};
}
