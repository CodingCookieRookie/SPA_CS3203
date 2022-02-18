#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include <algorithm>

#include "../source/QPS-NEW/PQLEvaluator.h"
#include "../source/PKB/RS2.h"
#include "../source/PKB/Entity.h"
#include "../source/PKB/Parent.h"
#include "../source/PKB/ParentT.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
    TEST_CLASS(TestParentTInstructions)
    {
    private:
        TEST_METHOD_CLEANUP(cleanUpTables) {
            Entity::performCleanUp();
            ParentT::performCleanUp();
            Parent::performCleanUp();
        }
    public:

        TEST_METHOD(executeParentStarInstruction_twoConstants_evaluatedTableFormed) {

            // 1. Setup:
            // Parent (1, 2) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "1");
            rhsRef = std::make_pair(PqlReferenceType::integer, "2");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Parent, lhsRef, rhsRef);

            // PKB inserts statements
            StmtIndex stmt1 = Entity::insertStmt(StatementType::assignType);
            StmtIndex stmt2 = Entity::insertStmt(StatementType::assignType);
            Parent::insert(stmt1, stmt2);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            Assert::AreEqual(size_t(0), evTable.getNumRow());
            Assert::AreEqual(true, evTable.getEvResult());

            // 3. Clean-up:
            Entity::performCleanUp();
            Parent::performCleanUp();
        }

        TEST_METHOD(executeParentStarInstruction_lhsConstRhsStmt_evaluatedTableFormed)
        {

            // 1. Setup:
            // Parent*(2, s2) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "1");
            rhsRef = std::make_pair(PqlReferenceType::synonym, "s2");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::ParentT, lhsRef, rhsRef);

            // PKB inserts 4 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 4; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            std::unordered_map<StmtIndex,
                std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
            uPredSucTable[stmts[0]] = { stmts[1] }; // 1 parents 2 (ignored)
            uPredSucTable[stmts[1]] = { stmts[2], stmts[3] }; // 2 parents 3 and 4
            ParentT::populate(uPredSucTable);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(3), evTable.getNumRow());

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("s2") != tableRef.end());
            Assert::AreEqual(false, tableRef.find("s3") != tableRef.end());

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> values{ 2, 3, 4 };
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
            ParentT::performCleanUp();
            Parent::performCleanUp();
        }

        TEST_METHOD(executeParentStarInstruction_lhsStmtRhsConst_evaluatedTableFormed)
        {

            // 1. Setup:
            // Parent*(s1, 4) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
            rhsRef = std::make_pair(PqlReferenceType::integer, "4");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::ParentT, lhsRef, rhsRef);

            // PKB inserts 4 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 4; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            std::unordered_map<StmtIndex,
                std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
            uPredSucTable[stmts[0]] = { stmts[1] }; // 1 parents 2
            uPredSucTable[stmts[1]] = { stmts[2], stmts[3] }; // 2 parents 3 and 4
            uPredSucTable[stmts[2]] = { stmts[3] }; //  3 parents 4
            ParentT::populate(uPredSucTable);


            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(3), evTable.getNumRow());

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
            Assert::AreEqual(false, tableRef.find("s5") != tableRef.end());

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> values{ 1, 2, 3 };
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
            ParentT::performCleanUp();
            Parent::performCleanUp();
        }

        TEST_METHOD(executeParentStarInstruction_twoStmts_evaluatedTableFormed)
        {

            // 1. Setup:
            // Follows'*(s1, s2) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
            rhsRef = std::make_pair(PqlReferenceType::synonym, "s2");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::ParentT, lhsRef, rhsRef);

            // PKB inserts 4 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 4; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            std::unordered_map<StmtIndex,
                std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
            uPredSucTable[stmts[0]] = { stmts[1], stmts[2] }; // 1 parents 2 and 3
            uPredSucTable[stmts[1]] = { stmts[2], stmts[3] }; // 2 parents 3 and 4
            uPredSucTable[stmts[2]] = { stmts[3] }; //  3 parents 4
            ParentT::populate(uPredSucTable);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(6), evTable.getNumRow()); // {1 2, 1 3, 1 4, 2 3, 2 4, 3 4}

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
            Assert::AreEqual(false, tableRef.find("s12") != tableRef.end());

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> s1values{ 1, 1, 1, 2, 2, 3 };
            auto actuals1Values = tableRef.at("s1");
            bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
            Assert::AreEqual(true, areVecEqual);
            std::vector<int> s2values{ 2, 3, 4, 3, 4, 4 };
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
            ParentT::performCleanUp();
            Parent::performCleanUp();
        }

        TEST_METHOD(executeParentStarInstruction_twoStmtsStress_evaluatedTableFormed)
        {

            // 1. Setup:
            // Parent*(s1, s2) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
            rhsRef = std::make_pair(PqlReferenceType::synonym, "s2");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::ParentT, lhsRef, rhsRef);

            // PKB inserts 19 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 19; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            std::unordered_map<StmtIndex,
                std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
            for (int i = 0; i < 18; i++) {
                uPredSucTable[stmts[i]] = { stmts[i + 1] }; // i parents i + 1, simulate 19 nesting levels
            }
            ParentT::populate(uPredSucTable);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(171), evTable.getNumRow());
            // {1 2, 1 3, ..., 1 19, 2 3, 2 4, ..., 2 19, ... , 18 19}. Hence, (19*18)/2 = 171

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
            Assert::AreEqual(false, tableRef.find("s207") != tableRef.end());

            // Test Table size:
            Assert::AreEqual(size_t(2), tableRef.size()); // 2 columns: s1 and s2

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> s1values, wildcardValues;
            for (int i = 0; i < 18; i++) {
                for (int j = 0; j < (18 - i); j++) {
                    s1values.emplace_back(i + 1);
                }
            }
            for (int i = 0; i < 18; i++) {
                for (int j = i; j < 18; j++) {
                    wildcardValues.emplace_back(j + 2);
                }
            }

            auto actuals1Values = tableRef.at("s1");
            bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
            Assert::AreEqual(true, areVecEqual); // s1values == {19 1s, 18 2s, ... 1 19}
            auto actuals2Values = tableRef.at("s2");
            bool areVecEqual2 = std::equal(wildcardValues.begin(), wildcardValues.end(), actuals2Values.begin());
            Assert::AreEqual(true, areVecEqual2); // wildcardValues == {2-19, 3-19, ..., 18, 19, 19}


            auto actualEntities = evTable.getEntities();
            Assert::AreEqual(true, actualEntities.find("s1") != actualEntities.end());
            Assert::AreEqual(false, actualEntities.find("_") != actualEntities.end());
            Assert::AreEqual(false, actualEntities.find("s207") != actualEntities.end());
            bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("s2");
            Assert::AreEqual(true, isPqlEntityType);

            // Test EvResult:
            bool actualEvResult = evTable.getEvResult();
            Assert::AreEqual(true, actualEvResult);

            // 3. Clean-up:
            Entity::performCleanUp();
            ParentT::performCleanUp();
            Parent::performCleanUp();
        }

        TEST_METHOD(executeParentStarInstruction_lhsStmtRhsWildcardStress_evaluatedTableFormed)
        {

            // 1. Setup:
            // Parent*(s1, _) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
            rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::ParentT, lhsRef, rhsRef);

            // PKB inserts 99 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 99; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            std::unordered_map<StmtIndex,
                std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
            for (int i = 0; i < 99-1; i++) {
                uPredSucTable[stmts[i]] = { stmts[i + 1] }; // i parents i + 1
            }
            ParentT::populate(uPredSucTable);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t((99 * 98)/2), evTable.getNumRow());
            // {1 2, 1 3, ..., 1 99, 2 3, 2 4, ..., 2 99, ... , 98 99}. (99*98)/2

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
            Assert::AreEqual(false, tableRef.find("s207") != tableRef.end());

            // Test Table size:
            Assert::AreEqual(size_t(1), tableRef.size()); // RHS wildcard will have no column (not of concern)

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> s1values, wildcardValues;
            for (int i = 0; i < 99 -1; i++) {
                for (int j = 0; j < (99 - 1 - i); j++) {
                    s1values.emplace_back(i + 1);
                }
            }

            auto actuals1Values = tableRef.at("s1");
            bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
            Assert::AreEqual(true, areVecEqual); // s1values == {2371 1s, 2370 2s, ... 1 2371}

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
            ParentT::performCleanUp();
            Parent::performCleanUp();
        }

        TEST_METHOD(executeParentStarInstruction_lhsWildcardRhsConstStress_evaluatedTableFormed)
        {
            // 1. Setup:
            // Parent*(_, 87) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            rhsRef = std::make_pair(PqlReferenceType::integer, "87");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::ParentT, lhsRef, rhsRef);

            // PKB inserts 99 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 99; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            std::unordered_map<StmtIndex,
                std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
            for (int i = 0; i < 99 - 1; i++) {
                uPredSucTable[stmts[i]] = { stmts[i + 1] }; // i parents i + 1
            }
            ParentT::populate(uPredSucTable);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(86), evTable.getNumRow());

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("_") != tableRef.end());
            Assert::AreEqual(false, tableRef.find("stress") != tableRef.end());

            // Test Table size:
            Assert::AreEqual(size_t(1), tableRef.size()); // LHS wildcard will still have column (innerJoinMerge() will drop it during merge)

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> s1values, wildcardValues;
            for (int i = 0; i < 86; i++) {
                wildcardValues.emplace_back(i + 1);
            }
            auto actualWildcardValues = tableRef.at("_");
            std::sort(actualWildcardValues.begin(), actualWildcardValues.end());
            bool areVecEqual = std::equal(wildcardValues.begin(), wildcardValues.end(), actualWildcardValues.begin());
            Assert::AreEqual(true, areVecEqual); // actualWildcardValues == {1, 2, ..., 86}

            auto actualEntities = evTable.getEntities();
            Assert::AreEqual(true, actualEntities.find("_") != actualEntities.end());
            Assert::AreEqual(false, actualEntities.find("87") != actualEntities.end());
            Assert::AreEqual(false, actualEntities.find("stress") != actualEntities.end());
            bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("_");
            Assert::AreEqual(true, isPqlEntityType);

            // Test EvResult:
            bool actualEvResult = evTable.getEvResult();
            Assert::AreEqual(true, actualEvResult);

            // 3. Clean-up:
            Entity::performCleanUp();
            ParentT::performCleanUp();
            Parent::performCleanUp();
        }

        TEST_METHOD(executeParentStarInstruction_twoWildcards_evaluatedTableFormed)
        {

            // 1. Setup:
            // Parent*(_, _) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::ParentT, lhsRef, rhsRef);

            // PKB inserts 3 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 4; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            std::unordered_map<StmtIndex,
                std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
            uPredSucTable[stmts[0]] = { stmts[1], stmts[2] }; // 1 parents 2 and 3
            uPredSucTable[stmts[1]] = { stmts[2], stmts[3] }; // 2 parents 3 and 4
            uPredSucTable[stmts[2]] = { stmts[3] }; //  3 parents 4
            ParentT::populate(uPredSucTable);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(0), evTable.getNumRow()); // Evaluating 2 wildcards will return a boolean with no table.

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
            Assert::AreEqual(true, actualEvResult); // because Follows* rs exist

            // 3. Clean-up:
            Entity::performCleanUp();
            ParentT::performCleanUp();
            Parent::performCleanUp();
        }
    };
}