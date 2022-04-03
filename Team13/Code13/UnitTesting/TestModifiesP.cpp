#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "../source/PKB/ModifiesP.h"
#include "../source/PKB/UsesP.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestModifiesP) {
private:
	VarIndex varIndex1 = { 1 };
	VarIndex varIndex2 = { 2 };

	ProcIndex procIndex1 = { 3 };
	ProcIndex procIndex2 = { 4 };

	TEST_METHOD_CLEANUP(cleanUpModifiesP) {
		ModifiesP::performCleanUp();
	}

public:
	TEST_METHOD(insert_getFromRightArg) {
		std::vector<ProcIndex> expectedAns{ procIndex1 };

		ModifiesP::insert(procIndex1, varIndex1);
		auto procedures = ModifiesP::getFromRightArg(varIndex1);
		Assert::IsTrue(expectedAns == procedures);

		/* Check if other relationship gets affected */
		auto procedures2 = UsesP::getFromRightArg(varIndex1);
		Assert::IsTrue(0 == procedures2.size());
		UsesP::performCleanUp();
	};

	TEST_METHOD(contains) {
		ModifiesP::insert(procIndex1, varIndex1);
		Assert::AreEqual(true, ModifiesP::contains(procIndex1, varIndex1));
		Assert::AreEqual(false, ModifiesP::contains(procIndex1, varIndex2));
		Assert::AreEqual(false, ModifiesP::contains(procIndex2, varIndex1));
		Assert::AreEqual(false, ModifiesP::contains(procIndex2, varIndex2));
	};

	TEST_METHOD(insert_getFromLeftArg) {
		std::vector<VarIndex> expectedAns{ varIndex1, varIndex2 };

		ModifiesP::insert(procIndex1, varIndex1);
		ModifiesP::insert(procIndex1, varIndex2);
		auto variables = ModifiesP::getFromLeftArg(procIndex1);
		Assert::IsTrue(expectedAns == variables);
	};

	TEST_METHOD(getAllInfo) {
		std::vector<ProcIndex> procedures{ procIndex1, procIndex1, procIndex2, procIndex2 };
		std::vector<VarIndex> variables{ varIndex1, varIndex2, varIndex1, varIndex2 };
		std::tuple<std::vector<ProcIndex>, std::vector<VarIndex>> expectedAns = std::make_tuple(procedures, variables);

		ModifiesP::insert(procIndex1, varIndex1);
		ModifiesP::insert(procIndex1, varIndex2);
		ModifiesP::insert(procIndex2, varIndex1);
		ModifiesP::insert(procIndex2, varIndex2);

		auto procVarInfo = ModifiesP::getAllInfo();
		Assert::IsTrue(expectedAns == procVarInfo);
	};

	TEST_METHOD(populateFromSubSynonyms) {
		std::vector<VarIndex> expectedAns{ varIndex1, varIndex2 };

		std::unordered_set<ProcIndex> subStmts{ procIndex1 };
		ModifiesP::insert(procIndex1, varIndex1);
		ModifiesP::insert(procIndex1, varIndex2);
		ModifiesP::populateFromSubSynonyms(procIndex2, subStmts);

		auto variables = ModifiesP::getFromLeftArg(procIndex2);
		Assert::IsTrue(expectedAns == variables);
	};
	};
}
