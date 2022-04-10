#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/Relationship/Next.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestNext) {
private:
	StmtIndex predecessor1 = { 1 };
	StmtIndex predecessor2 = { 2 };
	StmtIndex successor1 = { 3 };
	StmtIndex successor2 = { 4 };

	Next* next;

	TEST_METHOD_INITIALIZE(init) {
		next = new Next();
	}

public:
	TEST_METHOD(insert_getFromLeftArg_onePredOneSuc) {
		next->insert(predecessor1, successor1);
		next->insert(predecessor2, successor2);

		auto statements = next->getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1 } == statements);

		statements = next->getFromLeftArg(predecessor2);
		Assert::IsTrue(std::vector<StmtIndex> { successor2 } == statements);

		statements = next->getFromLeftArg(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromLeftArg_onePredMultSuc) {
		next->insert(predecessor1, successor1);
		next->insert(predecessor1, successor2);

		auto statements = next->getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1, successor2 } == statements);

		statements = next->getFromLeftArg(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromRightArg_onePredOneSuc) {
		next->insert(predecessor1, successor1);
		next->insert(predecessor2, successor2);

		auto statements = next->getFromRightArg(successor1);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor1} == statements);

		statements = next->getFromRightArg(successor2);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor2} == statements);

		statements = next->getFromRightArg(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromRightArg_multPredOneSuc) {
		std::vector<StmtIndex> expectedAns{ predecessor1, predecessor2 };

		next->insert(predecessor1, successor1);
		next->insert(predecessor2, successor1);
		auto statements = next->getFromRightArg(successor1);
		Assert::IsTrue(std::vector<StmtIndex> { predecessor1, predecessor2 } == statements);

		statements = next->getFromRightArg(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_contains_onePredOneSuc) {
		next->insert(predecessor1, successor1);

		Assert::IsTrue(next->contains(predecessor1, successor1));
		Assert::IsFalse(next->contains(successor1, predecessor1));
		Assert::IsFalse(next->contains(predecessor2, successor1));
		Assert::IsFalse(next->contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_contains_onePredMultSuc) {
		next->insert(predecessor1, successor1);
		next->insert(predecessor1, successor2);

		Assert::IsTrue(next->contains(predecessor1, successor1));
		Assert::IsFalse(next->contains(successor1, predecessor1));
		Assert::IsFalse(next->contains(predecessor2, successor1));
		Assert::IsTrue(next->contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_getAllInfo_onePredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		next->insert(predecessor1, successor1);
		next->insert(predecessor2, successor2);

		auto predSucInfo = next->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_onePredMultSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor1 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		next->insert(predecessor1, successor1);
		next->insert(predecessor1, successor2);

		auto predSucInfo = next->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_multPredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor1 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns =
			std::make_tuple(predecessors, successors);

		next->insert(predecessor1, successor1);
		next->insert(predecessor2, successor1);

		auto predSucInfo = next->getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};
	};
}
