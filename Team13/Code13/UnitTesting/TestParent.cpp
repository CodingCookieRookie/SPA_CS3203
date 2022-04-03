#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "../source/PKB/Follows.h"
#include "../source/PKB/Parent.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestParent) {
private:
	StmtIndex predecessor1 = { 1 };
	StmtIndex predecessor2 = { 2 };
	StmtIndex successor1 = { 3 };
	StmtIndex successor2 = { 4 };

	TEST_METHOD_CLEANUP(cleanUpParent) {
		Parent::performCleanUp();
	}

public:
	TEST_METHOD(insert_getFromLeftArg_onePredOneSuc) {
		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor2, successor2);

		auto statements = Parent::getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1 } == statements);

		statements = Parent::getFromLeftArg(predecessor2);
		Assert::IsTrue(std::vector<StmtIndex> { successor2 } == statements);

		statements = Parent::getFromLeftArg(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromLeftArg_onePredMultSuc) {
		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor1, successor2);

		auto statements = Parent::getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1, successor2 } == statements);

		statements = Parent::getFromLeftArg(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromRightArg_onePredOneSuc) {
		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor2, successor2);

		auto statements = Parent::getFromRightArg(successor1);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor1} == statements);

		statements = Parent::getFromRightArg(successor2);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor2} == statements);

		statements = Parent::getFromRightArg(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromRightArg_multPredOneSuc) {
		std::vector<StmtIndex> expectedAns{ predecessor1, predecessor2 };

		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor2, successor1);
		auto statements = Parent::getFromRightArg(successor1);
		Assert::IsTrue(std::vector<StmtIndex> { predecessor1, predecessor2 } == statements);

		statements = Parent::getFromRightArg(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_contains_onePredOneSuc) {
		Parent::insert(predecessor1, successor1);

		Assert::IsTrue(Parent::contains(predecessor1, successor1));
		Assert::IsFalse(Parent::contains(successor1, predecessor1));
		Assert::IsFalse(Parent::contains(predecessor2, successor1));
		Assert::IsFalse(Parent::contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_contains_onePredMultSuc) {
		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor1, successor2);

		Assert::IsTrue(Parent::contains(predecessor1, successor1));
		Assert::IsFalse(Parent::contains(successor1, predecessor1));
		Assert::IsFalse(Parent::contains(predecessor2, successor1));
		Assert::IsTrue(Parent::contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_getAllInfo_onePredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns = std::make_tuple(predecessors, successors);

		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor2, successor2);

		auto predSucInfo = Parent::getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_onePredMultSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor1 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns = std::make_tuple(predecessors, successors);

		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor1, successor2);

		auto predSucInfo = Parent::getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_multPredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor1 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns = std::make_tuple(predecessors, successors);

		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor2, successor1);

		auto predSucInfo = Parent::getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getPredSucTable_onePredOneSuc) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> expectedAns;
		expectedAns[predecessor1].insert(successor1);
		expectedAns[predecessor2].insert(successor2);

		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor2, successor2);

		auto parentTable = Parent::getPredSucTable();
		Assert::IsTrue(expectedAns == parentTable);
	};

	TEST_METHOD(insert_getPredSucTable_onePredMultSuc) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> expectedAns;
		expectedAns[predecessor1].insert(successor1);
		expectedAns[predecessor1].insert(successor2);

		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor1, successor2);

		auto parentTable = Parent::getPredSucTable();
		Assert::IsTrue(expectedAns == parentTable);
	};
	};
}
