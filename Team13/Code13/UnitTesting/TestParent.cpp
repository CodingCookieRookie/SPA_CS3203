#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/common/Types.h"
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
	TEST_METHOD(insert_getSuccessorStmts) {
		std::vector<int> expectedAns{ successor1.index, successor2.index };

		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor1, successor2);
		auto statements = Parent::getSuccessorStmts(predecessor1);
		Assert::IsTrue(expectedAns == statements);

		statements = Parent::getSuccessorStmts(successor1);
		Assert::IsTrue(0 == statements.size());

		/* Check Follows data does not get affected */
		auto followsStmts = Follows::getSuccessorStmts(predecessor1);
		Assert::IsTrue(0 == followsStmts.size());
		Follows::performCleanUp();
	};

	TEST_METHOD(insert_getPredecessorStmts) {
		std::vector<int> expectedAns{ predecessor1.index };

		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor1, successor2);
		auto statements = Parent::getPredecessorStmts(successor1);
		Assert::IsTrue(expectedAns == statements);

		statements = Parent::getPredecessorStmts(successor2);
		Assert::IsTrue(expectedAns == statements);

		statements = Parent::getPredecessorStmts(predecessor1);
		Assert::IsTrue(0 == statements.size());

		/* Check Follows data does not get affected */
		auto followsStmts = Follows::getPredecessorStmts(predecessor1);
		Assert::IsTrue(0 == followsStmts.size());
		Follows::performCleanUp();
	};

	TEST_METHOD(containsPredecessor) {
		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor2, successor2);

		Assert::IsTrue(Parent::containsPredecessor(predecessor1, successor1));
		Assert::IsFalse(Parent::containsPredecessor(successor1, predecessor1));
		Assert::IsFalse(Parent::containsPredecessor(predecessor2, successor1));
		Assert::IsFalse(Parent::containsPredecessor(predecessor1, successor2));
	};

	TEST_METHOD(containsSuccessor) {
		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor2, successor2);

		Assert::IsTrue(Parent::containsSuccessor(predecessor1, successor1));
		Assert::IsFalse(Parent::containsSuccessor(successor1, predecessor1));
		Assert::IsFalse(Parent::containsSuccessor(predecessor2, successor1));
		Assert::IsFalse(Parent::containsSuccessor(predecessor1, successor2));
	};

	TEST_METHOD(getAllPredecessorSuccessorInfo) {
		std::vector<int> predecessors{ predecessor1.index, predecessor2.index };
		std::vector<int> successors{ successor1.index, successor2.index };
		std::tuple<std::vector<int>, std::vector<int>> expectedAns = std::make_tuple(predecessors, successors);

		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor2, successor2);

		auto predSucInfo = Parent::getAllPredecessorSuccessorInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(getPredSucTable) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
			StmtIndex::HashFunction> expectedAns;
		expectedAns[predecessor1].insert(successor1);
		expectedAns[predecessor2].insert(successor2);

		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor2, successor2);

		auto followsTable = Parent::getPredSucTable();

		Assert::IsTrue(expectedAns == followsTable);
	};

	TEST_METHOD(getSucPredTable) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
			StmtIndex::HashFunction> expectedAns;
		expectedAns[successor1].insert(predecessor1);
		expectedAns[successor2].insert(predecessor2);

		Parent::insert(predecessor1, successor1);
		Parent::insert(predecessor2, successor2);

		auto followsTable = Parent::getSucPredTable();

		Assert::IsTrue(expectedAns == followsTable);
	};
	};
}
