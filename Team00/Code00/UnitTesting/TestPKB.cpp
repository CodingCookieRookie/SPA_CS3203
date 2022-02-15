#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPKB) {
private:
	StmtIndex stmtIdx2 = StmtIndex(2);
	StmtIndex stmtIdx3 = StmtIndex(3);
	StmtIndex stmtIdx4 = StmtIndex(4);

	VarIndex varIdx1 = VarIndex(5);
	VarIndex varIdx2 = VarIndex(6);

	TEST_METHOD_CLEANUP(cleanUpPKB) {
		Entity::performCleanUp();
		Container::performCleanUp();
		Uses::performCleanUp();
		Modifies::performCleanUp();
		Follows::performCleanUp();
		FollowsT::performCleanUp();
		Parent::performCleanUp();
		ParentT::performCleanUp();
	}

public:
	TEST_METHOD(populateRecursiveInfo_1) {
		/*if { stmt3; stmt4; while(){}; }*/
		StmtIndex stmtIdx1 = Entity::insertStmt(StatementType::ifType);
		StmtIndex stmtIdx2 = Entity::insertStmt(StatementType::whileType);

		Container::insertStmtInContainer(stmtIdx1, stmtIdx3);
		Container::insertStmtInContainer(stmtIdx1, stmtIdx4);
		Container::insertStmtInContainer(stmtIdx1, stmtIdx2);

		Uses::insert(stmtIdx3, varIdx1);
		Modifies::insert(stmtIdx3, varIdx2);

		Follows::insert(stmtIdx3, stmtIdx4);
		Follows::insert(stmtIdx4, stmtIdx2);

		Parent::insert(stmtIdx1, stmtIdx3);
		Parent::insert(stmtIdx1, stmtIdx4);
		Parent::insert(stmtIdx1, stmtIdx2);

		PKB::populateRecursiveInfo();

		/*Check Uses*/
		Assert::IsTrue(std::vector<int>{ varIdx1.index } == Uses::getVariables(stmtIdx1));
		Assert::IsTrue(0 == Uses::getVariables(stmtIdx2).size());

		/*Check Modifies*/
		Assert::IsTrue(std::vector<int>{varIdx2.index} == Modifies::getVariables(stmtIdx1));
		Assert::IsTrue(0 == Modifies::getVariables(stmtIdx2).size());

		/*Check Container*/
		Assert::IsTrue(std::unordered_set<StmtIndex, StmtIndex::HashFunction>{stmtIdx3, stmtIdx4, stmtIdx2} ==
			Container::getStmtsInContainer(stmtIdx1));

		/*Check FollowsT*/
		Assert::IsTrue(std::vector<int>{stmtIdx4.index, stmtIdx2.index} ==
			FollowsT::getSuccessorStmts(stmtIdx3));

		/*Check ParentT*/
		Assert::IsTrue(std::vector<int>{stmtIdx3.index, stmtIdx4.index, stmtIdx2.index} ==
			ParentT::getSuccessorStmts(stmtIdx1));
	};

	TEST_METHOD(populateRecursiveInfo_2) {
		/*if { stmt3; while(){ stmt4; }; }*/
		StmtIndex stmtIdx1 = Entity::insertStmt(StatementType::ifType);
		StmtIndex stmtIdx2 = Entity::insertStmt(StatementType::whileType);

		Container::insertStmtInContainer(stmtIdx1, stmtIdx3);
		Container::insertStmtInContainer(stmtIdx1, stmtIdx2);
		Container::insertStmtInContainer(stmtIdx2, stmtIdx4);

		Uses::insert(stmtIdx3, varIdx1);
		Modifies::insert(stmtIdx3, varIdx2);
		Uses::insert(stmtIdx4, varIdx2);
		Modifies::insert(stmtIdx4, varIdx1);

		Follows::insert(stmtIdx3, stmtIdx2);

		Parent::insert(stmtIdx1, stmtIdx3);
		Parent::insert(stmtIdx2, stmtIdx4);
		Parent::insert(stmtIdx1, stmtIdx2);

		PKB::populateRecursiveInfo();

		/*Check Uses*/
		Assert::IsTrue(std::vector<int>{ varIdx1.index, varIdx2.index } == Uses::getVariables(stmtIdx1));

		/*Check Modifies*/
		Assert::IsTrue(std::vector<int>{varIdx2.index, varIdx1.index} == Modifies::getVariables(stmtIdx1));

		/*Check Container*/
		Assert::IsTrue(std::unordered_set<StmtIndex, StmtIndex::HashFunction>{stmtIdx3, stmtIdx4, stmtIdx2} ==
			Container::getStmtsInContainer(stmtIdx1));

		/*Check FollowsT*/
		Assert::IsTrue(std::vector<int>{stmtIdx2.index} == FollowsT::getSuccessorStmts(stmtIdx3));

		/*Check ParentT*/
		Assert::IsTrue(std::vector<int>{stmtIdx3.index, stmtIdx2.index, stmtIdx4.index} ==
			ParentT::getSuccessorStmts(stmtIdx1));
	};

	TEST_METHOD(populateRecursiveInfo_3) {
		/*if { while(){ stmt3; stmt4; }; }*/
		StmtIndex stmtIdx1 = Entity::insertStmt(StatementType::ifType);
		StmtIndex stmtIdx2 = Entity::insertStmt(StatementType::whileType);

		Container::insertStmtInContainer(stmtIdx1, stmtIdx2);
		Container::insertStmtInContainer(stmtIdx2, stmtIdx3);
		Container::insertStmtInContainer(stmtIdx2, stmtIdx4);

		Uses::insert(stmtIdx3, varIdx1);
		Modifies::insert(stmtIdx3, varIdx2);
		Uses::insert(stmtIdx4, varIdx2);
		Modifies::insert(stmtIdx4, varIdx1);

		Follows::insert(stmtIdx3, stmtIdx4);

		Parent::insert(stmtIdx1, stmtIdx2);
		Parent::insert(stmtIdx2, stmtIdx3);
		Parent::insert(stmtIdx2, stmtIdx4);

		PKB::populateRecursiveInfo();

		/*Check Uses*/
		Assert::IsTrue(std::vector<int>{ varIdx1.index, varIdx2.index } == Uses::getVariables(stmtIdx1));

		/*Check Modifies*/
		Assert::IsTrue(std::vector<int>{varIdx2.index, varIdx1.index} == Modifies::getVariables(stmtIdx1));

		/*Check Container*/
		Assert::IsTrue(std::unordered_set<StmtIndex, StmtIndex::HashFunction>{stmtIdx2, stmtIdx3, stmtIdx4} ==
			Container::getStmtsInContainer(stmtIdx1));

		/*Check FollowsT*/
		Assert::IsTrue(std::vector<int>{stmtIdx4.index} == FollowsT::getSuccessorStmts(stmtIdx3));

		/*Check ParentT*/
		Assert::IsTrue(std::vector<int>{stmtIdx2.index, stmtIdx3.index, stmtIdx4.index} ==
			ParentT::getSuccessorStmts(stmtIdx1));
	};
	};
}
