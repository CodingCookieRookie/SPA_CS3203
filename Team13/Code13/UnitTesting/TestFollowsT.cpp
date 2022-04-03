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
	TEST_METHOD(populate_getFromLeftArg_branched) {
		std::vector<StmtIndex> followsTExpAns{ stmtIdx2, stmtIdx3, stmtIdx4, stmtIdx5 };

		Follows::insert(stmtIdx1, stmtIdx2);
		Follows::insert(stmtIdx1, stmtIdx5);
		Follows::insert(stmtIdx2, stmtIdx3);
		Follows::insert(stmtIdx2, stmtIdx4);
		FollowsT::populate();

		auto followsTStmts = FollowsT::getFromLeftArg(stmtIdx1);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsTEmptyStmts = FollowsT::getFromLeftArg(stmtIdx5);
		Assert::IsTrue(0 == followsTEmptyStmts.size());

		/* Check ParentT data does not get affected */
		auto parentTStmts = ParentT::getFromLeftArg(stmtIdx1);
		Assert::IsTrue(0 == parentTStmts.size());
		ParentT::performCleanUp();
	};

	TEST_METHOD(populate_getFromLeftArg_linear) {
		std::vector<StmtIndex> followsTExpAns{ stmtIdx1, stmtIdx2, stmtIdx3, stmtIdx4 };

		Follows::insert(stmtIdx1, stmtIdx2);
		Follows::insert(stmtIdx2, stmtIdx3);
		Follows::insert(stmtIdx3, stmtIdx4);
		Follows::insert(stmtIdx5, stmtIdx1);
		FollowsT::populate();

		auto followsTStmts = FollowsT::getFromLeftArg(stmtIdx5);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsTEmptyStmts = FollowsT::getFromLeftArg(stmtIdx4);
		Assert::IsTrue(0 == followsTEmptyStmts.size());

		/* Check ParentT data does not get affected */
		auto parentTStmts = ParentT::getFromLeftArg(stmtIdx5);
		Assert::IsTrue(0 == parentTStmts.size());
		ParentT::performCleanUp();
	};

	TEST_METHOD(populate_getFromRightArg_branched) {
		std::vector<StmtIndex> followsTExpAns{ stmtIdx4, stmtIdx2, stmtIdx1 };

		Follows::insert(stmtIdx1, stmtIdx2);
		Follows::insert(stmtIdx2, stmtIdx3);
		Follows::insert(stmtIdx2, stmtIdx4);
		Follows::insert(stmtIdx4, stmtIdx5);
		FollowsT::populate();

		auto followsTStmts = FollowsT::getFromRightArg(stmtIdx5);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsTEmptyStmts = FollowsT::getFromRightArg(stmtIdx1);
		Assert::IsTrue(0 == followsTEmptyStmts.size());

		/* Check ParentTs data does not get affected */
		auto parentTStmts = ParentT::getFromRightArg(stmtIdx5);
		Assert::IsTrue(0 == parentTStmts.size());
		ParentT::performCleanUp();
	};

	TEST_METHOD(populate_getFromRightArg_linear) {
		std::vector<StmtIndex> followsTExpAns{ stmtIdx3, stmtIdx2, stmtIdx1, stmtIdx5 };

		Follows::insert(stmtIdx1, stmtIdx2);
		Follows::insert(stmtIdx2, stmtIdx3);
		Follows::insert(stmtIdx3, stmtIdx4);
		Follows::insert(stmtIdx5, stmtIdx1);
		FollowsT::populate();

		auto followsTStmts = FollowsT::getFromRightArg(stmtIdx4);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsTEmptyStmts = FollowsT::getFromRightArg(stmtIdx5);
		Assert::IsTrue(0 == followsTEmptyStmts.size());

		/* Check ParentTs data does not get affected */
		auto parentTStmts = ParentT::getFromRightArg(stmtIdx4);
		Assert::IsTrue(0 == parentTStmts.size());
		ParentT::performCleanUp();
	};

	TEST_METHOD(contains) {
		Follows::insert(stmtIdx1, stmtIdx2);
		Follows::insert(stmtIdx2, stmtIdx3);
		Follows::insert(stmtIdx2, stmtIdx4);
		FollowsT::populate();

		Assert::IsTrue(FollowsT::contains(stmtIdx1, stmtIdx4));
		Assert::IsFalse(FollowsT::contains(stmtIdx4, stmtIdx1));
		Assert::IsFalse(FollowsT::contains(stmtIdx3, stmtIdx4)); /* siblings */
	};

	TEST_METHOD(getAllInfo) {
		std::vector<StmtIndex> followsTpredecessors{ stmtIdx2, stmtIdx1, stmtIdx1 };
		std::vector<StmtIndex> followsTsuccessors{ stmtIdx3, stmtIdx2, stmtIdx3 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> followsTExpAns =
			std::make_tuple(followsTpredecessors, followsTsuccessors);

		Follows::insert(stmtIdx1, stmtIdx2);
		Follows::insert(stmtIdx2, stmtIdx3);
		FollowsT::populate();

		auto followsTInfo = FollowsT::getAllInfo();
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
