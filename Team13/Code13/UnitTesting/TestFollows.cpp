#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/Follows.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestFollows) {
private:
	StmtIndex predecessor1 = { 1 };
	StmtIndex predecessor2 = { 2 };
	StmtIndex successor1 = { 3 };
	StmtIndex successor2 = { 4 };

	Follows* follows;

	TEST_METHOD_INITIALIZE(init) {
		follows = new Follows();
	}

public:
	TEST_METHOD(insert_getFromLeftArg_onePredOneSuc) {
		follows->insert(predecessor1, successor1);
		follows->insert(predecessor2, successor2);

		auto statements = follows->getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1 } == statements);

		statements = follows->getFromLeftArg(predecessor2);
		Assert::IsTrue(std::vector<StmtIndex> { successor2 } == statements);

		statements = follows->getFromLeftArg(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromLeftArg_onePredMultSuc) {
		follows->insert(predecessor1, successor1);
		follows->insert(predecessor1, successor2);

		auto statements = follows->getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1, successor2 } == statements);

		statements = follows->getFromLeftArg(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromRightArg_onePredOneSuc) {
		follows->insert(predecessor1, successor1);
		follows->insert(predecessor2, successor2);

		auto statements = follows->getFromRightArg(successor1);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor1} == statements);

		statements = follows->getFromRightArg(successor2);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor2} == statements);

		statements = follows->getFromRightArg(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromRightArg_multPredOneSuc) {
		std::vector<StmtIndex> expectedAns{ predecessor1, predecessor2 };

		follows->insert(predecessor1, successor1);
		follows->insert(predecessor2, successor1);
		auto statements = follows->getFromRightArg(successor1);
		Assert::IsTrue(std::vector<StmtIndex> { predecessor1, predecessor2 } == statements);

		statements = follows->getFromRightArg(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_contains_onePredOneSuc) {
		follows->insert(predecessor1, successor1);

		Assert::IsTrue(follows->contains(predecessor1, successor1));
		Assert::IsFalse(follows->contains(successor1, predecessor1));
		Assert::IsFalse(follows->contains(predecessor2, successor1));
		Assert::IsFalse(follows->contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_contains_onePredMultSuc) {
		follows->insert(predecessor1, successor1);
		follows->insert(predecessor1, successor2);

		Assert::IsTrue(follows->contains(predecessor1, successor1));
		Assert::IsFalse(follows->contains(successor1, predecessor1));
		Assert::IsFalse(follows->contains(predecessor2, successor1));
		Assert::IsTrue(follows->contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_getAllInfo_onePredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns = std::make_tuple(predecessors, successors);

		follows->insert(predecessor1, successor1);
		follows->insert(predecessor2, successor2);

		auto predSucInfo = follows->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_onePredMultSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor1 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns = std::make_tuple(predecessors, successors);

		follows->insert(predecessor1, successor1);
		follows->insert(predecessor1, successor2);

		auto predSucInfo = follows->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_multPredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor1 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns = std::make_tuple(predecessors, successors);

		follows->insert(predecessor1, successor1);
		follows->insert(predecessor2, successor1);

		auto predSucInfo = follows->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};
	};
}
