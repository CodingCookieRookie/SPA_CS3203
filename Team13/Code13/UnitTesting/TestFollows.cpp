#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "../source/PKB/Follows.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestFollows) {
private:
	StmtIndex predecessor1 = { 1 };
	StmtIndex predecessor2 = { 2 };
	StmtIndex successor1 = { 3 };
	StmtIndex successor2 = { 4 };

	TEST_METHOD_CLEANUP(cleanUpFollows) {
		Follows::performCleanUp();
	}

public:
	TEST_METHOD(insert_getFromLeftArg_onePredOneSuc) {
		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor2, successor2);

		auto statements = Follows::getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1 } == statements);

		statements = Follows::getFromLeftArg(predecessor2);
		Assert::IsTrue(std::vector<StmtIndex> { successor2 } == statements);

		statements = Follows::getFromLeftArg(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromLeftArg_onePredMultSuc) {
		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor1, successor2);

		auto statements = Follows::getFromLeftArg(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1, successor2 } == statements);

		statements = Follows::getFromLeftArg(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromRightArg_onePredOneSuc) {
		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor2, successor2);

		auto statements = Follows::getFromRightArg(successor1);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor1} == statements);

		statements = Follows::getFromRightArg(successor2);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor2} == statements);

		statements = Follows::getFromRightArg(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getFromRightArg_multPredOneSuc) {
		std::vector<StmtIndex> expectedAns{ predecessor1, predecessor2 };

		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor2, successor1);
		auto statements = Follows::getFromRightArg(successor1);
		Assert::IsTrue(std::vector<StmtIndex> { predecessor1, predecessor2 } == statements);

		statements = Follows::getFromRightArg(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_contains_onePredOneSuc) {
		Follows::insert(predecessor1, successor1);

		Assert::IsTrue(Follows::contains(predecessor1, successor1));
		Assert::IsFalse(Follows::contains(successor1, predecessor1));
		Assert::IsFalse(Follows::contains(predecessor2, successor1));
		Assert::IsFalse(Follows::contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_contains_onePredMultSuc) {
		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor1, successor2);

		Assert::IsTrue(Follows::contains(predecessor1, successor1));
		Assert::IsFalse(Follows::contains(successor1, predecessor1));
		Assert::IsFalse(Follows::contains(predecessor2, successor1));
		Assert::IsTrue(Follows::contains(predecessor1, successor2));
	};

	TEST_METHOD(insert_getAllInfo_onePredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns = std::make_tuple(predecessors, successors);

		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor2, successor2);

		auto predSucInfo = Follows::getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_onePredMultSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor1 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns = std::make_tuple(predecessors, successors);

		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor1, successor2);

		auto predSucInfo = Follows::getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllInfo_multPredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor1 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns = std::make_tuple(predecessors, successors);

		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor2, successor1);

		auto predSucInfo = Follows::getAllInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getPredSucTable_onePredOneSuc) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> expectedAns;
		expectedAns[predecessor1].insert(successor1);
		expectedAns[predecessor2].insert(successor2);

		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor2, successor2);

		auto followsTable = Follows::getPredSucTable();
		Assert::IsTrue(expectedAns == followsTable);
	};

	TEST_METHOD(insert_getPredSucTable_onePredMultSuc) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> expectedAns;
		expectedAns[predecessor1].insert(successor1);
		expectedAns[predecessor1].insert(successor2);

		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor1, successor2);

		auto followsTable = Follows::getPredSucTable();
		Assert::IsTrue(expectedAns == followsTable);
	};
	};
}
