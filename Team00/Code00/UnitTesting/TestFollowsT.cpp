#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/common/Types.h"
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

	TEST_METHOD_CLEANUP(cleanUpFollowsT) {
		FollowsT::performCleanUp();
	}

public:
	TEST_METHOD(populate_getSuccessorStmts_branched) {
		std::vector<int> followsTExpAns{ stmt2.index, stmt3.index, stmt4.index, stmt5.index };

		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2, stmt5 };
		uPredSucTable[stmt2] = { stmt3, stmt4 };

		FollowsT::populate(uPredSucTable);

		auto followsTStmts = FollowsT::getSuccessorStmts(stmt1);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsTEmptyStmts = FollowsT::getSuccessorStmts(stmt5);
		Assert::IsTrue(0 == followsTEmptyStmts.size());

		/*Check ParentT data does not get affected*/
		auto parentTStmts = ParentT::getSuccessorStmts(stmt1);
		Assert::IsTrue(0 == parentTStmts.size());
		ParentT::performCleanUp();
	};

	TEST_METHOD(populate_getSuccessorStmts_linear) {
		std::vector<int> followsTExpAns{ stmt1.index, stmt2.index, stmt3.index, stmt4.index };

		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2 };
		uPredSucTable[stmt2] = { stmt3 };
		uPredSucTable[stmt3] = { stmt4 };
		uPredSucTable[stmt5] = { stmt1 };

		FollowsT::populate(uPredSucTable);

		auto followsTStmts = FollowsT::getSuccessorStmts(stmt5);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsTEmptyStmts = FollowsT::getSuccessorStmts(stmt4);
		Assert::IsTrue(0 == followsTEmptyStmts.size());

		/*Check ParentT data does not get affected*/
		auto parentTStmts = ParentT::getSuccessorStmts(stmt5);
		Assert::IsTrue(0 == parentTStmts.size());
		ParentT::performCleanUp();
	};

	TEST_METHOD(populate_getPredecessorStmts_branched) {
		std::vector<int> followsTExpAns{ stmt4.index, stmt2.index, stmt1.index };

		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2 };
		uPredSucTable[stmt2] = { stmt3, stmt4 };
		uPredSucTable[stmt4] = { stmt5 };

		FollowsT::populate(uPredSucTable);

		auto followsTStmts = FollowsT::getPredecessorStmts(stmt5);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsTEmptyStmts = FollowsT::getPredecessorStmts(stmt1);
		Assert::IsTrue(0 == followsTEmptyStmts.size());

		/*Check ParentTs data does not get affected*/
		auto parentTStmts = ParentT::getPredecessorStmts(stmt5);
		Assert::IsTrue(0 == parentTStmts.size());
		ParentT::performCleanUp();
	};

	TEST_METHOD(populate_getPredecessorStmts_linear) {
		std::vector<int> followsTExpAns{ stmt3.index, stmt2.index, stmt1.index, stmt5.index };

		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2 };
		uPredSucTable[stmt2] = { stmt3 };
		uPredSucTable[stmt3] = { stmt4 };
		uPredSucTable[stmt5] = { stmt1 };

		FollowsT::populate(uPredSucTable);

		auto followsTStmts = FollowsT::getPredecessorStmts(stmt4);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsTEmptyStmts = FollowsT::getPredecessorStmts(stmt5);
		Assert::IsTrue(0 == followsTEmptyStmts.size());

		/*Check ParentTs data does not get affected*/
		auto parentTStmts = ParentT::getPredecessorStmts(stmt4);
		Assert::IsTrue(0 == parentTStmts.size());
		ParentT::performCleanUp();
	};

	TEST_METHOD(containsSuccessor) {
		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2 };
		uPredSucTable[stmt2] = { stmt3, stmt4 };

		FollowsT::populate(uPredSucTable);

		Assert::IsTrue(FollowsT::containsSuccessor(stmt1, stmt4));
		Assert::IsFalse(FollowsT::containsSuccessor(stmt4, stmt1));
		Assert::IsFalse(FollowsT::containsSuccessor(stmt3, stmt4)); /*siblings*/
	};

	TEST_METHOD(containsPredecessor) {
		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2 };
		uPredSucTable[stmt2] = { stmt3, stmt4 };

		FollowsT::populate(uPredSucTable);

		Assert::IsTrue(FollowsT::containsPredecessor(stmt1, stmt4));
		Assert::IsFalse(FollowsT::containsPredecessor(stmt4, stmt1));
		Assert::IsFalse(FollowsT::containsPredecessor(stmt3, stmt4)); /*siblings*/
	};

	TEST_METHOD(getAllPredecessorSuccessorInfo) {
		std::vector<int> followsTpredecessors{ stmt2.index, stmt1.index, stmt1.index };
		std::vector<int> followsTsuccessors{ stmt3.index, stmt2.index, stmt3.index };
		std::tuple<std::vector<int>, std::vector<int>> followsTExpAns =
			std::make_tuple(followsTpredecessors, followsTsuccessors);

		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2 };
		uPredSucTable[stmt2] = { stmt3 };

		FollowsT::populate(uPredSucTable);

		auto followsTInfo = FollowsT::getAllPredecessorSuccessorInfo();

		Assert::IsTrue(followsTExpAns == followsTInfo);
	};

	TEST_METHOD(getPredSucTable) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
			StmtIndex::HashFunction> followsTExpAns;
		followsTExpAns[stmt1].insert(stmt2);
		followsTExpAns[stmt1].insert(stmt3);
		followsTExpAns[stmt2].insert(stmt3);

		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2 };
		uPredSucTable[stmt2] = { stmt3 };

		FollowsT::populate(uPredSucTable);

		auto followsTTable = FollowsT::getPredSucTable();

		Assert::IsTrue(followsTExpAns == followsTTable);
	};

	TEST_METHOD(getSucPredTable) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
			StmtIndex::HashFunction> followsTExpAns;
		followsTExpAns[stmt2].insert(stmt1);
		followsTExpAns[stmt3].insert(stmt1);
		followsTExpAns[stmt3].insert(stmt2);

		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2 };
		uPredSucTable[stmt2] = { stmt3 };

		FollowsT::populate(uPredSucTable);

		auto followsTTable = FollowsT::getSucPredTable();

		Assert::IsTrue(followsTExpAns == followsTTable);
	};
	};
};
