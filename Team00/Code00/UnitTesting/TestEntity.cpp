#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/common/Types.h"
#include "../source/PKB/Entity.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestEntity) {
private:
	string varName1 = "future";
	string varName2 = "present";

	VarIndex varIdx1 = VarIndex(1);
	VarIndex varIdx2 = VarIndex(2);

	StmtIndex stmtIdx1 = StmtIndex(1);
	StmtIndex stmtIdx2 = StmtIndex(2);

	ProcIndex procIdx1 = ProcIndex(1);
	ProcIndex procIdx2 = ProcIndex(2);

	TEST_METHOD_CLEANUP(clearEntityTables) {
		Entity::performCleanUp();
	}

public:
	TEST_METHOD(insertVar_getVarName_differentVariables) {
		Entity::insertVar(varName1);
		Entity::insertVar(varName2);

		string res1 = Entity::getVarName(varIdx1);
		string res2 = Entity::getVarName(varIdx2);
		Assert::AreEqual(varName1, res1);
		Assert::AreEqual(varName2, res2);
	}

	TEST_METHOD(insertVar_getVarIdx_differentVariables) {
		Entity::insertVar(varName1);
		Entity::insertVar(varName2);

		VarIndex res1 = Entity::getVarIdx(varName1);
		VarIndex res2 = Entity::getVarIdx(varName2);
		Assert::AreEqual(varIdx1, res1);
		Assert::AreEqual(varIdx2, res2);
	}

	TEST_METHOD(insertVar_getVarIdx_sameVariable) {
		Entity::insertVar(varName1);
		Entity::insertVar(varName1);

		VarIndex res = Entity::getVarIdx(varName1);
		Assert::AreEqual(varIdx1, res);
	}

	TEST_METHOD(insertVar_getAllVars_differentVariables) {
		vector<string> expectedRes;
		expectedRes.push_back(varName1);
		expectedRes.push_back(varName2);

		Entity::insertVar(varName1);
		Entity::insertVar(varName2);

		vector<string> res = Entity::getAllVars();
		Assert::AreEqual(expectedRes, res);
	}

	TEST_METHOD(insertVar_getAllVars_sameVariable) {
		vector<string> expectedRes;
		expectedRes.push_back(varName1);

		Entity::insertVar(varName1);
		Entity::insertVar(varName1);

		vector<string> res = Entity::getAllVars();
		Assert::AreEqual(expectedRes, res);
	}

	// BELOW OUTDATED
	TEST_METHOD(insertVar_singleVariable) {
		//StmtIndex stmtIdx = { 1 };
		//StmtIndex stmtIdx2 = { 5 };
		VarIndex varIdx = { 2 };
		VarIndex varIdx2 = VarIndex(2);
		//VarIndex varIdx2 = { 4 };
		//ProcIndex procIdx = { 3 };
		Entity::insertVar("future");
		Entity::insertVar("present");
		VarIndex res = Entity::getVarIdx("present");
		Assert::IsTrue(res == varIdx);
		Assert::IsTrue(res == varIdx2);
	}

	TEST_METHOD(TestMethod6) { //"persistence"
		Entity::insertVar("future");
		Entity::insertVar("present");
		VarIndex res = Entity::getVarIdx("present");
		Assert::AreEqual(res.index, 4);
	}

	TEST_METHOD(TestMethod6333) {
		Entity::insertConst(5);
		Entity::insertConst(6);
		vector<int> res = Entity::getAllConsts();
		Assert::IsTrue(res.size() == 2);
	}

	TEST_METHOD(Codecleanup) {
		TestMethod6333();
		Entity::insertConst(7);
		Entity::insertConst(8);
		vector<int> res = Entity::getAllConsts();
		Assert::IsTrue(res.size() == 4);
	}

	//TEST_METHOD(TestMethod5) {
	//	StmtIndex stmtIdx = { 1 };
	//	StmtIndex stmtIdx2 = { 5 };
	//	VarIndex varIdx = { 2 };
	//	VarIndex varIdx2 = { 4 };
	//	ProcIndex procIdx = { 3 };
	//	ProcIndex procIdx2 = { 4 };
	//	Entity::insertStmtFromProc(procIdx.index, stmtIdx.index);
	//	Entity::insertStmtFromProc(procIdx2.index, stmtIdx2.index);
	//	unordered_set<int> res1 = Entity::getStmtsFromProc(procIdx.index);
	//	auto res2 = res1.begin();
	//	Assert::AreEqual(*res2, stmtIdx.index);
	//	Assert::AreEqual(*res2, 1);
	//	//Assert::AreEqual(*res3, stmtIdx2.index);
	//}

	//TEST_METHOD(TestMethod7) {
	//	Entity::insertVar("future");
	//	Entity::insertVar("present");
	//	vector<string> res = Entity::getAllVars();
	//	Assert::AreEqual(*res.begin(), Entity::getVarName(1));
	//}

	TEST_METHOD(TestMethod8) {
		Entity::insertVar("future");
		Entity::insertVar("present");
		string res = Entity::getVarName(VarIndex(2));
		Assert::AreEqual(res, string("present"));
	}
	};
}
