#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS/PQLEvaluator.h"
#include "../source/QPS/PQLParser.h"
#include "../source/PKB/UsesS.h"
#include "../source/PKB/UsesP.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	// Uses (a/r/s/a1, v) or Uses(a/r/s/a1, "x") or Uses (a/r/s/a1, _ )
	// Uses (1, v)	=> true or Uses (1, _ ) (under statement)
	// Uses (p/p1, v) or Uses(p/p1, "x") or Uses (p/p1, _ )	proc
	TEST_CLASS(TestUsesInstruction) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Attribute::performCleanUp();
		Entity::performCleanUp();
		UsesS::performCleanUp();
		UsesP::performCleanUp();
	}
public:
	TEST_METHOD(execute_lhsSynonymRhsSynonymStmt) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::Synonym, "a1");
		rhsRef = std::make_pair(PqlReferenceType::Synonym, "v");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesS, lhsRef, rhsRef);

		// PKB inserts uses
		Entity::insertStmt(StatementType::printType);   // insert dummy stmt
		StmtIndex stmt = Entity::insertStmt(StatementType::readType);
		Entity::insertVar("randomVar"); // insert dummy var
		VarIndex varIndex = Entity::insertVar("v");
		UsesS::insert(stmt, varIndex);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 \nSynonym: v Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsSynonymRhsIdentStmt) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::Synonym, "a1");
		rhsRef = std::make_pair(PqlReferenceType::Ident, "x");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesS, lhsRef, rhsRef);

		// PKB inserts uses
		Entity::insertStmt(StatementType::readType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		UsesS::insert(stmt, varIndex);
		std::vector<int> allstmts = UsesS::getFromVariable(varIndex);
		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsSynonymRhsWildCardStmt) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::Synonym, "a1");
		rhsRef = std::make_pair(PqlReferenceType::Wildcard, "_");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesS, lhsRef, rhsRef);

		// PKB inserts uses
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::printType);
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		UsesS::insert(stmt, varIndex);
		UsesS::insert(stmt, varIndex2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(2), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 2 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsConstRhsSynonym_EvTableTrue) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::Integer, "2");
		rhsRef = std::make_pair(PqlReferenceType::Synonym, "a1");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesS, lhsRef, rhsRef);

		// PKB inserts uses
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::readType);
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		UsesS::insert(stmt, varIndex);
		UsesS::insert(stmt, varIndex2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(2), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 3 \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsConstRhsSynonym_EvTableFalse) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::Integer, "2");
		rhsRef = std::make_pair(PqlReferenceType::Synonym, "a1");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesS, lhsRef, rhsRef);

		// PKB inserts uses
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::readType);
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: a1 Values: \n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsConstRhsSynonym_ConstOutOfBoundsEvTableFalse) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::Integer, "50");
		rhsRef = std::make_pair(PqlReferenceType::Synonym, "a1");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesS, lhsRef, rhsRef);

		// PKB inserts modifies
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::readType);
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		UsesS::insert(stmt, varIndex);
		UsesS::insert(stmt, varIndex2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(false, evTable.getEvResult());
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 0\n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsConstRhsSynonym_VarOutOfBoundsEvTableFalse) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::Integer, "2");
		rhsRef = std::make_pair(PqlReferenceType::Ident, "fhg");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesS, lhsRef, rhsRef);

		// PKB inserts modifies
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::readType);
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		UsesS::insert(stmt, varIndex);
		UsesS::insert(stmt, varIndex2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(false, evTable.getEvResult());
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 0\n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsConstRhsWildcard_EvTableTrue) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::Integer, "2");
		rhsRef = std::make_pair(PqlReferenceType::Wildcard, "_");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesS, lhsRef, rhsRef);

		// PKB inserts uses
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::readType);
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		UsesS::insert(stmt, varIndex);
		UsesS::insert(stmt, varIndex2);

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(true, evTable.getEvResult());
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 0\n";
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsConstRhsWildcard_EvTableFalse) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::Integer, "2");
		rhsRef = std::make_pair(PqlReferenceType::Wildcard, "_");
		Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesS, lhsRef, rhsRef);

		// PKB inserts uses
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::readType);
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(false, evTable.getEvResult());
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 0\n";
		Assert::AreEqual(expected, evTable.getTableString());
	}
	};
}
