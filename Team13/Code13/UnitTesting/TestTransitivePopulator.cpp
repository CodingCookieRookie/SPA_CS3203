#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/TransitivePopulator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPKB) {
private:
	VarIndex varIdx1 = VarIndex(5);
	VarIndex varIdx2 = VarIndex(6);

	TEST_METHOD_CLEANUP(cleanUpPKB) {
		Attribute::performCleanUp();
		Entity::performCleanUp();
		Calls::performCleanUp();
		CallsT::performCleanUp();
		Container::performCleanUp();
		UsesS::performCleanUp();
		UsesP::performCleanUp();
		ModifiesS::performCleanUp();
		ModifiesP::performCleanUp();
		Follows::performCleanUp();
		FollowsT::performCleanUp();
		Parent::performCleanUp();
		ParentT::performCleanUp();
	}

public:
	TEST_METHOD(populateRecursiveInfo_singleProc_1) {
		/* if { stmt2; stmt3; while(){}; } */
		StmtIndex stmtIdx1 = Entity::insertStmt(StatementType::ifType);
		StmtIndex stmtIdx2 = Entity::insertStmt(StatementType::assignType);
		StmtIndex stmtIdx3 = Entity::insertStmt(StatementType::assignType);
		StmtIndex stmtIdx4 = Entity::insertStmt(StatementType::whileType);

		Container::insertStmtInContainer(stmtIdx1, stmtIdx2);
		Container::insertStmtInContainer(stmtIdx1, stmtIdx3);
		Container::insertStmtInContainer(stmtIdx1, stmtIdx4);

		UsesS::insert(stmtIdx2, varIdx1);
		ModifiesS::insert(stmtIdx2, varIdx2);

		Follows::insert(stmtIdx2, stmtIdx3);
		Follows::insert(stmtIdx3, stmtIdx4);

		Parent::insert(stmtIdx1, stmtIdx2);
		Parent::insert(stmtIdx1, stmtIdx3);
		Parent::insert(stmtIdx1, stmtIdx4);

		TransitivePopulator::populateRecursiveInfo();

		/* Check Uses */
		Assert::IsTrue(std::vector<VarIndex>{ varIdx1 } == UsesS::getVariables(stmtIdx1));
		Assert::IsTrue(0 == UsesS::getVariables(stmtIdx4).size());

		/* Check Modifies */
		Assert::IsTrue(std::vector<VarIndex>{varIdx2} == ModifiesS::getVariables(stmtIdx1));
		Assert::IsTrue(0 == ModifiesS::getVariables(stmtIdx4).size());

		/* Check Container */
		Assert::IsTrue(std::unordered_set<StmtIndex>{stmtIdx2, stmtIdx3, stmtIdx4} ==
			Container::getStmtsInContainer(stmtIdx1));

		/* Check FollowsT */
		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx3, stmtIdx4} ==
			FollowsT::getSuccessors(stmtIdx2));

		/* Check ParentT */
		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx2, stmtIdx3, stmtIdx4} ==
			ParentT::getSuccessors(stmtIdx1));
	};

	TEST_METHOD(populateRecursiveInfo_singleProc_2) {
		/* if { stmt2; while(){ stmt4; }; } */
		StmtIndex stmtIdx1 = Entity::insertStmt(StatementType::ifType);
		StmtIndex stmtIdx2 = Entity::insertStmt(StatementType::assignType);
		StmtIndex stmtIdx3 = Entity::insertStmt(StatementType::whileType);
		StmtIndex stmtIdx4 = Entity::insertStmt(StatementType::assignType);

		Container::insertStmtInContainer(stmtIdx1, stmtIdx2);
		Container::insertStmtInContainer(stmtIdx1, stmtIdx3);
		Container::insertStmtInContainer(stmtIdx2, stmtIdx4);

		UsesS::insert(stmtIdx2, varIdx1);
		ModifiesS::insert(stmtIdx2, varIdx2);
		UsesS::insert(stmtIdx4, varIdx2);
		ModifiesS::insert(stmtIdx4, varIdx1);

		Follows::insert(stmtIdx2, stmtIdx3);

		Parent::insert(stmtIdx1, stmtIdx2);
		Parent::insert(stmtIdx3, stmtIdx4);
		Parent::insert(stmtIdx1, stmtIdx3);

		TransitivePopulator::populateRecursiveInfo();

		/* Check Uses */
		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2 } == UsesS::getVariables(stmtIdx1));

		/* Check Modifies */
		Assert::IsTrue(std::vector<VarIndex>{varIdx2, varIdx1} == ModifiesS::getVariables(stmtIdx1));

		/* Check Container */
		Assert::IsTrue(std::unordered_set<StmtIndex>{stmtIdx2, stmtIdx3, stmtIdx4} ==
			Container::getStmtsInContainer(stmtIdx1));

		/* Check FollowsT */
		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx3} == FollowsT::getSuccessors(stmtIdx2));

		/* Check ParentT */
		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx2, stmtIdx3, stmtIdx4} ==
			ParentT::getSuccessors(stmtIdx1));
	};

	TEST_METHOD(populateRecursiveInfo_singleProc_3) {
		/* if { while(){ stmt3; stmt4; }; } */
		StmtIndex stmtIdx1 = Entity::insertStmt(StatementType::ifType);
		StmtIndex stmtIdx2 = Entity::insertStmt(StatementType::whileType);
		StmtIndex stmtIdx3 = Entity::insertStmt(StatementType::assignType);
		StmtIndex stmtIdx4 = Entity::insertStmt(StatementType::assignType);

		Container::insertStmtInContainer(stmtIdx1, stmtIdx2);
		Container::insertStmtInContainer(stmtIdx2, stmtIdx3);
		Container::insertStmtInContainer(stmtIdx2, stmtIdx4);

		UsesS::insert(stmtIdx3, varIdx1);
		ModifiesS::insert(stmtIdx3, varIdx2);
		UsesS::insert(stmtIdx4, varIdx2);
		ModifiesS::insert(stmtIdx4, varIdx1);

		Follows::insert(stmtIdx3, stmtIdx4);

		Parent::insert(stmtIdx1, stmtIdx2);
		Parent::insert(stmtIdx2, stmtIdx3);
		Parent::insert(stmtIdx2, stmtIdx4);

		TransitivePopulator::populateRecursiveInfo();

		/* Check Uses */
		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2 } == UsesS::getVariables(stmtIdx1));

		/* Check Modifies */
		Assert::IsTrue(std::vector<VarIndex>{varIdx2, varIdx1} == ModifiesS::getVariables(stmtIdx1));

		/* Check Container */
		Assert::IsTrue(std::unordered_set<StmtIndex>{stmtIdx2, stmtIdx3, stmtIdx4} ==
			Container::getStmtsInContainer(stmtIdx1));

		/* Check FollowsT */
		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx4} == FollowsT::getSuccessors(stmtIdx3));

		/* Check ParentT */
		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx2, stmtIdx3, stmtIdx4} ==
			ParentT::getSuccessors(stmtIdx1));
	};

	TEST_METHOD(populateRecursiveInfo_multipleProcs_1) {
		/* proc1 { calls proc2(); } proc2 { call proc3(); } proc3 { stmt3; } */
		ProcIndex procIdx1 = Entity::insertProc("proc1");
		ProcIndex procIdx2 = Entity::insertProc("proc2");
		ProcIndex procIdx3 = Entity::insertProc("proc3");

		StmtIndex stmtIdx3 = Entity::insertStmt(StatementType::assignType);

		Calls::insert(procIdx1, procIdx2);
		Calls::insert(procIdx2, procIdx3);

		UsesP::insert(procIdx3, varIdx1);
		ModifiesP::insert(procIdx3, varIdx2);

		TransitivePopulator::populateRecursiveInfo();

		/* Check Uses */
		Assert::IsTrue(std::vector<VarIndex>{ varIdx1 } == UsesP::getVariables(procIdx1));

		/* Check Modifies */
		Assert::IsTrue(std::vector<VarIndex>{varIdx2} == ModifiesP::getVariables(procIdx1));
	};

	TEST_METHOD(populateRecursiveInfo_multipleProcs_2) {
		/* proc1 { calls proc2(); } proc2 { call proc3(); } proc3 { if { stmt4; while(){ stmt6; }; } } */
		ProcIndex procIdx1 = Entity::insertProc("proc1");
		ProcIndex procIdx2 = Entity::insertProc("proc2");
		ProcIndex procIdx3 = Entity::insertProc("proc3");

		StmtIndex stmtIdx1 = Entity::insertStmt(StatementType::callType);
		StmtIndex stmtIdx2 = Entity::insertStmt(StatementType::callType);
		StmtIndex stmtIdx3 = Entity::insertStmt(StatementType::ifType);
		StmtIndex stmtIdx4 = Entity::insertStmt(StatementType::assignType);
		StmtIndex stmtIdx5 = Entity::insertStmt(StatementType::whileType);
		StmtIndex stmtIdx6 = Entity::insertStmt(StatementType::assignType);

		Calls::insert(procIdx1, procIdx2);
		Calls::insert(procIdx2, procIdx3);
		UsesP::insert(procIdx3, varIdx1);
		UsesP::insert(procIdx3, varIdx2);
		ModifiesP::insert(procIdx3, varIdx1);
		ModifiesP::insert(procIdx3, varIdx2);

		Container::insertStmtInContainer(stmtIdx3, stmtIdx4);
		Container::insertStmtInContainer(stmtIdx3, stmtIdx5);
		Container::insertStmtInContainer(stmtIdx5, stmtIdx6);

		UsesS::insert(stmtIdx4, varIdx1);
		ModifiesS::insert(stmtIdx4, varIdx2);
		UsesS::insert(stmtIdx6, varIdx2);
		ModifiesS::insert(stmtIdx6, varIdx1);

		Follows::insert(stmtIdx4, stmtIdx5);

		Parent::insert(stmtIdx3, stmtIdx4);
		Parent::insert(stmtIdx3, stmtIdx5);
		Parent::insert(stmtIdx5, stmtIdx6);

		TransitivePopulator::populateRecursiveInfo();

		/* Check Uses */
		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2 } == UsesS::getVariables(stmtIdx3));
		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2 } == UsesP::getVariables(procIdx1));
		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2 } == UsesP::getVariables(procIdx2));

		/* Check Modifies */
		Assert::IsTrue(std::vector<VarIndex>{varIdx2, varIdx1} == ModifiesS::getVariables(stmtIdx3));
		Assert::IsTrue(std::vector<VarIndex>{varIdx1, varIdx2} == ModifiesP::getVariables(procIdx1));
		Assert::IsTrue(std::vector<VarIndex>{varIdx1, varIdx2} == ModifiesP::getVariables(procIdx2));

		/* Check Container */
		Assert::IsTrue(std::unordered_set<StmtIndex>{stmtIdx4, stmtIdx5, stmtIdx6} ==
			Container::getStmtsInContainer(stmtIdx3));

		/* Check FollowsT */
		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx5} == FollowsT::getSuccessors(stmtIdx4));

		/* Check ParentT */
		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx4, stmtIdx5, stmtIdx6} ==
			ParentT::getSuccessors(stmtIdx3));
	};
	};
}