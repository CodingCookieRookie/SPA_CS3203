#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/TransitivePopulator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestTransitivePopulator) {
private:
	VarIndex varIdx1 = VarIndex(1);
	VarIndex varIdx2 = VarIndex(2);
	VarIndex varIdx3 = VarIndex(3);
	VarIndex varIdx4 = VarIndex(4);

	TEST_METHOD_CLEANUP(cleanUpTransitivePopulator) {
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

	TEST_METHOD(populateRecursiveInfo_populateRS1ProcsFromStmts) {
		/* proc1 { stmt1; stmt2; } */
		ProcIndex procIdx1 = Entity::insertProc("proc1");

		StmtIndex stmtIdx1 = Entity::insertStmt(StatementType::assignType);
		StmtIndex stmtIdx2 = Entity::insertStmt(StatementType::assignType);

		UsesS::insert(stmtIdx1, varIdx1);
		UsesS::insert(stmtIdx2, varIdx2);
		ModifiesS::insert(stmtIdx1, varIdx3);
		ModifiesS::insert(stmtIdx2, varIdx4);

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);

		TransitivePopulator::populateRecursiveInfo();

		/* Check Uses */
		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2 } == UsesP::getVariables(procIdx1));

		/* Check Modifies */
		Assert::IsTrue(std::vector<VarIndex>{ varIdx3, varIdx4 } == ModifiesP::getVariables(procIdx1));
	};

	TEST_METHOD(populateRecursiveInfo_populateRS1ProcsFromNestedCalls) {
		/* proc1 { stmt1; stmt2; calls proc2; } proc2 { stmt4 } */
		ProcIndex procIdx1 = Entity::insertProc("proc1");
		ProcIndex procIdx2 = Entity::insertProc("proc2");

		StmtIndex stmtIdx1 = Entity::insertStmt(StatementType::assignType);
		StmtIndex stmtIdx2 = Entity::insertStmt(StatementType::assignType);
		StmtIndex stmtIdx3 = Entity::insertStmt(StatementType::callType, std::string("proc2"));
		StmtIndex stmtIdx4 = Entity::insertStmt(StatementType::assignType);

		Calls::insert(procIdx1, procIdx2);

		UsesS::insert(stmtIdx1, varIdx1);
		UsesS::insert(stmtIdx2, varIdx2);
		UsesS::insert(stmtIdx4, varIdx3);
		ModifiesS::insert(stmtIdx1, varIdx3);
		ModifiesS::insert(stmtIdx2, varIdx4);
		ModifiesS::insert(stmtIdx4, varIdx1);

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);
		Entity::insertStmtFromProc(procIdx1, stmtIdx3);
		Entity::insertStmtFromProc(procIdx2, stmtIdx4);

		TransitivePopulator::populateRecursiveInfo();

		/* Check Uses */
		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2, varIdx3 } == UsesP::getVariables(procIdx1));

		/* Check Modifies */
		Assert::IsTrue(std::vector<VarIndex>{ varIdx3, varIdx4, varIdx1 } == ModifiesP::getVariables(procIdx1));
	};

	TEST_METHOD(populateRecursiveInfo_singleProc_bothStmtsInIfWhileEmpty) {
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

	TEST_METHOD(populateRecursiveInfo_singleProc_oneStmtInIfOneStmtInWhile) {
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

	TEST_METHOD(populateRecursiveInfo_singleProc_allStmtsInWhile) {
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

	TEST_METHOD(populateRecursiveInfo_multipleProcs_nestedProcCalls) {
		/* proc1 { calls proc2(); } proc2 { call proc3(); } proc3 { stmt3; } */
		ProcIndex procIdx1 = Entity::insertProc("proc1");
		ProcIndex procIdx2 = Entity::insertProc("proc2");
		ProcIndex procIdx3 = Entity::insertProc("proc3");

		StmtIndex stmtIdx1 = Entity::insertStmt(StatementType::callType, std::string("proc2"));
		StmtIndex stmtIdx2 = Entity::insertStmt(StatementType::callType, std::string("proc3"));
		StmtIndex stmtIdx3 = Entity::insertStmt(StatementType::assignType);

		Calls::insert(procIdx1, procIdx2);
		Calls::insert(procIdx2, procIdx3);

		UsesS::insert(stmtIdx3, varIdx1);
		ModifiesS::insert(stmtIdx3, varIdx2);

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx2, stmtIdx2);
		Entity::insertStmtFromProc(procIdx3, stmtIdx3);

		TransitivePopulator::populateRecursiveInfo();

		/* Check Uses */
		Assert::IsTrue(std::vector<VarIndex>{ varIdx1 } == UsesP::getVariables(procIdx1));

		/* Check Modifies */
		Assert::IsTrue(std::vector<VarIndex>{varIdx2} == ModifiesP::getVariables(procIdx1));
	};

	TEST_METHOD(populateRecursiveInfo_multipleProcs_containerStmts) {
		/* proc1 { calls proc2(); } proc2 { call proc3(); } proc3 { if { stmt4; while(){ stmt6; }; } } */
		ProcIndex procIdx1 = Entity::insertProc("proc1");
		ProcIndex procIdx2 = Entity::insertProc("proc2");
		ProcIndex procIdx3 = Entity::insertProc("proc3");

		StmtIndex stmtIdx1 = Entity::insertStmt(StatementType::callType, std::string("proc2"));
		StmtIndex stmtIdx2 = Entity::insertStmt(StatementType::callType, std::string("proc3"));
		StmtIndex stmtIdx3 = Entity::insertStmt(StatementType::ifType);
		StmtIndex stmtIdx4 = Entity::insertStmt(StatementType::assignType);
		StmtIndex stmtIdx5 = Entity::insertStmt(StatementType::whileType);
		StmtIndex stmtIdx6 = Entity::insertStmt(StatementType::assignType);

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx2, stmtIdx2);
		Entity::insertStmtFromProc(procIdx3, stmtIdx3);
		Entity::insertStmtFromProc(procIdx3, stmtIdx4);
		Entity::insertStmtFromProc(procIdx3, stmtIdx5);
		Entity::insertStmtFromProc(procIdx3, stmtIdx6);

		Calls::insert(procIdx1, procIdx2);
		Calls::insert(procIdx2, procIdx3);

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
		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx6, stmtIdx3, stmtIdx5, stmtIdx1, stmtIdx2} ==
			UsesS::getFromVariable(varIdx2)); // calls stmt uses var

		/* Check Modifies */
		Assert::IsTrue(std::vector<VarIndex>{varIdx2, varIdx1} == ModifiesS::getVariables(stmtIdx3));
		Assert::IsTrue(std::vector<VarIndex>{varIdx2, varIdx1} == ModifiesP::getVariables(procIdx1));
		Assert::IsTrue(std::vector<VarIndex>{varIdx2, varIdx1} == ModifiesP::getVariables(procIdx2));
		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx6, stmtIdx3, stmtIdx5, stmtIdx1, stmtIdx2} ==
			ModifiesS::getFromVariable(varIdx1)); // calls stmt modifies var

		/* Check Container */
		Assert::IsTrue(std::unordered_set<StmtIndex>{stmtIdx4, stmtIdx5, stmtIdx6} ==
			Container::getStmtsInContainer(stmtIdx3));

		/* Check FollowsT */
		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx5} == FollowsT::getSuccessors(stmtIdx4));

		/* Check ParentT */
		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx4, stmtIdx5, stmtIdx6} ==
			ParentT::getSuccessors(stmtIdx3));
	};

	TEST_METHOD(populateRecursiveInfo_multipleProcs_readStmts) {
		/* proc1 { call proc2(); } proc2 { print x; call proc3(); } proc3 { print y; } */
		ProcIndex procIdx1 = Entity::insertProc("proc1");
		ProcIndex procIdx2 = Entity::insertProc("proc2");
		ProcIndex procIdx3 = Entity::insertProc("proc3");

		StmtIndex stmtIdx1 = Entity::insertStmt(StatementType::callType, std::string("proc2"));
		StmtIndex stmtIdx2 = Entity::insertStmt(StatementType::printType, std::string("x"));
		StmtIndex stmtIdx3 = Entity::insertStmt(StatementType::callType, std::string("proc3"));
		StmtIndex stmtIdx4 = Entity::insertStmt(StatementType::printType, std::string("y"));

		Calls::insert(procIdx1, procIdx2);
		Calls::insert(procIdx2, procIdx3);

		UsesS::insert(stmtIdx2, varIdx1);
		UsesS::insert(stmtIdx4, varIdx2);

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx2, stmtIdx2);
		Entity::insertStmtFromProc(procIdx2, stmtIdx3);
		Entity::insertStmtFromProc(procIdx3, stmtIdx4);

		TransitivePopulator::populateRecursiveInfo();

		/* Check Uses */
		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2 } == UsesS::getVariables(stmtIdx1));
		Assert::IsTrue(std::vector<VarIndex>{varIdx2} == UsesS::getVariables(stmtIdx3));
	};

	TEST_METHOD(populateRecursiveInfo_multipleProcs_printStmts) {
		/* proc1 { call proc2(); } proc2 { read x; call proc3(); } proc3 { read y; } */
		ProcIndex procIdx1 = Entity::insertProc("proc1");
		ProcIndex procIdx2 = Entity::insertProc("proc2");
		ProcIndex procIdx3 = Entity::insertProc("proc3");

		StmtIndex stmtIdx1 = Entity::insertStmt(StatementType::callType, std::string("proc2"));
		StmtIndex stmtIdx2 = Entity::insertStmt(StatementType::readType, std::string("x"));
		StmtIndex stmtIdx3 = Entity::insertStmt(StatementType::callType, std::string("proc3"));
		StmtIndex stmtIdx4 = Entity::insertStmt(StatementType::readType, std::string("y"));

		Calls::insert(procIdx1, procIdx2);
		Calls::insert(procIdx2, procIdx3);

		ModifiesS::insert(stmtIdx2, varIdx1);
		ModifiesS::insert(stmtIdx4, varIdx2);

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx2, stmtIdx2);
		Entity::insertStmtFromProc(procIdx2, stmtIdx3);
		Entity::insertStmtFromProc(procIdx3, stmtIdx4);

		TransitivePopulator::populateRecursiveInfo();

		/* Check Uses */
		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2 } == ModifiesS::getVariables(stmtIdx1));
		Assert::IsTrue(std::vector<VarIndex>{varIdx2} == ModifiesS::getVariables(stmtIdx3));
	};

	TEST_METHOD(populateRecursiveInfo_multipleProcs_readAndPrintStmts) {
		/* proc1 { call proc2(); } proc2 { print x; call proc3(); read p; } proc3 { print y; read r; } */
		ProcIndex procIdx1 = Entity::insertProc("proc1");
		ProcIndex procIdx2 = Entity::insertProc("proc2");
		ProcIndex procIdx3 = Entity::insertProc("proc3");

		StmtIndex stmtIdx1 = Entity::insertStmt(StatementType::callType, std::string("proc2"));
		StmtIndex stmtIdx2 = Entity::insertStmt(StatementType::printType, std::string("x"));
		StmtIndex stmtIdx3 = Entity::insertStmt(StatementType::callType, std::string("proc3"));
		StmtIndex stmtIdx4 = Entity::insertStmt(StatementType::readType, std::string("p"));
		StmtIndex stmtIdx5 = Entity::insertStmt(StatementType::printType, std::string("y"));
		StmtIndex stmtIdx6 = Entity::insertStmt(StatementType::readType, std::string("r"));

		Calls::insert(procIdx1, procIdx2);
		Calls::insert(procIdx2, procIdx3);

		UsesS::insert(stmtIdx2, varIdx1);
		UsesS::insert(stmtIdx5, varIdx3);
		ModifiesS::insert(stmtIdx4, varIdx2);
		ModifiesS::insert(stmtIdx6, varIdx4);

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx2, stmtIdx2);
		Entity::insertStmtFromProc(procIdx2, stmtIdx3);
		Entity::insertStmtFromProc(procIdx2, stmtIdx4);
		Entity::insertStmtFromProc(procIdx3, stmtIdx5);
		Entity::insertStmtFromProc(procIdx3, stmtIdx6);

		TransitivePopulator::populateRecursiveInfo();

		/* Check Uses */
		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx3 } == UsesS::getVariables(stmtIdx1));
		Assert::IsTrue(std::vector<VarIndex>{varIdx3} == UsesS::getVariables(stmtIdx3));

		/* Check Modifies */
		Assert::IsTrue(std::vector<VarIndex>{ varIdx2, varIdx4 } == ModifiesS::getVariables(stmtIdx1));
		Assert::IsTrue(std::vector<VarIndex>{ varIdx4 } == ModifiesS::getVariables(stmtIdx3));
	};

	TEST_METHOD(populateRecursiveInfo_multipleProcs_callInWhile) {
		/* proc p { while() { calls q; } } proc q { x = x + 1 } */
		ProcIndex procIdx1 = Entity::insertProc("p");
		ProcIndex procIdx2 = Entity::insertProc("q");

		StmtIndex stmtIdx1 = Entity::insertStmt(StatementType::whileType);
		StmtIndex stmtIdx2 = Entity::insertStmt(StatementType::callType, std::string("q"));
		StmtIndex stmtIdx3 = Entity::insertStmt(StatementType::assignType);

		VarIndex varIdx1 = Entity::insertVar("x");

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);
		Entity::insertStmtFromProc(procIdx2, stmtIdx3);

		Container::insertStmtInContainer(stmtIdx1, stmtIdx2);

		Calls::insert(procIdx1, procIdx2);

		UsesS::insert(stmtIdx3, varIdx1);
		ModifiesS::insert(stmtIdx3, varIdx1);

		TransitivePopulator::populateRecursiveInfo();

		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx3, stmtIdx2, stmtIdx1 } == ModifiesS::getFromVariable(varIdx1));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx3, stmtIdx2, stmtIdx1 } == UsesS::getFromVariable(varIdx1));
	}
	};
}
