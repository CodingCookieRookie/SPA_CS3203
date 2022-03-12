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
	TEST_METHOD(insert_getSuccessors_onePredOneSuc) {
		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor2, successor2);

		auto statements = Parent::getSuccessors(predecessor1);
		Assert::IsTrue(std::vector<int> { successor1.index } == statements);

		statements = Parent::getSuccessors(predecessor2);
		Assert::IsTrue(std::vector<int> { successor2.index } == statements);

		statements = Parent::getSuccessors(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getSuccessors_onePredMultSuc) {
		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor1, successor2);

		auto statements = Parent::getSuccessors(predecessor1);
		Assert::IsTrue(std::vector<int> { successor1.index, successor2.index } == statements);

		statements = Parent::getSuccessors(successor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getPredecessors_onePredOneSuc) {
		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor2, successor2);

		auto statements = Parent::getPredecessors(successor1);
		Assert::IsTrue(std::vector<int> {predecessor1.index} == statements);

		statements = Parent::getPredecessors(successor2);
		Assert::IsTrue(std::vector<int> {predecessor2.index} == statements);

		statements = Parent::getPredecessors(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_getPredecessors_multPredOneSuc) {
		std::vector<int> expectedAns{ predecessor1.index, predecessor2.index };

		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor2, successor1);
		auto statements = Parent::getPredecessors(successor1);
		Assert::IsTrue(std::vector<int> { predecessor1.index, predecessor2.index } == statements);

		statements = Parent::getPredecessors(predecessor1);
		Assert::IsTrue(0 == statements.size());
	};

	TEST_METHOD(insert_containsPredecessor_onePredOneSuc) {
		Parent::insert(predecessor1, successor1);

		Assert::IsTrue(Parent::containsPredecessor(predecessor1, successor1));
		Assert::IsFalse(Parent::containsPredecessor(successor1, predecessor1));
		Assert::IsFalse(Parent::containsPredecessor(predecessor2, successor1));
		Assert::IsFalse(Parent::containsPredecessor(predecessor1, successor2));
	};

	TEST_METHOD(insert_containsPredecessor_onePredMultSuc) {
		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor1, successor2);

		Assert::IsTrue(Parent::containsPredecessor(predecessor1, successor1));
		Assert::IsFalse(Parent::containsPredecessor(successor1, predecessor1));
		Assert::IsFalse(Parent::containsPredecessor(predecessor2, successor1));
		Assert::IsTrue(Parent::containsPredecessor(predecessor1, successor2));
	};

	TEST_METHOD(insert_containsSuccessor_onePredOneSuc) {
		Parent::insert(predecessor1, successor1);

		Assert::IsTrue(Parent::containsSuccessor(predecessor1, successor1));
		Assert::IsFalse(Parent::containsSuccessor(successor1, predecessor1));
		Assert::IsFalse(Parent::containsSuccessor(predecessor2, successor1));
		Assert::IsFalse(Parent::containsSuccessor(predecessor1, successor2));
	};

	TEST_METHOD(insert_containsSuccessor_onePredMultSuc) {
		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor1, successor2);

		Assert::IsTrue(Parent::containsSuccessor(predecessor1, successor1));
		Assert::IsFalse(Parent::containsSuccessor(successor1, predecessor1));
		Assert::IsFalse(Parent::containsSuccessor(predecessor2, successor1));
		Assert::IsTrue(Parent::containsSuccessor(predecessor1, successor2));
	};

	TEST_METHOD(insert_getAllPredecessorSuccessorInfo_onePredOneSuc) {
		std::vector<int> predecessors{ predecessor1.index, predecessor2.index };
		std::vector<int> successors{ successor1.index, successor2.index };
		std::tuple<std::vector<int>, std::vector<int>> expectedAns = std::make_tuple(predecessors, successors);

		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor2, successor2);

		auto predSucInfo = Parent::getAllPredecessorSuccessorInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllPredecessorSuccessorInfo_onePredMultSuc) {
		std::vector<int> predecessors{ predecessor1.index, predecessor1.index };
		std::vector<int> successors{ successor1.index, successor2.index };
		std::tuple<std::vector<int>, std::vector<int>> expectedAns = std::make_tuple(predecessors, successors);

		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor1, successor2);

		auto predSucInfo = Parent::getAllPredecessorSuccessorInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllPredecessorSuccessorInfo_multPredOneSuc) {
		std::vector<int> predecessors{ predecessor1.index, predecessor2.index };
		std::vector<int> successors{ successor1.index, successor1.index };
		std::tuple<std::vector<int>, std::vector<int>> expectedAns = std::make_tuple(predecessors, successors);

		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor2, successor1);

		auto predSucInfo = Parent::getAllPredecessorSuccessorInfo();
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
