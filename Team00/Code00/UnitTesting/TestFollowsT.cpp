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
	TEST_METHOD(generate_getSuccessorStmts_branched) {
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
		FollowsT::generate();

		auto followsTStmts = FollowsT::getSuccessorStmts(stmt1);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsStmts = Follows::getSuccessorStmts(stmt1);
		Assert::IsTrue(followsExpAns == followsStmts);

		/*TODO: When parent code is added, check it does not get affected*/
	};

	TEST_METHOD(generate_getSuccessorStmts_linear) {
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
		FollowsT::generate();

		auto followsTStmts = FollowsT::getSuccessorStmts(stmt5);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsStmts = Follows::getSuccessorStmts(stmt5);
		Assert::IsTrue(followsExpAns == followsStmts);

		/*TODO: When parent code is added, check it does not get affected*/
	};
	};
}
