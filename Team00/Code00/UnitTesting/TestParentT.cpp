#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/common/Types.h"
#include "../source/PKB/FollowsT.h"
#include "../source/PKB/Parent.h"
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

	TEST_METHOD_CLEANUP(cleanUpParentParentT) {
		ParentT::performCleanUp();
		Parent::performCleanUp();
	}

public:
	TEST_METHOD(populate_getSuccessorStmts_branched) {
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> parentTExpAns;
		parentTExpAns.insert(stmt2);
		parentTExpAns.insert(stmt3);
		parentTExpAns.insert(stmt4);
		parentTExpAns.insert(stmt5);

		std::unordered_set<StmtIndex, StmtIndex::HashFunction> parentExpAns;
		parentExpAns.insert(stmt2);
		parentExpAns.insert(stmt5);

		Parent::insert(stmt1, stmt2);
		Parent::insert(stmt2, stmt3);
		Parent::insert(stmt2, stmt4);
		Parent::insert(stmt1, stmt5);
		ParentT::populate();

		auto parentTStmts = ParentT::getSuccessorStmts(stmt1);
		Assert::IsTrue(parentTExpAns == parentTStmts);

		auto parentStmts = Parent::getSuccessorStmts(stmt1);
		Assert::IsTrue(parentExpAns == parentStmts);

		auto parentTEmptyStmts = ParentT::getSuccessorStmts(stmt5);
		Assert::IsTrue(0 == parentTEmptyStmts.size());

		/*Check FollowsT data does not get affected*/
		auto followsTStmts = FollowsT::getSuccessorStmts(stmt1);
		Assert::IsTrue(0 == followsTStmts.size());
		FollowsT::performCleanUp();
	};

	TEST_METHOD(populate_getSuccessorStmts_linear) {
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> parentTExpAns;
		parentTExpAns.insert(stmt1);
		parentTExpAns.insert(stmt2);
		parentTExpAns.insert(stmt3);
		parentTExpAns.insert(stmt4);

		std::unordered_set<StmtIndex, StmtIndex::HashFunction> parentExpAns;
		parentExpAns.insert(stmt1);

		Parent::insert(stmt1, stmt2);
		Parent::insert(stmt2, stmt3);
		Parent::insert(stmt3, stmt4);
		Parent::insert(stmt5, stmt1);
		ParentT::populate();

		auto parentTStmts = ParentT::getSuccessorStmts(stmt5);
		Assert::IsTrue(parentTExpAns == parentTStmts);

		auto parentStmts = Parent::getSuccessorStmts(stmt5);
		Assert::IsTrue(parentExpAns == parentStmts);

		auto parentTEmptyStmts = ParentT::getSuccessorStmts(stmt4);
		Assert::IsTrue(0 == parentTEmptyStmts.size());

		/*Check FollowsT data does not get affected*/
		auto followsTStmts = FollowsT::getSuccessorStmts(stmt5);
		Assert::IsTrue(0 == followsTStmts.size());
		FollowsT::performCleanUp();
	};

	TEST_METHOD(populate_getPredecessorStmts_branched) {
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> parentTExpAns;
		parentTExpAns.insert(stmt1);
		parentTExpAns.insert(stmt2);
		parentTExpAns.insert(stmt4);

		std::unordered_set<StmtIndex, StmtIndex::HashFunction> parentExpAns;
		parentExpAns.insert(stmt4);

		Parent::insert(stmt1, stmt2);
		Parent::insert(stmt2, stmt3);
		Parent::insert(stmt2, stmt4);
		Parent::insert(stmt4, stmt5);
		ParentT::populate();

		auto parentTStmts = ParentT::getPredecessorStmts(stmt5);
		Assert::IsTrue(parentTExpAns == parentTStmts);

		auto parentStmts = Parent::getPredecessorStmts(stmt5);
		Assert::IsTrue(parentExpAns == parentStmts);

		auto parentTEmptyStmts = ParentT::getPredecessorStmts(stmt1);
		Assert::IsTrue(0 == parentTEmptyStmts.size());

		/*Check FollowsT data does not get affected*/
		auto followsTStmts = FollowsT::getPredecessorStmts(stmt5);
		Assert::IsTrue(0 == followsTStmts.size());
		FollowsT::performCleanUp();
	};

	TEST_METHOD(populate_getPredecessorStmts_linear) {
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> parentTExpAns;
		parentTExpAns.insert(stmt1);
		parentTExpAns.insert(stmt2);
		parentTExpAns.insert(stmt3);
		parentTExpAns.insert(stmt5);

		std::unordered_set<StmtIndex, StmtIndex::HashFunction> parentExpAns;
		parentExpAns.insert(stmt3);

		Parent::insert(stmt1, stmt2);
		Parent::insert(stmt2, stmt3);
		Parent::insert(stmt3, stmt4);
		Parent::insert(stmt5, stmt1);
		ParentT::populate();

		auto parentTStmts = ParentT::getPredecessorStmts(stmt4);
		Assert::IsTrue(parentTExpAns == parentTStmts);

		auto parentStmts = Parent::getPredecessorStmts(stmt4);
		Assert::IsTrue(parentExpAns == parentStmts);

		auto parentTEmptyStmts = ParentT::getPredecessorStmts(stmt5);
		Assert::IsTrue(0 == parentTEmptyStmts.size());

		/*Check FollowsT data does not get affected*/
		auto followsTStmts = FollowsT::getPredecessorStmts(stmt4);
		Assert::IsTrue(0 == followsTStmts.size());
		FollowsT::performCleanUp();
	};

	TEST_METHOD(containsSuccessor) {
		Parent::insert(stmt1, stmt2);
		Parent::insert(stmt2, stmt3);
		Parent::insert(stmt2, stmt4);
		ParentT::populate();

		Assert::IsTrue(ParentT::containsSuccessor(stmt1, stmt4));
		Assert::IsFalse(ParentT::containsSuccessor(stmt4, stmt1));
		Assert::IsFalse(ParentT::containsSuccessor(stmt3, stmt4)); /*siblings*/

		Assert::IsTrue(Parent::containsSuccessor(stmt1, stmt2));
		Assert::IsFalse(Parent::containsSuccessor(stmt1, stmt4));
	};

	TEST_METHOD(containsPredecessor) {
		Parent::insert(stmt1, stmt2);
		Parent::insert(stmt2, stmt3);
		Parent::insert(stmt2, stmt4);
		ParentT::populate();

		Assert::IsTrue(ParentT::containsPredecessor(stmt1, stmt4));
		Assert::IsFalse(ParentT::containsPredecessor(stmt4, stmt1));
		Assert::IsFalse(ParentT::containsPredecessor(stmt3, stmt4)); /*siblings*/

		Assert::IsTrue(Parent::containsPredecessor(stmt1, stmt2));
		Assert::IsFalse(Parent::containsPredecessor(stmt1, stmt4));
	};

	TEST_METHOD(getAllPredecessorSuccessorInfo) {
		std::vector<std::tuple<StmtIndex, StmtIndex>> parentTExpAns;
		parentTExpAns.push_back(std::make_tuple(stmt1, stmt2));
		parentTExpAns.push_back(std::make_tuple(stmt1, stmt3));
		parentTExpAns.push_back(std::make_tuple(stmt2, stmt3));

		std::vector<std::tuple<StmtIndex, StmtIndex>> parentExpAns;
		parentExpAns.push_back(std::make_tuple(stmt1, stmt2));
		parentExpAns.push_back(std::make_tuple(stmt2, stmt3));

		Parent::insert(stmt1, stmt2);
		Parent::insert(stmt2, stmt3);
		ParentT::populate();

		auto parentTInfo = ParentT::getAllPredecessorSuccessorInfo();
		auto parentInfo = Parent::getAllPredecessorSuccessorInfo();

		Assert::IsTrue(parentTExpAns == parentTInfo);
		Assert::IsTrue(parentExpAns == parentInfo);
	};

	TEST_METHOD(getPredSucTable) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> parentTExpAns;
		parentTExpAns[stmt1].insert(stmt2);
		parentTExpAns[stmt1].insert(stmt3);
		parentTExpAns[stmt2].insert(stmt3);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> parentExpAns;
		parentExpAns[stmt1].insert(stmt2);
		parentExpAns[stmt2].insert(stmt3);

		Parent::insert(stmt1, stmt2);
		Parent::insert(stmt2, stmt3);
		ParentT::populate();

		auto parentTTable = ParentT::getPredSucTable();
		auto parentTable = Parent::getPredSucTable();

		Assert::IsTrue(parentTExpAns == parentTTable);
		Assert::IsTrue(parentExpAns == parentTable);
	};

	TEST_METHOD(getSucPredTable) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> parentTExpAns;
		parentTExpAns[stmt2].insert(stmt1);
		parentTExpAns[stmt3].insert(stmt1);
		parentTExpAns[stmt3].insert(stmt2);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> parentExpAns;
		parentExpAns[stmt2].insert(stmt1);
		parentExpAns[stmt3].insert(stmt2);

		Parent::insert(stmt1, stmt2);
		Parent::insert(stmt2, stmt3);
		ParentT::populate();

		auto parentTTable = ParentT::getSucPredTable();
		auto parentTable = Parent::getSucPredTable();

		Assert::IsTrue(parentTExpAns == parentTTable);
		Assert::IsTrue(parentExpAns == parentTable);
	};
	};
};
