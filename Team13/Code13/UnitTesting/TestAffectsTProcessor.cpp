#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/PKBGetter.h"
#include "../source/PKB/PKBInserter.h"
#include "../source/QPS/AffectsTProcessor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestAffectsTProcessor) {
private:
	AffectsTProcessor* affectsTProcessor;
	StmtIndex stmtIdx1;
	StmtIndex stmtIdx2;
	StmtIndex stmtIdx3;
	StmtIndex stmtIdx4;
	StmtIndex stmtIdx5;
	StmtIndex stmtIdx6;
	StmtIndex stmtIdx7;
	StmtIndex stmtIdx8;
	StmtIndex stmtIdx9;
	StmtIndex stmtIdx10;
	StmtIndex stmtIdx11;
	StmtIndex stmtIdx12;
	StmtIndex stmtIdx13;
	StmtIndex stmtIdx14;
	ProcIndex procIdx1;
	ProcIndex procIdx2;
	VarIndex varIdx1;
	VarIndex varIdx2;
	VarIndex varIdx3;
	VarIndex varIdx4;
	PKB pkb;
	PKBGetter* pkbGetter;
	PKBInserter* pkbInserter;

	TEST_METHOD_INITIALIZE(init) {
		AffectsTCache* affectsTCache = new AffectsTCache();
		AffectsCache* affectsCache = new AffectsCache();
		AffectsProcessor* affectsProcessor = new AffectsProcessor(affectsCache);
		affectsTProcessor = new AffectsTProcessor(affectsTCache, affectsProcessor);
		pkb = PKB();
		pkbGetter = new PKBGetter(pkb);
		pkbInserter = new PKBInserter(pkb);
	}

