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
	TEST_METHOD(insert_getFromVariable) {
		std::vector<int> expectedAns{ procIndex1.index };

		ModifiesP::insert(procIndex1, varIndex1);
		auto procedures = ModifiesP::getFromVariable(varIndex1);
		Assert::IsTrue(expectedAns == procedures);

		/* Check if other relationship gets affected */
		auto procedures2 = UsesP::getFromVariable(varIndex1);
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

	TEST_METHOD(insert_getVariables) {
		std::vector<int> expectedAns{ varIndex1.index, varIndex2.index };

		ModifiesP::insert(procIndex1, varIndex1);
		ModifiesP::insert(procIndex1, varIndex2);
		auto variables = ModifiesP::getVariables(procIndex1);
		Assert::IsTrue(expectedAns == variables);
	};

	TEST_METHOD(getAllSynonymVarInfo) {
		std::vector<int> procedures{ procIndex1.index, procIndex1.index, procIndex2.index, procIndex2.index };
		std::vector<int> variables{ varIndex1.index, varIndex2.index, varIndex1.index, varIndex2.index };
		std::tuple<std::vector<int>, std::vector<int>> expectedAns = std::make_tuple(procedures, variables);

		ModifiesP::insert(procIndex1, varIndex1);
		ModifiesP::insert(procIndex1, varIndex2);
		ModifiesP::insert(procIndex2, varIndex1);
		ModifiesP::insert(procIndex2, varIndex2);

		auto procVarInfo = ModifiesP::getAllSynonymVarInfo();
		Assert::IsTrue(expectedAns == procVarInfo);
	};

	TEST_METHOD(populateFromSubSynonyms) {
		std::vector<int> expectedAns{ varIndex1.index, varIndex2.index };

		std::unordered_set<ProcIndex> subStmts{ procIndex1 };
		ModifiesP::insert(procIndex1, varIndex1);
		ModifiesP::insert(procIndex1, varIndex2);
		ModifiesP::populateFromSubSynonyms(procIndex2, subStmts);

		auto variables = ModifiesP::getVariables(procIndex2);
		Assert::IsTrue(expectedAns == variables);
	};
	};
}
