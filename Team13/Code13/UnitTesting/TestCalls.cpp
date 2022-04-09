#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/Calls.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestCalls) {
private:
	ProcIndex predecessor1 = { 1 };
	ProcIndex predecessor2 = { 2 };
	ProcIndex successor1 = { 3 };
	ProcIndex successor2 = { 4 };

	Calls* calls;

	TEST_METHOD_INITIALIZE(init) {
		calls = new Calls();
	}

public:
	TEST_METHOD(insert_getFromLeftArg_onePredOneSuc) {
		calls->insert(predecessor1, successor1);
		calls->insert(predecessor2, successor2);

		auto procedures = calls->getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<ProcIndex> { successor1 } == procedures);

		procedures = calls->getFromLeftArg(predecessor2);
		Assert::IsTrue(std::vector<ProcIndex> { successor2 } == procedures);

		procedures = calls->getFromLeftArg(successor1);
		Assert::IsTrue(0 == procedures.size());
	};

	TEST_METHOD(insert_getFromLeftArg_onePredMultSuc) {
		calls->insert(predecessor1, successor1);
		calls->insert(predecessor1, successor2);

		auto procedures = calls->getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<ProcIndex> { successor1, successor2 } == procedures);

		procedures = calls->getFromLeftArg(successor1);
		Assert::IsTrue(0 == procedures.size());
	};

	TEST_METHOD(insert_getFromRightArg_onePredOneSuc) {
		calls->insert(predecessor1, successor1);
		calls->insert(predecessor2, successor2);

		auto procedures = calls->getFromRightArg(successor1);
		Assert::IsTrue(std::vector<ProcIndex> {predecessor1} == procedures);

		procedures = calls->getFromRightArg(successor2);
		Assert::IsTrue(std::vector<ProcIndex> {predecessor2} == procedures);

		procedures = calls->getFromRightArg(predecessor1);
		Assert::IsTrue(0 == procedures.size());
	};

	TEST_METHOD(insert_getFromRightArg_multPredOneSuc) {
		std::vector<ProcIndex> expectedAns{ predecessor1, predecessor2 };

		calls->insert(predecessor1, successor1);
		calls->insert(predecessor2, successor1);
		auto procedures = calls->getFromRightArg(successor1);
		Assert::IsTrue(std::vector<ProcIndex> { predecessor1, predecessor2 } == procedures);

		procedures = calls->getFromRightArg(predecessor1);
		Assert::IsTrue(0 == procedures.size());
	};

	TEST_METHOD(insert_contains_onePredOneSuc) {
		calls->insert(predecessor1, successor1);

		Assert::IsTrue(calls->contains(predecessor1, successor1));
		Assert::IsFalse(calls->contains(successor1, predecessor1));
		Assert::IsFalse(calls->contains(predecessor2, successor1));
		Assert::IsFalse(calls->contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_contains_onePredMultSuc) {
		calls->insert(predecessor1, successor1);
		calls->insert(predecessor1, successor2);

		Assert::IsTrue(calls->contains(predecessor1, successor1));
		Assert::IsFalse(calls->contains(successor1, predecessor1));
		Assert::IsFalse(calls->contains(predecessor2, successor1));
		Assert::IsTrue(calls->contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_getAllInfo_onePredOneSuc) {
		std::vector<ProcIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<ProcIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<ProcIndex>, std::vector<ProcIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		calls->insert(predecessor1, successor1);
		calls->insert(predecessor2, successor2);

		auto predSucInfo = calls->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_onePredMultSuc) {
		std::vector<ProcIndex> predecessors{ predecessor1, predecessor1 };
		std::vector<ProcIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<ProcIndex>, std::vector<ProcIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		calls->insert(predecessor1, successor1);
		calls->insert(predecessor1, successor2);

		auto predSucInfo = calls->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_multPredOneSuc) {
		std::vector<ProcIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<ProcIndex> successors{ successor1, successor1 };
		std::tuple<std::vector<ProcIndex>, std::vector<ProcIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		calls->insert(predecessor1, successor1);
		calls->insert(predecessor2, successor1);

		auto predSucInfo = calls->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};
	};
}
