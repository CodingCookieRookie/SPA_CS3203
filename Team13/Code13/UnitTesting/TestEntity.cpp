#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "../source/PKB/Attribute.h"
#include "../source/PKB/Entity.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestEntity) {
private:
	std::string varName1 = "future";
	std::string varName2 = "present";
	std::string varName3 = "past";

	std::string procName1 = "Peter";
	std::string procName2 = "Stephen";

	StatementType assignType = StatementType::ASSIGN_TYPE;
	StatementType whileType = StatementType::WHILE_TYPE;
	StatementType callType = StatementType::CALL_TYPE;
	StatementType readType = StatementType::READ_TYPE;
	StatementType printType = StatementType::PRINT_TYPE;
	StatementType ifType = StatementType::IF_TYPE;

	VarIndex varIdx1 = VarIndex(1);
	VarIndex varIdx2 = VarIndex(2);
	VarIndex varIdx3 = VarIndex(3);

	ProcIndex procIdx1 = ProcIndex(1);
	ProcIndex procIdx2 = ProcIndex(2);

	StmtIndex stmtIdx1 = StmtIndex(1);
	StmtIndex stmtIdx2 = StmtIndex(2);
	StmtIndex stmtIdx3 = StmtIndex(3);

	TEST_METHOD_CLEANUP(cleanUpEntities) {
		Attribute::performCleanUp();
		Entity::performCleanUp();
	}

