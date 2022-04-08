#include "CppUnitTest.h"
#include "stdafx.h"

#include "../source/PKB/PKBGetter.h"
#include "../source/PKB/PKBInserter.h"
#include "QPS/WithInstruction.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestWithInstruction) {
private:
	PKB* pkb;
	PKBGetter* pkbGetter;
	PKBInserter* pkbInserter;

	TEST_METHOD_INITIALIZE(init) {
		pkb = new PKB();
		pkbGetter = new PKBGetter(pkb);
		pkbInserter = new PKBInserter(pkb);
	}

public:
	TEST_METHOD(withInstruction_testStringTwoAttributeComparison_successful) {
		/* Insert test program:
		* procedure proc1 {
		* read proc1;
		* print read;
		* read = proc1 + 2;
		* }
		*/
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmtIndex1 = pkbInserter->insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "proc1");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex1, varProc1);
		pkbInserter->insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "read");
		pkbInserter->insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex2, varRead);
		StmtIndex stmtIndex3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex3, varRead);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex3, varProc1);
		pkbInserter->insertConst(2);

		/* 1. Setup:
		* Clause: with read.varName = proc.procName
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::VAR_NAME, "read");
		rhsRef = std::make_pair(PqlReferenceType::PROC_NAME, "proc");
		Instruction* instruction = new WithStringInstruction(lhsRef, rhsRef, EntityType::READ, EntityType::PROCEDURE, pkbGetter);

		/* Test getSynonyms() */
		std::unordered_set<std::string> synonyms = instruction->getSynonyms();
		Assert::IsTrue(synonyms.find("read") != synonyms.end());
		Assert::IsTrue(synonyms.find("proc") != synonyms.end());

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::unordered_map<std::string, std::vector<int>> table = evTable.getTableRef();
		Assert::AreEqual(size_t(2), table.size());
		Assert::IsTrue(table.find("read") != table.end());
		Assert::IsTrue(table.find("proc") != table.end());
	}

	TEST_METHOD(withInstruction_testStringAttributeIdentComparison_successful) {
		/* Insert test program:
		* procedure proc1 {
		* read proc1;
		* print read;
		* read = proc1 + 2;
		* }
		*/
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");

		StmtIndex stmtIndex1 = pkbInserter->insertStmt(StatementType::READ_TYPE);
		pkbInserter->insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		VarIndex varProc1 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "proc1");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex1, varProc1);	// read proc1;

		StmtIndex stmtIndex2 = pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		pkbInserter->insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		VarIndex varRead = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "read");
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex2, varRead);	// print read

		StmtIndex stmtIndex3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		/*pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex3, varRead);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex3, varProc1);
		pkbInserter->insertConst(2);*/

		/*ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmtIndex1 = pkbInserter->insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "proc1");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex1, varProc1);
		pkbInserter->insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "read");
		pkbInserter->insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex2, varRead);
		StmtIndex stmtIndex3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex3, varRead);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex3, varProc1);
		pkbInserter->insertConst(2);*/

		/* 1. Setup:
		* Clause: with print.varName = "read"
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::VAR_NAME, "print");
		rhsRef = std::make_pair(PqlReferenceType::IDENT, "read");
		Instruction* instruction = new WithStringInstruction(lhsRef, rhsRef, EntityType::PRINT, EntityType::VARIABLE, pkbGetter);

		/* Test getSynonyms() */
		std::unordered_set<std::string> synonyms = instruction->getSynonyms();
		Assert::IsTrue(synonyms.find("read") == synonyms.end());
		Assert::IsTrue(synonyms.find("print") != synonyms.end());

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());									// Error in this line -> actual gives 0
		std::unordered_map<std::string, std::vector<int>> table = evTable.getTableRef();
		Assert::AreEqual(size_t(1), table.size());
		Assert::IsTrue(table.find("print") != table.end());
	}

	TEST_METHOD(withInstruction_testStringTwoIdentComparison_successful) {
		/* Insert test program:
		* procedure proc1 {
		* read proc1;
		* print read;
		* read = proc1 + 2;
		* }
		*/
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmtIndex1 = pkbInserter->insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "proc1");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex1, varProc1);
		pkbInserter->insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "read");
		pkbInserter->insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex2, varRead);
		StmtIndex stmtIndex3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex3, varRead);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex3, varProc1);
		pkbInserter->insertConst(2);

		/* 1. Setup:
		* Clause: with "print" = "read"
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::IDENT, "print");
		rhsRef = std::make_pair(PqlReferenceType::IDENT, "read");
		Instruction* instruction = new WithStringInstruction(lhsRef, rhsRef, EntityType::CONSTANT, EntityType::CONSTANT, pkbGetter);

		/* Test getSynonyms() */
		std::unordered_set<std::string> synonyms = instruction->getSynonyms();
		Assert::IsTrue(synonyms.find("read") == synonyms.end());
		Assert::IsTrue(synonyms.find("print") == synonyms.end());

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::unordered_map<std::string, std::vector<int>> table = evTable.getTableRef();
		Assert::AreEqual(size_t(0), table.size());
		Assert::AreEqual(false, evTable.getEvResult());
	}

	TEST_METHOD(withInstruction_testIntegerTwoAttributeComparison_successful) {
		/* Insert test program:
		* procedure proc1 {
		* read proc1;
		* print read;
		* read = proc1 + 2;
		* }
		*/
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmtIndex1 = pkbInserter->insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "proc1");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex1, varProc1);
		pkbInserter->insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "read");
		pkbInserter->insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex2, varRead);
		StmtIndex stmtIndex3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex3, varRead);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex3, varProc1);
		pkbInserter->insertConst(2);

		/* 1. Setup:
		* Clause: with stmt.stmt# = constant.value
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::STMT_NUM, "stmt");
		rhsRef = std::make_pair(PqlReferenceType::VALUE, "constant");
		Instruction* instruction = new WithIntegerInstruction(lhsRef, rhsRef, EntityType::STMT, EntityType::CONSTANT, pkbGetter);

		/* Test getSynonyms() */
		std::unordered_set<std::string> synonyms = instruction->getSynonyms();
		Assert::IsTrue(synonyms.find("stmt") != synonyms.end());
		Assert::IsTrue(synonyms.find("constant") != synonyms.end());

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::unordered_map<std::string, std::vector<int>> table = evTable.getTableRef();
		Assert::AreEqual(size_t(2), table.size());
		Assert::IsTrue(table.find("stmt") != table.end());
		Assert::IsTrue(table.find("constant") != table.end());
	}

	TEST_METHOD(withInstruction_testIntegerAttributeIntegerComparison_successful) {
		/* Insert test program:
		* procedure proc1 {
		* read proc1;
		* print read;
		* read = proc1 + 2;
		* }
		*/
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmtIndex1 = pkbInserter->insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "proc1");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex1, varProc1);
		pkbInserter->insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "read");
		pkbInserter->insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex2, varRead);
		StmtIndex stmtIndex3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex3, varRead);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex3, varProc1);
		pkbInserter->insertConst(2);

		/* 1. Setup:
		* Clause: with 1 = read.stmt#
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::INTEGER, "1");
		rhsRef = std::make_pair(PqlReferenceType::STMT_NUM, "read");
		Instruction* instruction = new WithIntegerInstruction(lhsRef, rhsRef, EntityType::CONSTANT, EntityType::READ, pkbGetter);

		/* Test getSynonyms() */
		std::unordered_set<std::string> synonyms = instruction->getSynonyms();
		Assert::IsTrue(synonyms.find("read") != synonyms.end());
		Assert::IsTrue(synonyms.find("1") == synonyms.end());

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::unordered_map<std::string, std::vector<int>> table = evTable.getTableRef();
		Assert::AreEqual(size_t(1), table.size());
		Assert::IsTrue(table.find("read") != table.end());
	}

	TEST_METHOD(withInstruction_testIntegerTwoIntegerComparison_successful) {
		/* Insert test program:
		* procedure proc1 {
		* read proc1;
		* print read;
		* read = proc1 + 2;
		* }
		*/
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmtIndex1 = pkbInserter->insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "proc1");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex1, varProc1);
		pkbInserter->insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "read");
		pkbInserter->insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex2, varRead);
		StmtIndex stmtIndex3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex3, varRead);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex3, varProc1);
		pkbInserter->insertConst(2);

		/* 1. Setup:
		* Clause: with 3 = 0
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::INTEGER, "3");
		rhsRef = std::make_pair(PqlReferenceType::INTEGER, "0");
		Instruction* instruction = new WithIntegerInstruction(lhsRef, rhsRef, EntityType::CONSTANT, EntityType::CONSTANT, pkbGetter);

		/* Test getSynonyms() */
		std::unordered_set<std::string> synonyms = instruction->getSynonyms();
		Assert::IsTrue(synonyms.find("3") == synonyms.end());
		Assert::IsTrue(synonyms.find("0") == synonyms.end());

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::unordered_map<std::string, std::vector<int>> table = evTable.getTableRef();
		Assert::AreEqual(size_t(0), table.size());
		Assert::AreEqual(false, evTable.getEvResult());
	}
	};
}
