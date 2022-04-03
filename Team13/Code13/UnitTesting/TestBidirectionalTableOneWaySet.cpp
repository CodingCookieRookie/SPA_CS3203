#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "PKB/BidirectionalTable/BidirectionalTableOneWaySet.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestBidirectionalTableOneWaySet) {
public:

	TEST_METHOD(insert_contains_stmts) {
		StatementType assignType = StatementType::ASSIGN_TYPE;
		StatementType readType = StatementType::READ_TYPE;
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		BidirectionalTableOneWaySet<StatementType, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<StatementType, StmtIndex>();
		bidirectionalTable.insert(assignType, stmtIdx1);
		bidirectionalTable.insert(readType, stmtIdx2);

		Assert::IsTrue(bidirectionalTable.contains(stmtIdx1));
		Assert::IsTrue(bidirectionalTable.contains(stmtIdx2));
	}

	TEST_METHOD(insert_getSize_stmts) {
		StatementType assignType = StatementType::ASSIGN_TYPE;
		StatementType readType = StatementType::READ_TYPE;
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		BidirectionalTableOneWaySet<StatementType, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<StatementType, StmtIndex>();
		bidirectionalTable.insert(assignType, stmtIdx1);
		bidirectionalTable.insert(readType, stmtIdx2);

		Assert::IsTrue(size_t(2) == bidirectionalTable.getSize());
	}

	TEST_METHOD(insert_getFromLeftArg_procStmts_sameProc) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		std::vector<StmtIndex> expectedRes{ stmtIdx1, stmtIdx2 };

		BidirectionalTableOneWaySet<ProcIndex, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<ProcIndex, StmtIndex>();
		bidirectionalTable.insert(procIdx1, stmtIdx1);
		bidirectionalTable.insert(procIdx1, stmtIdx2);

		Assert::IsTrue(expectedRes == bidirectionalTable.getFromLeftArg(procIdx1));
		Assert::IsTrue(size_t(0) == bidirectionalTable.getFromLeftArg(procIdx2).size());
	}

	TEST_METHOD(insert_getFromLeftArg_procStmts_differentProc) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		std::vector<StmtIndex> expectedRes1{ stmtIdx1 };
		std::vector<StmtIndex> expectedRes2{ stmtIdx2 };

		BidirectionalTableOneWaySet<ProcIndex, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<ProcIndex, StmtIndex>();
		bidirectionalTable.insert(procIdx1, stmtIdx1);
		bidirectionalTable.insert(procIdx2, stmtIdx2);

		Assert::IsTrue(expectedRes1 == bidirectionalTable.getFromLeftArg(procIdx1));
		Assert::IsTrue(expectedRes2 == bidirectionalTable.getFromLeftArg(procIdx2));
	}

	TEST_METHOD(insert_getFromLeftArg_stmts_sameType) {
		StatementType assignType = StatementType::ASSIGN_TYPE;
		StatementType readType = StatementType::READ_TYPE;
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		std::vector<StmtIndex> expectedRes{ stmtIdx1, stmtIdx2 };

		BidirectionalTableOneWaySet<StatementType, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<StatementType, StmtIndex>();
		bidirectionalTable.insert(assignType, stmtIdx1);
		bidirectionalTable.insert(assignType, stmtIdx2);

		Assert::IsTrue(expectedRes == bidirectionalTable.getFromLeftArg(assignType));
		Assert::IsTrue(size_t(0) == bidirectionalTable.getFromLeftArg(readType).size());
	}

	TEST_METHOD(insert_getFromLeftArg_stmts_differentType) {
		StatementType assignType = StatementType::ASSIGN_TYPE;
		StatementType readType = StatementType::READ_TYPE;
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		std::vector<StmtIndex> expectedRes1{ stmtIdx1 };
		std::vector<StmtIndex> expectedRes2{ stmtIdx2 };

		BidirectionalTableOneWaySet<StatementType, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<StatementType, StmtIndex>();
		bidirectionalTable.insert(assignType, stmtIdx1);
		bidirectionalTable.insert(readType, stmtIdx2);

		Assert::IsTrue(expectedRes1 == bidirectionalTable.getFromLeftArg(assignType));
		Assert::IsTrue(expectedRes2 == bidirectionalTable.getFromLeftArg(readType));
	}

	TEST_METHOD(insert_getFromLeftArg_patternVarStmt_sameVar) {
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		std::vector<StmtIndex> expectedRes{ stmtIdx1, stmtIdx2 };

		BidirectionalTableOneWaySet<VarIndex, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx1);
		bidirectionalTable.insert(varIdx1, stmtIdx2);

		Assert::IsTrue(expectedRes == bidirectionalTable.getFromLeftArg(varIdx1));
		Assert::IsTrue(size_t(0) == bidirectionalTable.getFromLeftArg(varIdx2).size());
	}

	TEST_METHOD(insert_getFromLeftArg_patternVarStmt_differentVar) {
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		std::vector<StmtIndex> expectedRes1{ stmtIdx1 };
		std::vector<StmtIndex> expectedRes2{ stmtIdx2 };

		BidirectionalTableOneWaySet<VarIndex, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx1);
		bidirectionalTable.insert(varIdx2, stmtIdx2);

		Assert::IsTrue(expectedRes1 == bidirectionalTable.getFromLeftArg(varIdx1));
		Assert::IsTrue(expectedRes2 == bidirectionalTable.getFromLeftArg(varIdx2));
	}

	TEST_METHOD(insert_getFromLeftArg_patternPostfixExprStmt_sameExpr) {
		std::string postfix1 = " a b + ";
		std::string postfix2 = " c a b * + ";
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		std::vector<StmtIndex> expectedRes{ stmtIdx1, stmtIdx2 };

		BidirectionalTableOneWaySet<std::string, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<std::string, StmtIndex>();
		bidirectionalTable.insert(postfix1, stmtIdx1);
		bidirectionalTable.insert(postfix1, stmtIdx2);

		Assert::IsTrue(expectedRes == bidirectionalTable.getFromLeftArg(postfix1));
		Assert::IsTrue(size_t(0) == bidirectionalTable.getFromLeftArg(postfix2).size());
	}

	TEST_METHOD(insert_getFromLeftArg_patternPostfixExprStmt_differentExpr) {
		std::string postfix1 = " a b + ";
		std::string postfix2 = " c a b * + ";
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		std::vector<StmtIndex> expectedRes1{ stmtIdx1 };
		std::vector<StmtIndex> expectedRes2{ stmtIdx2 };

		BidirectionalTableOneWaySet<std::string, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<std::string, StmtIndex>();
		bidirectionalTable.insert(postfix1, stmtIdx1);
		bidirectionalTable.insert(postfix2, stmtIdx2);

		Assert::IsTrue(expectedRes1 == bidirectionalTable.getFromLeftArg(postfix1));
		Assert::IsTrue(expectedRes2 == bidirectionalTable.getFromLeftArg(postfix2));
	}

	TEST_METHOD(insert_getFromRightArg_procStmts_sameProc) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		BidirectionalTableOneWaySet<ProcIndex, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<ProcIndex, StmtIndex>();
		bidirectionalTable.insert(procIdx1, stmtIdx1);
		bidirectionalTable.insert(procIdx1, stmtIdx2);

		Assert::IsTrue(procIdx1 == bidirectionalTable.getFromRightArg(stmtIdx1));
		Assert::IsTrue(procIdx1 == bidirectionalTable.getFromRightArg(stmtIdx2));
	}

	TEST_METHOD(insert_getFromRightArg_procStmts_differentProc) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		BidirectionalTableOneWaySet<ProcIndex, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<ProcIndex, StmtIndex>();
		bidirectionalTable.insert(procIdx1, stmtIdx1);
		bidirectionalTable.insert(procIdx2, stmtIdx2);

		Assert::IsTrue(procIdx1 == bidirectionalTable.getFromRightArg(stmtIdx1));
		Assert::IsTrue(procIdx2 == bidirectionalTable.getFromRightArg(stmtIdx2));
	}

	TEST_METHOD(insert_getFromRightArg_stmts_sameType) {
		StatementType assignType = StatementType::ASSIGN_TYPE;
		StatementType readType = StatementType::READ_TYPE;
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		BidirectionalTableOneWaySet<StatementType, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<StatementType, StmtIndex>();
		bidirectionalTable.insert(assignType, stmtIdx1);
		bidirectionalTable.insert(assignType, stmtIdx2);

		Assert::IsTrue(assignType == bidirectionalTable.getFromRightArg(stmtIdx1));
		Assert::IsTrue(assignType == bidirectionalTable.getFromRightArg(stmtIdx2));
	}

	TEST_METHOD(insert_getFromRightArg_stmts_differentType) {
		StatementType assignType = StatementType::ASSIGN_TYPE;
		StatementType readType = StatementType::READ_TYPE;
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		BidirectionalTableOneWaySet<StatementType, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<StatementType, StmtIndex>();
		bidirectionalTable.insert(assignType, stmtIdx1);
		bidirectionalTable.insert(readType, stmtIdx2);

		Assert::IsTrue(assignType == bidirectionalTable.getFromRightArg(stmtIdx1));
		Assert::IsTrue(readType == bidirectionalTable.getFromRightArg(stmtIdx2));
	}

	TEST_METHOD(insert_getFromRightArg_patternVarStmt_sameVar) {
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		BidirectionalTableOneWaySet<VarIndex, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx1);
		bidirectionalTable.insert(varIdx1, stmtIdx2);

		Assert::IsTrue(varIdx1 == bidirectionalTable.getFromRightArg(stmtIdx1));
		Assert::IsTrue(varIdx1 == bidirectionalTable.getFromRightArg(stmtIdx2));
	}

	TEST_METHOD(insert_getFromRightArg_patternVarStmt_differentVar) {
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		BidirectionalTableOneWaySet<VarIndex, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx1);
		bidirectionalTable.insert(varIdx2, stmtIdx2);

		Assert::IsTrue(varIdx1 == bidirectionalTable.getFromRightArg(stmtIdx1));
		Assert::IsTrue(varIdx2 == bidirectionalTable.getFromRightArg(stmtIdx2));
	}

	TEST_METHOD(insert_getFromRightArg_patternPostfixExprStmt_sameExpr) {
		std::string postfix = " a b + ";
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		BidirectionalTableOneWaySet<std::string, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<std::string, StmtIndex>();
		bidirectionalTable.insert(postfix, stmtIdx1);
		bidirectionalTable.insert(postfix, stmtIdx2);

		Assert::IsTrue(postfix == bidirectionalTable.getFromRightArg(stmtIdx1));
		Assert::IsTrue(postfix == bidirectionalTable.getFromRightArg(stmtIdx2));
	}

	TEST_METHOD(insert_getFromRightArg_patternPostfixExprStmt_differentExpr) {
		std::string postfix1 = " a b + ";
		std::string postfix2 = " c a b * + ";
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		BidirectionalTableOneWaySet<std::string, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<std::string, StmtIndex>();
		bidirectionalTable.insert(postfix1, stmtIdx1);
		bidirectionalTable.insert(postfix2, stmtIdx2);

		Assert::IsTrue(postfix1 == bidirectionalTable.getFromRightArg(stmtIdx1));
		Assert::IsTrue(postfix2 == bidirectionalTable.getFromRightArg(stmtIdx2));
	}

	TEST_METHOD(insert_getAll_procStmts_sameProc) {
		ProcIndex procIdx1 = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);

		std::vector<ProcIndex> expectedProcList{ procIdx1 , procIdx1 , procIdx1 };
		std::vector<StmtIndex> expectedStmtList{ stmtIdx1 , stmtIdx2, stmtIdx3 };
		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes = std::make_tuple(expectedProcList, expectedStmtList);

		BidirectionalTableOneWaySet<ProcIndex, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<ProcIndex, StmtIndex>();
		bidirectionalTable.insert(procIdx1, stmtIdx1);
		bidirectionalTable.insert(procIdx1, stmtIdx2);
		bidirectionalTable.insert(procIdx1, stmtIdx3);

		std::tuple<std::vector<ProcIndex>, std::vector<StmtIndex>> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_procStmts_differentProc) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);

		std::vector<ProcIndex> expectedProcList{ procIdx1 , procIdx1 , procIdx2 };
		std::vector<StmtIndex> expectedStmtList{ stmtIdx1 , stmtIdx2, stmtIdx3 };
		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes = std::make_tuple(expectedProcList, expectedStmtList);

		BidirectionalTableOneWaySet<ProcIndex, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<ProcIndex, StmtIndex>();
		bidirectionalTable.insert(procIdx1, stmtIdx1);
		bidirectionalTable.insert(procIdx1, stmtIdx2);
		bidirectionalTable.insert(procIdx2, stmtIdx3);

		std::tuple<std::vector<ProcIndex>, std::vector<StmtIndex>> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_stmts_sameType) {
		StatementType assignType = StatementType::ASSIGN_TYPE;
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);

		std::vector<StatementType> expectedStmtTypeList{ assignType , assignType , assignType };
		std::vector<StmtIndex> expectedStmtList{ stmtIdx1 , stmtIdx2, stmtIdx3 };
		std::tuple<std::vector<StatementType>, std::vector<StmtIndex>> expectedRes = std::make_tuple(expectedStmtTypeList, expectedStmtList);

		BidirectionalTableOneWaySet<StatementType, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<StatementType, StmtIndex>();
		bidirectionalTable.insert(assignType, stmtIdx1);
		bidirectionalTable.insert(assignType, stmtIdx2);
		bidirectionalTable.insert(assignType, stmtIdx3);

		std::tuple<std::vector<StatementType>, std::vector<StmtIndex>> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_stmts_differentType) {
		StatementType assignType = StatementType::ASSIGN_TYPE;
		StatementType readType = StatementType::READ_TYPE;
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);

		std::vector<StatementType> expectedStmtTypeList{ assignType , assignType , readType };
		std::vector<StmtIndex> expectedStmtList{ stmtIdx1 , stmtIdx2, stmtIdx3 };
		std::tuple<std::vector<StatementType>, std::vector<StmtIndex>> expectedRes = std::make_tuple(expectedStmtTypeList, expectedStmtList);

		BidirectionalTableOneWaySet<StatementType, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<StatementType, StmtIndex>();
		bidirectionalTable.insert(assignType, stmtIdx1);
		bidirectionalTable.insert(assignType, stmtIdx2);
		bidirectionalTable.insert(readType, stmtIdx3);

		std::tuple<std::vector<StatementType>, std::vector<StmtIndex>> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_patternVarStmt_sameVar) {
		VarIndex varIdx1 = VarIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);

		std::vector<VarIndex> expectedVarList{ varIdx1 , varIdx1 , varIdx1 };
		std::vector<StmtIndex> expectedStmtList{ stmtIdx1 , stmtIdx2, stmtIdx3 };
		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes = std::make_tuple(expectedVarList, expectedStmtList);

		BidirectionalTableOneWaySet<VarIndex, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx1);
		bidirectionalTable.insert(varIdx1, stmtIdx2);
		bidirectionalTable.insert(varIdx1, stmtIdx3);

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_patternVarStmt_differentVar) {
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);

		std::vector<VarIndex> expectedVarList{ varIdx1 , varIdx1 , varIdx2 };
		std::vector<StmtIndex> expectedStmtList{ stmtIdx1 , stmtIdx2, stmtIdx3 };
		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes = std::make_tuple(expectedVarList, expectedStmtList);

		BidirectionalTableOneWaySet<VarIndex, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx1);
		bidirectionalTable.insert(varIdx1, stmtIdx2);
		bidirectionalTable.insert(varIdx2, stmtIdx3);

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_patternPostfixExprStmt_sameExpr) {
		std::string postfix = " a b + ";
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);

		std::vector<std::string> expectedExprList{ postfix , postfix , postfix };
		std::vector<StmtIndex> expectedStmtList{ stmtIdx1 , stmtIdx2, stmtIdx3 };
		std::tuple<std::vector<std::string>, std::vector<StmtIndex>> expectedRes = std::make_tuple(expectedExprList, expectedStmtList);

		BidirectionalTableOneWaySet<std::string, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<std::string, StmtIndex>();
		bidirectionalTable.insert(postfix, stmtIdx1);
		bidirectionalTable.insert(postfix, stmtIdx2);
		bidirectionalTable.insert(postfix, stmtIdx3);

		std::tuple<std::vector<std::string>, std::vector<StmtIndex>> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_patternPostfixExprStmt_differentExpr) {
		std::string postfix1 = " a b + ";
		std::string postfix2 = " c a b * + ";
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);

		std::vector<std::string> expectedExprList{ postfix1 , postfix1 , postfix2 };
		std::vector<StmtIndex> expectedStmtList{ stmtIdx1 , stmtIdx2, stmtIdx3 };
		std::tuple<std::vector<std::string>, std::vector<StmtIndex>> expectedRes = std::make_tuple(expectedExprList, expectedStmtList);

		BidirectionalTableOneWaySet<std::string, StmtIndex> bidirectionalTable = BidirectionalTableOneWaySet<std::string, StmtIndex>();
		bidirectionalTable.insert(postfix1, stmtIdx1);
		bidirectionalTable.insert(postfix1, stmtIdx2);
		bidirectionalTable.insert(postfix2, stmtIdx3);

		std::tuple<std::vector<std::string>, std::vector<StmtIndex>> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}
	};
}
