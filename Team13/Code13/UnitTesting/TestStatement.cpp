#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/Statement.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestStatement) {
private:
	StatementType assignType = StatementType::ASSIGN_TYPE;
	StatementType whileType = StatementType::WHILE_TYPE;
	StatementType callType = StatementType::CALL_TYPE;
	StatementType readType = StatementType::READ_TYPE;
	StatementType printType = StatementType::PRINT_TYPE;
	StatementType ifType = StatementType::IF_TYPE;

	StmtIndex stmtIdx1 = StmtIndex(1);
	StmtIndex stmtIdx2 = StmtIndex(2);
	StmtIndex stmtIdx3 = StmtIndex(3);
public:
	TEST_METHOD(insertStmt_containsStmt_stmtExists) {
		Statement statement = Statement();

		std::string callStmtNameAttribute = "Peter";

		StmtIndex idx1 = statement.insertStmt(assignType);
		StmtIndex idx2 = statement.insertStmt(callType, callStmtNameAttribute);

		bool res1 = statement.containsStmt(1);
		bool res2 = statement.containsStmt(2);
		Assert::IsTrue(res1);
		Assert::IsTrue(res2);
	}

	TEST_METHOD(insertStmt_containsStmt_stmtDoesNotExist) {
		Statement statement = Statement();

		StmtIndex idx = statement.insertStmt(assignType);

		bool res = statement.containsStmt(2);
		Assert::IsFalse(res);
	}

	TEST_METHOD(insertStmt_differentStmts) {
		Statement statement = Statement();

		StmtIndex res1 = statement.insertStmt(assignType);
		StmtIndex res2 = statement.insertStmt(whileType);

		Assert::IsTrue(stmtIdx1 == res1);
		Assert::IsTrue(stmtIdx2 == res2);
	}

	TEST_METHOD(insertStmt_getStmtIdxFromType_differentStmtTypes) {
		Statement statement = Statement();

		std::vector<StmtIndex> expectedRes1 = { stmtIdx1 };
		std::vector<StmtIndex> expectedRes2 = { stmtIdx2 };

		statement.insertStmt(assignType);
		statement.insertStmt(whileType);

		std::vector<StmtIndex> res1 = statement.getStmtIdxFromType(assignType);
		std::vector<StmtIndex> res2 = statement.getStmtIdxFromType(whileType);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes2 == res2);
	}

	TEST_METHOD(insertStmt_getStmtIdxFromType_sameStmtType) {
		Statement statement = Statement();

		std::vector<StmtIndex> expectedRes = { stmtIdx1, stmtIdx2 };

		statement.insertStmt(assignType);
		statement.insertStmt(assignType);

		std::vector<StmtIndex> res = statement.getStmtIdxFromType(assignType);
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertStmt_getTypeFromStmtIdx) {
		Statement statement = Statement();

		std::string callStmtNameAttribute = "Peter";
		std::string readStmtNameAttribute = "x";
		std::string printStmtNameAttribute = "y";

		StmtIndex stmtIdx1 = statement.insertStmt(assignType);
		StmtIndex stmtIdx2 = statement.insertStmt(callType, callStmtNameAttribute);
		StmtIndex stmtIdx3 = statement.insertStmt(ifType);
		StmtIndex stmtIdx4 = statement.insertStmt(printType, printStmtNameAttribute);
		StmtIndex stmtIdx5 = statement.insertStmt(readType, readStmtNameAttribute);
		StmtIndex stmtIdx6 = statement.insertStmt(whileType);

		StatementType res1 = statement.getTypeFromStmtIdx(stmtIdx1);
		StatementType res2 = statement.getTypeFromStmtIdx(stmtIdx2);
		StatementType res3 = statement.getTypeFromStmtIdx(stmtIdx3);
		StatementType res4 = statement.getTypeFromStmtIdx(stmtIdx4);
		StatementType res5 = statement.getTypeFromStmtIdx(stmtIdx5);
		StatementType res6 = statement.getTypeFromStmtIdx(stmtIdx6);

		Assert::IsTrue(assignType == res1);
		Assert::IsTrue(callType == statement.getTypeFromStmtIdx(stmtIdx2));
		Assert::IsTrue(ifType == statement.getTypeFromStmtIdx(stmtIdx3));
		Assert::IsTrue(printType == statement.getTypeFromStmtIdx(stmtIdx4));
		Assert::IsTrue(readType == statement.getTypeFromStmtIdx(stmtIdx5));
		Assert::IsTrue(whileType == statement.getTypeFromStmtIdx(stmtIdx6));
	}

	TEST_METHOD(insertStmt_getAllStmts_differentStmts) {
		Statement statement = Statement();

		std::vector<StmtIndex> expectedRes = { stmtIdx1 , stmtIdx2 };

		statement.insertStmt(assignType);
		statement.insertStmt(whileType);

		std::vector<StmtIndex> res = statement.getAllStmts();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertStmt_getAllContainerStmts_noContainerStmts) {
		Statement statement = Statement();

		statement.insertStmt(assignType);
		statement.insertStmt(assignType);

		std::vector<StmtIndex> res = statement.getAllContainerStmts();
		Assert::IsTrue(0 == res.size());
	}

	TEST_METHOD(insertStmt_isContainerStmt_allContainerStmts) {
		Statement statement = Statement();

		std::vector<StmtIndex> expectedRes = { stmtIdx1 , stmtIdx2 , stmtIdx3 };

		statement.insertStmt(ifType);
		statement.insertStmt(whileType);
		statement.insertStmt(ifType);

		std::vector<StmtIndex> res = statement.getAllContainerStmts();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertStmt_isContainerStmt_mixedStmts) {
		Statement statement = Statement();

		std::vector<StmtIndex> expectedRes = { stmtIdx2 , stmtIdx3 };

		statement.insertStmt(assignType);
		statement.insertStmt(whileType);
		statement.insertStmt(ifType);

		std::vector<StmtIndex> res = statement.getAllContainerStmts();
		Assert::IsTrue(expectedRes == res);
	}
	};
}
