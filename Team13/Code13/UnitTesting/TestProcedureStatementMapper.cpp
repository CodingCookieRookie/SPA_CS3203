#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/ProcedureStatementMapper.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestProcedureStatementMapper) {
private:
	ProcIndex procIdx1 = ProcIndex(1);
	ProcIndex procIdx2 = ProcIndex(2);

	StmtIndex stmtIdx1 = StmtIndex(1);
	StmtIndex stmtIdx2 = StmtIndex(2);
	StmtIndex stmtIdx3 = StmtIndex(3);

public:

	TEST_METHOD(insertStmtFromProc_getStmtsFromProc_singleProcAndStmt) {
		ProcedureStatementMapper procStmt = ProcedureStatementMapper();

		std::vector<StmtIndex> expectedRes;
		expectedRes.push_back(stmtIdx1);

		procStmt.insertStmtFromProc(procIdx1, stmtIdx1);

		std::vector<StmtIndex> res = procStmt.getStmtsFromProc(procIdx1);
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertStmtFromProc_getStmtsFromProc_multipleProcsAndStmts) {
		ProcedureStatementMapper procStmt = ProcedureStatementMapper();

		std::vector<StmtIndex> expectedRes1 = { stmtIdx1 , stmtIdx2 };
		std::vector<StmtIndex> expectedRes2 = { stmtIdx3 };

		procStmt.insertStmtFromProc(procIdx1, stmtIdx1);
		procStmt.insertStmtFromProc(procIdx1, stmtIdx2);
		procStmt.insertStmtFromProc(procIdx2, stmtIdx3);

		std::vector<StmtIndex> res1 = procStmt.getStmtsFromProc(procIdx1);
		std::vector<StmtIndex> res2 = procStmt.getStmtsFromProc(procIdx2);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes2 == res2);
	}

	TEST_METHOD(insertStmtFromProc_getStmtsFromProc_multipleStmtsSameProc) {
		ProcedureStatementMapper procStmt = ProcedureStatementMapper();

		std::vector<StmtIndex> expectedRes = { stmtIdx1 , stmtIdx2, stmtIdx3 };

		procStmt.insertStmtFromProc(procIdx1, stmtIdx1);
		procStmt.insertStmtFromProc(procIdx1, stmtIdx2);
		procStmt.insertStmtFromProc(procIdx1, stmtIdx3);

		std::vector<StmtIndex> res = procStmt.getStmtsFromProc(procIdx1);
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertStmtFromProc_getProcFromStmt) {
		ProcedureStatementMapper procStmt = ProcedureStatementMapper();

		procStmt.insertStmtFromProc(procIdx1, stmtIdx1);
		procStmt.insertStmtFromProc(procIdx2, stmtIdx2);

		ProcIndex res1 = procStmt.getProcFromStmt(stmtIdx1);
		ProcIndex res2 = procStmt.getProcFromStmt(stmtIdx2);
		Assert::IsTrue(procIdx1 == res1);
		Assert::IsTrue(procIdx2 == res2);
	}

	TEST_METHOD(insertStmtFromProc_getAllProcStmts_multipleStmtsSameProc) {
		ProcedureStatementMapper procStmt = ProcedureStatementMapper();

		std::vector<ProcIndex> expectedProcIdxLst{ procIdx1 , procIdx1 };
		std::vector<StmtIndex> expectedStmtIdxLst{ stmtIdx1 , stmtIdx2 };
		std::tuple<std::vector<ProcIndex>, std::vector<StmtIndex>> expectedRes =
			std::make_tuple(expectedProcIdxLst, expectedStmtIdxLst);

		procStmt.insertStmtFromProc(procIdx1, stmtIdx1);
		procStmt.insertStmtFromProc(procIdx1, stmtIdx2);

		std::tuple<std::vector<ProcIndex>, std::vector<StmtIndex>>  res1 = procStmt.getAllProcStmts();
		Assert::IsTrue(expectedRes == res1);

		std::get<0>(expectedRes).push_back(procIdx1);
		std::get<1>(expectedRes).push_back(stmtIdx3);
		procStmt.insertStmtFromProc(procIdx1, stmtIdx3);

		std::tuple<std::vector<ProcIndex>, std::vector<StmtIndex>>  res2 = procStmt.getAllProcStmts();
		Assert::IsTrue(expectedRes == res2);
	}

	TEST_METHOD(insertStmtFromProc_getAllProcStmts_multipleProcsAndStmts) {
		ProcedureStatementMapper procStmt = ProcedureStatementMapper();

		std::vector<ProcIndex> expectedProcIdxLst{ procIdx1 , procIdx1 };
		std::vector<StmtIndex> expectedStmtIdxLst{ stmtIdx1 , stmtIdx2 };
		std::tuple<std::vector<ProcIndex>, std::vector<StmtIndex>> expectedRes =
			std::make_tuple(expectedProcIdxLst, expectedStmtIdxLst);

		procStmt.insertStmtFromProc(procIdx1, stmtIdx1);
		procStmt.insertStmtFromProc(procIdx1, stmtIdx2);

		std::tuple<std::vector<ProcIndex>, std::vector<StmtIndex>>  res1 = procStmt.getAllProcStmts();
		Assert::IsTrue(expectedRes == res1);

		std::get<0>(expectedRes).push_back(procIdx2);
		std::get<1>(expectedRes).push_back(stmtIdx3);
		procStmt.insertStmtFromProc(procIdx2, stmtIdx3);

		std::tuple<std::vector<ProcIndex>, std::vector<StmtIndex>>  res2 = procStmt.getAllProcStmts();
		Assert::IsTrue(expectedRes == res2);
	}
	};
}
