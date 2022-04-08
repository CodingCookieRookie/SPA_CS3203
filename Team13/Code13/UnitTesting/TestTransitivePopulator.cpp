#include "stdafx.h"
#include "CppUnitTest.h"

//#include "../source/PKB/PKBGetter.h"
//#include "../source/PKB/PKBInserter.h"
//#include "../source/PKB/TransitivePopulator.h"
//
//using namespace Microsoft::VisualStudio::CppUnitTestFramework;
//
//namespace UnitTesting {
//	TEST_CLASS(TestTransitivePopulator) {
//private:
//	VarIndex varIdx1 = VarIndex(1);
//	VarIndex varIdx2 = VarIndex(2);
//	VarIndex varIdx3 = VarIndex(3);
//	VarIndex varIdx4 = VarIndex(4);
//
//	PKB* pkb;
//	PKBGetter* pkbGetter;
//	PKBInserter* pkbInserter;
//
//	TEST_METHOD_INITIALIZE(init) {
//		pkb = new PKB();
//		pkbGetter = new PKBGetter(pkb);
//		pkbInserter = new PKBInserter(pkb);
//	}
//
//public:
//
//	TEST_METHOD(populateRecursiveInfo_populateRS1ProcsFromStmts) {
//		/* proc1 { stmt1; stmt2; } */
//		ProcIndex procIdx1 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
//
//		StmtIndex stmtIdx1 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
//		StmtIndex stmtIdx2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
//
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx1, varIdx1);
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx2, varIdx2);
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx1, varIdx3);
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx2, varIdx4);
//
//		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx1);
//		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx2);
//
//		TransitivePopulator::populateRecursiveInfo();
//
//		/* Check Uses */
//		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2 } == UsesP::getFromLeftArg(procIdx1));
//
//		/* Check Modifies */
//		Assert::IsTrue(std::vector<VarIndex>{ varIdx3, varIdx4 } == ModifiesP::getFromLeftArg(procIdx1));
//	};
//
//	TEST_METHOD(populateRecursiveInfo_populateRS1ProcsFromNestedCalls) {
//		/* proc1 { stmt1; stmt2; calls proc2; } proc2 { stmt4 } */
//		ProcIndex procIdx1 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
//		ProcIndex procIdx2 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc2");
//
//		StmtIndex stmtIdx1 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
//		StmtIndex stmtIdx2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
//		StmtIndex stmtIdx3 = pkbInserter->insertStmt(StatementType::CALL_TYPE, std::string("proc2"));
//		StmtIndex stmtIdx4 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
//
//		Calls::insert(procIdx1, procIdx2);
//
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx1, varIdx1);
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx2, varIdx2);
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx4, varIdx3);
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx1, varIdx3);
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx2, varIdx4);
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx4, varIdx1);
//
//		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx1);
//		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx2);
//		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx3);
//		pkbInserter->insertStmtFromProc(procIdx2, stmtIdx4);
//
//		TransitivePopulator::populateRecursiveInfo();
//
//		/* Check Uses */
//		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2, varIdx3 } == UsesP::getFromLeftArg(procIdx1));
//
//		/* Check Modifies */
//		Assert::IsTrue(std::vector<VarIndex>{ varIdx3, varIdx4, varIdx1 } == ModifiesP::getFromLeftArg(procIdx1));
//	};
//
//	TEST_METHOD(populateRecursiveInfo_singleProc_bothStmtsInIfWhileEmpty) {
//		/* if { stmt2; stmt3; while(){}; } */
//		StmtIndex stmtIdx1 = pkbInserter->insertStmt(StatementType::IF_TYPE);
//		StmtIndex stmtIdx2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
//		StmtIndex stmtIdx3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
//		StmtIndex stmtIdx4 = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
//
//		Container::insertStmtInContainer(stmtIdx1, stmtIdx2);
//		Container::insertStmtInContainer(stmtIdx1, stmtIdx3);
//		Container::insertStmtInContainer(stmtIdx1, stmtIdx4);
//
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx2, varIdx1);
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx2, varIdx2);
//
//		Follows::insert(stmtIdx2, stmtIdx3);
//		Follows::insert(stmtIdx3, stmtIdx4);
//
//		Parent::insert(stmtIdx1, stmtIdx2);
//		Parent::insert(stmtIdx1, stmtIdx3);
//		Parent::insert(stmtIdx1, stmtIdx4);
//
//		TransitivePopulator::populateRecursiveInfo();
//
//		/* Check Uses */
//		Assert::IsTrue(std::vector<VarIndex>{ varIdx1 } == UsesS::getFromLeftArg(stmtIdx1));
//		Assert::IsTrue(0 == UsesS::getFromLeftArg(stmtIdx4).size());
//
//		/* Check Modifies */
//		Assert::IsTrue(std::vector<VarIndex>{varIdx2} == ModifiesS::getFromLeftArg(stmtIdx1));
//		Assert::IsTrue(0 == ModifiesS::getFromLeftArg(stmtIdx4).size());
//
//		/* Check Container */
//		Assert::IsTrue(std::unordered_set<StmtIndex>{stmtIdx2, stmtIdx3, stmtIdx4} ==
//			Container::getStmtsInContainer(stmtIdx1));
//
//		/* Check FollowsT */
//		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx3, stmtIdx4} ==
//			FollowsT::getFromLeftArg(stmtIdx2));
//
//		/* Check ParentT */
//		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx2, stmtIdx3, stmtIdx4} ==
//			ParentT::getFromLeftArg(stmtIdx1));
//	};
//
//	TEST_METHOD(populateRecursiveInfo_singleProc_oneStmtInIfOneStmtInWhile) {
//		/* if { stmt2; while(){ stmt4; }; } */
//		StmtIndex stmtIdx1 = pkbInserter->insertStmt(StatementType::IF_TYPE);
//		StmtIndex stmtIdx2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
//		StmtIndex stmtIdx3 = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
//		StmtIndex stmtIdx4 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
//
//		Container::insertStmtInContainer(stmtIdx1, stmtIdx2);
//		Container::insertStmtInContainer(stmtIdx1, stmtIdx3);
//		Container::insertStmtInContainer(stmtIdx2, stmtIdx4);
//
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx2, varIdx1);
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx2, varIdx2);
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx4, varIdx2);
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx4, varIdx1);
//
//		Follows::insert(stmtIdx2, stmtIdx3);
//
//		Parent::insert(stmtIdx1, stmtIdx2);
//		Parent::insert(stmtIdx3, stmtIdx4);
//		Parent::insert(stmtIdx1, stmtIdx3);
//
//		TransitivePopulator::populateRecursiveInfo();
//
//		/* Check Uses */
//		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2 } == UsesS::getFromLeftArg(stmtIdx1));
//
//		/* Check Modifies */
//		Assert::IsTrue(std::vector<VarIndex>{varIdx2, varIdx1} == ModifiesS::getFromLeftArg(stmtIdx1));
//
//		/* Check Container */
//		Assert::IsTrue(std::unordered_set<StmtIndex>{stmtIdx2, stmtIdx3, stmtIdx4} ==
//			Container::getStmtsInContainer(stmtIdx1));
//
//		/* Check FollowsT */
//		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx3} == FollowsT::getFromLeftArg(stmtIdx2));
//
//		/* Check ParentT */
//		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx2, stmtIdx3, stmtIdx4} ==
//			ParentT::getFromLeftArg(stmtIdx1));
//	};
//
//	TEST_METHOD(populateRecursiveInfo_singleProc_allStmtsInWhile) {
//		/* if { while(){ stmt3; stmt4; }; } */
//		StmtIndex stmtIdx1 = pkbInserter->insertStmt(StatementType::IF_TYPE);
//		StmtIndex stmtIdx2 = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
//		StmtIndex stmtIdx3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
//		StmtIndex stmtIdx4 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
//
//		Container::insertStmtInContainer(stmtIdx1, stmtIdx2);
//		Container::insertStmtInContainer(stmtIdx2, stmtIdx3);
//		Container::insertStmtInContainer(stmtIdx2, stmtIdx4);
//
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx3, varIdx1);
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx3, varIdx2);
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx4, varIdx2);
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx4, varIdx1);
//
//		Follows::insert(stmtIdx3, stmtIdx4);
//
//		Parent::insert(stmtIdx1, stmtIdx2);
//		Parent::insert(stmtIdx2, stmtIdx3);
//		Parent::insert(stmtIdx2, stmtIdx4);
//
//		TransitivePopulator::populateRecursiveInfo();
//
//		/* Check Uses */
//		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2 } == UsesS::getFromLeftArg(stmtIdx1));
//
//		/* Check Modifies */
//		Assert::IsTrue(std::vector<VarIndex>{varIdx2, varIdx1} == ModifiesS::getFromLeftArg(stmtIdx1));
//
//		/* Check Container */
//		Assert::IsTrue(std::unordered_set<StmtIndex>{stmtIdx2, stmtIdx3, stmtIdx4} ==
//			Container::getStmtsInContainer(stmtIdx1));
//
//		/* Check FollowsT */
//		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx4} == FollowsT::getFromLeftArg(stmtIdx3));
//
//		/* Check ParentT */
//		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx2, stmtIdx3, stmtIdx4} ==
//			ParentT::getFromLeftArg(stmtIdx1));
//	};
//
//	TEST_METHOD(populateRecursiveInfo_multipleProcs_nestedProcCalls) {
//		/* proc1 { calls proc2(); } proc2 { call proc3(); } proc3 { stmt3; } */
//		ProcIndex procIdx1 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
//		ProcIndex procIdx2 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc2");
//		ProcIndex procIdx3 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc3");
//
//		StmtIndex stmtIdx1 = pkbInserter->insertStmt(StatementType::CALL_TYPE, std::string("proc2"));
//		StmtIndex stmtIdx2 = pkbInserter->insertStmt(StatementType::CALL_TYPE, std::string("proc3"));
//		StmtIndex stmtIdx3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
//
//		Calls::insert(procIdx1, procIdx2);
//		Calls::insert(procIdx2, procIdx3);
//
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx3, varIdx1);
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx3, varIdx2);
//
//		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx1);
//		pkbInserter->insertStmtFromProc(procIdx2, stmtIdx2);
//		pkbInserter->insertStmtFromProc(procIdx3, stmtIdx3);
//
//		TransitivePopulator::populateRecursiveInfo();
//
//		/* Check Uses */
//		Assert::IsTrue(std::vector<VarIndex>{ varIdx1 } == UsesP::getFromLeftArg(procIdx1));
//
//		/* Check Modifies */
//		Assert::IsTrue(std::vector<VarIndex>{varIdx2} == ModifiesP::getFromLeftArg(procIdx1));
//	};
//
//	TEST_METHOD(populateRecursiveInfo_multipleProcs_containerStmts) {
//		/* proc1 { calls proc2(); } proc2 { call proc3(); } proc3 { if { stmt4; while(){ stmt6; }; } } */
//		ProcIndex procIdx1 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
//		ProcIndex procIdx2 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc2");
//		ProcIndex procIdx3 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc3");
//
//		StmtIndex stmtIdx1 = pkbInserter->insertStmt(StatementType::CALL_TYPE, std::string("proc2"));
//		StmtIndex stmtIdx2 = pkbInserter->insertStmt(StatementType::CALL_TYPE, std::string("proc3"));
//		StmtIndex stmtIdx3 = pkbInserter->insertStmt(StatementType::IF_TYPE);
//		StmtIndex stmtIdx4 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
//		StmtIndex stmtIdx5 = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
//		StmtIndex stmtIdx6 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
//
//		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx1);
//		pkbInserter->insertStmtFromProc(procIdx2, stmtIdx2);
//		pkbInserter->insertStmtFromProc(procIdx3, stmtIdx3);
//		pkbInserter->insertStmtFromProc(procIdx3, stmtIdx4);
//		pkbInserter->insertStmtFromProc(procIdx3, stmtIdx5);
//		pkbInserter->insertStmtFromProc(procIdx3, stmtIdx6);
//
//		Calls::insert(procIdx1, procIdx2);
//		Calls::insert(procIdx2, procIdx3);
//
//		Container::insertStmtInContainer(stmtIdx3, stmtIdx4);
//		Container::insertStmtInContainer(stmtIdx3, stmtIdx5);
//		Container::insertStmtInContainer(stmtIdx5, stmtIdx6);
//
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx4, varIdx1);
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx4, varIdx2);
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx6, varIdx2);
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx6, varIdx1);
//
//		Follows::insert(stmtIdx4, stmtIdx5);
//
//		Parent::insert(stmtIdx3, stmtIdx4);
//		Parent::insert(stmtIdx3, stmtIdx5);
//		Parent::insert(stmtIdx5, stmtIdx6);
//
//		TransitivePopulator::populateRecursiveInfo();
//
//		/* Check Uses */
//		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2 } == UsesS::getFromLeftArg(stmtIdx3));
//		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2 } == UsesP::getFromLeftArg(procIdx1));
//		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2 } == UsesP::getFromLeftArg(procIdx2));
//		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx6, stmtIdx3, stmtIdx5, stmtIdx1, stmtIdx2} ==
//			pkbGetter->getRSInfoFromRightArg(RelationshipType::USES_S, varIdx2)); // calls stmt uses var
//
//		/* Check Modifies */
//		Assert::IsTrue(std::vector<VarIndex>{varIdx2, varIdx1} == ModifiesS::getFromLeftArg(stmtIdx3));
//		Assert::IsTrue(std::vector<VarIndex>{varIdx2, varIdx1} == ModifiesP::getFromLeftArg(procIdx1));
//		Assert::IsTrue(std::vector<VarIndex>{varIdx2, varIdx1} == ModifiesP::getFromLeftArg(procIdx2));
//		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx6, stmtIdx3, stmtIdx5, stmtIdx1, stmtIdx2} ==
//			pkbGetter->getRSInfoFromRightArg(RelationshipType::MODIFIES_S, varIdx1)); // calls stmt modifies var
//
//		/* Check Container */
//		Assert::IsTrue(std::unordered_set<StmtIndex>{stmtIdx4, stmtIdx5, stmtIdx6} ==
//			Container::getStmtsInContainer(stmtIdx3));
//
//		/* Check FollowsT */
//		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx5} == FollowsT::getFromLeftArg(stmtIdx4));
//
//		/* Check ParentT */
//		Assert::IsTrue(std::vector<StmtIndex>{stmtIdx4, stmtIdx5, stmtIdx6} ==
//			ParentT::getFromLeftArg(stmtIdx3));
//	};
//
//	TEST_METHOD(populateRecursiveInfo_multipleProcs_readStmts) {
//		/* proc1 { call proc2(); } proc2 { print x; call proc3(); } proc3 { print y; } */
//		ProcIndex procIdx1 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
//		ProcIndex procIdx2 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc2");
//		ProcIndex procIdx3 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc3");
//
//		StmtIndex stmtIdx1 = pkbInserter->insertStmt(StatementType::CALL_TYPE, std::string("proc2"));
//		StmtIndex stmtIdx2 = pkbInserter->insertStmt(StatementType::PRINT_TYPE, std::string("x"));
//		StmtIndex stmtIdx3 = pkbInserter->insertStmt(StatementType::CALL_TYPE, std::string("proc3"));
//		StmtIndex stmtIdx4 = pkbInserter->insertStmt(StatementType::PRINT_TYPE, std::string("y"));
//
//		Calls::insert(procIdx1, procIdx2);
//		Calls::insert(procIdx2, procIdx3);
//
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx2, varIdx1);
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx4, varIdx2);
//
//		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx1);
//		pkbInserter->insertStmtFromProc(procIdx2, stmtIdx2);
//		pkbInserter->insertStmtFromProc(procIdx2, stmtIdx3);
//		pkbInserter->insertStmtFromProc(procIdx3, stmtIdx4);
//
//		TransitivePopulator::populateRecursiveInfo();
//
//		/* Check Uses */
//		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2 } == UsesS::getFromLeftArg(stmtIdx1));
//		Assert::IsTrue(std::vector<VarIndex>{varIdx2} == UsesS::getFromLeftArg(stmtIdx3));
//	};
//
//	TEST_METHOD(populateRecursiveInfo_multipleProcs_printStmts) {
//		/* proc1 { call proc2(); } proc2 { read x; call proc3(); } proc3 { read y; } */
//		ProcIndex procIdx1 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
//		ProcIndex procIdx2 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc2");
//		ProcIndex procIdx3 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc3");
//
//		StmtIndex stmtIdx1 = pkbInserter->insertStmt(StatementType::CALL_TYPE, std::string("proc2"));
//		StmtIndex stmtIdx2 = pkbInserter->insertStmt(StatementType::READ_TYPE, std::string("x"));
//		StmtIndex stmtIdx3 = pkbInserter->insertStmt(StatementType::CALL_TYPE, std::string("proc3"));
//		StmtIndex stmtIdx4 = pkbInserter->insertStmt(StatementType::READ_TYPE, std::string("y"));
//
//		Calls::insert(procIdx1, procIdx2);
//		Calls::insert(procIdx2, procIdx3);
//
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx2, varIdx1);
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx4, varIdx2);
//
//		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx1);
//		pkbInserter->insertStmtFromProc(procIdx2, stmtIdx2);
//		pkbInserter->insertStmtFromProc(procIdx2, stmtIdx3);
//		pkbInserter->insertStmtFromProc(procIdx3, stmtIdx4);
//
//		TransitivePopulator::populateRecursiveInfo();
//
//		/* Check Uses */
//		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx2 } == ModifiesS::getFromLeftArg(stmtIdx1));
//		Assert::IsTrue(std::vector<VarIndex>{varIdx2} == ModifiesS::getFromLeftArg(stmtIdx3));
//	};
//
//	TEST_METHOD(populateRecursiveInfo_multipleProcs_readAndPrintStmts) {
//		/* proc1 { call proc2(); } proc2 { print x; call proc3(); read p; } proc3 { print y; read r; } */
//		ProcIndex procIdx1 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
//		ProcIndex procIdx2 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc2");
//		ProcIndex procIdx3 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc3");
//
//		StmtIndex stmtIdx1 = pkbInserter->insertStmt(StatementType::CALL_TYPE, std::string("proc2"));
//		StmtIndex stmtIdx2 = pkbInserter->insertStmt(StatementType::PRINT_TYPE, std::string("x"));
//		StmtIndex stmtIdx3 = pkbInserter->insertStmt(StatementType::CALL_TYPE, std::string("proc3"));
//		StmtIndex stmtIdx4 = pkbInserter->insertStmt(StatementType::READ_TYPE, std::string("p"));
//		StmtIndex stmtIdx5 = pkbInserter->insertStmt(StatementType::PRINT_TYPE, std::string("y"));
//		StmtIndex stmtIdx6 = pkbInserter->insertStmt(StatementType::READ_TYPE, std::string("r"));
//
//		Calls::insert(procIdx1, procIdx2);
//		Calls::insert(procIdx2, procIdx3);
//
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx2, varIdx1);
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx5, varIdx3);
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx4, varIdx2);
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx6, varIdx4);
//
//		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx1);
//		pkbInserter->insertStmtFromProc(procIdx2, stmtIdx2);
//		pkbInserter->insertStmtFromProc(procIdx2, stmtIdx3);
//		pkbInserter->insertStmtFromProc(procIdx2, stmtIdx4);
//		pkbInserter->insertStmtFromProc(procIdx3, stmtIdx5);
//		pkbInserter->insertStmtFromProc(procIdx3, stmtIdx6);
//
//		TransitivePopulator::populateRecursiveInfo();
//
//		/* Check Uses */
//		Assert::IsTrue(std::vector<VarIndex>{ varIdx1, varIdx3 } == UsesS::getFromLeftArg(stmtIdx1));
//		Assert::IsTrue(std::vector<VarIndex>{varIdx3} == UsesS::getFromLeftArg(stmtIdx3));
//
//		/* Check Modifies */
//		Assert::IsTrue(std::vector<VarIndex>{ varIdx2, varIdx4 } == ModifiesS::getFromLeftArg(stmtIdx1));
//		Assert::IsTrue(std::vector<VarIndex>{ varIdx4 } == ModifiesS::getFromLeftArg(stmtIdx3));
//	};
//
//	TEST_METHOD(populateRecursiveInfo_multipleProcs_callInWhile) {
//		/* proc p { while() { calls q; } } proc q { x = x + 1 } */
//		ProcIndex procIdx1 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "p");
//		ProcIndex procIdx2 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "q");
//
//		StmtIndex stmtIdx1 = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
//		StmtIndex stmtIdx2 = pkbInserter->insertStmt(StatementType::CALL_TYPE, std::string("q"));
//		StmtIndex stmtIdx3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
//
//		VarIndex varIdx1 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
//
//		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx1);
//		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx2);
//		pkbInserter->insertStmtFromProc(procIdx2, stmtIdx3);
//
//		Container::insertStmtInContainer(stmtIdx1, stmtIdx2);
//
//		Calls::insert(procIdx1, procIdx2);
//
//		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx3, varIdx1);
//		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx3, varIdx1);
//
//		TransitivePopulator::populateRecursiveInfo();
//
//		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx3, stmtIdx2, stmtIdx1 } == pkbGetter->getRSInfoFromRightArg(RelationshipType::MODIFIES_S, varIdx1));
//		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx3, stmtIdx2, stmtIdx1 } == pkbGetter->getRSInfoFromRightArg(RelationshipType::USES_S, varIdx1));
//	}
//	};
//}
