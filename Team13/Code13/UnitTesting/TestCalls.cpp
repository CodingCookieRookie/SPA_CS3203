#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "../source/PKB/Calls.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestCalls) {
private:
	ProcIndex predecessor1 = { 1 };
	ProcIndex predecessor2 = { 2 };
	ProcIndex successor1 = { 3 };
	ProcIndex successor2 = { 4 };

	TEST_METHOD_CLEANUP(cleanUpCalls) {
		Calls::performCleanUp();
	}

public:
	TEST_METHOD(insert_getSuccessors_onePredOneSuc) {
		Calls::insert(predecessor1, successor1);
		Calls::insert(predecessor2, successor2);

		auto procedures = Calls::getSuccessors(predecessor1);
		Assert::IsTrue(std::vector<ProcIndex> { successor1 } == procedures);

		procedures = Calls::getSuccessors(predecessor2);
		Assert::IsTrue(std::vector<ProcIndex> { successor2 } == procedures);

		procedures = Calls::getSuccessors(successor1);
		Assert::IsTrue(0 == procedures.size());
	};

	TEST_METHOD(insert_getSuccessors_onePredMultSuc) {
		Calls::insert(predecessor1, successor1);
		Calls::insert(predecessor1, successor2);

		auto procedures = Calls::getSuccessors(predecessor1);
		Assert::IsTrue(std::vector<ProcIndex> { successor1, successor2 } == procedures);

		procedures = Calls::getSuccessors(successor1);
		Assert::IsTrue(0 == procedures.size());
	};

	TEST_METHOD(insert_getPredecessors_onePredOneSuc) {
		Calls::insert(predecessor1, successor1);
		Calls::insert(predecessor2, successor2);

		auto procedures = Calls::getPredecessors(successor1);
		Assert::IsTrue(std::vector<ProcIndex> {predecessor1} == procedures);

		procedures = Calls::getPredecessors(successor2);
		Assert::IsTrue(std::vector<ProcIndex> {predecessor2} == procedures);

		procedures = Calls::getPredecessors(predecessor1);
		Assert::IsTrue(0 == procedures.size());
	};

	TEST_METHOD(insert_getPredecessors_multPredOneSuc) {
		std::vector<ProcIndex> expectedAns{ predecessor1, predecessor2 };

		Calls::insert(predecessor1, successor1);
		Calls::insert(predecessor2, successor1);
		auto procedures = Calls::getPredecessors(successor1);
		Assert::IsTrue(std::vector<ProcIndex> { predecessor1, predecessor2 } == procedures);

		procedures = Calls::getPredecessors(predecessor1);
		Assert::IsTrue(0 == procedures.size());
	};

	TEST_METHOD(insert_containsPredecessor_onePredOneSuc) {
		Calls::insert(predecessor1, successor1);

		Assert::IsTrue(Calls::containsPredecessor(predecessor1, successor1));
		Assert::IsFalse(Calls::containsPredecessor(successor1, predecessor1));
		Assert::IsFalse(Calls::containsPredecessor(predecessor2, successor1));
		Assert::IsFalse(Calls::containsPredecessor(predecessor1, successor2));
	};

	TEST_METHOD(insert_containsPredecessor_onePredMultSuc) {
		Calls::insert(predecessor1, successor1);
		Calls::insert(predecessor1, successor2);

		Assert::IsTrue(Calls::containsPredecessor(predecessor1, successor1));
		Assert::IsFalse(Calls::containsPredecessor(successor1, predecessor1));
		Assert::IsFalse(Calls::containsPredecessor(predecessor2, successor1));
		Assert::IsTrue(Calls::containsPredecessor(predecessor1, successor2));
	};

	TEST_METHOD(insert_containsSuccessor_onePredOneSuc) {
		Calls::insert(predecessor1, successor1);

		Assert::IsTrue(Calls::containsSuccessor(predecessor1, successor1));
		Assert::IsFalse(Calls::containsSuccessor(successor1, predecessor1));
		Assert::IsFalse(Calls::containsSuccessor(predecessor2, successor1));
		Assert::IsFalse(Calls::containsSuccessor(predecessor1, successor2));
	};

	TEST_METHOD(insert_containsSuccessor_onePredMultSuc) {
		Calls::insert(predecessor1, successor1);
		Calls::insert(predecessor1, successor2);

		Assert::IsTrue(Calls::containsSuccessor(predecessor1, successor1));
		Assert::IsFalse(Calls::containsSuccessor(successor1, predecessor1));
		Assert::IsFalse(Calls::containsSuccessor(predecessor2, successor1));
		Assert::IsTrue(Calls::containsSuccessor(predecessor1, successor2));
	};

	TEST_METHOD(insert_getAllPredecessorSuccessorInfo_onePredOneSuc) {
		std::vector<ProcIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<ProcIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<ProcIndex>, std::vector<ProcIndex>> expectedAns = std::make_tuple(predecessors, successors);

		Calls::insert(predecessor1, successor1);
		Calls::insert(predecessor2, successor2);

		auto predSucInfo = Calls::getAllPredecessorSuccessorInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllPredecessorSuccessorInfo_onePredMultSuc) {
		std::vector<ProcIndex> predecessors{ predecessor1, predecessor1 };
		std::vector<ProcIndex> successors{ successor1, successor2 };
		std::tuple<std::vector<ProcIndex>, std::vector<ProcIndex>> expectedAns = std::make_tuple(predecessors, successors);

		Calls::insert(predecessor1, successor1);
		Calls::insert(predecessor1, successor2);

		auto predSucInfo = Calls::getAllPredecessorSuccessorInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getAllPredecessorSuccessorInfo_multPredOneSuc) {
		std::vector<ProcIndex> predecessors{ predecessor1, predecessor2 };
		std::vector<ProcIndex> successors{ successor1, successor1 };
		std::tuple<std::vector<ProcIndex>, std::vector<ProcIndex>> expectedAns = std::make_tuple(predecessors, successors);

		Calls::insert(predecessor1, successor1);
		Calls::insert(predecessor2, successor1);

		auto predSucInfo = Calls::getAllPredecessorSuccessorInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(insert_getPredSucTable_onePredOneSuc) {
		std::unordered_map<ProcIndex, std::unordered_set<ProcIndex>> expectedAns;
		expectedAns[predecessor1].insert(successor1);
		expectedAns[predecessor2].insert(successor2);

		Calls::insert(predecessor1, successor1);
		Calls::insert(predecessor2, successor2);

		auto callsTable = Calls::getPredSucTable();
		Assert::IsTrue(expectedAns == callsTable);
	};

	TEST_METHOD(insert_getPredSucTable_onePredMultSuc) {
		std::unordered_map<ProcIndex, std::unordered_set<ProcIndex>> expectedAns;
		expectedAns[predecessor1].insert(successor1);
		expectedAns[predecessor1].insert(successor2);

		Calls::insert(predecessor1, successor1);
		Calls::insert(predecessor1, successor2);

		auto callsTable = Calls::getPredSucTable();
		Assert::IsTrue(expectedAns == callsTable);
	};
	};
}