public:

	TEST_METHOD(insertVar_containsVar_variableExists) {
		VarIndex idx = Entity::insertVar(varName1);

		bool res = Entity::containsVar(varName1);
		Assert::IsTrue(res);
	}

	TEST_METHOD(insertVar_containsVar_variableDoesNotExist) {
		VarIndex idx = Entity::insertVar(varName1);

		bool res = Entity::containsVar(varName2);
		Assert::IsFalse(res);
	}

	TEST_METHOD(insertVar_getVarName_differentVariables) {
		VarIndex idx1 = Entity::insertVar(varName1);
		VarIndex idx2 = Entity::insertVar(varName2);

		std::string res1 = Entity::getVarName(varIdx1);
		std::string res2 = Entity::getVarName(varIdx2);
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
		std::vector<VarIndex> expectedRes;
		expectedRes.push_back(varIdx1);
		expectedRes.push_back(varIdx2);

		Entity::insertVar(varName1);
		Entity::insertVar(varName2);

		std::vector<VarIndex> res = Entity::getAllVars();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertVar_getAllVars_sameVar) {
		std::vector<VarIndex> expectedRes;
		expectedRes.push_back(varIdx1);

		Entity::insertVar(varName1);
		Entity::insertVar(varName1);

		std::vector<VarIndex> res = Entity::getAllVars();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertProc_containsProc_procExists) {
		ProcIndex idx = Entity::insertProc(procName1);

		bool res = Entity::containsProc(procName1);
		Assert::IsTrue(res);
	}

	TEST_METHOD(insertProc_containsProc_procDoesNotExist) {
		ProcIndex idx = Entity::insertProc(procName1);

		bool res = Entity::containsProc(procName2);
		Assert::IsFalse(res);
	}

	TEST_METHOD(insertProc_getProcName_differentProcs) {
		ProcIndex idx1 = Entity::insertProc(procName1);
		ProcIndex idx2 = Entity::insertProc(procName2);

		std::string res1 = Entity::getProcName(procIdx1);
		std::string res2 = Entity::getProcName(procIdx2);
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
		std::vector<ProcIndex> expectedRes;
		expectedRes.push_back(procIdx1);
		expectedRes.push_back(procIdx2);

		Entity::insertProc(procName1);
		Entity::insertProc(procName2);

		std::vector<ProcIndex> res = Entity::getAllProcs();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertProc_getAllProcs_sameProc) {
		std::vector<ProcIndex> expectedRes;
		expectedRes.push_back(procIdx1);

		Entity::insertProc(procName1);
		Entity::insertProc(procName1);

		std::vector<ProcIndex> res = Entity::getAllProcs();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertConst_getAllConsts_differentConsts) {
		std::vector<ConstValue> expectedRes;
		expectedRes.push_back(1);
		expectedRes.push_back(2);

		Entity::insertConst(1);
		Entity::insertConst(2);

		std::vector<ConstValue> res = Entity::getAllConsts();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertConst_getAllConsts_sameConst) {
		std::vector<ConstValue> expectedRes;
		expectedRes.push_back(1);

		Entity::insertConst(1);
		Entity::insertConst(1);

		std::vector<ConstValue> res = Entity::getAllConsts();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertStmt_containsStmt_stmtExists) {
		StmtIndex idx = Entity::insertStmt(assignType);

		bool res = Entity::containsStmt(1);
		Assert::IsTrue(res);
	}

	TEST_METHOD(insertStmt_containsStmt_stmtDoesNotExist) {
		StmtIndex idx = Entity::insertStmt(assignType);

		bool res = Entity::containsStmt(2);
		Assert::IsFalse(res);
	}

	TEST_METHOD(insertStmt_differentStmts) {
		StmtIndex res1 = Entity::insertStmt(assignType);
		StmtIndex res2 = Entity::insertStmt(whileType);

		Assert::IsTrue(stmtIdx1 == res1);
		Assert::IsTrue(stmtIdx2 == res2);
	}

	TEST_METHOD(insertStmt_getStmtIdxFromType_differentStmts) {
		std::vector<StmtIndex> expectedRes1;
		expectedRes1.push_back(stmtIdx1);

		std::vector<StmtIndex> expectedRes2;
		expectedRes2.push_back(stmtIdx2);

		Entity::insertStmt(assignType);
		Entity::insertStmt(whileType);

		std::vector<StmtIndex> res1 = Entity::getStmtIdxFromType(assignType);
		std::vector<StmtIndex> res2 = Entity::getStmtIdxFromType(whileType);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes2 == res2);
	}

	TEST_METHOD(insertStmt_getTypeFromStmtIdx) {
		StmtIndex stmtIdx1 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmtIdx2 = Entity::insertStmt(StatementType::CALL_TYPE);
		StmtIndex stmtIdx3 = Entity::insertStmt(StatementType::IF_TYPE);
		StmtIndex stmtIdx4 = Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmtIdx5 = Entity::insertStmt(StatementType::READ_TYPE);
		StmtIndex stmtIdx6 = Entity::insertStmt(StatementType::WHILE_TYPE);

		StatementType res1 = Entity::getTypeFromStmtIdx(stmtIdx1);
		StatementType res2 = Entity::getTypeFromStmtIdx(stmtIdx2);
		StatementType res3 = Entity::getTypeFromStmtIdx(stmtIdx3);
		StatementType res4 = Entity::getTypeFromStmtIdx(stmtIdx4);
		StatementType res5 = Entity::getTypeFromStmtIdx(stmtIdx5);
		StatementType res6 = Entity::getTypeFromStmtIdx(stmtIdx6);

		Assert::IsTrue(StatementType::ASSIGN_TYPE == res1);
		Assert::IsTrue(StatementType::CALL_TYPE == Entity::getTypeFromStmtIdx(stmtIdx2));
		Assert::IsTrue(StatementType::IF_TYPE == Entity::getTypeFromStmtIdx(stmtIdx3));
		Assert::IsTrue(StatementType::PRINT_TYPE == Entity::getTypeFromStmtIdx(stmtIdx4));
		Assert::IsTrue(StatementType::READ_TYPE == Entity::getTypeFromStmtIdx(stmtIdx5));
		Assert::IsTrue(StatementType::WHILE_TYPE == Entity::getTypeFromStmtIdx(stmtIdx6));
	}

	TEST_METHOD(insertStmt_getAllStmts_differentStmts) {
		std::vector<StmtIndex> expectedRes;
		expectedRes.push_back(stmtIdx1);
		expectedRes.push_back(stmtIdx2);

		Entity::insertStmt(assignType);
		Entity::insertStmt(whileType);

		std::vector<StmtIndex> res = Entity::getAllStmts();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertStmt_getAllContainerStmts_noContainerStmts) {
		Entity::insertStmt(assignType);
		Entity::insertStmt(assignType);

		std::vector<StmtIndex> res = Entity::getAllContainerStmts();
		Assert::IsTrue(0 == res.size());
	}

	TEST_METHOD(insertStmt_isContainerStmt_allContainerStmts) {
		std::vector<StmtIndex> expectedRes;
		expectedRes.push_back(stmtIdx1);
		expectedRes.push_back(stmtIdx2);
		expectedRes.push_back(stmtIdx3);

		Entity::insertStmt(ifType);
		Entity::insertStmt(whileType);
		Entity::insertStmt(ifType);

		std::vector<StmtIndex> res = Entity::getAllContainerStmts();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertStmt_isContainerStmt_mixedStmts) {
		std::vector<StmtIndex> expectedRes;
		expectedRes.push_back(stmtIdx2);
		expectedRes.push_back(stmtIdx3);

		Entity::insertStmt(assignType);
		Entity::insertStmt(whileType);
		Entity::insertStmt(ifType);

		std::vector<StmtIndex> res = Entity::getAllContainerStmts();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertStmtFromProc_getStmtsFromProc_singleProcAndStmt) {
		std::unordered_set<StmtIndex> expectedRes;
		expectedRes.insert(stmtIdx1);

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);

		std::unordered_set<StmtIndex> res = Entity::getStmtsFromProc(procIdx1);
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertStmtFromProc_getStmtsFromProc_multipleProcAndStmt) {
		std::unordered_set<StmtIndex> expectedRes1;
		expectedRes1.insert(stmtIdx1);
		expectedRes1.insert(stmtIdx2);

		std::unordered_set<StmtIndex> expectedRes2;
		expectedRes2.insert(stmtIdx2);

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);
		Entity::insertStmtFromProc(procIdx2, stmtIdx2);

		std::unordered_set<StmtIndex> res1 = Entity::getStmtsFromProc(procIdx1);
		std::unordered_set<StmtIndex> res2 = Entity::getStmtsFromProc(procIdx2);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes2 == res2);
	}

	TEST_METHOD(insertStmtFromProc_getProcFromStmt) {
		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx2, stmtIdx2);

		ProcIndex res1 = Entity::getProcFromStmt(stmtIdx1);
		ProcIndex res2 = Entity::getProcFromStmt(stmtIdx2);
		Assert::IsTrue(procIdx1 == res1);
		Assert::IsTrue(procIdx2 == res2);
	}

	TEST_METHOD(insertStmtFromProc_getAllProcStmts_multipleProcAndStmt) {
		std::unordered_map<ProcIndex, std::unordered_set<StmtIndex>> expectedRes;
		expectedRes[procIdx1].insert(stmtIdx1);
		expectedRes[procIdx1].insert(stmtIdx2);

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);

		std::unordered_map<ProcIndex, std::unordered_set<StmtIndex>>  res1 = Entity::getAllProcStmts();
		Assert::IsTrue(expectedRes == res1);

		expectedRes[procIdx2].insert(stmtIdx2);
		Entity::insertStmtFromProc(procIdx2, stmtIdx2);

		std::unordered_map<ProcIndex, std::unordered_set<StmtIndex>>  res2 = Entity::getAllProcStmts();
		Assert::IsTrue(expectedRes == res2);
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
		std::vector<ConstValue> res1 = Entity::getAllConsts();
		Assert::IsTrue(res1.size() == 2);

		Entity::performCleanUp();

		Entity::insertConst(3);
		std::vector<ConstValue> res2 = Entity::getAllConsts();
		Assert::IsTrue(res2.size() == 1);
	}
	};
}
