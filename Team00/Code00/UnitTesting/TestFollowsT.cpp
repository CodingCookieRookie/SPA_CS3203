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

	TEST_METHOD_CLEANUP(cleanUpEntities) {
		FollowsT::performCleanUp();
	}

public:
	TEST_METHOD(insert_getSuccessorStmts) {
		unordered_set<StmtIndex, StmtIndex::HashFunction> followsTExpAns;
		followsTExpAns.insert(stmt2);
		followsTExpAns.insert(stmt3);
		followsTExpAns.insert(stmt4);

		unordered_set<StmtIndex, StmtIndex::HashFunction> followsExpAns;
		followsExpAns.insert(stmt2);

		Follows::insert(stmt1, stmt2);
		Follows::insert(stmt2, stmt3);
		Follows::insert(stmt2, stmt4);
		FollowsT::generate();

		auto followsTStmts = FollowsT::getSuccessorStmts(stmt1);
		Assert::IsTrue(followsTExpAns == followsTStmts);

		auto followsStmts = Follows::getSuccessorStmts(stmt1);
		Assert::IsTrue(followsExpAns == followsStmts);

		/*TODO: When parent code is added, check it does not get affected*/
	};
	};
}
