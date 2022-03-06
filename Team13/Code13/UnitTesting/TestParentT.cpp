#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "../source/PKB/FollowsT.h"
#include "../source/PKB/ParentT.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestParentT) {
private:
	StmtIndex stmt1 = { 1 };
	StmtIndex stmt2 = { 2 };
	StmtIndex stmt3 = { 3 };
	StmtIndex stmt4 = { 4 };
	StmtIndex stmt5 = { 5 };

	TEST_METHOD_CLEANUP(cleanUpParentT) {
		ParentT::performCleanUp();
	}

public:
	TEST_METHOD(populate_getSuccessorStmts_branched) {
		std::vector<int> parentTExpAns{ stmt2.index, stmt3.index, stmt4.index, stmt5.index };

		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2, stmt5 };
		uPredSucTable[stmt2] = { stmt3, stmt4 };

		ParentT::populate(uPredSucTable);

		auto parentTStmts = ParentT::getSuccessorStmts(stmt1);
		Assert::IsTrue(parentTExpAns == parentTStmts);

		auto parentTEmptyStmts = ParentT::getSuccessorStmts(stmt5);
		Assert::IsTrue(0 == parentTEmptyStmts.size());

		/* Check ParentT data does not get affected */
		auto followsTStmts = FollowsT::getSuccessorStmts(stmt1);
		Assert::IsTrue(0 == followsTStmts.size());
		FollowsT::performCleanUp();
	};

	TEST_METHOD(populate_getSuccessorStmts_linear) {
		std::vector<int> parentTExpAns{ stmt1.index, stmt2.index, stmt3.index, stmt4.index };

		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2 };
		uPredSucTable[stmt2] = { stmt3 };
		uPredSucTable[stmt3] = { stmt4 };
		uPredSucTable[stmt5] = { stmt1 };

		ParentT::populate(uPredSucTable);

		auto parentTStmts = ParentT::getSuccessorStmts(stmt5);
		Assert::IsTrue(parentTExpAns == parentTStmts);

		auto parentTEmptyStmts = ParentT::getSuccessorStmts(stmt4);
		Assert::IsTrue(0 == parentTEmptyStmts.size());

		/* Check ParentT data does not get affected */
		auto followsTStmts = FollowsT::getSuccessorStmts(stmt5);
		Assert::IsTrue(0 == followsTStmts.size());
		FollowsT::performCleanUp();
	};

	TEST_METHOD(populate_getPredecessorStmts_branched) {
		std::vector<int> parentTExpAns{ stmt4.index, stmt2.index, stmt1.index };

		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2 };
		uPredSucTable[stmt2] = { stmt3, stmt4 };
		uPredSucTable[stmt4] = { stmt5 };

		ParentT::populate(uPredSucTable);

		auto parentTStmts = ParentT::getPredecessorStmts(stmt5);
		Assert::IsTrue(parentTExpAns == parentTStmts);

		auto parentTEmptyStmts = ParentT::getPredecessorStmts(stmt1);
		Assert::IsTrue(0 == parentTEmptyStmts.size());

		/* Check ParentT data does not get affected */
		auto followsTStmts = FollowsT::getSuccessorStmts(stmt5);
		Assert::IsTrue(0 == followsTStmts.size());
		FollowsT::performCleanUp();
	};

	TEST_METHOD(populate_getPredecessorStmts_linear) {
		std::vector<int> parentTExpAns{ stmt3.index, stmt2.index, stmt1.index, stmt5.index };

		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2 };
		uPredSucTable[stmt2] = { stmt3 };
		uPredSucTable[stmt3] = { stmt4 };
		uPredSucTable[stmt5] = { stmt1 };

		ParentT::populate(uPredSucTable);

		auto parentTStmts = ParentT::getPredecessorStmts(stmt4);
		Assert::IsTrue(parentTExpAns == parentTStmts);

		auto parentTEmptyStmts = ParentT::getPredecessorStmts(stmt5);
		Assert::IsTrue(0 == parentTEmptyStmts.size());

		/* Check ParentT data does not get affected */
		auto followsTStmts = FollowsT::getSuccessorStmts(stmt4);
		Assert::IsTrue(0 == followsTStmts.size());
		FollowsT::performCleanUp();
	};

	TEST_METHOD(containsSuccessor) {
		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2 };
		uPredSucTable[stmt2] = { stmt3, stmt4 };

		ParentT::populate(uPredSucTable);

		Assert::IsTrue(ParentT::containsSuccessor(stmt1, stmt4));
		Assert::IsFalse(ParentT::containsSuccessor(stmt4, stmt1));
		Assert::IsFalse(ParentT::containsSuccessor(stmt3, stmt4)); /* siblings */
	};

	TEST_METHOD(containsPredecessor) {
		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2 };
		uPredSucTable[stmt2] = { stmt3, stmt4 };

		ParentT::populate(uPredSucTable);

		Assert::IsTrue(ParentT::containsPredecessor(stmt1, stmt4));
		Assert::IsFalse(ParentT::containsPredecessor(stmt4, stmt1));
		Assert::IsFalse(ParentT::containsPredecessor(stmt3, stmt4)); /* siblings */
	};

	TEST_METHOD(getAllPredecessorSuccessorInfo) {
		std::vector<int> parentTpredecessors{ stmt2.index, stmt1.index, stmt1.index };
		std::vector<int> parentTsuccessors{ stmt3.index, stmt2.index, stmt3.index };
		std::tuple<std::vector<int>, std::vector<int>> parentTExpAns =
			std::make_tuple(parentTpredecessors, parentTsuccessors);

		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2 };
		uPredSucTable[stmt2] = { stmt3 };

		ParentT::populate(uPredSucTable);

		auto parentTInfo = ParentT::getAllPredecessorSuccessorInfo();

		Assert::IsTrue(parentTExpAns == parentTInfo);
	};

	TEST_METHOD(getPredSucTable) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
			StmtIndex::HashFunction> parentTExpAns;
		parentTExpAns[stmt1].insert(stmt2);
		parentTExpAns[stmt1].insert(stmt3);
		parentTExpAns[stmt2].insert(stmt3);

		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2 };
		uPredSucTable[stmt2] = { stmt3 };

		ParentT::populate(uPredSucTable);

		auto parentTTable = ParentT::getPredSucTable();

		Assert::IsTrue(parentTExpAns == parentTTable);
	};

	TEST_METHOD(getSucPredTable) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
			StmtIndex::HashFunction> parentTExpAns;
		parentTExpAns[stmt2].insert(stmt1);
		parentTExpAns[stmt3].insert(stmt1);
		parentTExpAns[stmt3].insert(stmt2);

		std::unordered_map<StmtIndex,
			std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
		uPredSucTable[stmt1] = { stmt2 };
		uPredSucTable[stmt2] = { stmt3 };

		ParentT::populate(uPredSucTable);

		auto parentTTable = ParentT::getSucPredTable();

		Assert::IsTrue(parentTExpAns == parentTTable);
	};
	};
};
