#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "../source/PKB/FollowsT.h"
#include "../source/PKB/ParentT.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestParentT) {
private:
	StmtIndex stmtIdx1 = { 1 };
	StmtIndex stmtIdx2 = { 2 };
	StmtIndex stmtIdx3 = { 3 };
	StmtIndex stmtIdx4 = { 4 };
	StmtIndex stmtIdx5 = { 5 };

	TEST_METHOD_CLEANUP(cleanUpParentT) {
		Parent::performCleanUp();
		ParentT::performCleanUp();
	}

public:
	TEST_METHOD(populate_getSuccessors_branched) {
		std::vector<StmtIndex> parentTExpAns{ stmtIdx2, stmtIdx3, stmtIdx4, stmtIdx5 };

		Parent::insert(stmtIdx1, stmtIdx2);
		Parent::insert(stmtIdx1, stmtIdx5);
		Parent::insert(stmtIdx2, stmtIdx3);
		Parent::insert(stmtIdx2, stmtIdx4);
		ParentT::populate();

		auto parentTStmts = ParentT::getSuccessors(stmtIdx1);
		Assert::IsTrue(parentTExpAns == parentTStmts);

		auto parentTEmptyStmts = ParentT::getSuccessors(stmtIdx5);
		Assert::IsTrue(0 == parentTEmptyStmts.size());

		/* Check ParentT data does not get affected */
		auto followsTStmts = FollowsT::getSuccessors(stmtIdx1);
		Assert::IsTrue(0 == followsTStmts.size());
		FollowsT::performCleanUp();
	};

	TEST_METHOD(populate_getSuccessors_linear) {
		std::vector<StmtIndex> parentTExpAns{ stmtIdx1, stmtIdx2, stmtIdx3, stmtIdx4 };

		Parent::insert(stmtIdx1, stmtIdx2);
		Parent::insert(stmtIdx2, stmtIdx3);
		Parent::insert(stmtIdx3, stmtIdx4);
		Parent::insert(stmtIdx5, stmtIdx1);
		ParentT::populate();

		auto parentTStmts = ParentT::getSuccessors(stmtIdx5);
		Assert::IsTrue(parentTExpAns == parentTStmts);

		auto parentTEmptyStmts = ParentT::getSuccessors(stmtIdx4);
		Assert::IsTrue(0 == parentTEmptyStmts.size());

		/* Check ParentT data does not get affected */
		auto followsTStmts = FollowsT::getSuccessors(stmtIdx5);
		Assert::IsTrue(0 == followsTStmts.size());
		FollowsT::performCleanUp();
	};

	TEST_METHOD(populate_getPredecessors_branched) {
		std::vector<StmtIndex> parentTExpAns{ stmtIdx4, stmtIdx2, stmtIdx1 };

		Parent::insert(stmtIdx1, stmtIdx2);
		Parent::insert(stmtIdx2, stmtIdx3);
		Parent::insert(stmtIdx2, stmtIdx4);
		Parent::insert(stmtIdx4, stmtIdx5);
		ParentT::populate();

		auto parentTStmts = ParentT::getPredecessors(stmtIdx5);
		Assert::IsTrue(parentTExpAns == parentTStmts);

		auto parentTEmptyStmts = ParentT::getPredecessors(stmtIdx1);
		Assert::IsTrue(0 == parentTEmptyStmts.size());

		/* Check ParentT data does not get affected */
		auto followsTStmts = FollowsT::getSuccessors(stmtIdx5);
		Assert::IsTrue(0 == followsTStmts.size());
		FollowsT::performCleanUp();
	};

	TEST_METHOD(populate_getPredecessors_linear) {
		std::vector<StmtIndex> parentTExpAns{ stmtIdx3, stmtIdx2, stmtIdx1, stmtIdx5 };

		Parent::insert(stmtIdx1, stmtIdx2);
		Parent::insert(stmtIdx2, stmtIdx3);
		Parent::insert(stmtIdx3, stmtIdx4);
		Parent::insert(stmtIdx5, stmtIdx1);
		ParentT::populate();

		auto parentTStmts = ParentT::getPredecessors(stmtIdx4);
		Assert::IsTrue(parentTExpAns == parentTStmts);

		auto parentTEmptyStmts = ParentT::getPredecessors(stmtIdx5);
		Assert::IsTrue(0 == parentTEmptyStmts.size());

		/* Check ParentT data does not get affected */
		auto followsTStmts = FollowsT::getSuccessors(stmtIdx4);
		Assert::IsTrue(0 == followsTStmts.size());
		FollowsT::performCleanUp();
	};

	TEST_METHOD(containsSuccessor) {
		Parent::insert(stmtIdx1, stmtIdx2);
		Parent::insert(stmtIdx2, stmtIdx3);
		Parent::insert(stmtIdx2, stmtIdx4);
		ParentT::populate();

		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx1, stmtIdx4));
		Assert::IsFalse(ParentT::containsSuccessor(stmtIdx4, stmtIdx1));
		Assert::IsFalse(ParentT::containsSuccessor(stmtIdx3, stmtIdx4)); /* siblings */
	};

	TEST_METHOD(containsPredecessor) {
		Parent::insert(stmtIdx1, stmtIdx2);
		Parent::insert(stmtIdx2, stmtIdx3);
		Parent::insert(stmtIdx2, stmtIdx4);
		ParentT::populate();

		Assert::IsTrue(ParentT::containsPredecessor(stmtIdx1, stmtIdx4));
		Assert::IsFalse(ParentT::containsPredecessor(stmtIdx4, stmtIdx1));
		Assert::IsFalse(ParentT::containsPredecessor(stmtIdx3, stmtIdx4)); /* siblings */
	};

	TEST_METHOD(getAllPredecessorSuccessorInfo) {
		std::vector<StmtIndex> parentTpredecessors{ stmtIdx2, stmtIdx1, stmtIdx1 };
		std::vector<StmtIndex> parentTsuccessors{ stmtIdx3, stmtIdx2, stmtIdx3 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> parentTExpAns =
			std::make_tuple(parentTpredecessors, parentTsuccessors);

		Parent::insert(stmtIdx1, stmtIdx2);
		Parent::insert(stmtIdx2, stmtIdx3);
		ParentT::populate();

		auto parentTInfo = ParentT::getAllPredecessorSuccessorInfo();
		Assert::IsTrue(parentTExpAns == parentTInfo);
	};

	TEST_METHOD(getPredSucTable) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> parentTExpAns;
		parentTExpAns[stmtIdx1].insert(stmtIdx2);
		parentTExpAns[stmtIdx1].insert(stmtIdx3);
		parentTExpAns[stmtIdx2].insert(stmtIdx3);

		Parent::insert(stmtIdx1, stmtIdx2);
		Parent::insert(stmtIdx2, stmtIdx3);
		ParentT::populate();

		auto parentTTable = ParentT::getPredSucTable();
		Assert::IsTrue(parentTExpAns == parentTTable);
	};
	};
};
