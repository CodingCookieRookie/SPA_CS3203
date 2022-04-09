#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/Parent.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestParent) {
private:
	StmtIndex predecessor1 = { 1 };
	StmtIndex predecessor2 = { 2 };
	StmtIndex successor1 = { 3 };
	StmtIndex successor2 = { 4 };

	Parent* parent;

	TEST_METHOD_INITIALIZE(init) {
		parent = new Parent();
	}

public:
	TEST_METHOD(insert_getFromLeftArg_onePredOneSuc) {
		parent->insert(predecessor1, successor1);
		parent->insert(predecessor2, successor2);

		auto statements = parent->getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1 } == statements);

		statements = parent->getFromLeftArg(predecessor2);
		Assert::IsTrue(std::vector<StmtIndex> { successor2 } == statements);

		statements = parent->getFromLeftArg(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromLeftArg_onePredMultSuc) {
		parent->insert(predecessor1, successor1);
		parent->insert(predecessor1, successor2);

		auto statements = parent->getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1, successor2 } == statements);

		statements = parent->getFromLeftArg(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromRightArg_onePredOneSuc) {
		parent->insert(predecessor1, successor1);
		parent->insert(predecessor2, successor2);

		auto statements = parent->getFromRightArg(successor1);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor1} == statements);

		statements = parent->getFromRightArg(successor2);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor2} == statements);

		statements = parent->getFromRightArg(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromRightArg_multPredOneSuc) {
		std::vector<StmtIndex> expectedAns{ predecessor1, predecessor2 };

		parent->insert(predecessor1, successor1);
		parent->insert(predecessor2, successor1);
		auto statements = parent->getFromRightArg(successor1);
		Assert::IsTrue(std::vector<StmtIndex> { predecessor1, predecessor2 } == statements);

		statements = parent->getFromRightArg(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_contains_onePredOneSuc) {
		parent->insert(predecessor1, successor1);

		Assert::IsTrue(parent->contains(predecessor1, successor1));
		Assert::IsFalse(parent->contains(successor1, predecessor1));
		Assert::IsFalse(parent->contains(predecessor2, successor1));
		Assert::IsFalse(parent->contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_contains_onePredMultSuc) {
		parent->insert(predecessor1, successor1);
		parent->insert(predecessor1, successor2);

		Assert::IsTrue(parent->contains(predecessor1, successor1));
		Assert::IsFalse(parent->contains(successor1, predecessor1));
		Assert::IsFalse(parent->contains(predecessor2, successor1));
		Assert::IsTrue(parent->contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_getAllInfo_onePredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		parent->insert(predecessor1, successor1);
		parent->insert(predecessor2, successor2);

		auto predSucInfo = parent->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_onePredMultSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor1 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		parent->insert(predecessor1, successor1);
		parent->insert(predecessor1, successor2);

		auto predSucInfo = parent->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_multPredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor1 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		parent->insert(predecessor1, successor1);
		parent->insert(predecessor2, successor1);

		auto predSucInfo = parent->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};
	};
}
