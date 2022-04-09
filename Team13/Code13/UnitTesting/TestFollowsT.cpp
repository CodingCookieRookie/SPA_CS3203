#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/FollowsT.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestFollowsT) {
private:
	StmtIndex predecessor1 = { 1 };
	StmtIndex predecessor2 = { 2 };
	StmtIndex successor1 = { 3 };
	StmtIndex successor2 = { 4 };

	FollowsT* followsT;

	TEST_METHOD_INITIALIZE(init) {
		followsT = new FollowsT();
	}

public:
	TEST_METHOD(insert_getFromLeftArg_onePredOneSuc) {
		followsT->insert(predecessor1, successor1);
		followsT->insert(predecessor2, successor2);

		auto statements = followsT->getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1 } == statements);

		statements = followsT->getFromLeftArg(predecessor2);
		Assert::IsTrue(std::vector<StmtIndex> { successor2 } == statements);

		statements = followsT->getFromLeftArg(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromLeftArg_onePredMultSuc) {
		followsT->insert(predecessor1, successor1);
		followsT->insert(predecessor1, successor2);

		auto statements = followsT->getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1, successor2 } == statements);

		statements = followsT->getFromLeftArg(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromRightArg_onePredOneSuc) {
		followsT->insert(predecessor1, successor1);
		followsT->insert(predecessor2, successor2);

		auto statements = followsT->getFromRightArg(successor1);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor1} == statements);

		statements = followsT->getFromRightArg(successor2);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor2} == statements);

		statements = followsT->getFromRightArg(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromRightArg_multPredOneSuc) {
		std::vector<StmtIndex> expectedAns{ predecessor1, predecessor2 };

		followsT->insert(predecessor1, successor1);
		followsT->insert(predecessor2, successor1);
		auto statements = followsT->getFromRightArg(successor1);
		Assert::IsTrue(std::vector<StmtIndex> { predecessor1, predecessor2 } == statements);

		statements = followsT->getFromRightArg(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_contains_onePredOneSuc) {
		followsT->insert(predecessor1, successor1);

		Assert::IsTrue(followsT->contains(predecessor1, successor1));
		Assert::IsFalse(followsT->contains(successor1, predecessor1));
		Assert::IsFalse(followsT->contains(predecessor2, successor1));
		Assert::IsFalse(followsT->contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_contains_onePredMultSuc) {
		followsT->insert(predecessor1, successor1);
		followsT->insert(predecessor1, successor2);

		Assert::IsTrue(followsT->contains(predecessor1, successor1));
		Assert::IsFalse(followsT->contains(successor1, predecessor1));
		Assert::IsFalse(followsT->contains(predecessor2, successor1));
		Assert::IsTrue(followsT->contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_getAllInfo_onePredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		followsT->insert(predecessor1, successor1);
		followsT->insert(predecessor2, successor2);

		auto predSucInfo = followsT->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_onePredMultSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor1 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		followsT->insert(predecessor1, successor1);
		followsT->insert(predecessor1, successor2);

		auto predSucInfo = followsT->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_multPredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor1 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		followsT->insert(predecessor1, successor1);
		followsT->insert(predecessor2, successor1);

		auto predSucInfo = followsT->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};
	};
}
