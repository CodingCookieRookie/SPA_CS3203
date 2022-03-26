#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS/PQLEvaluator.h"
#include "../source/QPS/PQLParser.h"
#include "../source/PKB/ModifiesS.h"
#include "../source/PKB/ModifiesP.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	// Modifies (a/r/s/a1, v) or Modifies(a/r/s/a1, "x") or Modifies (a/r/s/a1, _ )
	// Modifies (1, v)	=> true or Modifies (1, _ ) (under statement)
	// Modifies (p/p1, v) or Modifies(p/p1, "x") or Modifies (p/p1, _ )	proc
	TEST_CLASS(TestModifiesInstruction) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Attribute::performCleanUp();
		Entity::performCleanUp();
		ModifiesS::performCleanUp();
		ModifiesP::performCleanUp();
	}
public:
	TEST_METHOD(execute_lhsSynonymRhsSynonymStmt) {
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::Synonym, "a1");
		rhsRef = std::make_pair(PqlReferenceType::Synonym, "v");
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef);

		// PKB inserts modifies
		Entity::insertStmt(StatementType::printType);   // insert dummy stmt
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("randomVar"); // insert dummy var
		VarIndex varIndex = Entity::insertVar("v");
		ModifiesS::insert(stmt, varIndex);

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
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef);

		// PKB inserts modifies
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		ModifiesS::insert(stmt, varIndex);

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
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef);

		// PKB inserts modifies
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		ModifiesS::insert(stmt, varIndex);
		ModifiesS::insert(stmt, varIndex2);

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
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef);

		// PKB inserts modifies
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		ModifiesS::insert(stmt, varIndex);
		ModifiesS::insert(stmt, varIndex2);

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
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef);

		// PKB inserts modifies
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
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
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef);

		// PKB inserts modifies
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		ModifiesS::insert(stmt, varIndex);
		ModifiesS::insert(stmt, varIndex2);

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
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef);

		// PKB inserts modifies
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		ModifiesS::insert(stmt, varIndex);
		ModifiesS::insert(stmt, varIndex2);

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
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef);

		// PKB inserts modifies
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("randomVar");
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		ModifiesS::insert(stmt, varIndex);
		ModifiesS::insert(stmt, varIndex2);

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
		Instruction* instruction = new ModifiesSInstruction(lhsRef, rhsRef);

		// PKB inserts modifies
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
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
