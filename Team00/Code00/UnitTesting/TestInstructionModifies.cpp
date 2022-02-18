#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include <algorithm>

#include "../source/QPS-NEW/PQLEvaluator.h"
#include "../source/QPS-NEW/PQLParser.h"
#include "../source/PKB/RS2.h"
#include "../source/PKB/Follows.h"
#include "../source/PKB/FollowsT.h"
#include "../source/PKB/Parent.h"
#include "../source/PKB/ParentT.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	// Modifies (a/r/s/a1, v) or Modifies(a/r/s/a1, "x") or Modifies (a/r/s/a1, _ ) 
	// Modifies (1, v)	=> true or Modifies (1, _ ) (under statement)
	// Modifies (p/p1, v) or Modifies(p/p1, "x") or Modifies (p/p1, _ )	proc
	TEST_CLASS(TestInstructionModifies)
	{
	private:

	public:
        TEST_METHOD(executeInstruction_modifies_lhsSynonymRhsSynonym)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "a1");
            rhsRef = std::make_pair(PqlReferenceType::synonym, "v");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::ModifiesS, lhsRef, rhsRef);

            // PKB inserts modifies
            Entity::insertStmt(StatementType::printType);   // insert dummy stmt
            StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
            Entity::insertVar("randomVar"); // insert dummy var
            VarIndex varIndex = Entity::insertVar("v");
            Modifies::insert(stmt, varIndex);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();
            Assert::AreEqual(size_t(1), evTable.getNumRow());
            std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 \nSynonym: v Values: 2 \n";
            Assert::AreEqual(expected, evTable.getTableString());

            // 3. Clean-up:
            Entity::performCleanUp();
            Modifies::performCleanUp();
        }

        TEST_METHOD(executeInstruction_modifies_lhsSynonymRhsIdent)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "a1");
            rhsRef = std::make_pair(PqlReferenceType::ident, "x");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::ModifiesS, lhsRef, rhsRef);

            // PKB inserts modifies
            Entity::insertStmt(StatementType::printType);
            StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
            Entity::insertVar("randomVar");
            VarIndex varIndex = Entity::insertVar("x");
            Modifies::insert(stmt, varIndex);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();
            Assert::AreEqual(size_t(1), evTable.getNumRow());
            std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 \nSynonym: x Values: 2 \n";
            Assert::AreEqual(expected, evTable.getTableString());

            // 3. Clean-up:
            Entity::performCleanUp();
            Modifies::performCleanUp();
        }

        TEST_METHOD(executeInstruction_modifies_lhsSynonymRhsWildCard)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "a1");
            rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::ModifiesS, lhsRef, rhsRef);

            // PKB inserts modifies
            Entity::insertStmt(StatementType::printType);
            StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
            Entity::insertVar("randomVar");
            VarIndex varIndex = Entity::insertVar("x");
            VarIndex varIndex2 = Entity::insertVar("y");
            Modifies::insert(stmt, varIndex);
            Modifies::insert(stmt, varIndex2);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();
            Assert::AreEqual(size_t(2), evTable.getNumRow());
            std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 2 \n";
            Assert::AreEqual(expected, evTable.getTableString());

            // 3. Clean-up:
            Entity::performCleanUp();
            Modifies::performCleanUp();
        }

        TEST_METHOD(executeInstruction_modifiesEvTableTrue_lhsConstRhsSynonym)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "1");
            rhsRef = std::make_pair(PqlReferenceType::synonym, "a1");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::ModifiesS, lhsRef, rhsRef);

            // PKB inserts modifies
            Entity::insertStmt(StatementType::printType);
            StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
            Entity::insertVar("randomVar");
            VarIndex varIndex = Entity::insertVar("x");
            VarIndex varIndex2 = Entity::insertVar("y");
            Modifies::insert(stmt, varIndex);
            Modifies::insert(stmt, varIndex2);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();
            Assert::AreEqual(size_t(0), evTable.getNumRow());
            std::string expected = "Table String: size: 0\n";
            Assert::AreEqual(expected, evTable.getTableString());

            // 3. Clean-up:
            Entity::performCleanUp();
            Modifies::performCleanUp();
        }

        TEST_METHOD(executeInstruction_modifiesEvTableFalse_lhsConstRhsSynonym)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "1");
            rhsRef = std::make_pair(PqlReferenceType::synonym, "a1");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::ModifiesS, lhsRef, rhsRef);

            // PKB inserts modifies
            Entity::insertStmt(StatementType::printType);
            StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
            Entity::insertVar("randomVar");
            VarIndex varIndex = Entity::insertVar("x");
            VarIndex varIndex2 = Entity::insertVar("y");

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();
            Assert::AreEqual(size_t(0), evTable.getNumRow());
            std::string expected = "Table String: size: 0\n";
            Assert::AreEqual(expected, evTable.getTableString());

            // 3. Clean-up:
            Entity::performCleanUp();
            Modifies::performCleanUp();
        }

        TEST_METHOD(executeInstruction_modifiesEvTableTrue_lhsConstRhsWildcard)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "1");
            rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::ModifiesS, lhsRef, rhsRef);

            // PKB inserts modifies
            Entity::insertStmt(StatementType::printType);
            StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
            Entity::insertVar("randomVar");
            VarIndex varIndex = Entity::insertVar("x");
            VarIndex varIndex2 = Entity::insertVar("y");
            Modifies::insert(stmt, varIndex);
            Modifies::insert(stmt, varIndex2);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();
            Assert::AreEqual(size_t(0), evTable.getNumRow());
            std::string expected = "Table String: size: 0\n";
            Assert::AreEqual(expected, evTable.getTableString());

            // 3. Clean-up:
            Entity::performCleanUp();
            Modifies::performCleanUp();
        }

        TEST_METHOD(executeInstruction_modifiesEvTableFalse_lhsConstRhsWildcard)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "1");
            rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::ModifiesS, lhsRef, rhsRef);

            // PKB inserts modifies
            Entity::insertStmt(StatementType::printType);
            StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
            Entity::insertVar("randomVar");
            VarIndex varIndex = Entity::insertVar("x");
            VarIndex varIndex2 = Entity::insertVar("y");

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();
            Assert::AreEqual(size_t(0), evTable.getNumRow());
            std::string expected = "Table String: size: 0\n";
            Assert::AreEqual(expected, evTable.getTableString());

            // 3. Clean-up:
            Entity::performCleanUp();
            Modifies::performCleanUp();
        }

	};
}
