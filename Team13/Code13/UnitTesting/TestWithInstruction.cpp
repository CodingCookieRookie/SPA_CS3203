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
		StmtIndex stmtIndex1 = Entity::insertStmt(StatementType::readType);
		VarIndex varProc1 = Entity::insertVar("proc1");
		ModifiesS::insert(stmtIndex1, varProc1);
		Attribute::insertStmtByName(stmtIndex1, StatementType::readType, std::string("proc1"));
		StmtIndex stmtIndex2 = Entity::insertStmt(StatementType::printType);
		VarIndex varRead = Entity::insertVar("read");
		Attribute::insertStmtByName(stmtIndex2, StatementType::printType, std::string("read"));
		UsesS::insert(stmtIndex2, varRead);
		StmtIndex stmtIndex3 = Entity::insertStmt(StatementType::assignType);
		ModifiesS::insert(stmtIndex3, varRead);
		UsesS::insert(stmtIndex3, varProc1);
		Entity::insertConst(2);


		/* 1. Setup:
		* Clause: with read.varName = proc.procName
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::varName, "read");
		rhsRef = std::make_pair(PqlReferenceType::procName, "proc");
		Instruction* instruction = new WithInstruction(lhsRef, rhsRef, PqlEntityType::Read, PqlEntityType::Procedure, PqlAttributeType::string);

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::unordered_map<std::string, PqlEntityType> entities = evTable.getEntities();
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
		StmtIndex stmtIndex1 = Entity::insertStmt(StatementType::readType);
		VarIndex varProc1 = Entity::insertVar("proc1");
		ModifiesS::insert(stmtIndex1, varProc1);
		Attribute::insertStmtByName(stmtIndex1, StatementType::readType, std::string("proc1"));
		StmtIndex stmtIndex2 = Entity::insertStmt(StatementType::printType);
		VarIndex varRead = Entity::insertVar("read");
		Attribute::insertStmtByName(stmtIndex2, StatementType::printType, std::string("read"));
		UsesS::insert(stmtIndex2, varRead);
		StmtIndex stmtIndex3 = Entity::insertStmt(StatementType::assignType);
		ModifiesS::insert(stmtIndex3, varRead);
		UsesS::insert(stmtIndex3, varProc1);
		Entity::insertConst(2);


		/* 1. Setup:
		* Clause: with print.varName = "read"
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::varName, "print");
		rhsRef = std::make_pair(PqlReferenceType::ident, "read");
		/* TODO: Redesign WithInstruction*/
		Instruction* instruction = new WithInstruction(lhsRef, rhsRef, PqlEntityType::Print, PqlEntityType::Constant, PqlAttributeType::string);

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::unordered_map<std::string, PqlEntityType> entities = evTable.getEntities();
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
		StmtIndex stmtIndex1 = Entity::insertStmt(StatementType::readType);
		VarIndex varProc1 = Entity::insertVar("proc1");
		ModifiesS::insert(stmtIndex1, varProc1);
		Attribute::insertStmtByName(stmtIndex1, StatementType::readType, std::string("proc1"));
		StmtIndex stmtIndex2 = Entity::insertStmt(StatementType::printType);
		VarIndex varRead = Entity::insertVar("read");
		Attribute::insertStmtByName(stmtIndex2, StatementType::printType, std::string("read"));
		UsesS::insert(stmtIndex2, varRead);
		StmtIndex stmtIndex3 = Entity::insertStmt(StatementType::assignType);
		ModifiesS::insert(stmtIndex3, varRead);
		UsesS::insert(stmtIndex3, varProc1);
		Entity::insertConst(2);


		/* 1. Setup:
		* Clause: with "print" = "read"
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::ident, "print");
		rhsRef = std::make_pair(PqlReferenceType::ident, "read");
		/* TODO: Redesign WithInstruction*/
		Instruction* instruction = new WithInstruction(lhsRef, rhsRef, PqlEntityType::Constant, PqlEntityType::Constant, PqlAttributeType::string);

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::unordered_map<std::string, PqlEntityType> entities = evTable.getEntities();
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
		StmtIndex stmtIndex1 = Entity::insertStmt(StatementType::readType);
		VarIndex varProc1 = Entity::insertVar("proc1");
		ModifiesS::insert(stmtIndex1, varProc1);
		Attribute::insertStmtByName(stmtIndex1, StatementType::readType, std::string("proc1"));
		StmtIndex stmtIndex2 = Entity::insertStmt(StatementType::printType);
		VarIndex varRead = Entity::insertVar("read");
		Attribute::insertStmtByName(stmtIndex2, StatementType::printType, std::string("read"));
		UsesS::insert(stmtIndex2, varRead);
		StmtIndex stmtIndex3 = Entity::insertStmt(StatementType::assignType);
		ModifiesS::insert(stmtIndex3, varRead);
		UsesS::insert(stmtIndex3, varProc1);
		Entity::insertConst(2);


		/* 1. Setup:
		* Clause: with stmt.stmt# = constant.value
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::stmtNum, "stmt");
		rhsRef = std::make_pair(PqlReferenceType::value, "constant");
		Instruction* instruction = new WithInstruction(lhsRef, rhsRef, PqlEntityType::Stmt, PqlEntityType::Constant, PqlAttributeType::integer);

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::unordered_map<std::string, PqlEntityType> entities = evTable.getEntities();
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
		StmtIndex stmtIndex1 = Entity::insertStmt(StatementType::readType);
		VarIndex varProc1 = Entity::insertVar("proc1");
		ModifiesS::insert(stmtIndex1, varProc1);
		Attribute::insertStmtByName(stmtIndex1, StatementType::readType, std::string("proc1"));
		StmtIndex stmtIndex2 = Entity::insertStmt(StatementType::printType);
		VarIndex varRead = Entity::insertVar("read");
		Attribute::insertStmtByName(stmtIndex2, StatementType::printType, std::string("read"));
		UsesS::insert(stmtIndex2, varRead);
		StmtIndex stmtIndex3 = Entity::insertStmt(StatementType::assignType);
		ModifiesS::insert(stmtIndex3, varRead);
		UsesS::insert(stmtIndex3, varProc1);
		Entity::insertConst(2);


		/* 1. Setup:
		* Clause: with 1 = read.stmt#
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::integer, "1");
		rhsRef = std::make_pair(PqlReferenceType::stmtNum, "read");
		/* TODO: Redesign WithInstruction*/
		Instruction* instruction = new WithInstruction(lhsRef, rhsRef, PqlEntityType::Constant, PqlEntityType::Read, PqlAttributeType::integer);

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::unordered_map<std::string, PqlEntityType> entities = evTable.getEntities();
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
		StmtIndex stmtIndex1 = Entity::insertStmt(StatementType::readType);
		VarIndex varProc1 = Entity::insertVar("proc1");
		ModifiesS::insert(stmtIndex1, varProc1);
		Attribute::insertStmtByName(stmtIndex1, StatementType::readType, std::string("proc1"));
		StmtIndex stmtIndex2 = Entity::insertStmt(StatementType::printType);
		VarIndex varRead = Entity::insertVar("read");
		Attribute::insertStmtByName(stmtIndex2, StatementType::printType, std::string("read"));
		UsesS::insert(stmtIndex2, varRead);
		StmtIndex stmtIndex3 = Entity::insertStmt(StatementType::assignType);
		ModifiesS::insert(stmtIndex3, varRead);
		UsesS::insert(stmtIndex3, varProc1);
		Entity::insertConst(2);


		/* 1. Setup:
		* Clause: with 3 = 0
		*/
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::integer, "3");
		rhsRef = std::make_pair(PqlReferenceType::integer, "0");
		/* TODO: Redesign WithInstruction*/
		Instruction* instruction = new WithInstruction(lhsRef, rhsRef, PqlEntityType::Constant, PqlEntityType::Constant, PqlAttributeType::integer);

		/* 2. Check result of EvTable */
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::unordered_map<std::string, PqlEntityType> entities = evTable.getEntities();
		Assert::AreEqual(size_t(0), entities.size());
		Assert::AreEqual(false, evTable.getEvResult());
	}
	};
}
