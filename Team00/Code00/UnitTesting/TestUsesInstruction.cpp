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
    // Uses (a/r/s/a1, v) or Uses(a/r/s/a1, "x") or Uses (a/r/s/a1, _ ) 
    // Uses (1, v)	=> true or Uses (1, _ ) (under statement)
    // Uses (p/p1, v) or Uses(p/p1, "x") or Uses (p/p1, _ )	proc
    TEST_CLASS(TestUsesInstruction)
    {
    private:

    public:
        TEST_METHOD(execute_lhsSynonymRhsSynonymStmt)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "a1");
            rhsRef = std::make_pair(PqlReferenceType::synonym, "v");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesS, lhsRef, rhsRef);

            // PKB inserts modifies
            Entity::insertStmt(StatementType::printType);   // insert dummy stmt
            StmtIndex stmt = Entity::insertStmt(StatementType::readType);
            Entity::insertVar("randomVar"); // insert dummy var
            VarIndex varIndex = Entity::insertVar("v");
            Uses::insert(stmt, varIndex);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();
            Assert::AreEqual(size_t(1), evTable.getNumRow());
            std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 \nSynonym: v Values: 2 \n";
            Assert::AreEqual(expected, evTable.getTableString());

            // 3. Clean-up:
            Entity::performCleanUp();
            Uses::performCleanUp();
        }

        TEST_METHOD(execute_lhsSynonymRhsIdentStmt)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "a1");
            rhsRef = std::make_pair(PqlReferenceType::ident, "x");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesS, lhsRef, rhsRef);

            // PKB inserts modifies
            Entity::insertStmt(StatementType::printType);
            StmtIndex stmt = Entity::insertStmt(StatementType::readType);
            Entity::insertVar("randomVar");
            VarIndex varIndex = Entity::insertVar("x");
            Uses::insert(stmt, varIndex);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();
            Assert::AreEqual(size_t(1), evTable.getNumRow());
            std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 \nSynonym: x Values: 2 \n";
            Assert::AreEqual(expected, evTable.getTableString());

            // 3. Clean-up:
            Entity::performCleanUp();
            Uses::performCleanUp();
        }

        TEST_METHOD(execute_lhsSynonymRhsWildCardStmt)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "a1");
            rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesS, lhsRef, rhsRef);

            // PKB inserts modifies
            Entity::insertStmt(StatementType::printType);
            StmtIndex stmt = Entity::insertStmt(StatementType::printType);
            Entity::insertVar("randomVar");
            VarIndex varIndex = Entity::insertVar("x");
            VarIndex varIndex2 = Entity::insertVar("y");
            Uses::insert(stmt, varIndex);
            Uses::insert(stmt, varIndex2);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();
            Assert::AreEqual(size_t(2), evTable.getNumRow());
            std::string expected = "Table String: size: 1\nSynonym: a1 Values: 2 2 \n";
            Assert::AreEqual(expected, evTable.getTableString());

            // 3. Clean-up:
            Entity::performCleanUp();
            Uses::performCleanUp();
        }

        // <-- Iteration 2 -->
        //TEST_METHOD(execute_lhsSynonymRhsSynonymProc)
        //{

        //    // 1. Setup:
        //    PqlReference lhsRef, rhsRef;
        //    lhsRef = std::make_pair(PqlReferenceType::synonym, "p");
        //    rhsRef = std::make_pair(PqlReferenceType::synonym, "v");
        //    Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesP, lhsRef, rhsRef);

        //    // PKB inserts modifies
        //    Entity::insertProc("randomProc");
        //    ProcIndex procIndex = Entity::insertProc("p");
        //    Entity::insertVar("randomVar");
        //    VarIndex varIndex = Entity::insertVar("x");
        //    Uses::insert(procIndex, varIndex);
        //    Assert::AreEqual(Uses::contains(procIndex, varIndex), true);
        //    // 2. Main test:
        //    EvaluatedTable evTable = instruction->execute();
        //    Assert::AreEqual(size_t(1), evTable.getNumRow());
        //    std::string expected = "Table String: size: 2\nSynonym: p Values: 2 \nSynonym: v Values: 2 \n";
        //    Assert::AreEqual(expected, evTable.getTableString());

        //    // 3. Clean-up:
        //    Entity::performCleanUp();
        //    Uses::performCleanUp();
        //}

        //TEST_METHOD(execute_lhsSynonymRhsIdentProc)
        //{

        //    // 1. Setup:
        //    PqlReference lhsRef, rhsRef;
        //    lhsRef = std::make_pair(PqlReferenceType::synonym, "p");
        //    rhsRef = std::make_pair(PqlReferenceType::ident, "x");
        //    Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesP, lhsRef, rhsRef);

        //    // PKB inserts modifies
        //    Entity::insertProc("randomProc");
        //    ProcIndex procIndex = Entity::insertProc("p");
        //    Entity::insertVar("randomVar");
        //    VarIndex varIndex = Entity::insertVar("x");
        //    Uses::insert(procIndex, varIndex);

        //    // 2. Main test:
        //    EvaluatedTable evTable = instruction->execute();
        //    Assert::AreEqual(size_t(1), evTable.getNumRow());
        //    std::string expected = "Table String: size: 2\nSynonym: p Values: 2 \nSynonym: x Values: 2 \n";
        //    Assert::AreEqual(expected, evTable.getTableString());

        //    // 3. Clean-up:
        //    Entity::performCleanUp();
        //    Uses::performCleanUp();
        //}

        //TEST_METHOD(execute_lhsSynonymRhsWildCardProc)
        //{

        //    // 1. Setup:
        //    PqlReference lhsRef, rhsRef;
        //    lhsRef = std::make_pair(PqlReferenceType::synonym, "p");
        //    rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
        //    Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesP, lhsRef, rhsRef);

        //    // PKB inserts modifies
        //    Entity::insertProc("randomProc");
        //    ProcIndex procIndex = Entity::insertProc("p");
        //    Entity::insertVar("randomVar");
        //    VarIndex varIndex = Entity::insertVar("x");
        //    VarIndex varIndex2 = Entity::insertVar("y");
        //    Uses::insert(procIndex, varIndex);
        //    Uses::insert(procIndex, varIndex2);

        //    // 2. Main test:
        //    EvaluatedTable evTable = instruction->execute();
        //    Assert::AreEqual(size_t(2), evTable.getNumRow());
        //    std::string expected = "Table String: size: 1\nSynonym: p Values: 2 2 \n";
        //    Assert::AreEqual(expected, evTable.getTableString());

        //    // 3. Clean-up:
        //    Entity::performCleanUp();
        //    Uses::performCleanUp();
        //}

        TEST_METHOD(execute_lhsConstRhsSynonym_EvTableTrue)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "1");
            rhsRef = std::make_pair(PqlReferenceType::synonym, "a1");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesS, lhsRef, rhsRef);

            // PKB inserts modifies
            Entity::insertStmt(StatementType::printType);
            StmtIndex stmt = Entity::insertStmt(StatementType::readType);
            Entity::insertVar("randomVar");
            VarIndex varIndex = Entity::insertVar("x");
            VarIndex varIndex2 = Entity::insertVar("y");
            Uses::insert(stmt, varIndex);
            Uses::insert(stmt, varIndex2);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();
            Assert::AreEqual(size_t(0), evTable.getNumRow());
            std::string expected = "Table String: size: 0\n";
            Assert::AreEqual(expected, evTable.getTableString());

            // 3. Clean-up:
            Entity::performCleanUp();
            Uses::performCleanUp();
        }

        TEST_METHOD(execute_lhsConstRhsSynonym_EvTableFalse)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "1");
            rhsRef = std::make_pair(PqlReferenceType::synonym, "a1");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesS, lhsRef, rhsRef);

            // PKB inserts modifies
            Entity::insertStmt(StatementType::printType);
            StmtIndex stmt = Entity::insertStmt(StatementType::readType);
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
            Uses::performCleanUp();
        }

        TEST_METHOD(execute_lhsConstRhsWildcard_EvTableTrue)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "1");
            rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesS, lhsRef, rhsRef);

            // PKB inserts modifies
            Entity::insertStmt(StatementType::printType);
            StmtIndex stmt = Entity::insertStmt(StatementType::readType);
            Entity::insertVar("randomVar");
            VarIndex varIndex = Entity::insertVar("x");
            VarIndex varIndex2 = Entity::insertVar("y");
            Uses::insert(stmt, varIndex);
            Uses::insert(stmt, varIndex2);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();
            Assert::AreEqual(size_t(0), evTable.getNumRow());
            std::string expected = "Table String: size: 0\n";
            Assert::AreEqual(expected, evTable.getTableString());

            // 3. Clean-up:
            Entity::performCleanUp();
            Uses::performCleanUp();
        }

        TEST_METHOD(execute_lhsConstRhsWildcard_EvTableFalse)
        {

            // 1. Setup:
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "1");
            rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::UsesS, lhsRef, rhsRef);

            // PKB inserts modifies
            Entity::insertStmt(StatementType::printType);
            StmtIndex stmt = Entity::insertStmt(StatementType::readType);
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
            Uses::performCleanUp();
        }

    };
}
