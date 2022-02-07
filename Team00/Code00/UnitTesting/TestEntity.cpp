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
	string varName3 = "past";

	string procName1 = "Peter";
	string procName2 = "Stephen";

	StatementType stmtType1 = StatementType::assignType;
	StatementType stmtType2 = StatementType::whileType;

	VarIndex varIdx1 = VarIndex(1);
	VarIndex varIdx2 = VarIndex(2);
	VarIndex varIdx3 = VarIndex(3);

	ProcIndex procIdx1 = ProcIndex(1);
	ProcIndex procIdx2 = ProcIndex(2);

	StmtIndex stmtIdx1 = StmtIndex(1);
	StmtIndex stmtIdx2 = StmtIndex(2);

	TEST_METHOD_CLEANUP(cleanUpEntities) {
		Entity::performCleanUp();
	}

public:
	TEST_METHOD(insertVar_getVarName_differentVariables) {
		VarIndex idx1 = Entity::insertVar(varName1);
		VarIndex idx2 = Entity::insertVar(varName2);

		string res1 = Entity::getVarName(varIdx1);
		string res2 = Entity::getVarName(varIdx2);
		Assert::AreEqual(varName1, res1);
		Assert::AreEqual(varName2, res2);
		Assert::IsTrue(varIdx1 == idx1);
		Assert::IsTrue(varIdx2 == idx2);
	}

	TEST_METHOD(insertVar_getVarIdx_differentVariables) {
		VarIndex idx1 = Entity::insertVar(varName1);
		VarIndex idx2 = Entity::insertVar(varName2);

		VarIndex res1 = Entity::getVarIdx(varName1);
		VarIndex res2 = Entity::getVarIdx(varName2);
		Assert::IsTrue(varIdx1 == res1);
		Assert::IsTrue(varIdx2 == res2);
		Assert::IsTrue(idx1 == res1);
		Assert::IsTrue(idx2 == res2);
	}

	TEST_METHOD(insertVar_getVarIdx_sameVar) {
		VarIndex idx1 = Entity::insertVar(varName1);
		VarIndex idx2 = Entity::insertVar(varName1);

		VarIndex res = Entity::getVarIdx(varName1);
		Assert::IsTrue(varIdx1 == res);
		Assert::IsTrue(idx1 == res);
	}

	TEST_METHOD(insertVar_getAllVars_differentVars) {
		vector<string> expectedRes;
		expectedRes.push_back(varName1);
		expectedRes.push_back(varName2);

		Entity::insertVar(varName1);
		Entity::insertVar(varName2);

		vector<string> res = Entity::getAllVars();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertVar_getAllVars_sameVar) {
		vector<string> expectedRes;
		expectedRes.push_back(varName1);

		Entity::insertVar(varName1);
		Entity::insertVar(varName1);

		vector<string> res = Entity::getAllVars();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertProc_getProcName_differentProcs) {
		ProcIndex idx1 = Entity::insertProc(procName1);
		ProcIndex idx2 = Entity::insertProc(procName2);

		string res1 = Entity::getProcName(procIdx1);
		string res2 = Entity::getProcName(procIdx2);
		Assert::AreEqual(procName1, res1);
		Assert::AreEqual(procName2, res2);
		Assert::IsTrue(procIdx1 == idx1);
		Assert::IsTrue(procIdx2 == idx2);
	}

	TEST_METHOD(insertProc_getProcIdx_differentProcs) {
		ProcIndex idx1 = Entity::insertProc(procName1);
		ProcIndex idx2 = Entity::insertProc(procName2);

		ProcIndex res1 = Entity::getProcIdx(procName1);
		ProcIndex res2 = Entity::getProcIdx(procName2);
		Assert::IsTrue(procIdx1 == res1);
		Assert::IsTrue(procIdx2 == res2);
		Assert::IsTrue(procIdx1 == idx1);
		Assert::IsTrue(procIdx2 == idx2);
	}

	TEST_METHOD(insertProc_getProcIdx_sameProc) {
		ProcIndex idx1 = Entity::insertProc(procName1);
		ProcIndex idx2 = Entity::insertProc(procName1);

		ProcIndex res = Entity::getProcIdx(procName1);
		Assert::IsTrue(procIdx1 == res);
		Assert::IsTrue(idx1 == res);
	}

	TEST_METHOD(insertProc_getAllProcs_differentProcs) {
		vector<string> expectedRes;
		expectedRes.push_back(procName1);
		expectedRes.push_back(procName2);

		Entity::insertProc(procName1);
		Entity::insertProc(procName2);

		vector<string> res = Entity::getAllProcs();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertProc_getAllProcs_sameProc) {
		vector<string> expectedRes;
		expectedRes.push_back(procName1);

		Entity::insertProc(procName1);
		Entity::insertProc(procName1);

		vector<string> res = Entity::getAllProcs();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertConst_getAllConsts_differentConsts) {
		vector<int> expectedRes;
		expectedRes.push_back(1);
		expectedRes.push_back(2);

		Entity::insertConst(1);
		Entity::insertConst(2);

		vector<int> res = Entity::getAllConsts();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertConst_getAllConsts_sameConst) {
		vector<int> expectedRes;
		expectedRes.push_back(1);

		Entity::insertConst(1);
		Entity::insertConst(1);

		vector<int> res = Entity::getAllConsts();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertStmt_differentStmts) {
		StmtIndex res1 = Entity::insertStmt(stmtType1);
		StmtIndex res2 = Entity::insertStmt(stmtType2);

		Assert::IsTrue(stmtIdx1 == res1);
		Assert::IsTrue(stmtIdx2 == res2);
	}

	TEST_METHOD(insertStmt_getStmtIdxFromType_differentStmts) {
		unordered_set<StmtIndex, StmtIndex::HashFunction> expectedRes1;
		expectedRes1.insert(stmtIdx1);

		unordered_set<StmtIndex, StmtIndex::HashFunction> expectedRes2;
		expectedRes2.insert(stmtIdx2);

		Entity::insertStmt(stmtType1);
		Entity::insertStmt(stmtType2);

		unordered_set<StmtIndex, StmtIndex::HashFunction> res1 = Entity::getStmtIdxFromType(stmtType1);
		unordered_set<StmtIndex, StmtIndex::HashFunction> res2 = Entity::getStmtIdxFromType(stmtType2);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes2 == res2);
	}

	TEST_METHOD(insertStmt_getAllStmts_differentStmts) {
		vector<StmtIndex> expectedRes;
		expectedRes.push_back(stmtIdx1);
		expectedRes.push_back(stmtIdx2);

		Entity::insertStmt(stmtType1);
		Entity::insertStmt(stmtType2);

		vector<StmtIndex> res = Entity::getAllStmts();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertStmtFromProc_getStmtsFromProc_singleProcAndStmt) {
		unordered_set<StmtIndex, StmtIndex::HashFunction> expectedRes;
		expectedRes.insert(stmtIdx1);

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);

		unordered_set<StmtIndex, StmtIndex::HashFunction> res = Entity::getStmtsFromProc(procIdx1);
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertStmtFromProc_getStmtsFromProc_multipleProcAndStmt) {
		unordered_set<StmtIndex, StmtIndex::HashFunction> expectedRes1;
		expectedRes1.insert(stmtIdx1);
		expectedRes1.insert(stmtIdx2);

		unordered_set<StmtIndex, StmtIndex::HashFunction> expectedRes2;
		expectedRes2.insert(stmtIdx2);

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);
		Entity::insertStmtFromProc(procIdx2, stmtIdx2);

		unordered_set<StmtIndex, StmtIndex::HashFunction> res1 = Entity::getStmtsFromProc(procIdx1);
		unordered_set<StmtIndex, StmtIndex::HashFunction> res2 = Entity::getStmtsFromProc(procIdx2);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes2 == res2);
	}

	TEST_METHOD(performCleanUp_VarMap) {
		Entity::insertVar(varName1);
		Entity::insertVar(varName2);
		VarIndex res = Entity::getVarIdx(varName1);
		Assert::IsTrue(varIdx1 == res);

		Entity::performCleanUp();

		Entity::insertVar(varName3);
		VarIndex res2 = Entity::getVarIdx(varName3);
		Assert::IsTrue(varIdx1 == res2);
		Assert::IsFalse(varIdx3 == res2);
	}

	TEST_METHOD(performCleanUp_ConstSet) {
		Entity::insertConst(2);
		Entity::insertConst(4);
		vector<int> res1 = Entity::getAllConsts();
		Assert::IsTrue(res1.size() == 2);

		Entity::performCleanUp();

		Entity::insertConst(3);
		vector<int> res2 = Entity::getAllConsts();
		Assert::IsTrue(res2.size() == 1);
	}
	};
}
