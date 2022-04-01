#include "stdafx.h"
#include "CppUnitTest.h"

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

	TEST_METHOD_INITIALIZE(init) {
		AffectsTCache* affectsTCache = new AffectsTCache();
		AffectsCache* affectsCache = new AffectsCache();
		AffectsProcessor* affectsProcessor = new AffectsProcessor(affectsCache);
		affectsTProcessor = new AffectsTProcessor(affectsTCache, affectsProcessor);
	}

	TEST_METHOD_CLEANUP(cleanUpCalls) {
		affectsTProcessor->performCleanUp();
		Next::performCleanUp();
		UsesS::performCleanUp();
		ModifiesS::performCleanUp();
		Entity::performCleanUp();
		Attribute::performCleanUp();
	}

public:

	void populateBasicAffectsTCase() {
		/*
		procedure p {
		1.    x = a;
		2.    a = x + 1;
		3.    x = a + 1; }
		*/

		stmtIdx1 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx2 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx3 = Entity::insertStmt(StatementType::ASSIGN_TYPE);

		procIdx1 = Entity::insertProc("p");

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);
		Entity::insertStmtFromProc(procIdx1, stmtIdx3);

		varIdx1 = Entity::insertVar("x");
		varIdx2 = Entity::insertVar("a");

		ModifiesS::insert(stmtIdx1, varIdx1);
		ModifiesS::insert(stmtIdx2, varIdx2);
		ModifiesS::insert(stmtIdx3, varIdx1);

		UsesS::insert(stmtIdx1, varIdx2);
		UsesS::insert(stmtIdx2, varIdx1);
		UsesS::insert(stmtIdx3, varIdx2);

		Next::insert(stmtIdx1, stmtIdx2);
		Next::insert(stmtIdx2, stmtIdx3);
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

		stmtIdx1 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx2 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx3 = Entity::insertStmt(StatementType::WHILE_TYPE);
		stmtIdx4 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx5 = Entity::insertStmt(StatementType::CALL_TYPE, std::string("Third"));
		stmtIdx6 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx7 = Entity::insertStmt(StatementType::IF_TYPE);
		stmtIdx8 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx9 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx10 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx11 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx12 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx13 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx14 = Entity::insertStmt(StatementType::PRINT_TYPE);

		procIdx1 = Entity::insertProc("Second");
		procIdx1 = Entity::insertProc("Third");

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);
		Entity::insertStmtFromProc(procIdx1, stmtIdx3);
		Entity::insertStmtFromProc(procIdx1, stmtIdx4);
		Entity::insertStmtFromProc(procIdx1, stmtIdx5);
		Entity::insertStmtFromProc(procIdx1, stmtIdx6);
		Entity::insertStmtFromProc(procIdx1, stmtIdx7);
		Entity::insertStmtFromProc(procIdx1, stmtIdx8);
		Entity::insertStmtFromProc(procIdx1, stmtIdx9);
		Entity::insertStmtFromProc(procIdx1, stmtIdx10);
		Entity::insertStmtFromProc(procIdx1, stmtIdx11);
		Entity::insertStmtFromProc(procIdx1, stmtIdx12);
		Entity::insertStmtFromProc(procIdx2, stmtIdx13);

		varIdx1 = Entity::insertVar("i");
		varIdx2 = Entity::insertVar("x");
		varIdx3 = Entity::insertVar("y");
		varIdx4 = Entity::insertVar("z");

		ModifiesS::insert(stmtIdx1, varIdx2);
		ModifiesS::insert(stmtIdx2, varIdx1);

		ModifiesS::insert(stmtIdx3, varIdx2);
		ModifiesS::insert(stmtIdx3, varIdx4);
		ModifiesS::insert(stmtIdx3, varIdx1);
		ModifiesS::insert(stmtIdx4, varIdx2);
		ModifiesS::insert(stmtIdx5, varIdx4);
		ModifiesS::insert(stmtIdx6, varIdx1);

		ModifiesS::insert(stmtIdx7, varIdx2);
		ModifiesS::insert(stmtIdx7, varIdx4);
		ModifiesS::insert(stmtIdx8, varIdx2);
		ModifiesS::insert(stmtIdx9, varIdx4);

		ModifiesS::insert(stmtIdx10, varIdx4);
		ModifiesS::insert(stmtIdx11, varIdx3);
		ModifiesS::insert(stmtIdx12, varIdx2);
		ModifiesS::insert(stmtIdx13, varIdx4);

		UsesS::insert(stmtIdx3, varIdx1);
		UsesS::insert(stmtIdx3, varIdx2);
		UsesS::insert(stmtIdx3, varIdx3);
		UsesS::insert(stmtIdx3, varIdx4);
		UsesS::insert(stmtIdx3, varIdx1);
		UsesS::insert(stmtIdx4, varIdx2);
		UsesS::insert(stmtIdx4, varIdx3);
		UsesS::insert(stmtIdx5, varIdx4);
		UsesS::insert(stmtIdx6, varIdx1);

		UsesS::insert(stmtIdx7, varIdx1);
		UsesS::insert(stmtIdx7, varIdx2);
		UsesS::insert(stmtIdx8, varIdx2);

		UsesS::insert(stmtIdx10, varIdx4);
		UsesS::insert(stmtIdx10, varIdx2);
		UsesS::insert(stmtIdx10, varIdx1);
		UsesS::insert(stmtIdx11, varIdx4);
		UsesS::insert(stmtIdx12, varIdx2);
		UsesS::insert(stmtIdx12, varIdx3);
		UsesS::insert(stmtIdx12, varIdx4);
		UsesS::insert(stmtIdx14, varIdx4);

		// non-container stmts
		Next::insert(stmtIdx1, stmtIdx2);
		Next::insert(stmtIdx2, stmtIdx3);
		// while container stmts
		Next::insert(stmtIdx3, stmtIdx4);
		Next::insert(stmtIdx4, stmtIdx5);
		Next::insert(stmtIdx5, stmtIdx6);
		Next::insert(stmtIdx6, stmtIdx3);
		// if container stmts
		Next::insert(stmtIdx3, stmtIdx7);
		Next::insert(stmtIdx7, stmtIdx8);
		Next::insert(stmtIdx8, stmtIdx10);
		Next::insert(stmtIdx7, stmtIdx9);
		Next::insert(stmtIdx9, stmtIdx10);
		// non-container stmts
		Next::insert(stmtIdx10, stmtIdx11);
		Next::insert(stmtIdx11, stmtIdx12);
		Next::insert(stmtIdx13, stmtIdx14);
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

		stmtIdx1 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx2 = Entity::insertStmt(StatementType::WHILE_TYPE);
		stmtIdx3 = Entity::insertStmt(StatementType::IF_TYPE);
		stmtIdx4 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx5 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx6 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx7 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx8 = Entity::insertStmt(StatementType::ASSIGN_TYPE);

		procIdx1 = Entity::insertProc("One");

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);
		Entity::insertStmtFromProc(procIdx1, stmtIdx3);
		Entity::insertStmtFromProc(procIdx1, stmtIdx4);
		Entity::insertStmtFromProc(procIdx1, stmtIdx5);
		Entity::insertStmtFromProc(procIdx1, stmtIdx6);
		Entity::insertStmtFromProc(procIdx1, stmtIdx7);
		Entity::insertStmtFromProc(procIdx1, stmtIdx8);

		varIdx1 = Entity::insertVar("a");
		varIdx2 = Entity::insertVar("b");
		varIdx3 = Entity::insertVar("c");
		varIdx4 = Entity::insertVar("d");

		ModifiesS::insert(stmtIdx1, varIdx1);
		ModifiesS::insert(stmtIdx2, varIdx2);
		ModifiesS::insert(stmtIdx2, varIdx3);
		ModifiesS::insert(stmtIdx2, varIdx4);
		ModifiesS::insert(stmtIdx3, varIdx2);
		ModifiesS::insert(stmtIdx3, varIdx3);
		ModifiesS::insert(stmtIdx4, varIdx2);
		ModifiesS::insert(stmtIdx5, varIdx3);
		ModifiesS::insert(stmtIdx6, varIdx4);
		ModifiesS::insert(stmtIdx7, varIdx3);
		ModifiesS::insert(stmtIdx8, varIdx2);

		UsesS::insert(stmtIdx1, varIdx2);
		UsesS::insert(stmtIdx1, varIdx3);
		UsesS::insert(stmtIdx1, varIdx4);

		UsesS::insert(stmtIdx2, varIdx1);
		UsesS::insert(stmtIdx2, varIdx2);
		UsesS::insert(stmtIdx2, varIdx3);
		UsesS::insert(stmtIdx2, varIdx4);

		UsesS::insert(stmtIdx3, varIdx1);
		UsesS::insert(stmtIdx3, varIdx2);
		UsesS::insert(stmtIdx3, varIdx4);

		UsesS::insert(stmtIdx4, varIdx1);
		UsesS::insert(stmtIdx5, varIdx2);
		UsesS::insert(stmtIdx5, varIdx4);

		UsesS::insert(stmtIdx6, varIdx2);
		UsesS::insert(stmtIdx6, varIdx3);
		UsesS::insert(stmtIdx7, varIdx4);
		UsesS::insert(stmtIdx7, varIdx1);
		UsesS::insert(stmtIdx8, varIdx4);
		UsesS::insert(stmtIdx8, varIdx3);

		Next::insert(stmtIdx1, stmtIdx2);
		Next::insert(stmtIdx2, stmtIdx3);
		Next::insert(stmtIdx3, stmtIdx4);
		Next::insert(stmtIdx3, stmtIdx5);
		Next::insert(stmtIdx4, stmtIdx6);
		Next::insert(stmtIdx5, stmtIdx6);
		Next::insert(stmtIdx6, stmtIdx7);
		Next::insert(stmtIdx7, stmtIdx8);
		Next::insert(stmtIdx8, stmtIdx2);
	}

	// ----------------- doesRsHold --------------------------------
	TEST_METHOD(doesRsHold_basicAffectsTCase_true) {
		populateBasicAffectsTCase();

		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx2));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx3));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx2, stmtIdx3));
	};

	TEST_METHOD(doesRsHold_containerWithNestedCalls_true) {
		populateContainersWithNestedCalls();

		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx4));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx10));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx11));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx12));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx4, stmtIdx4));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx4, stmtIdx8));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx9, stmtIdx12));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx6, stmtIdx6));
	};

	TEST_METHOD(doesRsHold_containerWithNestedCalls_false_leftIdxIsNotAssignStmt) {
		populateContainersWithNestedCalls();

		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx5, stmtIdx10));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx3, stmtIdx6));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx7, stmtIdx8));
	};

	TEST_METHOD(doesRsHold_containerWithNestedCalls_false_rightIdxIsNotAssignStmt) {
		populateContainersWithNestedCalls();

		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx3));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx6, stmtIdx3));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx7));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx13, stmtIdx14));
	};

	TEST_METHOD(doesRsHold_containerWithNestedCalls_false_bothIdxNotAssignStmt) {
		populateContainersWithNestedCalls();

		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx5, stmtIdx5));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx3, stmtIdx3));
	};

	TEST_METHOD(doesRsHold_containerWithNestedCalls_false_noControlFlowPath) {
		populateContainersWithNestedCalls();

		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx8, stmtIdx8));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx8, stmtIdx9));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx4, stmtIdx1));
	};

	TEST_METHOD(doesRsHold_containerWithNestedCalls_false_differentProcs) {
		populateContainersWithNestedCalls();

		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx5, stmtIdx13));
		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx13, stmtIdx10));
	};

	TEST_METHOD(doesRsHold_nestedContainers_true) {
		populateNestedContainers();

		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx1, stmtIdx4));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx5, stmtIdx5));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx4, stmtIdx5));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx7, stmtIdx5));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx5, stmtIdx7));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx5, stmtIdx8));
		Assert::IsTrue(affectsTProcessor->doesRsHold(stmtIdx7, stmtIdx6));
	};

	TEST_METHOD(doesRsHold_nestedContainers_false) {
		populateNestedContainers();

		Assert::IsFalse(affectsTProcessor->doesRsHold(stmtIdx4, stmtIdx4));
	};

	// ----------------- getUsingLeftStmtIndex --------------------------------
	TEST_METHOD(getUsingLeftStmtIndex_basicAffectsTCase) {
		populateBasicAffectsTCase();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx2, stmtIdx3} == affectsTProcessor->getUsingLeftStmtIndex(stmtIdx1));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx3} == affectsTProcessor->getUsingLeftStmtIndex(stmtIdx2));
	};

	TEST_METHOD(getUsingLeftStmtIndex_containerWithNestedCalls) {
		populateContainersWithNestedCalls();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx4, stmtIdx12, stmtIdx8, stmtIdx10, stmtIdx11} ==
			affectsTProcessor->getUsingLeftStmtIndex(stmtIdx1));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx4, stmtIdx12, stmtIdx8, stmtIdx10, stmtIdx11} ==
			affectsTProcessor->getUsingLeftStmtIndex(stmtIdx4));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx10, stmtIdx11, stmtIdx12} ==
			affectsTProcessor->getUsingLeftStmtIndex(stmtIdx9));
	};

	TEST_METHOD(getUsingLeftStmtIndex_nestedContainers) {
		populateNestedContainers();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx4, stmtIdx5, stmtIdx6, stmtIdx7, stmtIdx8} ==
			affectsTProcessor->getUsingLeftStmtIndex(stmtIdx1));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx6, stmtIdx7, stmtIdx8, stmtIdx5} ==
			affectsTProcessor->getUsingLeftStmtIndex(stmtIdx4));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx6, stmtIdx7, stmtIdx8, stmtIdx5} ==
			affectsTProcessor->getUsingLeftStmtIndex(stmtIdx5));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx7, stmtIdx8, stmtIdx5, stmtIdx6} ==
			affectsTProcessor->getUsingLeftStmtIndex(stmtIdx6));
	};

	// ----------------- getUsingRightStmtIndex --------------------------------
	TEST_METHOD(getUsingRightStmtIndex_basicAffectsTCase) {
		populateBasicAffectsTCase();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx2, stmtIdx1} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx3));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx2));
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx1));
	};

	TEST_METHOD(getUsingRightStmtIndex_containerWithNestedCalls) {
		populateContainersWithNestedCalls();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1, stmtIdx4} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx4));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1, stmtIdx4} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx8));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx10, stmtIdx2, stmtIdx8, stmtIdx9, stmtIdx1, stmtIdx6, stmtIdx4} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx11));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx11, stmtIdx10, stmtIdx2, stmtIdx8, stmtIdx9, stmtIdx1, stmtIdx6, stmtIdx4} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx12));
	};

	TEST_METHOD(getUsingRightStmtIndex_nestedContainers) {
		populateNestedContainers();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx4));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1, stmtIdx8, stmtIdx7, stmtIdx6, stmtIdx4, stmtIdx5} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx5));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx7, stmtIdx6, stmtIdx4, stmtIdx5, stmtIdx1, stmtIdx8} ==
			affectsTProcessor->getUsingRightStmtIndex(stmtIdx8));
	};

	// ----------------- getAll --------------------------------
	TEST_METHOD(getAll_basicAffectsTCase) {
		populateBasicAffectsTCase();

		std::vector<StmtIndex> leftStmtIndices = { stmtIdx1, stmtIdx1, stmtIdx2 };
		std::vector<StmtIndex> rightStmtIndices = { stmtIdx2, stmtIdx3, stmtIdx3 };

		Assert::IsTrue(std::make_tuple(leftStmtIndices, rightStmtIndices) ==
			affectsTProcessor->getAll());
	};
	};
};
