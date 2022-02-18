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
	TEST_CLASS(TestModifiesInstruction)
	{
	private:
        TEST_METHOD_CLEANUP(cleanUpTables) {
            Entity::performCleanUp();
            Modifies::performCleanUp();
        }
	public:
        TEST_METHOD(execute_lhsSynonymRhsSynonymStmt)
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
        }

        TEST_METHOD(execute_lhsSynonymRhsIdentStmt)
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
            std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 \n";
            Assert::AreEqual(expected, evTable.getTableString());
        }

        TEST_METHOD(execute_lhsSynonymRhsWildCardStmt)
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
        }

        // <-- Iteration 2 -->
        //TEST_METHOD(execute_lhsSynonymRhsSynonymProc)
        //{

        //    // 1. Setup:
        //    PqlReference lhsRef, rhsRef;
        //    lhsRef = std::make_pair(PqlReferenceType::synonym, "p");
        //    rhsRef = std::make_pair(PqlReferenceType::synonym, "v");
        //    Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::ModifiesP, lhsRef, rhsRef);

        //    // PKB inserts modifies
        //    Entity::insertProc("randomProc");
        //    ProcIndex procIndex = Entity::insertProc("p");
        //    Entity::insertVar("randomVar");
        //    VarIndex varIndex = Entity::insertVar("x");
        //    Modifies::insert(procIndex, varIndex);

        //    // 2. Main test:
        //    EvaluatedTable evTable = instruction->execute();
        //    Assert::AreEqual(size_t(1), evTable.getNumRow());
        //    std::string expected = "Table String: size: 2\nSynonym: p Values: 2 \nSynonym: v Values: 2 \n";
        //    Assert::AreEqual(expected, evTable.getTableString());
        //}

        //TEST_METHOD(execute_lhsSynonymRhsIdentProc)
        //{

        //    // 1. Setup:
        //    PqlReference lhsRef, rhsRef;
        //    lhsRef = std::make_pair(PqlReferenceType::synonym, "p");
        //    rhsRef = std::make_pair(PqlReferenceType::ident, "x");
        //    Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::ModifiesP, lhsRef, rhsRef);

        //    // PKB inserts modifies
        //    Entity::insertProc("randomProc");
        //    ProcIndex procIndex = Entity::insertProc("p");
        //    Entity::insertVar("randomVar");
        //    VarIndex varIndex = Entity::insertVar("x");
        //    Modifies::insert(procIndex, varIndex);

        //    // 2. Main test:
        //    EvaluatedTable evTable = instruction->execute();
        //    Assert::AreEqual(size_t(1), evTable.getNumRow());
        //    std::string expected = "Table String: size: 1\nSynonym: p Values: 2 \n";
        //    Assert::AreEqual(expected, evTable.getTableString());

        //}

        //TEST_METHOD(execute_lhsSynonymRhsWildCardProc)
        //{

        //    // 1. Setup:
        //    PqlReference lhsRef, rhsRef;
        //    lhsRef = std::make_pair(PqlReferenceType::synonym, "p");
        //    rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
        //    Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::ModifiesP, lhsRef, rhsRef);

        //    // PKB inserts modifies
        //    Entity::insertProc("randomProc");
        //    ProcIndex procIndex = Entity::insertProc("p");
        //    Entity::insertVar("randomVar");
        //    VarIndex varIndex = Entity::insertVar("x");
        //    VarIndex varIndex2 = Entity::insertVar("y");
        //    Modifies::insert(procIndex, varIndex);
        //    Modifies::insert(procIndex, varIndex2);

        //    // 2. Main test:
        //    EvaluatedTable evTable = instruction->execute();
        //    Assert::AreEqual(size_t(2), evTable.getNumRow());
        //    std::string expected = "Table String: size: 1\nSynonym: p Values: 2 2 \n";
        //    Assert::AreEqual(expected, evTable.getTableString());
        //}

        TEST_METHOD(execute_lhsConstRhsSynonym_EvTableTrue)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "2");
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
        }

        TEST_METHOD(execute_lhsConstRhsSynonym_EvTableFalse)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "2");
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
        }

        TEST_METHOD(execute_lhsConstRhsWildcard_EvTableTrue)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "2");
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
        }

        TEST_METHOD(execute_lhsConstRhsWildcard_EvTableFalse)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "2");
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
        }

	};
}
