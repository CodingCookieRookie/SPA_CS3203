#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/common/Types.h"
#include "../source/PKB/Follows.h"
#include "../source/PKB/Parent.h"

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
	TEST_METHOD(insert_getSuccessorStmts) {
		std::vector<int> expectedAns{ successor1.index, successor2.index };

		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor1, successor2);
		auto statements = Follows::getSuccessorStmts(predecessor1);
		Assert::IsTrue(expectedAns == statements);

		statements = Follows::getSuccessorStmts(successor1);
		Assert::IsTrue(0 == statements.size());

		/*Check Parents data does not get affected*/
		auto parentStmts = Parent::getSuccessorStmts(predecessor1);
		Assert::IsTrue(0 == parentStmts.size());
		Parent::performCleanUp();
	};

	TEST_METHOD(insert_getPredecessorStmts) {
		std::vector<int> expectedAns{ predecessor1.index };

		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor1, successor2);
		auto statements = Follows::getPredecessorStmts(successor1);
		Assert::IsTrue(expectedAns == statements);

		statements = Follows::getPredecessorStmts(successor2);
		Assert::IsTrue(expectedAns == statements);

		statements = Follows::getPredecessorStmts(predecessor1);
		Assert::IsTrue(0 == statements.size());

		/*Check Parents data does not get affected*/
		auto parentStmts = Parent::getPredecessorStmts(predecessor1);
		Assert::IsTrue(0 == parentStmts.size());
		Parent::performCleanUp();
	};

	TEST_METHOD(containsPredecessor) {
		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor2, successor2);

		Assert::IsTrue(Follows::containsPredecessor(predecessor1, successor1));
		Assert::IsFalse(Follows::containsPredecessor(successor1, predecessor1));
		Assert::IsFalse(Follows::containsPredecessor(predecessor2, successor1));
		Assert::IsFalse(Follows::containsPredecessor(predecessor1, successor2));
	};

	TEST_METHOD(containsSuccessor) {
		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor2, successor2);

		Assert::IsTrue(Follows::containsSuccessor(predecessor1, successor1));
		Assert::IsFalse(Follows::containsSuccessor(successor1, predecessor1));
		Assert::IsFalse(Follows::containsSuccessor(predecessor2, successor1));
		Assert::IsFalse(Follows::containsSuccessor(predecessor1, successor2));
	};

	TEST_METHOD(getAllPredecessorSuccessorInfo) {
		std::vector<int> predecessors{ predecessor1.index, predecessor2.index };
		std::vector<int> successors{ successor1.index, successor2.index };
		std::tuple<std::vector<int>, std::vector<int>> expectedAns = std::make_tuple(predecessors, successors);

		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor2, successor2);

		auto predSucInfo = Follows::getAllPredecessorSuccessorInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};

	TEST_METHOD(getPredSucTable) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
			StmtIndex::HashFunction> expectedAns;
		expectedAns[predecessor1].insert(successor1);
		expectedAns[predecessor2].insert(successor2);

		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor2, successor2);

		auto followsTable = Follows::getPredSucTable();

		Assert::IsTrue(expectedAns == followsTable);
	};

	TEST_METHOD(getSucPredTable) {
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
			StmtIndex::HashFunction> expectedAns;
		expectedAns[successor1].insert(predecessor1);
		expectedAns[successor2].insert(predecessor2);

		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor2, successor2);

		auto followsTable = Follows::getSucPredTable();

		Assert::IsTrue(expectedAns == followsTable);
	};
	};
}
