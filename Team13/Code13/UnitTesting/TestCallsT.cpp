#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "../source/PKB/CallsT.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestCallsT) {
private:
	ProcIndex procIdx1 = { 1 };
	ProcIndex procIdx2 = { 2 };
	ProcIndex procIdx3 = { 3 };
	ProcIndex procIdx4 = { 4 };
	ProcIndex procIdx5 = { 5 };

	TEST_METHOD_CLEANUP(cleanUpCallsT) {
		Calls::performCleanUp();
		CallsT::performCleanUp();
	}

public:
	TEST_METHOD(populate_getFromLeftArg_branched) {
		std::vector<ProcIndex> callsTExpAns{ procIdx2, procIdx3, procIdx4, procIdx5 };

		Calls::insert(procIdx1, procIdx2);
		Calls::insert(procIdx1, procIdx5);
		Calls::insert(procIdx2, procIdx3);
		Calls::insert(procIdx2, procIdx4);
		CallsT::populate();

		auto callsTStmts = CallsT::getFromLeftArg(procIdx1);
		Assert::IsTrue(callsTExpAns == callsTStmts);

		auto callsTEmptyStmts = CallsT::getFromLeftArg(procIdx5);
		Assert::IsTrue(0 == callsTEmptyStmts.size());
	};

	TEST_METHOD(populate_getFromLeftArg_linear) {
		std::vector<ProcIndex> callsTExpAns{ procIdx1, procIdx2, procIdx3, procIdx4 };

		Calls::insert(procIdx1, procIdx2);
		Calls::insert(procIdx2, procIdx3);
		Calls::insert(procIdx3, procIdx4);
		Calls::insert(procIdx5, procIdx1);
		CallsT::populate();

		auto callsTStmts = CallsT::getFromLeftArg(procIdx5);
		Assert::IsTrue(callsTExpAns == callsTStmts);

		auto callsTEmptyStmts = CallsT::getFromLeftArg(procIdx4);
		Assert::IsTrue(0 == callsTEmptyStmts.size());
	};

	TEST_METHOD(populate_getFromRightArg_branched) {
		std::vector<ProcIndex> callsTExpAns{ procIdx4, procIdx2, procIdx1 };

		Calls::insert(procIdx1, procIdx2);
		Calls::insert(procIdx2, procIdx3);
		Calls::insert(procIdx2, procIdx4);
		Calls::insert(procIdx4, procIdx5);
		CallsT::populate();

		auto callsTStmts = CallsT::getFromRightArg(procIdx5);
		Assert::IsTrue(callsTExpAns == callsTStmts);

		auto callsTEmptyStmts = CallsT::getFromRightArg(procIdx1);
		Assert::IsTrue(0 == callsTEmptyStmts.size());
	};

	TEST_METHOD(populate_getFromRightArg_linear) {
		std::vector<ProcIndex> callsTExpAns{ procIdx3, procIdx2, procIdx1, procIdx5 };

		Calls::insert(procIdx1, procIdx2);
		Calls::insert(procIdx2, procIdx3);
		Calls::insert(procIdx3, procIdx4);
		Calls::insert(procIdx5, procIdx1);
		CallsT::populate();

		auto callsTStmts = CallsT::getFromRightArg(procIdx4);
		Assert::IsTrue(callsTExpAns == callsTStmts);

		auto callsTEmptyStmts = CallsT::getFromRightArg(procIdx5);
		Assert::IsTrue(0 == callsTEmptyStmts.size());
	};

	TEST_METHOD(contains) {
		Calls::insert(procIdx1, procIdx2);
		Calls::insert(procIdx2, procIdx3);
		Calls::insert(procIdx2, procIdx4);
		CallsT::populate();

		Assert::IsTrue(CallsT::contains(procIdx1, procIdx4));
		Assert::IsFalse(CallsT::contains(procIdx4, procIdx1));
		Assert::IsFalse(CallsT::contains(procIdx3, procIdx4)); /* siblings */
	};

	TEST_METHOD(getAllInfo) {
		std::vector<ProcIndex> callsTpredecessors{ procIdx2, procIdx1, procIdx1 };
		std::vector<ProcIndex> callsTsuccessors{ procIdx3, procIdx2, procIdx3 };
		std::tuple<std::vector<ProcIndex>, std::vector<ProcIndex>> callsTExpAns =
			std::make_tuple(callsTpredecessors, callsTsuccessors);

		Calls::insert(procIdx1, procIdx2);
		Calls::insert(procIdx2, procIdx3);
		CallsT::populate();

		auto callsTInfo = CallsT::getAllInfo();
		Assert::IsTrue(callsTExpAns == callsTInfo);
	};

	TEST_METHOD(getPredSucTable) {
		std::unordered_map<ProcIndex, std::unordered_set<ProcIndex>> callsTExpAns;
		callsTExpAns[procIdx1].insert(procIdx2);
		callsTExpAns[procIdx1].insert(procIdx3);
		callsTExpAns[procIdx2].insert(procIdx3);

		Calls::insert(procIdx1, procIdx2);
		Calls::insert(procIdx2, procIdx3);
		CallsT::populate();

		auto callsTTable = CallsT::getPredSucTable();
		Assert::IsTrue(callsTExpAns == callsTTable);
	};
	};
};
