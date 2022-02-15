#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/common/Types.h"
#include "../source/PKB/Follows.h"
#include "../source/PKB/FollowsT.h"
#include "../source/PKB/ParentT.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestFollowsT) {
private:
	StmtIndex stmt1 = { 1 };
	StmtIndex stmt2 = { 2 };
	StmtIndex stmt3 = { 3 };
	StmtIndex stmt4 = { 4 };
	StmtIndex stmt5 = { 5 };

	TEST_METHOD_CLEANUP(cleanUpFollowsFollowsT) {
		FollowsT::performCleanUp();
		Follows::performCleanUp();
	}

public:
	TEST_METHOD(populate_getSuccessorStmts_branched) {
		std::vector<int> followsTExpAns{ stmt2.index, stmt3.index, stmt4.index, stmt5.index };
		std::vector<int> followsExpAns{ stmt2.index, stmt5.index };

		Follows::insert(stmt1, stmt2);
		Follows::insert(stmt2, stmt3);
		Follows::insert(stmt2, stmt4);
		Follows::insert(stmt1, stmt5);
		FollowsT::populate();

		auto followsTStmts = FollowsT::getSuccessorStmts(stmt1);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsStmts = Follows::getSuccessorStmts(stmt1);
		Assert::IsTrue(followsExpAns == followsStmts);

		auto followsTEmptyStmts = FollowsT::getSuccessorStmts(stmt5);
		Assert::IsTrue(0 == followsTEmptyStmts.size());

		/*Check ParentT data does not get affected*/
		auto parentTStmts = ParentT::getSuccessorStmts(stmt1);
		Assert::IsTrue(0 == parentTStmts.size());
		ParentT::performCleanUp();
	};

	TEST_METHOD(populate_getSuccessorStmts_linear) {
		std::vector<int> followsTExpAns{ stmt1.index, stmt2.index, stmt3.index, stmt4.index };
		std::vector<int> followsExpAns{ stmt1.index };

		Follows::insert(stmt1, stmt2);
		Follows::insert(stmt2, stmt3);
		Follows::insert(stmt3, stmt4);
		Follows::insert(stmt5, stmt1);
		FollowsT::populate();

		auto followsTStmts = FollowsT::getSuccessorStmts(stmt5);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsStmts = Follows::getSuccessorStmts(stmt5);
		Assert::IsTrue(followsExpAns == followsStmts);

		auto followsTEmptyStmts = FollowsT::getSuccessorStmts(stmt4);
		Assert::IsTrue(0 == followsTEmptyStmts.size());

		/*Check ParentT data does not get affected*/
		auto parentTStmts = ParentT::getSuccessorStmts(stmt5);
		Assert::IsTrue(0 == parentTStmts.size());
		ParentT::performCleanUp();
	};

	TEST_METHOD(populate_getPredecessorStmts_branched) {
		std::vector<int> followsTExpAns{ stmt1.index, stmt2.index, stmt4.index };
		std::vector<int> followsExpAns{ stmt4.index };

		Follows::insert(stmt1, stmt2);
		Follows::insert(stmt2, stmt3);
		Follows::insert(stmt2, stmt4);
		Follows::insert(stmt4, stmt5);
		FollowsT::populate();

		auto followsTStmts = FollowsT::getPredecessorStmts(stmt5);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsStmts = Follows::getPredecessorStmts(stmt5);
		Assert::IsTrue(followsExpAns == followsStmts);

		auto followsTEmptyStmts = FollowsT::getPredecessorStmts(stmt1);
		Assert::IsTrue(0 == followsTEmptyStmts.size());

		/*Check ParentTs data does not get affected*/
		auto parentTStmts = ParentT::getPredecessorStmts(stmt5);
		Assert::IsTrue(0 == parentTStmts.size());
		ParentT::performCleanUp();
	};

	TEST_METHOD(populate_getPredecessorStmts_linear) {
		std::vector<int> followsTExpAns{ stmt1.index, stmt2.index, stmt3.index, stmt5.index };
		std::vector<int> followsExpAns{ stmt3.index };

		Follows::insert(stmt1, stmt2);
		Follows::insert(stmt2, stmt3);
		Follows::insert(stmt3, stmt4);
		Follows::insert(stmt5, stmt1);
		FollowsT::populate();

		auto followsTStmts = FollowsT::getPredecessorStmts(stmt4);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsStmts = Follows::getPredecessorStmts(stmt4);
		Assert::IsTrue(followsExpAns == followsStmts);

		auto followsTEmptyStmts = FollowsT::getPredecessorStmts(stmt5);
		Assert::IsTrue(0 == followsTEmptyStmts.size());

		/*Check ParentTs data does not get affected*/
		auto parentTStmts = ParentT::getPredecessorStmts(stmt4);
		Assert::IsTrue(0 == parentTStmts.size());
		ParentT::performCleanUp();
	};

	TEST_METHOD(containsSuccessor) {
		Follows::insert(stmt1, stmt2);
		Follows::insert(stmt2, stmt3);
		Follows::insert(stmt2, stmt4);
		FollowsT::populate();

		Assert::IsTrue(FollowsT::containsSuccessor(stmt1, stmt4));
		Assert::IsFalse(FollowsT::containsSuccessor(stmt4, stmt1));
		Assert::IsFalse(FollowsT::containsSuccessor(stmt3, stmt4)); /*siblings*/

		Assert::IsTrue(Follows::containsSuccessor(stmt1, stmt2));
		Assert::IsFalse(Follows::containsSuccessor(stmt1, stmt4));
	};

	TEST_METHOD(containsPredecessor) {
		Follows::insert(stmt1, stmt2);
		Follows::insert(stmt2, stmt3);
		Follows::insert(stmt2, stmt4);
		FollowsT::populate();

		Assert::IsTrue(FollowsT::containsPredecessor(stmt1, stmt4));
		Assert::IsFalse(FollowsT::containsPredecessor(stmt4, stmt1));
		Assert::IsFalse(FollowsT::containsPredecessor(stmt3, stmt4)); /*siblings*/

		Assert::IsTrue(Follows::containsPredecessor(stmt1, stmt2));
		Assert::IsFalse(Follows::containsPredecessor(stmt1, stmt4));
	};

	TEST_METHOD(getAllPredecessorSuccessorInfo) {
		std::vector<int> followsTpredecessors{ stmt1.index, stmt1.index, stmt2.index };
		std::vector<int> followsTsuccessors{ stmt2.index, stmt3.index, stmt3.index };
		std::tuple<std::vector<int>, std::vector<int>> followsTExpAns =
			std::make_tuple(followsTpredecessors, followsTsuccessors);

		std::vector<int> followsPredecessors{ stmt1.index, stmt2.index };
		std::vector<int> followsSuccessors{ stmt2.index, stmt3.index };
		std::tuple<std::vector<int>, std::vector<int>> followsExpAns =
			std::make_tuple(followsPredecessors, followsSuccessors);

		Follows::insert(stmt1, stmt2);
		Follows::insert(stmt2, stmt3);
		FollowsT::populate();

		auto followsTInfo = FollowsT::getAllPredecessorSuccessorInfo();
		auto followsInfo = Follows::getAllPredecessorSuccessorInfo();

		Assert::IsTrue(followsTExpAns == followsTInfo);
		Assert::IsTrue(followsExpAns == followsInfo);
	};

	TEST_METHOD(getPredSucTable) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
			StmtIndex::HashFunction> followsTExpAns;
		followsTExpAns[stmt1].insert(stmt2);
		followsTExpAns[stmt1].insert(stmt3);
		followsTExpAns[stmt2].insert(stmt3);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
			StmtIndex::HashFunction> followsExpAns;
		followsExpAns[stmt1].insert(stmt2);
		followsExpAns[stmt2].insert(stmt3);

		Follows::insert(stmt1, stmt2);
		Follows::insert(stmt2, stmt3);
		FollowsT::populate();

		auto followsTTable = FollowsT::getPredSucTable();
		auto followsTable = Follows::getPredSucTable();

		Assert::IsTrue(followsTExpAns == followsTTable);
		Assert::IsTrue(followsExpAns == followsTable);
	};

	TEST_METHOD(getSucPredTable) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
			StmtIndex::HashFunction> followsTExpAns;
		followsTExpAns[stmt2].insert(stmt1);
		followsTExpAns[stmt3].insert(stmt1);
		followsTExpAns[stmt3].insert(stmt2);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
			StmtIndex::HashFunction> followsExpAns;
		followsExpAns[stmt2].insert(stmt1);
		followsExpAns[stmt3].insert(stmt2);

		Follows::insert(stmt1, stmt2);
		Follows::insert(stmt2, stmt3);
		FollowsT::populate();

		auto followsTTable = FollowsT::getSucPredTable();
		auto followsTable = Follows::getSucPredTable();

		Assert::IsTrue(followsTExpAns == followsTTable);
		Assert::IsTrue(followsExpAns == followsTable);
	};
	};
};
