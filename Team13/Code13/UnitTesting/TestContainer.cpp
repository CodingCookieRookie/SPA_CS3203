#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "../source/PKB/Container.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestContainer) {
private:
	StmtIndex stmtIdx1 = StmtIndex(1);
	StmtIndex stmtIdx2 = StmtIndex(2);
	StmtIndex stmtIdx3 = StmtIndex(3);
	StmtIndex stmtIdx4 = StmtIndex(4);
	StmtIndex stmtIdx5 = StmtIndex(5);
	StmtIndex stmtIdx6 = StmtIndex(6);

	TEST_METHOD_CLEANUP(cleanUpContainerInfo) {
		Container::performCleanUp();
	}

public:

	TEST_METHOD(insertStmtInContainer_getStmtsInContainer) {
		std::unordered_set<StmtIndex> expectedRes;
		expectedRes.insert(stmtIdx2);

		Container::insertStmtInContainer(stmtIdx1, stmtIdx2);
		Container::insertStmtInContainer(stmtIdx2, stmtIdx3);

		std::unordered_set<StmtIndex> res = Container::getStmtsInContainer(stmtIdx1);
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertStmtInContainer_getStmtsInContainer_noContainedStmt) {
		std::unordered_set<StmtIndex> expectedRes;
		std::unordered_set<StmtIndex> res = Container::getStmtsInContainer(stmtIdx1);
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertStmtInContainer_getContainersOfStmt) {
		std::unordered_set<StmtIndex> expectedRes;
		expectedRes.insert(stmtIdx1);
		expectedRes.insert(stmtIdx3);

		Container::insertStmtInContainer(stmtIdx1, stmtIdx2);
		Container::insertStmtInContainer(stmtIdx4, stmtIdx3);
		Container::insertStmtInContainer(stmtIdx3, stmtIdx2);

		std::unordered_set<StmtIndex> res = Container::getContainersOfStmt(stmtIdx2);
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertStmtInContainer_getContainersOfStmt_noContainerStmt) {
		std::unordered_set<StmtIndex> expectedRes;

		Container::insertStmtInContainer(stmtIdx2, stmtIdx3);

		std::unordered_set<StmtIndex> res = Container::getContainersOfStmt(stmtIdx2);
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(populate_getStmtsInContainer_branched) {
		std::unordered_set<StmtIndex> expectedRes;
		expectedRes.insert(stmtIdx2);
		expectedRes.insert(stmtIdx5);

		Container::insertStmtInContainer(stmtIdx1, stmtIdx2);
		Container::insertStmtInContainer(stmtIdx1, stmtIdx5);
		Container::insertStmtInContainer(stmtIdx2, stmtIdx3);
		Container::insertStmtInContainer(stmtIdx2, stmtIdx4);

		std::unordered_set<StmtIndex> res1 = Container::getStmtsInContainer(stmtIdx1);
		Assert::IsTrue(expectedRes == res1);

		expectedRes.insert(stmtIdx3);
		expectedRes.insert(stmtIdx4);

		Container::populate();

		std::unordered_set<StmtIndex> res2 = Container::getStmtsInContainer(stmtIdx1);
		Assert::IsTrue(expectedRes == res2);
	};

	TEST_METHOD(populate_getStmtsInContainer_linear) {
		std::unordered_set<StmtIndex> expectedRes;
		expectedRes.insert(stmtIdx4);

		Container::insertStmtInContainer(stmtIdx2, stmtIdx3);
		Container::insertStmtInContainer(stmtIdx3, stmtIdx4);
		Container::insertStmtInContainer(stmtIdx4, stmtIdx5);
		Container::insertStmtInContainer(stmtIdx5, stmtIdx1);

		std::unordered_set<StmtIndex> res1 = Container::getStmtsInContainer(stmtIdx3);
		Assert::IsTrue(expectedRes == res1);

		expectedRes.insert(stmtIdx1);
		expectedRes.insert(stmtIdx5);

		Container::populate();

		std::unordered_set<StmtIndex> res2 = Container::getStmtsInContainer(stmtIdx3);
		Assert::IsTrue(expectedRes == res2);
	};

	TEST_METHOD(populate_getContainersOfStmt_branched) {
		std::unordered_set<StmtIndex> expectedRes;
		expectedRes.insert(stmtIdx5);

		Container::insertStmtInContainer(stmtIdx1, stmtIdx2);
		Container::insertStmtInContainer(stmtIdx2, stmtIdx3);
		Container::insertStmtInContainer(stmtIdx5, stmtIdx6);
		Container::insertStmtInContainer(stmtIdx2, stmtIdx5);
		Container::insertStmtInContainer(stmtIdx3, stmtIdx4);

		std::unordered_set<StmtIndex> res1 = Container::getContainersOfStmt(stmtIdx6);
		Assert::IsTrue(expectedRes == res1);

		expectedRes.insert(stmtIdx2);
		expectedRes.insert(stmtIdx1);

		Container::populate();

		std::unordered_set<StmtIndex> res2 = Container::getContainersOfStmt(stmtIdx6);
		Assert::IsTrue(expectedRes == res2);
	};

	TEST_METHOD(populate_getContainersOfStmt_linear) {
		std::unordered_set<StmtIndex> expectedRes;
		expectedRes.insert(stmtIdx3);

		Container::insertStmtInContainer(stmtIdx1, stmtIdx2);
		Container::insertStmtInContainer(stmtIdx2, stmtIdx5);
		Container::insertStmtInContainer(stmtIdx5, stmtIdx4);
		Container::insertStmtInContainer(stmtIdx4, stmtIdx3);
		Container::insertStmtInContainer(stmtIdx3, stmtIdx6);

		std::unordered_set<StmtIndex> res1 = Container::getContainersOfStmt(stmtIdx6);
		Assert::IsTrue(expectedRes == res1);

		expectedRes.insert(stmtIdx4);
		expectedRes.insert(stmtIdx5);
		expectedRes.insert(stmtIdx2);
		expectedRes.insert(stmtIdx1);

		Container::populate();

		std::unordered_set<StmtIndex> res2 = Container::getContainersOfStmt(stmtIdx6);
		Assert::IsTrue(expectedRes == res2);
	};

	TEST_METHOD(performCleanUp) {
		std::unordered_set<StmtIndex> expectedRes;
		expectedRes.insert(stmtIdx2);

		Container::insertStmtInContainer(stmtIdx1, stmtIdx2);
		std::unordered_set<StmtIndex> res1 = Container::getStmtsInContainer(stmtIdx1);
		Assert::IsTrue(expectedRes == res1);

		Container::performCleanUp();

		expectedRes.clear();
		std::unordered_set<StmtIndex> res2 = Container::getStmtsInContainer(stmtIdx1);
		Assert::IsTrue(expectedRes == res2);
	}
	};
}
