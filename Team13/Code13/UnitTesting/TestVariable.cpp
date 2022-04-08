#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/Variable.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestVariable) {
private:
	std::string varName1 = "future";
	std::string varName2 = "present";
	std::string varName3 = "past";

	VarIndex varIdx1 = VarIndex(1);
	VarIndex varIdx2 = VarIndex(2);
	VarIndex varIdx3 = VarIndex(3);

	Attribute* attribute;
	Variable* variable;

	TEST_METHOD_INITIALIZE(initTables) {
		attribute = new Attribute();
		variable = new Variable();
	}

public:

	TEST_METHOD(insert_containsVar_variableExists) {
		VarIndex idx = variable->insert(varName1, attribute);

		bool res = variable->contains(varName1);
		Assert::IsTrue(res);
	}

	TEST_METHOD(insert_containsVar_variableDoesNotExist) {
		VarIndex idx = variable->insert(varName1, attribute);

		bool res = variable->contains(varName2);
		Assert::IsFalse(res);
	}

	TEST_METHOD(insert_getVarName_differentVariables) {
		VarIndex idx1 = variable->insert(varName1, attribute);
		VarIndex idx2 = variable->insert(varName2, attribute);

		std::string res1 = variable->getName(varIdx1);
		std::string res2 = variable->getName(varIdx2);
		Assert::AreEqual(varName1, res1);
		Assert::AreEqual(varName2, res2);
		Assert::IsTrue(varIdx1 == idx1);
		Assert::IsTrue(varIdx2 == idx2);
	}

	TEST_METHOD(insert_getVarIdx_differentVariables) {
		VarIndex idx1 = variable->insert(varName1, attribute);
		VarIndex idx2 = variable->insert(varName2, attribute);

		VarIndex res1 = variable->getIndex(varName1);
		VarIndex res2 = variable->getIndex(varName2);

		Assert::IsTrue(varIdx1 == res1);
		Assert::IsTrue(varIdx2 == res2);
		Assert::IsTrue(idx1 == res1);
		Assert::IsTrue(idx2 == res2);
	}

	TEST_METHOD(insert_getVarIdx_sameVar) {
		VarIndex idx1 = variable->insert(varName1, attribute);
		VarIndex idx2 = variable->insert(varName1, attribute);

		VarIndex res = variable->getIndex(varName1);
		Assert::IsTrue(varIdx1 == res);
		Assert::IsTrue(idx1 == res);
	}

	TEST_METHOD(insert_getAllVars_differentVars) {
		std::vector<VarIndex> expectedRes = { varIdx1 , varIdx2 };

		variable->insert(varName1, attribute);
		variable->insert(varName2, attribute);

		std::vector<VarIndex> res = variable->getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAllVars_sameVar) {
		std::vector<VarIndex> expectedRes = { varIdx1 };

		variable->insert(varName1, attribute);
		variable->insert(varName1, attribute);

		std::vector<VarIndex> res = variable->getAll();
		Assert::IsTrue(expectedRes == res);
	}
	};
}