public:

	void populateBasicAffectsTCase() {
		/*
		procedure p {
		1.    x = a;
		2.    a = x + 1;
		3.    x = a + 1; }
		*/

		stmtIdx1 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);

		procIdx1 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "p");

		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx1);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx2);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx3);

		varIdx1 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		varIdx2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx1, varIdx1);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx2, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx3, varIdx1);

		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx1, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx2, varIdx1);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx3, varIdx2);

		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3);
	}

	void populateContainersWithNestedCalls() {
		/*
			procedure Second {
			01  x = 0;
			02  i = 5;
			03  while (i!=0) {
			04      x = x + 2*y;
			05      call Third;
			06      i = i - 1; }
			07  if (x==1) then {
			08      x = x+1; }
				else {
			09      z = 1; }
			10  z = z + x + i;
			11  y = z + 2;
			12  x = x * y + z; }

			procedure Third {
			13	z = 5;
			14	print z; }
			*/

		stmtIdx1 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx3 = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		stmtIdx4 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx5 = pkbInserter->insertStmt(StatementType::CALL_TYPE, std::string("Third"));
		stmtIdx6 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx7 = pkbInserter->insertStmt(StatementType::IF_TYPE);
		stmtIdx8 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx9 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx10 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx11 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx12 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx13 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx14 = pkbInserter->insertStmt(StatementType::PRINT_TYPE, std::string("z"));

		procIdx1 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "Second");
		procIdx1 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "Third");

		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx1);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx2);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx3);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx4);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx5);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx6);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx7);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx8);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx9);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx10);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx11);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx12);
		pkbInserter->insertStmtFromProc(procIdx2, stmtIdx13);

		varIdx1 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "i");
		varIdx2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		varIdx3 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		varIdx4 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "z");

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx1, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx2, varIdx1);

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx3, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx3, varIdx4);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx3, varIdx1);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx4, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx5, varIdx4);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx6, varIdx1);

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx7, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx7, varIdx4);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx8, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx9, varIdx4);

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx10, varIdx4);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx11, varIdx3);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx12, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx13, varIdx4);

		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx3, varIdx1);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx3, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx3, varIdx3);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx3, varIdx4);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx3, varIdx1);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx4, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx4, varIdx3);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx5, varIdx4);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx6, varIdx1);

		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx7, varIdx1);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx7, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx8, varIdx2);

		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx10, varIdx4);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx10, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx10, varIdx1);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx11, varIdx4);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx12, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx12, varIdx3);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx12, varIdx4);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx14, varIdx4);

		// non-container stmts
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3);
		// while container stmts
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx4);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx4, stmtIdx5);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx5, stmtIdx6);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx6, stmtIdx3);
		// if container stmts
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx7);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx7, stmtIdx8);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx8, stmtIdx10);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx7, stmtIdx9);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx9, stmtIdx10);
		// non-container stmts
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx10, stmtIdx11);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx11, stmtIdx12);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx13, stmtIdx14);
	}

	void populateNestedContainers() {
		/*
		procedure One {
		1.  a = b * c + d;
		2.  while (c > 0) {
		3.    if (d > 0) then {
		4.            b = a - 1;}
				  else {
		5.            c = b * d;}
		6.    d = b + c;
		7.    c = d * a;
		8.    b = d - c;}}
		*/

		stmtIdx1 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx2 = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		stmtIdx3 = pkbInserter->insertStmt(StatementType::IF_TYPE);
		stmtIdx4 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx5 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx6 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx7 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx8 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);

		procIdx1 = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "One");

		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx1);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx2);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx3);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx4);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx5);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx6);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx7);
		pkbInserter->insertStmtFromProc(procIdx1, stmtIdx8);

		varIdx1 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		varIdx2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		varIdx3 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "c");
		varIdx4 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "d");

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx1, varIdx1);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx2, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx2, varIdx3);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx2, varIdx4);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx3, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx3, varIdx3);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx4, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx5, varIdx3);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx6, varIdx4);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx7, varIdx3);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIdx8, varIdx2);

		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx1, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx1, varIdx3);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx1, varIdx4);

		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx2, varIdx1);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx2, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx2, varIdx3);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx2, varIdx4);

		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx3, varIdx1);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx3, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx3, varIdx4);

		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx4, varIdx1);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx5, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx5, varIdx4);

		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx6, varIdx2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx6, varIdx3);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx7, varIdx4);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx7, varIdx1);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx8, varIdx4);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIdx8, varIdx3);

		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx4);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx5);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx4, stmtIdx6);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx5, stmtIdx6);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx6, stmtIdx7);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx7, stmtIdx8);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx8, stmtIdx2);
	}

	// ----------------- doesRsHold --------------------------------
	TEST_METHOD(doesRsHold_basicAffectsTCase_true) {
		populateBasicAffectsTCase();

		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx2, pkbGetter));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx3, pkbGetter));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx2, stmtIdx3, pkbGetter));
	};

	TEST_METHOD(doesRsHold_containerWithNestedCalls_true) {
		populateContainersWithNestedCalls();

		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx4, pkbGetter));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx10, pkbGetter));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx11, pkbGetter));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx12, pkbGetter));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx4, stmtIdx4, pkbGetter));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx4, stmtIdx8, pkbGetter));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx9, stmtIdx12, pkbGetter));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx6, stmtIdx6, pkbGetter));
	};

	TEST_METHOD(doesRsHold_containerWithNestedCalls_false_leftIdxIsNotAssignStmt) {
		populateContainersWithNestedCalls();

		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx5, stmtIdx10, pkbGetter));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx3, stmtIdx6, pkbGetter));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx7, stmtIdx8, pkbGetter));
	};

	TEST_METHOD(doesRsHold_containerWithNestedCalls_false_rightIdxIsNotAssignStmt) {
		populateContainersWithNestedCalls();

		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx3, pkbGetter));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx6, stmtIdx3, pkbGetter));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx7, pkbGetter));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx13, stmtIdx14, pkbGetter));
	};

	TEST_METHOD(doesRsHold_containerWithNestedCalls_false_bothIdxNotAssignStmt) {
		populateContainersWithNestedCalls();

		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx5, stmtIdx5, pkbGetter));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx3, stmtIdx3, pkbGetter));
	};

	TEST_METHOD(doesRsHold_containerWithNestedCalls_false_noControlFlowPath) {
		populateContainersWithNestedCalls();

		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx8, stmtIdx8, pkbGetter));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx8, stmtIdx9, pkbGetter));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx4, stmtIdx1, pkbGetter));
	};

	TEST_METHOD(doesRsHold_containerWithNestedCalls_false_differentProcs) {
		populateContainersWithNestedCalls();

		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx5, stmtIdx13, pkbGetter));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx13, stmtIdx10, pkbGetter));
	};

	TEST_METHOD(doesRsHold_nestedContainers_true) {
		populateNestedContainers();

		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx4, pkbGetter));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx5, stmtIdx5, pkbGetter));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx4, stmtIdx5, pkbGetter));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx7, stmtIdx5, pkbGetter));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx5, stmtIdx7, pkbGetter));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx5, stmtIdx8, pkbGetter));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx7, stmtIdx6, pkbGetter));
	};

	TEST_METHOD(doesRsHold_nestedContainers_false) {
		populateNestedContainers();

		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx4, stmtIdx4, pkbGetter));
	};

	// ----------------- getUsingLeftStmtIndex --------------------------------
	TEST_METHOD(getUsingLeftStmtIndex_basicAffectsTCase) {
		populateBasicAffectsTCase();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx2, stmtIdx3} ==
			affectsTProcessor->getUsingLeftStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx3} ==
			affectsTProcessor->getUsingLeftStmtIndex(stmtIdx2, pkbGetter));
	};

	TEST_METHOD(getUsingLeftStmtIndex_containerWithNestedCalls) {
		populateContainersWithNestedCalls();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx4, stmtIdx12, stmtIdx8, stmtIdx10, stmtIdx11} ==
			affectsTProcessor->getUsingLeftStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx4, stmtIdx12, stmtIdx8, stmtIdx10, stmtIdx11} ==
			affectsTProcessor->getUsingLeftStmtIndex(stmtIdx4, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx10, stmtIdx11, stmtIdx12} ==
			affectsTProcessor->getUsingLeftStmtIndex(stmtIdx9, pkbGetter));
	};

	TEST_METHOD(getUsingLeftStmtIndex_nestedContainers) {
		populateNestedContainers();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx4, stmtIdx5, stmtIdx6, stmtIdx7, stmtIdx8} ==
			affectsTProcessor->getUsingLeftStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx6, stmtIdx7, stmtIdx8, stmtIdx5} ==
			affectsTProcessor->getUsingLeftStmtIndex(stmtIdx4, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx6, stmtIdx7, stmtIdx8, stmtIdx5} ==
			affectsTProcessor->getUsingLeftStmtIndex(stmtIdx5, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx7, stmtIdx8, stmtIdx5, stmtIdx6} ==
			affectsTProcessor->getUsingLeftStmtIndex(stmtIdx6, pkbGetter));
	};

	// ----------------- getUsingRightStmtIndex --------------------------------
	TEST_METHOD(getUsingRightStmtIndex_basicAffectsTCase) {
		populateBasicAffectsTCase();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx2, stmtIdx1} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx3, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx1, pkbGetter));
	};

	TEST_METHOD(getUsingRightStmtIndex_containerWithNestedCalls) {
		populateContainersWithNestedCalls();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1, stmtIdx4} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx4, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1, stmtIdx4} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx8, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx10, stmtIdx2, stmtIdx8, stmtIdx9, stmtIdx1, stmtIdx6, stmtIdx4} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx11, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx11, stmtIdx10, stmtIdx2, stmtIdx8, stmtIdx9, stmtIdx1, stmtIdx6, stmtIdx4} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx12, pkbGetter));
	};

	TEST_METHOD(getUsingRightStmtIndex_nestedContainers) {
		populateNestedContainers();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx4, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1, stmtIdx8, stmtIdx7, stmtIdx6, stmtIdx4, stmtIdx5} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx5, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx7, stmtIdx6, stmtIdx4, stmtIdx5, stmtIdx1, stmtIdx8} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx8, pkbGetter));
	};

	// ----------------- getAll --------------------------------
	TEST_METHOD(getAll_basicAffectsTCase) {
		populateBasicAffectsTCase();

		std::vector<StmtIndex> leftStmtIndices = { stmtIdx1, stmtIdx1, stmtIdx2 };
		std::vector<StmtIndex> rightStmtIndices = { stmtIdx2, stmtIdx3, stmtIdx3 };

		Assert::IsTrue(std::make_tuple(leftStmtIndices, rightStmtIndices) ==
			affectsTProcessor->getAll(pkbGetter));
	};
	};
};
