#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/UsesS.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestUsesS) {
private:
	VarIndex varIndex1 = { 1 };
	VarIndex varIndex2 = { 2 };

	StmtIndex stmtIndex1 = { 3 };
	StmtIndex stmtIndex2 = { 4 };

	UsesS* usesS;

	TEST_METHOD_INITIALIZE(init) {
		usesS = new UsesS();
	}

public:
	TEST_METHOD(insert_getFromRightArg) {
		std::vector<StmtIndex> expectedAns{ stmtIndex1 };

		usesS->insert(stmtIndex1, varIndex1);
		auto statements = usesS->getFromRightArg(varIndex1);
		Assert::IsTrue(expectedAns == statements);
	};

	TEST_METHOD(contains) {
		usesS->insert(stmtIndex1, varIndex1);
		Assert::AreEqual(true, usesS->contains(stmtIndex1, varIndex1));
		Assert::AreEqual(false, usesS->contains(stmtIndex1, varIndex2));
		Assert::AreEqual(false, usesS->contains(stmtIndex2, varIndex1));
		Assert::AreEqual(false, usesS->contains(stmtIndex2, varIndex2));
	};

	TEST_METHOD(insert_getFromLeftArg) {
		std::vector<VarIndex> expectedAns{ varIndex1, varIndex2 };

		usesS->insert(stmtIndex1, varIndex1);
		usesS->insert(stmtIndex1, varIndex2);
		auto variables = usesS->getFromLeftArg(stmtIndex1);
		Assert::IsTrue(expectedAns == variables);
	};

	TEST_METHOD(getAllInfo) {
		std::vector<StmtIndex> statements{ stmtIndex1, stmtIndex1, stmtIndex2, stmtIndex2 };
		std::vector<VarIndex> variables{ varIndex1, varIndex2, varIndex1, varIndex2 };
		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> expectedAns =
			std::make_tuple(statements, variables);

		usesS->insert(stmtIndex1, varIndex1);
		usesS->insert(stmtIndex1, varIndex2);
		usesS->insert(stmtIndex2, varIndex1);
		usesS->insert(stmtIndex2, varIndex2);

		auto procVarInfo = usesS->getAllInfo();
		Assert::IsTrue(expectedAns == procVarInfo);
	};
	};
}
