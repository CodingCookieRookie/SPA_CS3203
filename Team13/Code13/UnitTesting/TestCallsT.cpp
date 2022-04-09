#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/CallsT.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestCallsT) {
private:
	ProcIndex predecessor1 = { 1 };
	ProcIndex predecessor2 = { 2 };
	ProcIndex successor1 = { 3 };
	ProcIndex successor2 = { 4 };

	CallsT* callsT;

	TEST_METHOD_INITIALIZE(init) {
		callsT = new CallsT();
	}

public:
	TEST_METHOD(insert_getFromLeftArg_onePredOneSuc) {
		callsT->insert(predecessor1, successor1);
		callsT->insert(predecessor2, successor2);

		auto procedures = callsT->getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<ProcIndex> { successor1 } == procedures);

		procedures = callsT->getFromLeftArg(predecessor2);
		Assert::IsTrue(std::vector<ProcIndex> { successor2 } == procedures);

		procedures = callsT->getFromLeftArg(successor1);
		Assert::IsTrue(0 == procedures.size());
	};

	TEST_METHOD(insert_getFromLeftArg_onePredMultSuc) {
		callsT->insert(predecessor1, successor1);
		callsT->insert(predecessor1, successor2);

		auto procedures = callsT->getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<ProcIndex> { successor1, successor2 } == procedures);

		procedures = callsT->getFromLeftArg(successor1);
		Assert::IsTrue(0 == procedures.size());
	};

	TEST_METHOD(insert_getFromRightArg_onePredOneSuc) {
		callsT->insert(predecessor1, successor1);
		callsT->insert(predecessor2, successor2);

		auto procedures = callsT->getFromRightArg(successor1);
		Assert::IsTrue(std::vector<ProcIndex> {predecessor1} == procedures);

		procedures = callsT->getFromRightArg(successor2);
		Assert::IsTrue(std::vector<ProcIndex> {predecessor2} == procedures);

		procedures = callsT->getFromRightArg(predecessor1);
		Assert::IsTrue(0 == procedures.size());
	};

	TEST_METHOD(insert_getFromRightArg_multPredOneSuc) {
		std::vector<ProcIndex> expectedAns{ predecessor1, predecessor2 };

		callsT->insert(predecessor1, successor1);
		callsT->insert(predecessor2, successor1);
		auto procedures = callsT->getFromRightArg(successor1);
		Assert::IsTrue(std::vector<ProcIndex> { predecessor1, predecessor2 } == procedures);

		procedures = callsT->getFromRightArg(predecessor1);
		Assert::IsTrue(0 == procedures.size());
	};

	TEST_METHOD(insert_contains_onePredOneSuc) {
		callsT->insert(predecessor1, successor1);

		Assert::IsTrue(callsT->contains(predecessor1, successor1));
		Assert::IsFalse(callsT->contains(successor1, predecessor1));
		Assert::IsFalse(callsT->contains(predecessor2, successor1));
		Assert::IsFalse(callsT->contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_contains_onePredMultSuc) {
		callsT->insert(predecessor1, successor1);
		callsT->insert(predecessor1, successor2);

		Assert::IsTrue(callsT->contains(predecessor1, successor1));
		Assert::IsFalse(callsT->contains(successor1, predecessor1));
		Assert::IsFalse(callsT->contains(predecessor2, successor1));
		Assert::IsTrue(callsT->contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_getAllInfo_onePredOneSuc) {
		std::vector<ProcIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<ProcIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<ProcIndex>, std::vector<ProcIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		callsT->insert(predecessor1, successor1);
		callsT->insert(predecessor2, successor2);

		auto predSucInfo = callsT->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_onePredMultSuc) {
		std::vector<ProcIndex> predecessors{ predecessor1, predecessor1 };
		std::vector<ProcIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<ProcIndex>, std::vector<ProcIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		callsT->insert(predecessor1, successor1);
		callsT->insert(predecessor1, successor2);

		auto predSucInfo = callsT->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_multPredOneSuc) {
		std::vector<ProcIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<ProcIndex> successors{ successor1, successor1 };
		std::tuple<std::vector<ProcIndex>, std::vector<ProcIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		callsT->insert(predecessor1, successor1);
		callsT->insert(predecessor2, successor1);

		auto predSucInfo = callsT->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};
	};
}
