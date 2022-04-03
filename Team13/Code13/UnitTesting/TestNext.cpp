#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "../source/PKB/Next.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestNext) {
private:
	StmtIndex predecessor1 = { 1 };
	StmtIndex predecessor2 = { 2 };
	StmtIndex successor1 = { 3 };
	StmtIndex successor2 = { 4 };

	TEST_METHOD_CLEANUP(cleanUpNext) {
		Next::performCleanUp();
	}

public:
	TEST_METHOD(insert_getFromLeftArg_onePredOneSuc) {
		Next::insert(predecessor1, successor1);
		Next::insert(predecessor2, successor2);

		auto statements = Next::getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1 } == statements);

		statements = Next::getFromLeftArg(predecessor2);
		Assert::IsTrue(std::vector<StmtIndex> { successor2 } == statements);

		statements = Next::getFromLeftArg(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromLeftArg_onePredMultSuc) {
		Next::insert(predecessor1, successor1);
		Next::insert(predecessor1, successor2);

		auto statements = Next::getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1, successor2 } == statements);

		statements = Next::getFromLeftArg(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromRightArg_onePredOneSuc) {
		Next::insert(predecessor1, successor1);
		Next::insert(predecessor2, successor2);

		auto statements = Next::getFromRightArg(successor1);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor1} == statements);

		statements = Next::getFromRightArg(successor2);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor2} == statements);

		statements = Next::getFromRightArg(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromRightArg_multPredOneSuc) {
		std::vector<StmtIndex> expectedAns{ predecessor1, predecessor2 };

		Next::insert(predecessor1, successor1);
		Next::insert(predecessor2, successor1);
		auto statements = Next::getFromRightArg(successor1);
		Assert::IsTrue(std::vector<StmtIndex> { predecessor1, predecessor2 } == statements);

		statements = Next::getFromRightArg(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_contains_onePredOneSuc) {
		Next::insert(predecessor1, successor1);

		Assert::IsTrue(Next::contains(predecessor1, successor1));
		Assert::IsFalse(Next::contains(successor1, predecessor1));
		Assert::IsFalse(Next::contains(predecessor2, successor1));
		Assert::IsFalse(Next::contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_contains_onePredMultSuc) {
		Next::insert(predecessor1, successor1);
		Next::insert(predecessor1, successor2);

		Assert::IsTrue(Next::contains(predecessor1, successor1));
		Assert::IsFalse(Next::contains(successor1, predecessor1));
		Assert::IsFalse(Next::contains(predecessor2, successor1));
		Assert::IsTrue(Next::contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_getAllInfo_onePredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns = std::make_tuple(predecessors, successors);

		Next::insert(predecessor1, successor1);
		Next::insert(predecessor2, successor2);

		auto predSucInfo = Next::getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_onePredMultSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor1 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns = std::make_tuple(predecessors, successors);

		Next::insert(predecessor1, successor1);
		Next::insert(predecessor1, successor2);

		auto predSucInfo = Next::getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_multPredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor1 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns = std::make_tuple(predecessors, successors);

		Next::insert(predecessor1, successor1);
		Next::insert(predecessor2, successor1);

		auto predSucInfo = Next::getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getPredSucTable_onePredOneSuc) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> expectedAns;
		expectedAns[predecessor1].insert(successor1);
		expectedAns[predecessor2].insert(successor2);

		Next::insert(predecessor1, successor1);
		Next::insert(predecessor2, successor2);

		auto nextTable = Next::getPredSucTable();
		Assert::IsTrue(expectedAns == nextTable);
	};

	TEST_METHOD(insert_getPredSucTable_onePredMultSuc) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> expectedAns;
		expectedAns[predecessor1].insert(successor1);
		expectedAns[predecessor1].insert(successor2);

		Next::insert(predecessor1, successor1);
		Next::insert(predecessor1, successor2);

		auto nextTable = Next::getPredSucTable();
		Assert::IsTrue(expectedAns == nextTable);
	};
	};
}
