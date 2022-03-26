#include "CppUnitTest.h"
#include "stdafx.h"

#include "QPS/WithInstruction.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestWithInstruction) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Attribute::performCleanUp();
		Entity::performCleanUp();
		FollowsT::performCleanUp();
		Follows::performCleanUp();
		ParentT::performCleanUp();
		Parent::performCleanUp();
		ModifiesP::performCleanUp();
		ModifiesS::performCleanUp();
		UsesP::performCleanUp();
		UsesS::performCleanUp();
		Pattern::performCleanUp();
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
		ProcIndex procIndex = Entity::insertProc("proc1");
		StmtIndex stmtIndex1 = Entity::insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = Entity::insertVar("proc1");
		ModifiesS::insert(stmtIndex1, varProc1);
		Attribute::insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = Entity::insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = Entity::insertVar("read");
		Attribute::insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		UsesS::insert(stmtIndex2, varRead);
		StmtIndex stmtIndex3 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		ModifiesS::insert(stmtIndex3, varRead);
		UsesS::insert(stmtIndex3, varProc1);
		Entity::insertConst(2);

		/* 1. Setup:
		* Clause: with read.varName = proc.procName
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::VAR_NAME, "read");
		rhsRef = std::make_pair(PqlReferenceType::PROC_NAME, "proc");
		Instruction* instruction = new WithInstruction(lhsRef, rhsRef, EntityType::READ, EntityType::PROCEDURE, PqlAttributeType::STRING);

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::unordered_map<std::string, EntityType> entities = evTable.getEntities();
		Assert::AreEqual(size_t(2), entities.size());
		Assert::IsTrue(entities.find("read") != entities.end());
		Assert::IsTrue(entities.find("proc") != entities.end());
	}

	TEST_METHOD(withInstruction_testStringAttributeIdentComparison_successful) {
		/* Insert test program:
		* procedure proc1 {
		* read proc1;
		* print read;
		* read = proc1 + 2;
		* }
		*/
		ProcIndex procIndex = Entity::insertProc("proc1");
		StmtIndex stmtIndex1 = Entity::insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = Entity::insertVar("proc1");
		ModifiesS::insert(stmtIndex1, varProc1);
		Attribute::insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = Entity::insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = Entity::insertVar("read");
		Attribute::insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		UsesS::insert(stmtIndex2, varRead);
		StmtIndex stmtIndex3 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		ModifiesS::insert(stmtIndex3, varRead);
		UsesS::insert(stmtIndex3, varProc1);
		Entity::insertConst(2);

		/* 1. Setup:
		* Clause: with print.varName = "read"
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::VAR_NAME, "print");
		rhsRef = std::make_pair(PqlReferenceType::IDENT, "read");
		/* TODO: Redesign WithInstruction*/
		Instruction* instruction = new WithInstruction(lhsRef, rhsRef, EntityType::PRINT, EntityType::CONSTANT, PqlAttributeType::STRING);

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::unordered_map<std::string, EntityType> entities = evTable.getEntities();
		Assert::AreEqual(size_t(1), entities.size());
		Assert::IsTrue(entities.find("print") != entities.end());
	}

	TEST_METHOD(withInstruction_testStringTwoIdentComparison_successful) {
		/* Insert test program:
		* procedure proc1 {
		* read proc1;
		* print read;
		* read = proc1 + 2;
		* }
		*/
		ProcIndex procIndex = Entity::insertProc("proc1");
		StmtIndex stmtIndex1 = Entity::insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = Entity::insertVar("proc1");
		ModifiesS::insert(stmtIndex1, varProc1);
		Attribute::insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = Entity::insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = Entity::insertVar("read");
		Attribute::insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		UsesS::insert(stmtIndex2, varRead);
		StmtIndex stmtIndex3 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		ModifiesS::insert(stmtIndex3, varRead);
		UsesS::insert(stmtIndex3, varProc1);
		Entity::insertConst(2);

		/* 1. Setup:
		* Clause: with "print" = "read"
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::IDENT, "print");
		rhsRef = std::make_pair(PqlReferenceType::IDENT, "read");
		/* TODO: Redesign WithInstruction*/
		Instruction* instruction = new WithInstruction(lhsRef, rhsRef, EntityType::CONSTANT, EntityType::CONSTANT, PqlAttributeType::STRING);

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::unordered_map<std::string, EntityType> entities = evTable.getEntities();
		Assert::AreEqual(size_t(0), entities.size());
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
		ProcIndex procIndex = Entity::insertProc("proc1");
		StmtIndex stmtIndex1 = Entity::insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = Entity::insertVar("proc1");
		ModifiesS::insert(stmtIndex1, varProc1);
		Attribute::insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = Entity::insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = Entity::insertVar("read");
		Attribute::insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		UsesS::insert(stmtIndex2, varRead);
		StmtIndex stmtIndex3 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		ModifiesS::insert(stmtIndex3, varRead);
		UsesS::insert(stmtIndex3, varProc1);
		Entity::insertConst(2);

		/* 1. Setup:
		* Clause: with stmt.stmt# = constant.value
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::STMT_NUM, "stmt");
		rhsRef = std::make_pair(PqlReferenceType::VALUE, "constant");
		Instruction* instruction = new WithInstruction(lhsRef, rhsRef, EntityType::STMT, EntityType::CONSTANT, PqlAttributeType::INTEGER);

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::unordered_map<std::string, EntityType> entities = evTable.getEntities();
		Assert::AreEqual(size_t(2), entities.size());
		Assert::IsTrue(entities.find("stmt") != entities.end());
		Assert::IsTrue(entities.find("constant") != entities.end());
	}

	TEST_METHOD(withInstruction_testIntegerAttributeIntegerComparison_successful) {
		/* Insert test program:
		* procedure proc1 {
		* read proc1;
		* print read;
		* read = proc1 + 2;
		* }
		*/
		ProcIndex procIndex = Entity::insertProc("proc1");
		StmtIndex stmtIndex1 = Entity::insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = Entity::insertVar("proc1");
		ModifiesS::insert(stmtIndex1, varProc1);
		Attribute::insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = Entity::insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = Entity::insertVar("read");
		Attribute::insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		UsesS::insert(stmtIndex2, varRead);
		StmtIndex stmtIndex3 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		ModifiesS::insert(stmtIndex3, varRead);
		UsesS::insert(stmtIndex3, varProc1);
		Entity::insertConst(2);

		/* 1. Setup:
		* Clause: with 1 = read.stmt#
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::INTEGER, "1");
		rhsRef = std::make_pair(PqlReferenceType::STMT_NUM, "read");
		/* TODO: Redesign WithInstruction*/
		Instruction* instruction = new WithInstruction(lhsRef, rhsRef, EntityType::CONSTANT, EntityType::READ, PqlAttributeType::INTEGER);

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::unordered_map<std::string, EntityType> entities = evTable.getEntities();
		Assert::AreEqual(size_t(1), entities.size());
		Assert::IsTrue(entities.find("read") != entities.end());
	}

	TEST_METHOD(withInstruction_testIntegerTwoIntegerComparison_successful) {
		/* Insert test program:
		* procedure proc1 {
		* read proc1;
		* print read;
		* read = proc1 + 2;
		* }
		*/
		ProcIndex procIndex = Entity::insertProc("proc1");
		StmtIndex stmtIndex1 = Entity::insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = Entity::insertVar("proc1");
		ModifiesS::insert(stmtIndex1, varProc1);
		Attribute::insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = Entity::insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = Entity::insertVar("read");
		Attribute::insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		UsesS::insert(stmtIndex2, varRead);
		StmtIndex stmtIndex3 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		ModifiesS::insert(stmtIndex3, varRead);
		UsesS::insert(stmtIndex3, varProc1);
		Entity::insertConst(2);

		/* 1. Setup:
		* Clause: with 3 = 0
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::INTEGER, "3");
		rhsRef = std::make_pair(PqlReferenceType::INTEGER, "0");
		/* TODO: Redesign WithInstruction*/
		Instruction* instruction = new WithInstruction(lhsRef, rhsRef, EntityType::CONSTANT, EntityType::CONSTANT, PqlAttributeType::INTEGER);

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::unordered_map<std::string, EntityType> entities = evTable.getEntities();
		Assert::AreEqual(size_t(0), entities.size());
		Assert::AreEqual(false, evTable.getEvResult());
	}
	};
}
