#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/QPS/AffectsProcessor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestAffectsProcessor) {
private:
	AffectsProcessor* affectsProcessor;
	StmtIndex stmtIdx1;
	StmtIndex stmtIdx2;
	StmtIndex stmtIdx3;
	StmtIndex stmtIdx4;
	StmtIndex stmtIdx5;
	StmtIndex stmtIdx6;
	ProcIndex procIdx1;
	ProcIndex procIdx2;
	VarIndex varIdx1;
	VarIndex varIdx2;
	VarIndex varIdx3;
	VarIndex varIdx4;

	TEST_METHOD_INITIALIZE(init) {
		AffectsCache* affectsCache = new AffectsCache();
		affectsProcessor = new AffectsProcessor(affectsCache);
	}

	TEST_METHOD_CLEANUP(cleanUpCalls) {
		affectsProcessor->performCleanUp();
		Next::performCleanUp();
		UsesS::performCleanUp();
		ModifiesS::performCleanUp();
		Entity::performCleanUp();
		Attribute::performCleanUp();
	}

public:

	void populateCallsDoesNotModifyVar() {
		/*
		procedure p {
		1.    x = a;
		2.    call q; <-- q does not modify x
		3.    a = x; }
		*/

		stmtIdx1 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx2 = Entity::insertStmt(StatementType::CALL_TYPE, std::string("q"));
		stmtIdx3 = Entity::insertStmt(StatementType::ASSIGN_TYPE);

		procIdx1 = Entity::insertProc("p");

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);
		Entity::insertStmtFromProc(procIdx1, stmtIdx3);

		varIdx1 = Entity::insertVar("x");
		varIdx2 = Entity::insertVar("a");

		ModifiesS::insert(stmtIdx1, varIdx1);
		ModifiesS::insert(stmtIdx3, varIdx2);

		UsesS::insert(stmtIdx1, varIdx2);
		UsesS::insert(stmtIdx2, varIdx2);
		UsesS::insert(stmtIdx3, varIdx1);

		Next::insert(stmtIdx1, stmtIdx2);
		Next::insert(stmtIdx2, stmtIdx3);
	}

	void populateCallsModifiesVar() {
		/*
			procedure p {
			1.    x = a;
			2.    call q; <-- q modifes x
			3.    a = x; }
			*/

		stmtIdx1 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx2 = Entity::insertStmt(StatementType::CALL_TYPE, std::string("q"));
		stmtIdx3 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		procIdx1 = Entity::insertProc("p");

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);
		Entity::insertStmtFromProc(procIdx1, stmtIdx3);

		varIdx1 = Entity::insertVar("x");
		varIdx2 = Entity::insertVar("a");

		ModifiesS::insert(stmtIdx1, varIdx1);
		ModifiesS::insert(stmtIdx2, varIdx1);
		ModifiesS::insert(stmtIdx3, varIdx2);

		UsesS::insert(stmtIdx1, varIdx2);
		UsesS::insert(stmtIdx3, varIdx1);

		Next::insert(stmtIdx1, stmtIdx2);
		Next::insert(stmtIdx2, stmtIdx3);
	}

	void populateIfWithPathWhichDoeNotModifyVar() {
		/*
		procedure alpha {
		1.    x = 1;
		2.    if ( i != 2 ) {
		3.        x = a + 1; }
			  else {
		4.        a = x; }
		5.    a = x; }
		*/

		stmtIdx1 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx2 = Entity::insertStmt(StatementType::IF_TYPE);
		stmtIdx3 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx4 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx5 = Entity::insertStmt(StatementType::ASSIGN_TYPE);

		procIdx1 = Entity::insertProc("alpha");

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);
		Entity::insertStmtFromProc(procIdx1, stmtIdx3);
		Entity::insertStmtFromProc(procIdx1, stmtIdx4);
		Entity::insertStmtFromProc(procIdx1, stmtIdx5);

		varIdx1 = Entity::insertVar("x");
		varIdx2 = Entity::insertVar("i");
		varIdx3 = Entity::insertVar("a");

		ModifiesS::insert(stmtIdx1, varIdx1);
		ModifiesS::insert(stmtIdx2, varIdx1);
		ModifiesS::insert(stmtIdx2, varIdx3);
		ModifiesS::insert(stmtIdx3, varIdx1);
		ModifiesS::insert(stmtIdx4, varIdx3);
		ModifiesS::insert(stmtIdx5, varIdx3);

		UsesS::insert(stmtIdx2, varIdx2);
		UsesS::insert(stmtIdx2, varIdx3);
		UsesS::insert(stmtIdx2, varIdx1);
		UsesS::insert(stmtIdx3, varIdx3);
		UsesS::insert(stmtIdx4, varIdx1);
		UsesS::insert(stmtIdx5, varIdx1);

		Next::insert(stmtIdx1, stmtIdx2);
		Next::insert(stmtIdx2, stmtIdx3);
		Next::insert(stmtIdx2, stmtIdx4);
		Next::insert(stmtIdx3, stmtIdx5);
		Next::insert(stmtIdx4, stmtIdx5);
	}

	void populateIfWhereAllPathsModifyVar() {
		/*
		procedure alpha {
		1.    x = 1;
		2.    if ( i != 2 ) {
		3.        x = a + 1; }
			  else {
		4.        x = x; }
		5.    a = x; }
		*/

		stmtIdx1 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx2 = Entity::insertStmt(StatementType::IF_TYPE);
		stmtIdx3 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx4 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx5 = Entity::insertStmt(StatementType::ASSIGN_TYPE);

		procIdx1 = Entity::insertProc("alpha");

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);
		Entity::insertStmtFromProc(procIdx1, stmtIdx3);
		Entity::insertStmtFromProc(procIdx1, stmtIdx4);
		Entity::insertStmtFromProc(procIdx1, stmtIdx5);

		varIdx1 = Entity::insertVar("x");
		varIdx2 = Entity::insertVar("i");
		varIdx3 = Entity::insertVar("a");

		ModifiesS::insert(stmtIdx1, varIdx1);
		ModifiesS::insert(stmtIdx2, varIdx1);
		ModifiesS::insert(stmtIdx2, varIdx3);
		ModifiesS::insert(stmtIdx3, varIdx1);
		ModifiesS::insert(stmtIdx4, varIdx1);
		ModifiesS::insert(stmtIdx5, varIdx3);

		UsesS::insert(stmtIdx2, varIdx2);
		UsesS::insert(stmtIdx2, varIdx3);
		UsesS::insert(stmtIdx2, varIdx1);
		UsesS::insert(stmtIdx3, varIdx3);
		UsesS::insert(stmtIdx4, varIdx1);
		UsesS::insert(stmtIdx5, varIdx1);

		Next::insert(stmtIdx1, stmtIdx2);
		Next::insert(stmtIdx2, stmtIdx3);
		Next::insert(stmtIdx2, stmtIdx4);
		Next::insert(stmtIdx3, stmtIdx5);
		Next::insert(stmtIdx4, stmtIdx5);
	}

	void populateStmtsInDifferentProcedures() {
		/*
			procedure p {
			1.	a = 1;
			}
			procedure q {
			2.	x = a + 1;
			}
			*/

		stmtIdx1 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx2 = Entity::insertStmt(StatementType::ASSIGN_TYPE);

		procIdx1 = Entity::insertProc("p");
		procIdx2 = Entity::insertProc("q");

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx2, stmtIdx2);

		varIdx1 = Entity::insertVar("a");
		varIdx2 = Entity::insertVar("x");

		ModifiesS::insert(stmtIdx1, varIdx1);
		ModifiesS::insert(stmtIdx2, varIdx2);

		UsesS::insert(stmtIdx2, varIdx1);
	}

	void populatePrintInWhile() {
		/*
		procedure p {
		1.    x = a;
		2.	  while (x < 2) {
		3.      print x;
			  }
		4.    a = x; }
		*/

		stmtIdx1 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx2 = Entity::insertStmt(StatementType::WHILE_TYPE);
		stmtIdx3 = Entity::insertStmt(StatementType::PRINT_TYPE);
		stmtIdx4 = Entity::insertStmt(StatementType::ASSIGN_TYPE);

		procIdx1 = Entity::insertProc("p");

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);
		Entity::insertStmtFromProc(procIdx1, stmtIdx3);
		Entity::insertStmtFromProc(procIdx1, stmtIdx4);

		varIdx1 = Entity::insertVar("x");
		varIdx2 = Entity::insertVar("a");

		ModifiesS::insert(stmtIdx1, varIdx1);
		ModifiesS::insert(stmtIdx4, varIdx2);

		UsesS::insert(stmtIdx1, varIdx2);
		UsesS::insert(stmtIdx2, varIdx1);
		UsesS::insert(stmtIdx3, varIdx1);
		UsesS::insert(stmtIdx4, varIdx1);

		Next::insert(stmtIdx1, stmtIdx2);
		Next::insert(stmtIdx2, stmtIdx3);
		Next::insert(stmtIdx3, stmtIdx2);
		Next::insert(stmtIdx2, stmtIdx4);
	}

	void populateReadInWhile() {
		/*
		procedure p {
		1.    x = a;
		2.	  while (x < 2) {
		3.      read x;
			  }
		4.    a = x; }
		*/

		stmtIdx1 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx2 = Entity::insertStmt(StatementType::WHILE_TYPE);
		stmtIdx3 = Entity::insertStmt(StatementType::READ_TYPE);
		stmtIdx4 = Entity::insertStmt(StatementType::ASSIGN_TYPE);

		procIdx1 = Entity::insertProc("p");

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);
		Entity::insertStmtFromProc(procIdx1, stmtIdx3);
		Entity::insertStmtFromProc(procIdx1, stmtIdx4);

		varIdx1 = Entity::insertVar("x");
		varIdx2 = Entity::insertVar("a");

		ModifiesS::insert(stmtIdx1, varIdx1);
		ModifiesS::insert(stmtIdx2, varIdx1);
		ModifiesS::insert(stmtIdx3, varIdx1);
		ModifiesS::insert(stmtIdx4, varIdx2);

		UsesS::insert(stmtIdx1, varIdx2);
		UsesS::insert(stmtIdx2, varIdx1);
		UsesS::insert(stmtIdx4, varIdx1);

		Next::insert(stmtIdx1, stmtIdx2);
		Next::insert(stmtIdx2, stmtIdx3);
		Next::insert(stmtIdx2, stmtIdx4);
		Next::insert(stmtIdx3, stmtIdx2);
	}

	void populateStmtInWhileAffectsItself() {
		/*
		procedure p {
		1.    while (x > 0) {
		2.		x = x + 1
		3.	    a = a + 1
			  }
		}
		*/

		stmtIdx1 = Entity::insertStmt(StatementType::WHILE_TYPE);
		stmtIdx2 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx3 = Entity::insertStmt(StatementType::ASSIGN_TYPE);

		procIdx1 = Entity::insertProc("p");

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);
		Entity::insertStmtFromProc(procIdx1, stmtIdx3);

		varIdx1 = Entity::insertVar("x");
		varIdx2 = Entity::insertVar("a");

		ModifiesS::insert(stmtIdx1, varIdx1);
		ModifiesS::insert(stmtIdx1, varIdx2);
		ModifiesS::insert(stmtIdx2, varIdx1);
		ModifiesS::insert(stmtIdx3, varIdx2);

		UsesS::insert(stmtIdx1, varIdx1);
		UsesS::insert(stmtIdx1, varIdx2);
		UsesS::insert(stmtIdx2, varIdx1);
		UsesS::insert(stmtIdx3, varIdx2);

		Next::insert(stmtIdx1, stmtIdx2);
		Next::insert(stmtIdx2, stmtIdx3);
		Next::insert(stmtIdx3, stmtIdx2);
	}

	void populateStmtInIfDoesNotAffectItself() {
		/*
		procedure p {
		1.    if (x > 0) then {
		2.		x = x + 1
		3.	    a = a + 1
			  }
		}
		*/

		stmtIdx1 = Entity::insertStmt(StatementType::IF_TYPE);
		stmtIdx2 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx3 = Entity::insertStmt(StatementType::ASSIGN_TYPE);

		procIdx1 = Entity::insertProc("p");

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);
		Entity::insertStmtFromProc(procIdx1, stmtIdx3);

		varIdx1 = Entity::insertVar("x");
		varIdx2 = Entity::insertVar("a");

		ModifiesS::insert(stmtIdx1, varIdx1);
		ModifiesS::insert(stmtIdx1, varIdx2);
		ModifiesS::insert(stmtIdx2, varIdx1);
		ModifiesS::insert(stmtIdx3, varIdx2);

		UsesS::insert(stmtIdx1, varIdx1);
		UsesS::insert(stmtIdx1, varIdx2);
		UsesS::insert(stmtIdx2, varIdx1);
		UsesS::insert(stmtIdx3, varIdx2);

		Next::insert(stmtIdx1, stmtIdx2);
		Next::insert(stmtIdx2, stmtIdx3);
	}

	void populateIfNestedInWhile() {
		/*
		procedure p {
		1.	while (x > 0) {
		2.    if (x > 0) then {
		3.		x = x + 1
		4.	    a = a + 1
			  } else {
		5.		  a = x;
			  }
			}
		6.	x = a;
		}
		*/

		stmtIdx1 = Entity::insertStmt(StatementType::WHILE_TYPE);
		stmtIdx2 = Entity::insertStmt(StatementType::IF_TYPE);
		stmtIdx3 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx4 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx5 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		stmtIdx6 = Entity::insertStmt(StatementType::ASSIGN_TYPE);

		procIdx1 = Entity::insertProc("p");

		Entity::insertStmtFromProc(procIdx1, stmtIdx1);
		Entity::insertStmtFromProc(procIdx1, stmtIdx2);
		Entity::insertStmtFromProc(procIdx1, stmtIdx3);
		Entity::insertStmtFromProc(procIdx1, stmtIdx4);
		Entity::insertStmtFromProc(procIdx1, stmtIdx5);
		Entity::insertStmtFromProc(procIdx1, stmtIdx6);

		varIdx1 = Entity::insertVar("x");
		varIdx2 = Entity::insertVar("a");

		ModifiesS::insert(stmtIdx1, varIdx1);
		ModifiesS::insert(stmtIdx1, varIdx2);
		ModifiesS::insert(stmtIdx2, varIdx1);
		ModifiesS::insert(stmtIdx2, varIdx2);
		ModifiesS::insert(stmtIdx3, varIdx1);
		ModifiesS::insert(stmtIdx4, varIdx2);
		ModifiesS::insert(stmtIdx5, varIdx2);
		ModifiesS::insert(stmtIdx6, varIdx1);

		UsesS::insert(stmtIdx1, varIdx1);
		UsesS::insert(stmtIdx1, varIdx2);
		UsesS::insert(stmtIdx2, varIdx1);
		UsesS::insert(stmtIdx2, varIdx2);
		UsesS::insert(stmtIdx3, varIdx1);
		UsesS::insert(stmtIdx4, varIdx2);
		UsesS::insert(stmtIdx5, varIdx1);
		UsesS::insert(stmtIdx6, varIdx2);

		Next::insert(stmtIdx1, stmtIdx2);
		Next::insert(stmtIdx2, stmtIdx3);
		Next::insert(stmtIdx3, stmtIdx4);
		Next::insert(stmtIdx4, stmtIdx1);
		Next::insert(stmtIdx2, stmtIdx5);
		Next::insert(stmtIdx5, stmtIdx1);
		Next::insert(stmtIdx1, stmtIdx6);
	}

	// ----------------- doesRsHold --------------------------------
	TEST_METHOD(doesRsHold_callsDoesNotModifyVar_true) {
		populateCallsDoesNotModifyVar();
		Assert::IsTrue(affectsProcessor->doesRsHold(stmtIdx1, stmtIdx3));
	};

	TEST_METHOD(doesRsHold_callsDoesNotModifyVar_false_rightIdxIsNotAssignStmt) {
		populateCallsDoesNotModifyVar();
		Assert::IsFalse(affectsProcessor->doesRsHold(stmtIdx1, stmtIdx2));
	};

	TEST_METHOD(doesRsHold_callsDoesNotModifyVar_false_leftIdxIsNotAssignStmt) {
		populateCallsDoesNotModifyVar();
		Assert::IsFalse(affectsProcessor->doesRsHold(stmtIdx2, stmtIdx3));
	};

	TEST_METHOD(doesRsHold_callsDoesNotModifyVar_false_noControlFlow) {
		populateCallsDoesNotModifyVar();
		Assert::IsFalse(affectsProcessor->doesRsHold(stmtIdx3, stmtIdx1));
	};

	TEST_METHOD(doesRsHold_callsModifiesVar_false_callsModifiesVar) {
		populateCallsModifiesVar();
		Assert::IsFalse(affectsProcessor->doesRsHold(stmtIdx1, stmtIdx3));
	};

	TEST_METHOD(doesRsHold_ifWithPathWhichDoesNotModifyVar_true) {
		populateIfWithPathWhichDoeNotModifyVar();
		Assert::IsTrue(affectsProcessor->doesRsHold(stmtIdx1, stmtIdx5));
		Assert::IsTrue(affectsProcessor->doesRsHold(stmtIdx1, stmtIdx4));
		Assert::IsTrue(affectsProcessor->doesRsHold(stmtIdx3, stmtIdx5));
	};

	TEST_METHOD(doesRsHold_ifWithPathWhichDoesNotModifyVar_false_noControlFlow) {
		populateIfWithPathWhichDoeNotModifyVar();
		Assert::IsFalse(affectsProcessor->doesRsHold(stmtIdx3, stmtIdx4));
	};

	TEST_METHOD(doesRsHold_ifWithPathWhichDoesNotModifyVar_false_doesNotFulfilAffectsDefinition) {
		populateIfWithPathWhichDoeNotModifyVar();
		Assert::IsFalse(affectsProcessor->doesRsHold(stmtIdx1, stmtIdx3));
	};

	TEST_METHOD(doesRsHold_ifWhereAllPathsModifyVar_true) {
		populateIfWhereAllPathsModifyVar();
		Assert::IsTrue(affectsProcessor->doesRsHold(stmtIdx1, stmtIdx4));
		Assert::IsTrue(affectsProcessor->doesRsHold(stmtIdx3, stmtIdx5));
		Assert::IsTrue(affectsProcessor->doesRsHold(stmtIdx4, stmtIdx5));
	};

	TEST_METHOD(doesRsHold_ifWhereAllPathsModifyVar_false_allPossibleControlFlowPathsModifyVar) {
		populateIfWhereAllPathsModifyVar();
		Assert::IsFalse(affectsProcessor->doesRsHold(stmtIdx1, stmtIdx5));
	};

	TEST_METHOD(doesRsHold_stmtsInDifferentProcedures_false_stmtsInDifferentProcedures) {
		populateStmtsInDifferentProcedures();
		Assert::IsFalse(affectsProcessor->doesRsHold(stmtIdx1, stmtIdx2));
	};

	TEST_METHOD(doesRsHold_printInWhile_true) {
		populatePrintInWhile();
		Assert::IsTrue(affectsProcessor->doesRsHold(stmtIdx1, stmtIdx4));
	};

	TEST_METHOD(doesRsHold_printInWhile_false_notAssignStmt) {
		populatePrintInWhile();
		Assert::IsFalse(affectsProcessor->doesRsHold(stmtIdx1, stmtIdx2));
		Assert::IsFalse(affectsProcessor->doesRsHold(stmtIdx1, stmtIdx3));
	};

	TEST_METHOD(doesRsHold_readInWhile_true_pathExistsWithoutGoingThroughWhileBlock) {
		populateReadInWhile();
		Assert::IsTrue(affectsProcessor->doesRsHold(stmtIdx1, stmtIdx4));
	}

	TEST_METHOD(doesRsHold_readInWhile_false) {
		populateReadInWhile();
		Assert::IsFalse(affectsProcessor->doesRsHold(stmtIdx1, stmtIdx2));
		Assert::IsFalse(affectsProcessor->doesRsHold(stmtIdx1, stmtIdx3));
		Assert::IsFalse(affectsProcessor->doesRsHold(stmtIdx2, stmtIdx3));
		Assert::IsFalse(affectsProcessor->doesRsHold(stmtIdx2, stmtIdx4));
		Assert::IsFalse(affectsProcessor->doesRsHold(stmtIdx3, stmtIdx4));
	};

	TEST_METHOD(doesRsHold_stmtInWhileAffectsItself_true_stmtAffectsItself) {
		populateStmtInWhileAffectsItself();
		Assert::IsTrue(affectsProcessor->doesRsHold(stmtIdx2, stmtIdx2));
		Assert::IsTrue(affectsProcessor->doesRsHold(stmtIdx3, stmtIdx3));
	};

	TEST_METHOD(doesRsHold_stmtInIfDoesNotAffectItself_false_stmtDoesNotAffectItself) {
		populateStmtInIfDoesNotAffectItself();
		Assert::IsFalse(affectsProcessor->doesRsHold(stmtIdx2, stmtIdx2));
		Assert::IsFalse(affectsProcessor->doesRsHold(stmtIdx3, stmtIdx3));
	};

	TEST_METHOD(doesRsHold_ifNestedInWhile_true) {
		populateIfNestedInWhile();
		Assert::IsTrue(affectsProcessor->doesRsHold(stmtIdx3, stmtIdx3));
		Assert::IsTrue(affectsProcessor->doesRsHold(stmtIdx3, stmtIdx5));
		Assert::IsTrue(affectsProcessor->doesRsHold(stmtIdx4, stmtIdx4));
		Assert::IsTrue(affectsProcessor->doesRsHold(stmtIdx4, stmtIdx6));
		Assert::IsTrue(affectsProcessor->doesRsHold(stmtIdx5, stmtIdx4));
		Assert::IsTrue(affectsProcessor->doesRsHold(stmtIdx5, stmtIdx6));
	};

	// ----------------- getUsingLeftStmtIndex --------------------------------
	TEST_METHOD(getUsingLeftStmtIndex_callsDoesNotModifyVar) {
		populateCallsDoesNotModifyVar();
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx3} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx1));
	};

	TEST_METHOD(getUsingLeftStmtIndex_callsModifiesVar) {
		populateCallsModifiesVar();
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx1));
	};

	TEST_METHOD(getUsingLeftStmtIndex_ifWithPathWhichDoesNotModifyVar) {
		populateIfWithPathWhichDoeNotModifyVar();
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx4, stmtIdx5} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx1));
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx2));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx5} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx3));
	};

	TEST_METHOD(getUsingLeftStmtIndex_isWhereAllPathsModifyVar) {
		populateIfWhereAllPathsModifyVar();
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx4} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx1));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx5} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx4));
	};

	TEST_METHOD(getUsingLeftStmtIndex_stmtsInDifferentProcedures) {
		populateStmtsInDifferentProcedures();
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx1));
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx2));
	};

	TEST_METHOD(getUsingLeftStmtIndex_printInWhile) {
		populatePrintInWhile();
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx4} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx1));
	};

	TEST_METHOD(getUsingLeftStmtIndex_printInWhile_nonAssignStmts) {
		populatePrintInWhile();
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx2));
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx3));
	};

	TEST_METHOD(getUsingLeftStmtIndex_readInWhile) {
		populateReadInWhile();
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx4} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx1));
	};

	TEST_METHOD(getUsingLeftStmtIndex_readInWhile_nonAssignStmts) {
		populateReadInWhile();
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx2));
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx3));
	};

	TEST_METHOD(getUsingLeftStmtIndex_stmtInWhileAffectsItself) {
		populateStmtInWhileAffectsItself();
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx2} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx2));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx3} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx3));
	};

	TEST_METHOD(getUsingLeftStmtIndex_stmtInIfDoesNotAffectItself) {
		populateStmtInIfDoesNotAffectItself();
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx2));
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx3));
	};

	TEST_METHOD(getUsingLeftStmtIndex_ifNestedInWhile) {
		populateIfNestedInWhile();
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx3, stmtIdx5} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx3));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx6, stmtIdx4} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx4));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx6, stmtIdx4} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx5));
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingLeftStmtIndex(stmtIdx6));
	};

	// ----------------- getUsingRightStmtIndex --------------------------------
	TEST_METHOD(getUsingRightStmtIndex_callsDoesNotModifyVar) {
		populateCallsDoesNotModifyVar();
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx3));
	};

	TEST_METHOD(getUsingRightStmtIndex_callsModifiesVar) {
		populateCallsModifiesVar();
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx3));
	};

	TEST_METHOD(getUsingRightStmtIndex_ifWithPathWhichDoesNotModifyVar) {
		populateIfWithPathWhichDoeNotModifyVar();
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx2));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx4));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx3, stmtIdx1} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx5));
	};

	TEST_METHOD(getUsingRightStmtIndex_isWhereAllPathsModifyVar) {
		populateIfWhereAllPathsModifyVar();
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx3));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx4));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx3, stmtIdx4} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx5));
	};

	TEST_METHOD(getUsingRightStmtIndex_stmtsInDifferentProcedures) {
		populateStmtsInDifferentProcedures();
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx1));
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx2));
	};

	TEST_METHOD(getUsingRightStmtIndex_printInWhile) {
		populatePrintInWhile();
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx4));
	};

	TEST_METHOD(getUsingRightStmtIndex_printInWhile_nonAssignStmts) {
		populatePrintInWhile();
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx2));
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx3));
	};

	TEST_METHOD(getUsingRightStmtIndex_readInWhile) {
		populateReadInWhile();
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx4));
	};

	TEST_METHOD(getUsingRightStmtIndex_readInWhile_nonAssignStmts) {
		populateReadInWhile();
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx2));
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx3));
	};

	TEST_METHOD(getUsingRightStmtIndex_stmtInWhileAffectsItself) {
		populateStmtInWhileAffectsItself();
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx2} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx2));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx3} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx3));
	};

	TEST_METHOD(getUsingRightStmtIndex_stmtInIfDoesNotAffectItself) {
		populateStmtInIfDoesNotAffectItself();
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx2));
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx3));
	};

	TEST_METHOD(getUsingRightStmtIndex_ifNestedInWhile) {
		populateIfNestedInWhile();
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx3} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx3));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx4, stmtIdx5} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx4));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx3} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx5));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx4, stmtIdx5} ==
			affectsProcessor->getUsingRightStmtIndex(stmtIdx6));
	};

	// ----------------- getAll --------------------------------

	TEST_METHOD(getAll_ifNestedInWhile) {
		populateIfNestedInWhile();

		std::vector<StmtIndex> leftStmtIndices = { stmtIdx3, stmtIdx3, stmtIdx4, stmtIdx4, stmtIdx5, stmtIdx5 };
		std::vector<StmtIndex> rightStmtIndices = { stmtIdx3, stmtIdx5, stmtIdx6, stmtIdx4, stmtIdx6, stmtIdx4 };
		Assert::IsTrue(std::make_tuple(leftStmtIndices, rightStmtIndices) ==
			affectsProcessor->getAll());
	};
	};
};
