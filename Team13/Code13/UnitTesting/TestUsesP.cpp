#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/UsesP.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestUsesP) {
private:
	VarIndex varIndex1 = { 1 };
	VarIndex varIndex2 = { 2 };

	ProcIndex procIndex1 = { 3 };
	ProcIndex procIndex2 = { 4 };

	UsesP* usesP;

	TEST_METHOD_INITIALIZE(init) {
		usesP = new UsesP();
	}

public:
	TEST_METHOD(insert_getFromRightArg) {
		std::vector<ProcIndex> expectedAns{ procIndex1 };

		usesP->insert(procIndex1, varIndex1);
		auto procedures = usesP->getFromRightArg(varIndex1);
		Assert::IsTrue(expectedAns == procedures);
	};

	TEST_METHOD(contains) {
		usesP->insert(procIndex1, varIndex1);
		Assert::AreEqual(true, usesP->contains(procIndex1, varIndex1));
		Assert::AreEqual(false, usesP->contains(procIndex1, varIndex2));
		Assert::AreEqual(false, usesP->contains(procIndex2, varIndex1));
		Assert::AreEqual(false, usesP->contains(procIndex2, varIndex2));
	};

	TEST_METHOD(insert_getFromLeftArg) {
		std::vector<VarIndex> expectedAns{ varIndex1, varIndex2 };

		usesP->insert(procIndex1, varIndex1);
		usesP->insert(procIndex1, varIndex2);
		auto variables = usesP->getFromLeftArg(procIndex1);
		Assert::IsTrue(expectedAns == variables);
	};

	TEST_METHOD(getAllInfo) {
		std::vector<ProcIndex> procedures{ procIndex1, procIndex1, procIndex2, procIndex2 };
		std::vector<VarIndex> variables{ varIndex1, varIndex2, varIndex1, varIndex2 };
		std::tuple<std::vector<ProcIndex>, std::vector<VarIndex>> expectedAns =
			std::make_tuple(procedures, variables);

		usesP->insert(procIndex1, varIndex1);
		usesP->insert(procIndex1, varIndex2);
		usesP->insert(procIndex2, varIndex1);
		usesP->insert(procIndex2, varIndex2);

		auto procVarInfo = usesP->getAllInfo();
		Assert::IsTrue(expectedAns == procVarInfo);
	};
	};
}
