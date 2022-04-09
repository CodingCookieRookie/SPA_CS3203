#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/ParentT.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestParentT) {
private:
	StmtIndex predecessor1 = { 1 };
	StmtIndex predecessor2 = { 2 };
	StmtIndex successor1 = { 3 };
	StmtIndex successor2 = { 4 };

	ParentT* parentT;

	TEST_METHOD_INITIALIZE(init) {
		parentT = new ParentT();
	}

public:
	TEST_METHOD(insert_getFromLeftArg_onePredOneSuc) {
		parentT->insert(predecessor1, successor1);
		parentT->insert(predecessor2, successor2);

		auto statements = parentT->getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1 } == statements);

		statements = parentT->getFromLeftArg(predecessor2);
		Assert::IsTrue(std::vector<StmtIndex> { successor2 } == statements);

		statements = parentT->getFromLeftArg(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromLeftArg_onePredMultSuc) {
		parentT->insert(predecessor1, successor1);
		parentT->insert(predecessor1, successor2);

		auto statements = parentT->getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1, successor2 } == statements);

		statements = parentT->getFromLeftArg(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromRightArg_onePredOneSuc) {
		parentT->insert(predecessor1, successor1);
		parentT->insert(predecessor2, successor2);

		auto statements = parentT->getFromRightArg(successor1);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor1} == statements);

		statements = parentT->getFromRightArg(successor2);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor2} == statements);

		statements = parentT->getFromRightArg(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromRightArg_multPredOneSuc) {
		std::vector<StmtIndex> expectedAns{ predecessor1, predecessor2 };

		parentT->insert(predecessor1, successor1);
		parentT->insert(predecessor2, successor1);
		auto statements = parentT->getFromRightArg(successor1);
		Assert::IsTrue(std::vector<StmtIndex> { predecessor1, predecessor2 } == statements);

		statements = parentT->getFromRightArg(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_contains_onePredOneSuc) {
		parentT->insert(predecessor1, successor1);

		Assert::IsTrue(parentT->contains(predecessor1, successor1));
		Assert::IsFalse(parentT->contains(successor1, predecessor1));
		Assert::IsFalse(parentT->contains(predecessor2, successor1));
		Assert::IsFalse(parentT->contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_contains_onePredMultSuc) {
		parentT->insert(predecessor1, successor1);
		parentT->insert(predecessor1, successor2);

		Assert::IsTrue(parentT->contains(predecessor1, successor1));
		Assert::IsFalse(parentT->contains(successor1, predecessor1));
		Assert::IsFalse(parentT->contains(predecessor2, successor1));
		Assert::IsTrue(parentT->contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_getAllInfo_onePredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		parentT->insert(predecessor1, successor1);
		parentT->insert(predecessor2, successor2);

		auto predSucInfo = parentT->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_onePredMultSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor1 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		parentT->insert(predecessor1, successor1);
		parentT->insert(predecessor1, successor2);

		auto predSucInfo = parentT->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_multPredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor1 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		parentT->insert(predecessor1, successor1);
		parentT->insert(predecessor2, successor1);

		auto predSucInfo = parentT->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};
	};
}
