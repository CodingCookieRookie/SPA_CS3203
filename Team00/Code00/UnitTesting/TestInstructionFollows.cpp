#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include <algorithm>

#include "../source/QPS-NEW/PQLEvaluator.h"
#include "../source/PKB/RS2.h"
#include "../source/PKB/Follows.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
    TEST_CLASS(TestFollowsInstructions)
    {
    private:
        TEST_METHOD_CLEANUP(cleanUpTables) {
            Entity::performCleanUp();
            Follows::performCleanUp();
        }
    public:

        TEST_METHOD(executeFollowsInstruction_twoConstants_evaluatedTableFormed)
        {

            // 1. Setup:
            // Follows(1, 2) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "1");
            rhsRef = std::make_pair(PqlReferenceType::integer, "2");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Follows, lhsRef, rhsRef);

            // PKB inserts statements
            StmtIndex stmt1 = Entity::insertStmt(StatementType::assignType);
            StmtIndex stmt2 = Entity::insertStmt(StatementType::assignType);
            Follows::insert(stmt1, stmt2);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            Assert::AreEqual(size_t(0), evTable.getNumRow());
            Assert::AreEqual(true, evTable.getEvResult());

            // 3. Clean-up:
            Entity::performCleanUp();
            Follows::performCleanUp();
        }

        TEST_METHOD(executeFollowsInstruction_lhsConstantRhsStmt_evaluatedTableFormed)
        {

            // 1. Setup:
            // Follows(1, s2) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "1");
            rhsRef = std::make_pair(PqlReferenceType::synonym, "s2");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Follows, lhsRef, rhsRef);

            // PKB inserts statements
            StmtIndex stmt1 = Entity::insertStmt(StatementType::assignType);
            StmtIndex stmt2 = Entity::insertStmt(StatementType::assignType);
            Follows::insert(stmt1, stmt2);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(1), evTable.getNumRow());

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("s2") != tableRef.end());
            Assert::AreEqual(false, tableRef.find("s3") != tableRef.end());

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> values{ 2 };
            auto actualValues = tableRef.at("s2");
            bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
            Assert::AreEqual(true, areVecEqual);
            auto actualEntities = evTable.getEntities();
            Assert::AreEqual(true, actualEntities.find("s2") != actualEntities.end());
            Assert::AreEqual(false, actualEntities.find("s3") != actualEntities.end());
            bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("s2");
            Assert::AreEqual(true, isPqlEntityType);

            // Test EvResult:
            bool actualEvResult = evTable.getEvResult();
            Assert::AreEqual(true, actualEvResult);

            // 3. Clean-up:
            Entity::performCleanUp();
            Follows::performCleanUp();
        }

        TEST_METHOD(executeFollowsInstruction_lhsStmrRhsConst_evaluatedTableFormed)
        {

            // 1. Setup:
            // Follows(s1, 2) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
            rhsRef = std::make_pair(PqlReferenceType::integer, "2");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Follows, lhsRef, rhsRef);

            // PKB inserts statements
            StmtIndex stmt1 = Entity::insertStmt(StatementType::assignType);
            StmtIndex stmt2 = Entity::insertStmt(StatementType::assignType);
            Follows::insert(stmt1, stmt2);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(1), evTable.getNumRow());

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
            Assert::AreEqual(false, tableRef.find("s5") != tableRef.end());

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> values{ 1 };
            auto actualValues = tableRef.at("s1");
            bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
            Assert::AreEqual(true, areVecEqual);
            auto actualEntities = evTable.getEntities();
            Assert::AreEqual(true, actualEntities.find("s1") != actualEntities.end());
            Assert::AreEqual(false, actualEntities.find("s5") != actualEntities.end());
            bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("s1");
            Assert::AreEqual(true, isPqlEntityType);

            // Test EvResult:
            bool actualEvResult = evTable.getEvResult();
            Assert::AreEqual(true, actualEvResult);

            // 3. Clean-up:
            Entity::performCleanUp();
            Follows::performCleanUp();
        }

        TEST_METHOD(executeFollowsInstruction_twoStmts_evaluatedTableFormed)
        {

            // 1. Setup:
            // Follows(s1, s2) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
            rhsRef = std::make_pair(PqlReferenceType::synonym, "s2");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Follows, lhsRef, rhsRef);

            // PKB inserts 4 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 4; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            for (int i = 0; i < 3; i++) {
                Follows::insert(stmts[i], stmts[i + 1]);
            }

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(3), evTable.getNumRow());

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
            Assert::AreEqual(false, tableRef.find("s12") != tableRef.end());

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> s1values{ 1, 2, 3 };
            auto actuals1Values = tableRef.at("s1");
            bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
            Assert::AreEqual(true, areVecEqual);
            std::vector<int> s2values{ 2, 3, 4 };
            auto actuals2Values = tableRef.at("s2");
            bool areVecEqual2 = std::equal(s2values.begin(), s2values.end(), actuals2Values.begin());
            Assert::AreEqual(true, areVecEqual2);

            auto actualEntities = evTable.getEntities();
            Assert::AreEqual(true, actualEntities.find("s1") != actualEntities.end());
            Assert::AreEqual(true, actualEntities.find("s2") != actualEntities.end());
            Assert::AreEqual(false, actualEntities.find("s5") != actualEntities.end());
            bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("s1");
            bool isPqlEntityType2 = PqlEntityType::Stmt == actualEntities.at("s2");
            Assert::AreEqual(true, isPqlEntityType);
            Assert::AreEqual(true, isPqlEntityType2);

            // Test EvResult:
            bool actualEvResult = evTable.getEvResult();
            Assert::AreEqual(true, actualEvResult);

            // 3. Clean-up:
            Entity::performCleanUp();
            Follows::performCleanUp();
        }

        TEST_METHOD(executeFollowsInstruction_lhsStmtRhsWildcardStress_evaluatedTableFormed)
        {

            // 1. Setup:
            // Follows(s1, _) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
            rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Follows, lhsRef, rhsRef);

            // PKB inserts 19 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 19; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            for (int i = 0; i < 18; i++) {
                Follows::insert(stmts[i], stmts[i + 1]);
            }

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(18), evTable.getNumRow());

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
            Assert::AreEqual(false, tableRef.find("s207") != tableRef.end());

            // Test Table size:
            Assert::AreEqual(size_t(1), tableRef.size()); // RHS wildcard will not have column (not of concern)

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> s1values, wildcardValues;
            for (int i = 0; i < 18; i++) {
                s1values.emplace_back(i + 1);
            }
            auto actuals1Values = tableRef.at("s1");
            bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
            Assert::AreEqual(true, areVecEqual); // s1values == {1, 2, ... 18}

            auto actualEntities = evTable.getEntities();
            Assert::AreEqual(true, actualEntities.find("s1") != actualEntities.end());
            Assert::AreEqual(false, actualEntities.find("_") != actualEntities.end());
            Assert::AreEqual(false, actualEntities.find("s207") != actualEntities.end());
            bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("s1");
            Assert::AreEqual(true, isPqlEntityType);

            // Test EvResult:
            bool actualEvResult = evTable.getEvResult();
            Assert::AreEqual(true, actualEvResult);

            // 3. Clean-up:
            Entity::performCleanUp();
            Follows::performCleanUp();
        }

        TEST_METHOD(executeFollowsInstruction_twoWildcards_evaluatedTableFormed)
        {

            // 1. Setup:
            // Parent(_, _) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Follows, lhsRef, rhsRef);

            // PKB inserts 3 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 3; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            for (int i = 0; i < 2; i++) {
                Follows::insert(stmts[i], stmts[i + 1]);
            }

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(0), evTable.getNumRow()); // 

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(false, tableRef.find("_") != tableRef.end());
            Assert::AreEqual(false, tableRef.find("s1") != tableRef.end());

            // Test Table size:
            Assert::AreEqual(size_t(0), tableRef.size()); // Two wildcards will have no columns => only have boolean

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            auto actualEntities = evTable.getEntities();
            Assert::AreEqual(false, actualEntities.find("_") != actualEntities.end());
            Assert::AreEqual(false, actualEntities.find("s2") != actualEntities.end());

            // Test EvResult:
            bool actualEvResult = evTable.getEvResult();
            Assert::AreEqual(true, actualEvResult); // because Follows rs exist

            // 3. Clean-up:
            Entity::performCleanUp();
            Follows::performCleanUp();
        }
    };
}