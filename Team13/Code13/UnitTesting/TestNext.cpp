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
	TEST_METHOD(insert_getSuccessors_onePredOneSuc) {
		Next::insert(predecessor1, successor1);
		Next::insert(predecessor2, successor2);

		auto statements = Next::getSuccessors(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1 } == statements);

		statements = Next::getSuccessors(predecessor2);
		Assert::IsTrue(std::vector<StmtIndex> { successor2 } == statements);

		statements = Next::getSuccessors(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getSuccessors_onePredMultSuc) {
		Next::insert(predecessor1, successor1);
		Next::insert(predecessor1, successor2);

		auto statements = Next::getSuccessors(predecessor1);
		Assert::IsTrue(std::vector<StmtIndex> { successor1, successor2 } == statements);

		statements = Next::getSuccessors(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getPredecessors_onePredOneSuc) {
		Next::insert(predecessor1, successor1);
		Next::insert(predecessor2, successor2);

		auto statements = Next::getPredecessors(successor1);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor1} == statements);

		statements = Next::getPredecessors(successor2);
		Assert::IsTrue(std::vector<StmtIndex> {predecessor2} == statements);

		statements = Next::getPredecessors(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getPredecessors_multPredOneSuc) {
		std::vector<StmtIndex> expectedAns{ predecessor1, predecessor2 };

		Next::insert(predecessor1, successor1);
		Next::insert(predecessor2, successor1);
		auto statements = Next::getPredecessors(successor1);
		Assert::IsTrue(std::vector<StmtIndex> { predecessor1, predecessor2 } == statements);

		statements = Next::getPredecessors(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_containsPredecessor_onePredOneSuc) {
		Next::insert(predecessor1, successor1);

		Assert::IsTrue(Next::containsPredecessor(predecessor1, successor1));
		Assert::IsFalse(Next::containsPredecessor(successor1, predecessor1));
		Assert::IsFalse(Next::containsPredecessor(predecessor2, successor1));
		Assert::IsFalse(Next::containsPredecessor(predecessor1, successor2));
	};

	TEST_METHOD(insert_containsPredecessor_onePredMultSuc) {
		Next::insert(predecessor1, successor1);
		Next::insert(predecessor1, successor2);

		Assert::IsTrue(Next::containsPredecessor(predecessor1, successor1));
		Assert::IsFalse(Next::containsPredecessor(successor1, predecessor1));
		Assert::IsFalse(Next::containsPredecessor(predecessor2, successor1));
		Assert::IsTrue(Next::containsPredecessor(predecessor1, successor2));
	};

	TEST_METHOD(insert_containsSuccessor_onePredOneSuc) {
		Next::insert(predecessor1, successor1);

		Assert::IsTrue(Next::containsSuccessor(predecessor1, successor1));
		Assert::IsFalse(Next::containsSuccessor(successor1, predecessor1));
		Assert::IsFalse(Next::containsSuccessor(predecessor2, successor1));
		Assert::IsFalse(Next::containsSuccessor(predecessor1, successor2));
	};

	TEST_METHOD(insert_containsSuccessor_onePredMultSuc) {
		Next::insert(predecessor1, successor1);
		Next::insert(predecessor1, successor2);

		Assert::IsTrue(Next::containsSuccessor(predecessor1, successor1));
		Assert::IsFalse(Next::containsSuccessor(successor1, predecessor1));
		Assert::IsFalse(Next::containsSuccessor(predecessor2, successor1));
		Assert::IsTrue(Next::containsSuccessor(predecessor1, successor2));
	};

	TEST_METHOD(insert_getAllPredecessorSuccessorInfo_onePredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns = std::make_tuple(predecessors, successors);

		Next::insert(predecessor1, successor1);
		Next::insert(predecessor2, successor2);

		auto predSucInfo = Next::getAllPredecessorSuccessorInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllPredecessorSuccessorInfo_onePredMultSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor1 };
		std::vector<StmtIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns = std::make_tuple(predecessors, successors);

		Next::insert(predecessor1, successor1);
		Next::insert(predecessor1, successor2);

		auto predSucInfo = Next::getAllPredecessorSuccessorInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllPredecessorSuccessorInfo_multPredOneSuc) {
		std::vector<StmtIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<StmtIndex> successors{ successor1, successor1 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedAns = std::make_tuple(predecessors, successors);

		Next::insert(predecessor1, successor1);
		Next::insert(predecessor2, successor1);

		auto predSucInfo = Next::getAllPredecessorSuccessorInfo();
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
