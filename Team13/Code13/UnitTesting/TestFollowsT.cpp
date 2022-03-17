#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "../source/PKB/FollowsT.h"
#include "../source/PKB/ParentT.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestFollowsT) {
private:
	StmtIndex stmtIdx1 = { 1 };
	StmtIndex stmtIdx2 = { 2 };
	StmtIndex stmtIdx3 = { 3 };
	StmtIndex stmtIdx4 = { 4 };
	StmtIndex stmtIdx5 = { 5 };

	TEST_METHOD_CLEANUP(cleanUpFollowsT) {
		Follows::performCleanUp();
		FollowsT::performCleanUp();
	}

public:
	TEST_METHOD(populate_getSuccessors_branched) {
		std::vector<int> followsTExpAns{ stmtIdx2, stmtIdx3, stmtIdx4, stmtIdx5 };

		Follows::insert(stmtIdx1, stmtIdx2);
		Follows::insert(stmtIdx1, stmtIdx5);
		Follows::insert(stmtIdx2, stmtIdx3);
		Follows::insert(stmtIdx2, stmtIdx4);
		FollowsT::populate();

		auto followsTStmts = FollowsT::getSuccessors(stmtIdx1);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsTEmptyStmts = FollowsT::getSuccessors(stmtIdx5);
		Assert::IsTrue(0 == followsTEmptyStmts.size());

		/* Check ParentT data does not get affected */
		auto parentTStmts = ParentT::getSuccessors(stmtIdx1);
		Assert::IsTrue(0 == parentTStmts.size());
		ParentT::performCleanUp();
	};

	TEST_METHOD(populate_getSuccessors_linear) {
		std::vector<int> followsTExpAns{ stmtIdx1, stmtIdx2, stmtIdx3, stmtIdx4 };

		Follows::insert(stmtIdx1, stmtIdx2);
		Follows::insert(stmtIdx2, stmtIdx3);
		Follows::insert(stmtIdx3, stmtIdx4);
		Follows::insert(stmtIdx5, stmtIdx1);
		FollowsT::populate();

		auto followsTStmts = FollowsT::getSuccessors(stmtIdx5);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsTEmptyStmts = FollowsT::getSuccessors(stmtIdx4);
		Assert::IsTrue(0 == followsTEmptyStmts.size());

		/* Check ParentT data does not get affected */
		auto parentTStmts = ParentT::getSuccessors(stmtIdx5);
		Assert::IsTrue(0 == parentTStmts.size());
		ParentT::performCleanUp();
	};

	TEST_METHOD(populate_getPredecessors_branched) {
		std::vector<int> followsTExpAns{ stmtIdx4, stmtIdx2, stmtIdx1 };

		Follows::insert(stmtIdx1, stmtIdx2);
		Follows::insert(stmtIdx2, stmtIdx3);
		Follows::insert(stmtIdx2, stmtIdx4);
		Follows::insert(stmtIdx4, stmtIdx5);
		FollowsT::populate();

		auto followsTStmts = FollowsT::getPredecessors(stmtIdx5);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsTEmptyStmts = FollowsT::getPredecessors(stmtIdx1);
		Assert::IsTrue(0 == followsTEmptyStmts.size());

		/* Check ParentTs data does not get affected */
		auto parentTStmts = ParentT::getPredecessors(stmtIdx5);
		Assert::IsTrue(0 == parentTStmts.size());
		ParentT::performCleanUp();
	};

	TEST_METHOD(populate_getPredecessors_linear) {
		std::vector<int> followsTExpAns{ stmtIdx3, stmtIdx2, stmtIdx1, stmtIdx5 };

		Follows::insert(stmtIdx1, stmtIdx2);
		Follows::insert(stmtIdx2, stmtIdx3);
		Follows::insert(stmtIdx3, stmtIdx4);
		Follows::insert(stmtIdx5, stmtIdx1);
		FollowsT::populate();

		auto followsTStmts = FollowsT::getPredecessors(stmtIdx4);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsTEmptyStmts = FollowsT::getPredecessors(stmtIdx5);
		Assert::IsTrue(0 == followsTEmptyStmts.size());

		/* Check ParentTs data does not get affected */
		auto parentTStmts = ParentT::getPredecessors(stmtIdx4);
		Assert::IsTrue(0 == parentTStmts.size());
		ParentT::performCleanUp();
	};

	TEST_METHOD(containsSuccessor) {
		Follows::insert(stmtIdx1, stmtIdx2);
		Follows::insert(stmtIdx2, stmtIdx3);
		Follows::insert(stmtIdx2, stmtIdx4);
		FollowsT::populate();

		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx4));
		Assert::IsFalse(FollowsT::containsSuccessor(stmtIdx4, stmtIdx1));
		Assert::IsFalse(FollowsT::containsSuccessor(stmtIdx3, stmtIdx4)); /* siblings */
	};

	TEST_METHOD(containsPredecessor) {
		Follows::insert(stmtIdx1, stmtIdx2);
		Follows::insert(stmtIdx2, stmtIdx3);
		Follows::insert(stmtIdx2, stmtIdx4);
		FollowsT::populate();

		Assert::IsTrue(FollowsT::containsPredecessor(stmtIdx1, stmtIdx4));
		Assert::IsFalse(FollowsT::containsPredecessor(stmtIdx4, stmtIdx1));
		Assert::IsFalse(FollowsT::containsPredecessor(stmtIdx3, stmtIdx4)); /* siblings */
	};

	TEST_METHOD(getAllPredecessorSuccessorInfo) {
		std::vector<int> followsTpredecessors{ stmtIdx2, stmtIdx1, stmtIdx1 };
		std::vector<int> followsTsuccessors{ stmtIdx3, stmtIdx2, stmtIdx3 };
		std::tuple<std::vector<int>, std::vector<int>> followsTExpAns =
			std::make_tuple(followsTpredecessors, followsTsuccessors);

		Follows::insert(stmtIdx1, stmtIdx2);
		Follows::insert(stmtIdx2, stmtIdx3);
		FollowsT::populate();

		auto followsTInfo = FollowsT::getAllPredecessorSuccessorInfo();
		Assert::IsTrue(followsTExpAns == followsTInfo);
	};

	TEST_METHOD(getPredSucTable) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> followsTExpAns;
		followsTExpAns[stmtIdx1].insert(stmtIdx2);
		followsTExpAns[stmtIdx1].insert(stmtIdx3);
		followsTExpAns[stmtIdx2].insert(stmtIdx3);

		Follows::insert(stmtIdx1, stmtIdx2);
		Follows::insert(stmtIdx2, stmtIdx3);
		FollowsT::populate();

		auto followsTTable = FollowsT::getPredSucTable();
		Assert::IsTrue(followsTExpAns == followsTTable);
	};
	};
};
