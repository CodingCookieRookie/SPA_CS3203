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
		CallsT::performCleanUp();
	}

public:
	TEST_METHOD(populate_getSuccessors_branched) {
		std::vector<int> callsTExpAns{ procIdx2.index, procIdx3.index, procIdx4.index, procIdx5.index };

		std::unordered_map<ProcIndex, std::unordered_set<ProcIndex>> uPredSucTable;
		uPredSucTable[procIdx1] = { procIdx2, procIdx5 };
		uPredSucTable[procIdx2] = { procIdx3, procIdx4 };

		CallsT::populate(uPredSucTable);

		auto callsTStmts = CallsT::getSuccessors(procIdx1);
		Assert::IsTrue(callsTExpAns == callsTStmts);

		auto callsTEmptyStmts = CallsT::getSuccessors(procIdx5);
		Assert::IsTrue(0 == callsTEmptyStmts.size());
	};

	TEST_METHOD(populate_getSuccessors_linear) {
		std::vector<int> callsTExpAns{ procIdx1.index, procIdx2.index, procIdx3.index, procIdx4.index };

		std::unordered_map<ProcIndex, std::unordered_set<ProcIndex>> uPredSucTable;
		uPredSucTable[procIdx1] = { procIdx2 };
		uPredSucTable[procIdx2] = { procIdx3 };
		uPredSucTable[procIdx3] = { procIdx4 };
		uPredSucTable[procIdx5] = { procIdx1 };

		CallsT::populate(uPredSucTable);

		auto callsTStmts = CallsT::getSuccessors(procIdx5);
		Assert::IsTrue(callsTExpAns == callsTStmts);

		auto callsTEmptyStmts = CallsT::getSuccessors(procIdx4);
		Assert::IsTrue(0 == callsTEmptyStmts.size());
	};

	TEST_METHOD(populate_getPredecessors_branched) {
		std::vector<int> callsTExpAns{ procIdx4.index, procIdx2.index, procIdx1.index };

		std::unordered_map<ProcIndex, std::unordered_set<ProcIndex>> uPredSucTable;
		uPredSucTable[procIdx1] = { procIdx2 };
		uPredSucTable[procIdx2] = { procIdx3, procIdx4 };
		uPredSucTable[procIdx4] = { procIdx5 };

		CallsT::populate(uPredSucTable);

		auto callsTStmts = CallsT::getPredecessors(procIdx5);
		Assert::IsTrue(callsTExpAns == callsTStmts);

		auto callsTEmptyStmts = CallsT::getPredecessors(procIdx1);
		Assert::IsTrue(0 == callsTEmptyStmts.size());
	};

	TEST_METHOD(populate_getPredecessors_linear) {
		std::vector<int> callsTExpAns{ procIdx3.index, procIdx2.index, procIdx1.index, procIdx5.index };

		std::unordered_map<ProcIndex, std::unordered_set<ProcIndex>> uPredSucTable;
		uPredSucTable[procIdx1] = { procIdx2 };
		uPredSucTable[procIdx2] = { procIdx3 };
		uPredSucTable[procIdx3] = { procIdx4 };
		uPredSucTable[procIdx5] = { procIdx1 };

		CallsT::populate(uPredSucTable);

		auto callsTStmts = CallsT::getPredecessors(procIdx4);
		Assert::IsTrue(callsTExpAns == callsTStmts);

		auto callsTEmptyStmts = CallsT::getPredecessors(procIdx5);
		Assert::IsTrue(0 == callsTEmptyStmts.size());
	};

	TEST_METHOD(containsSuccessor) {
		std::unordered_map<ProcIndex, std::unordered_set<ProcIndex>> uPredSucTable;
		uPredSucTable[procIdx1] = { procIdx2 };
		uPredSucTable[procIdx2] = { procIdx3, procIdx4 };

		CallsT::populate(uPredSucTable);

		Assert::IsTrue(CallsT::containsSuccessor(procIdx1, procIdx4));
		Assert::IsFalse(CallsT::containsSuccessor(procIdx4, procIdx1));
		Assert::IsFalse(CallsT::containsSuccessor(procIdx3, procIdx4)); /* siblings */
	};

	TEST_METHOD(containsPredecessor) {
		std::unordered_map<ProcIndex, std::unordered_set<ProcIndex>> uPredSucTable;
		uPredSucTable[procIdx1] = { procIdx2 };
		uPredSucTable[procIdx2] = { procIdx3, procIdx4 };

		CallsT::populate(uPredSucTable);

		Assert::IsTrue(CallsT::containsPredecessor(procIdx1, procIdx4));
		Assert::IsFalse(CallsT::containsPredecessor(procIdx4, procIdx1));
		Assert::IsFalse(CallsT::containsPredecessor(procIdx3, procIdx4)); /* siblings */
	};

	TEST_METHOD(getAllPredecessorSuccessorInfo) {
		std::vector<int> callsTpredecessors{ procIdx2.index, procIdx1.index, procIdx1.index };
		std::vector<int> callsTsuccessors{ procIdx3.index, procIdx2.index, procIdx3.index };
		std::tuple<std::vector<int>, std::vector<int>> callsTExpAns =
			std::make_tuple(callsTpredecessors, callsTsuccessors);

		std::unordered_map<ProcIndex, std::unordered_set<ProcIndex>> uPredSucTable;
		uPredSucTable[procIdx1] = { procIdx2 };
		uPredSucTable[procIdx2] = { procIdx3 };

		CallsT::populate(uPredSucTable);

		auto callsTInfo = CallsT::getAllPredecessorSuccessorInfo();

		Assert::IsTrue(callsTExpAns == callsTInfo);
	};

	TEST_METHOD(getPredSucTable) {
		std::unordered_map<ProcIndex, std::unordered_set<ProcIndex>> callsTExpAns;
		callsTExpAns[procIdx1].insert(procIdx2);
		callsTExpAns[procIdx1].insert(procIdx3);
		callsTExpAns[procIdx2].insert(procIdx3);

		std::unordered_map<ProcIndex, std::unordered_set<ProcIndex>> uPredSucTable;
		uPredSucTable[procIdx1] = { procIdx2 };
		uPredSucTable[procIdx2] = { procIdx3 };

		CallsT::populate(uPredSucTable);

		auto callsTTable = CallsT::getPredSucTable();

		Assert::IsTrue(callsTExpAns == callsTTable);
	};

	TEST_METHOD(getSucPredTable) {
		std::unordered_map<ProcIndex, std::unordered_set<ProcIndex>> callsTExpAns;
		callsTExpAns[procIdx2].insert(procIdx1);
		callsTExpAns[procIdx3].insert(procIdx1);
		callsTExpAns[procIdx3].insert(procIdx2);

		std::unordered_map<ProcIndex, std::unordered_set<ProcIndex>> uSucPredTable;
		uSucPredTable[procIdx1] = { procIdx2 };
		uSucPredTable[procIdx2] = { procIdx3 };

		CallsT::populate(uSucPredTable);

		auto callsTTable = CallsT::getSucPredTable();

		Assert::IsTrue(callsTExpAns == callsTTable);
	};
	};
};
