#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/common/Types.h"
#include "../source/PKB/Follows.h"
#include "../source/PKB/FollowsT.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestFollowsT) {
private:
	StmtIndex stmt1 = { 1 };
	StmtIndex stmt2 = { 2 };
	StmtIndex stmt3 = { 3 };
	StmtIndex stmt4 = { 4 };
	StmtIndex stmt5 = { 5 };

	TEST_METHOD_CLEANUP(cleanUpEntities) {
		FollowsT::performCleanUp();
		Follows::performCleanUp();
	}

public:
	TEST_METHOD(populate_getSuccessorStmts_branched) {
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> followsTExpAns;
		followsTExpAns.insert(stmt2);
		followsTExpAns.insert(stmt3);
		followsTExpAns.insert(stmt4);
		followsTExpAns.insert(stmt5);

		std::unordered_set<StmtIndex, StmtIndex::HashFunction> followsExpAns;
		followsExpAns.insert(stmt2);
		followsExpAns.insert(stmt5);

		Follows::insert(stmt1, stmt2);
		Follows::insert(stmt2, stmt3);
		Follows::insert(stmt2, stmt4);
		Follows::insert(stmt1, stmt5);
		FollowsT::populate();

		auto followsTStmts = FollowsT::getSuccessorStmts(stmt1);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsStmts = Follows::getSuccessorStmts(stmt1);
		Assert::IsTrue(followsExpAns == followsStmts);

		/*TODO: When parent code is added, check it does not get affected*/
	};

	TEST_METHOD(populate_getSuccessorStmts_linear) {
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> followsTExpAns;
		followsTExpAns.insert(stmt1);
		followsTExpAns.insert(stmt2);
		followsTExpAns.insert(stmt3);
		followsTExpAns.insert(stmt4);

		std::unordered_set<StmtIndex, StmtIndex::HashFunction> followsExpAns;
		followsExpAns.insert(stmt1);

		Follows::insert(stmt1, stmt2);
		Follows::insert(stmt2, stmt3);
		Follows::insert(stmt3, stmt4);
		Follows::insert(stmt5, stmt1);
		FollowsT::populate();

		auto followsTStmts = FollowsT::getSuccessorStmts(stmt5);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsStmts = Follows::getSuccessorStmts(stmt5);
		Assert::IsTrue(followsExpAns == followsStmts);

		/*TODO: When parent code is added, check it does not get affected*/
	};

	TEST_METHOD(populate_getPredecessorStmts_branched) {
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> followsTExpAns;
		followsTExpAns.insert(stmt1);
		followsTExpAns.insert(stmt2);
		followsTExpAns.insert(stmt4);

		std::unordered_set<StmtIndex, StmtIndex::HashFunction> followsExpAns;
		followsExpAns.insert(stmt4);

		Follows::insert(stmt1, stmt2);
		Follows::insert(stmt2, stmt3);
		Follows::insert(stmt2, stmt4);
		Follows::insert(stmt4, stmt5);
		FollowsT::populate();

		auto followsTStmts = FollowsT::getPredecessorStmts(stmt5);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsStmts = Follows::getPredecessorStmts(stmt5);
		Assert::IsTrue(followsExpAns == followsStmts);

		/*TODO: When parent code is added, check it does not get affected*/
	};

	TEST_METHOD(populate_getPredecessorStmts_linear) {
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> followsTExpAns;
		followsTExpAns.insert(stmt1);
		followsTExpAns.insert(stmt2);
		followsTExpAns.insert(stmt3);
		followsTExpAns.insert(stmt5);

		std::unordered_set<StmtIndex, StmtIndex::HashFunction> followsExpAns;
		followsExpAns.insert(stmt3);

		Follows::insert(stmt1, stmt2);
		Follows::insert(stmt2, stmt3);
		Follows::insert(stmt3, stmt4);
		Follows::insert(stmt5, stmt1);
		FollowsT::populate();

		auto followsTStmts = FollowsT::getPredecessorStmts(stmt4);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsStmts = Follows::getPredecessorStmts(stmt4);
		Assert::IsTrue(followsExpAns == followsStmts);

		/*TODO: When parent code is added, check it does not get affected*/
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
		std::vector<std::tuple<StmtIndex, StmtIndex>> followsTExpAns;
		followsTExpAns.push_back(std::make_tuple(stmt1, stmt2));
		followsTExpAns.push_back(std::make_tuple(stmt1, stmt3));
		followsTExpAns.push_back(std::make_tuple(stmt2, stmt3));

		std::vector<std::tuple<StmtIndex, StmtIndex>> followsExpAns;
		followsExpAns.push_back(std::make_tuple(stmt1, stmt2));
		followsExpAns.push_back(std::make_tuple(stmt2, stmt3));

		Follows::insert(stmt1, stmt2);
		Follows::insert(stmt2, stmt3);
		FollowsT::populate();

		auto followsTInfo = FollowsT::getAllPredecessorSuccessorInfo();
		auto followsInfo = Follows::getAllPredecessorSuccessorInfo();

		Assert::IsTrue(followsTExpAns == followsTInfo);
		Assert::IsTrue(followsExpAns == followsInfo);
	};

	TEST_METHOD(getPredSucTable) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> followsTExpAns;
		followsTExpAns[stmt1].insert(stmt2);
		followsTExpAns[stmt1].insert(stmt3);
		followsTExpAns[stmt2].insert(stmt3);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> followsExpAns;
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
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> followsTExpAns;
		followsTExpAns[stmt2].insert(stmt1);
		followsTExpAns[stmt3].insert(stmt1);
		followsTExpAns[stmt3].insert(stmt2);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> followsExpAns;
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
