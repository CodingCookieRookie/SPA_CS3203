#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/common/Types.h"
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
	TEST_METHOD(insert_getSuccessorStmts) {
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> expectedAns;
		expectedAns.insert(successor1);
		expectedAns.insert(successor2);

		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor1, successor2);
		auto statements = Follows::getSuccessorStmts(predecessor1);
		Assert::IsTrue(expectedAns == statements);

		statements = Follows::getSuccessorStmts(successor1);
		Assert::IsTrue(0 == statements.size());

		/*TODO: When parent code is added, check it does not get affected*/
	};

	TEST_METHOD(insert_getPredecessorStmts) {
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> expectedAns;
		expectedAns.insert(predecessor1);

		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor1, successor2);
		auto statements = Follows::getPredecessorStmts(successor1);
		Assert::IsTrue(expectedAns == statements);

		statements = Follows::getPredecessorStmts(successor2);
		Assert::IsTrue(expectedAns == statements);

		statements = Follows::getPredecessorStmts(predecessor1);
		Assert::IsTrue(0 == statements.size());

		/*TODO: When parent code is added, check it does not get affected*/
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
		std::vector<std::tuple<StmtIndex, StmtIndex>> expectedAns;
		expectedAns.push_back(std::make_tuple(predecessor1, successor1));
		expectedAns.push_back(std::make_tuple(predecessor2, successor2));

		Follows::insert(predecessor1, successor1);
		Follows::insert(predecessor2, successor2);

		auto predSucInfo = Follows::getAllPredecessorSuccessorInfo();
		Assert::IsTrue(expectedAns == predSucInfo);
	};
	};
}
