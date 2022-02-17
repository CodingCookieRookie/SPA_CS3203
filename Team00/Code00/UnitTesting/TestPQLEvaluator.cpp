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

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
    TEST_CLASS(TestPQLEvaluator)
    {
    public:

        TEST_METHOD(evaluateQuery_declarationAndSelectOnly_designEntitiesExtracted)
        {
            // "stmt s; if ifs; Select s";
            typedef std::pair<PqlEntityType, std::string> DECLARATION;

            DECLARATION pair1 = DECLARATION(PqlEntityType::Stmt, "s");
            DECLARATION pair2 = DECLARATION(PqlEntityType::If, "ifs");

            std::vector<PQL_VARIABLE> declarations;
            declarations.push_back(pair1);
            declarations.push_back(pair2);

            std::vector<std::string> columns;
            columns.push_back("s");

            std::vector<ParsedRelationship> relationships;
            std::vector<ParsedPattern> patterns;
            ParsedQuery pq1 = ParsedQuery(declarations, columns, relationships, patterns);

            PQLEvaluator pqlEvaluator(pq1);
            
            Assert::AreEqual(size_t(2), pq1.getDeclarations().size());
            Assert::IsFalse(pq1.getColumns().empty());
            Assert::AreEqual(std::string("s"), pq1.getColumns()[0]);
        }

        // Follows Relationship Tests ====================================================================================================================


        TEST_METHOD(executeInstruction_follows_twoConstants)
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

        TEST_METHOD(executeInstruction_follows_lhsConstrhsStmt)
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

        TEST_METHOD(executeInstruction_follows_lhsStmtrhsConst)
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

        TEST_METHOD(executeInstruction_follows_twoStmts)
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

        TEST_METHOD(executeInstruction_follows_lhsStmtrhsWildcard_stress)
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
            Assert::AreEqual(size_t(2), tableRef.size()); // RHS wildcard will still have column (innerJoinMerge() will drop it during merge)

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> s1values, wildcardValues;
            for (int i = 0; i < 18; i++) {
                s1values.emplace_back(i + 1);
                wildcardValues.emplace_back(i + 2);
            }
            auto actuals1Values = tableRef.at("s1");
            bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
            Assert::AreEqual(true, areVecEqual); // s1values == {1, 2, ... 18}
            auto actuals2Values = tableRef.at("_");
            bool areVecEqual2 = std::equal(wildcardValues.begin(), wildcardValues.end(), actuals2Values.begin());
            Assert::AreEqual(true, areVecEqual2); // wildcardValues == {2, 3, ... 19}


            auto actualEntities = evTable.getEntities();
            Assert::AreEqual(true, actualEntities.find("s1") != actualEntities.end());
            Assert::AreEqual(true, actualEntities.find("_") != actualEntities.end());
            Assert::AreEqual(false, actualEntities.find("s207") != actualEntities.end());
            bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("_");
            Assert::AreEqual(true, isPqlEntityType);

            // Test EvResult:
            bool actualEvResult = evTable.getEvResult();
            Assert::AreEqual(true, actualEvResult);

            // 3. Clean-up:
            Entity::performCleanUp();
            Follows::performCleanUp();
        }

        TEST_METHOD(executeInstruction_follows_twoWildcards)
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

        // Follows* Relationship Tests ====================================================================================================================

        TEST_METHOD(executeInstruction_followsStar_twoConstants)
        {

            // 1. Setup:
            // Follows*(1, 2) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "1");
            rhsRef = std::make_pair(PqlReferenceType::integer, "4");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::FollowsT, lhsRef, rhsRef);

            // PKB inserts 4 statements, 3 Follows
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 4; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            std::unordered_map<StmtIndex,
                std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
            uPredSucTable[stmts[0]] = { stmts[1] };
            uPredSucTable[stmts[1]] = { stmts[2], stmts[3]};
            FollowsT::populate(uPredSucTable);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            Assert::AreEqual(size_t(0), evTable.getNumRow());
            Assert::AreEqual(true, evTable.getEvResult()); // Follows*(1, 4) should hold

            // 3. Clean-up:
            Entity::performCleanUp();
            FollowsT::performCleanUp();
            Follows::performCleanUp();

        }

        TEST_METHOD(executeInstruction_followsStar_lhsConstrhsStmt)
        {

            // 1. Setup:
            // Follows*(2, s2) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "2");
            rhsRef = std::make_pair(PqlReferenceType::synonym, "s2");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::FollowsT, lhsRef, rhsRef);

            // PKB inserts 4 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 4; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            std::unordered_map<StmtIndex,
                std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
            uPredSucTable[stmts[0]] = { stmts[1] }; // 1 follows 2 (ignored)
            uPredSucTable[stmts[1]] = { stmts[2], stmts[3] }; // 2 follows 3 and 4
            FollowsT::populate(uPredSucTable);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(2), evTable.getNumRow()); 

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("s2") != tableRef.end());
            Assert::AreEqual(false, tableRef.find("s3") != tableRef.end());

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> values{ 3, 4 };
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
            FollowsT::performCleanUp();
            Follows::performCleanUp();
        }

        TEST_METHOD(executeInstruction_followsStar_lhsStmtrhsConst)
        {

            // 1. Setup:
            // Follows*(s1, 4) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
            rhsRef = std::make_pair(PqlReferenceType::integer, "4");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::FollowsT, lhsRef, rhsRef);

            // PKB inserts 4 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 4; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            std::unordered_map<StmtIndex,
                std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
            uPredSucTable[stmts[0]] = { stmts[1] }; // 1 follows 2
            uPredSucTable[stmts[1]] = { stmts[2], stmts[3] }; // 2 follows 3 and 4
            uPredSucTable[stmts[2]] = { stmts[3] }; //  3 follows 4
            FollowsT::populate(uPredSucTable);


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
            FollowsT::performCleanUp();
            Follows::performCleanUp();
        }

        TEST_METHOD(executeInstruction_followsStar_twoStmts)
        {

            // 1. Setup:
            // Follows'*(s1, s2) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
            rhsRef = std::make_pair(PqlReferenceType::synonym, "s2");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::FollowsT, lhsRef, rhsRef);

            // PKB inserts 4 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 4; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            std::unordered_map<StmtIndex,
                std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
            uPredSucTable[stmts[0]] = { stmts[1], stmts[2] }; // 1 follows 2 and 3
            uPredSucTable[stmts[1]] = { stmts[2], stmts[3] }; // 2 follows 3 and 4
            uPredSucTable[stmts[2]] = { stmts[3] }; //  3 follows 4
            FollowsT::populate(uPredSucTable);

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
            Follows::performCleanUp();
            FollowsT::performCleanUp();
        }

        TEST_METHOD(executeInstruction_followsStar_lhsStmtrhsWildcard_stress)
        {

            // 1. Setup:
            // Follows*(s1, _) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
            rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::FollowsT, lhsRef, rhsRef);

            // PKB inserts 19 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 19; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            std::unordered_map<StmtIndex,
                std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
            for (int i = 0; i < 18; i++) {
                uPredSucTable[stmts[i]] = { stmts[i + 1] }; // i follows i + 1
            }
            FollowsT::populate(uPredSucTable);

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
            Assert::AreEqual(size_t(2), tableRef.size()); // RHS wildcard will still have column (innerJoinMerge() will drop it during merge)

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> s1values, wildcardValues;
            for (int i = 0; i < 18; i++) {
                for (int j = 0; j < (18-i); j++) {
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
            auto actualWildcardValues = tableRef.at("_");
            bool areVecEqual2 = std::equal(wildcardValues.begin(), wildcardValues.end(), actualWildcardValues.begin());
            Assert::AreEqual(true, areVecEqual2); // wildcardValues == {2-19, 3-19, ..., 18, 19, 19}


            auto actualEntities = evTable.getEntities();
            Assert::AreEqual(true, actualEntities.find("s1") != actualEntities.end());
            Assert::AreEqual(true, actualEntities.find("_") != actualEntities.end());
            Assert::AreEqual(false, actualEntities.find("s207") != actualEntities.end());
            bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("_");
            Assert::AreEqual(true, isPqlEntityType);

            // Test EvResult:
            bool actualEvResult = evTable.getEvResult();
            Assert::AreEqual(true, actualEvResult);

            // 3. Clean-up:
            Entity::performCleanUp();
            FollowsT::performCleanUp();
            Follows::performCleanUp();
        }

        // Parent Relationship Tests ======================================================================================================================

        TEST_METHOD(executeInstruction_parent_twoConstants) {

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

        TEST_METHOD(executeInstruction_parent_lhsConstrhsStmt)
        {

            // 1. Setup:
            // Parent(1, s2) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "1");
            rhsRef = std::make_pair(PqlReferenceType::synonym, "s2");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Parent, lhsRef, rhsRef);

            // PKB inserts statements
            StmtIndex stmt1 = Entity::insertStmt(StatementType::assignType);
            StmtIndex stmt2 = Entity::insertStmt(StatementType::assignType);
            Parent::insert(stmt1, stmt2);

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
            Parent::performCleanUp();
        }

        TEST_METHOD(executeInstruction_parent_lhsStmtrhsConst)
        {
            // 1. Setup:
            // Parent(s1, 2) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
            rhsRef = std::make_pair(PqlReferenceType::integer, "2");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Parent, lhsRef, rhsRef);

            // PKB inserts statements
            StmtIndex stmt1 = Entity::insertStmt(StatementType::assignType);
            StmtIndex stmt2 = Entity::insertStmt(StatementType::assignType);
            Parent::insert(stmt1, stmt2);

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(1), evTable.getNumRow());

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
            Assert::AreEqual(false, tableRef.find("s7") != tableRef.end());

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> values{ 1 };
            auto actualValues = tableRef.at("s1");
            bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
            Assert::AreEqual(true, areVecEqual);
            auto actualEntities = evTable.getEntities();
            Assert::AreEqual(true, actualEntities.find("s1") != actualEntities.end());
            Assert::AreEqual(false, actualEntities.find("s7") != actualEntities.end());
            bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("s1");
            Assert::AreEqual(true, isPqlEntityType);

            // Test EvResult:
            bool actualEvResult = evTable.getEvResult();
            Assert::AreEqual(true, actualEvResult);

            // 3. Clean-up:
            Entity::performCleanUp();
            Parent::performCleanUp();
        }

        TEST_METHOD(executeInstruction_parent_twoStmts)
        {

            // 1. Setup:
            // Parent(s1, s2) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
            rhsRef = std::make_pair(PqlReferenceType::synonym, "s2");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Parent, lhsRef, rhsRef);

            // PKB inserts 5 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 5; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            for (int i = 0; i < 4; i++) {
                Parent::insert(stmts[i], stmts[i + 1]);
            }

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(4), evTable.getNumRow());

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
            Assert::AreEqual(false, tableRef.find("s6") != tableRef.end());

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> s1values{ 1, 2, 3, 4 };
            auto actuals1Values = tableRef.at("s1");
            bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
            Assert::AreEqual(true, areVecEqual);
            std::vector<int> s2values{ 2, 3, 4, 5 };
            auto actuals2Values = tableRef.at("s2");
            bool areVecEqual2 = std::equal(s2values.begin(), s2values.end(), actuals2Values.begin());
            Assert::AreEqual(true, areVecEqual2);

            auto actualEntities = evTable.getEntities();
            Assert::AreEqual(true, actualEntities.find("s1") != actualEntities.end());
            Assert::AreEqual(true, actualEntities.find("s2") != actualEntities.end());
            Assert::AreEqual(false, actualEntities.find("s6") != actualEntities.end());
            bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("s1");
            bool isPqlEntityType2 = PqlEntityType::Stmt == actualEntities.at("s2");
            Assert::AreEqual(true, isPqlEntityType);
            Assert::AreEqual(true, isPqlEntityType2);

            // Test EvResult:
            bool actualEvResult = evTable.getEvResult();
            Assert::AreEqual(true, actualEvResult);

            // 3. Clean-up:
            Entity::performCleanUp();
            Parent::performCleanUp();
        }

        TEST_METHOD(executeInstruction_parent_lhsStmtRhsWildcard_stress)
        {

            // 1. Setup:
            // Parent(s1, _) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::synonym, "s1");
            rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Parent, lhsRef, rhsRef);

            // PKB inserts 37213 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 37213; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            for (int i = 0; i < 37213-1; i++) {
                Parent::insert(stmts[i], stmts[i + 1]);
            }

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(37213-1), evTable.getNumRow());

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
            Assert::AreEqual(false, tableRef.find("stress") != tableRef.end());

            // Test Table size:
            Assert::AreEqual(size_t(1), tableRef.size()); // RHS wildcard will still have column (innerJoinMerge() will drop it during merge)

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> s1values;
            for (int i = 0; i < 37213 - 1; i++) {
                s1values.emplace_back(i + 1);
            }
            auto actuals1Values = tableRef.at("s1");
            // Cannot check equality due to misordering (actuals1Values is not {1, 2, ... 37212}), this is PKB's optimisation.
            // Instead sort, and check for equality
            std::sort(actuals1Values.begin(), actuals1Values.end());
            bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
            Assert::AreEqual(true, areVecEqual); // s1values == {1, 2, ... 37212}

            auto actualEntities = evTable.getEntities();
            Assert::AreEqual(true, actualEntities.find("s1") != actualEntities.end());
            Assert::AreEqual(false, actualEntities.find("_") != actualEntities.end());
            Assert::AreEqual(false, actualEntities.find("stress") != actualEntities.end());
            bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("s1");
            Assert::AreEqual(true, isPqlEntityType);

            // Test EvResult:
            bool actualEvResult = evTable.getEvResult();
            Assert::AreEqual(true, actualEvResult);

            // 3. Clean-up:
            Entity::performCleanUp();
            Parent::performCleanUp();
        }

        TEST_METHOD(executeInstruction_parent_lhsWildcardRhsConst)
        {

            // 1. Setup:
            // Parent(_, 3) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            rhsRef = std::make_pair(PqlReferenceType::integer, "3");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Parent, lhsRef, rhsRef);

            // PKB inserts 3 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 3; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            for (int i = 0; i < 2; i++) {
                Parent::insert(stmts[i], stmts[i + 1]);
            }

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(1), evTable.getNumRow()); // Only 1 parent of 3

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("_") != tableRef.end());
            Assert::AreEqual(false, tableRef.find("23") != tableRef.end());

            // Test Table size:
            Assert::AreEqual(size_t(1), tableRef.size()); // RHS wildcard will still have column (innerJoinMerge() will drop it during merge)

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> wildcardValues;
            wildcardValues.emplace_back(2);
            auto actualWildcardValues = tableRef.at("_");
            std::sort(actualWildcardValues.begin(), actualWildcardValues.end());
            bool areVecEqual = std::equal(wildcardValues.begin(), wildcardValues.end(), actualWildcardValues.begin());
            Assert::AreEqual(true, areVecEqual); // wildcardValues == {2}

            auto actualEntities = evTable.getEntities();
            Assert::AreEqual(true, actualEntities.find("_") != actualEntities.end());
            Assert::AreEqual(false, actualEntities.find("2") != actualEntities.end());
            bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("_");
            Assert::AreEqual(true, isPqlEntityType);

            // Test EvResult:
            bool actualEvResult = evTable.getEvResult();
            Assert::AreEqual(true, actualEvResult);

            // 3. Clean-up:
            Entity::performCleanUp();
            Parent::performCleanUp();
        }

        //TEST_METHOD(executeInstruction_parent_lhsWildcardhsConst_stress_nestingParents)
        //{
        //    // TODO: PKB's Parent seems to be populating parent differently under high load.
        //    // Calling PKB to insert 927 Parent associations: 1 Parent 2, 2 Parent 3, ... , 927 Parent 928.
        //    // Expecting 27 to be a parent of 28
        //    // Gets 18 to be a parent of 28.

        //    // 1. Setup:
        //    // Parent(_, 2947) RelationshipInstruction
        //    PqlReference lhsRef, rhsRef;
        //    lhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
        //    rhsRef = std::make_pair(PqlReferenceType::integer, "28");
        //    Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Parent, lhsRef, rhsRef);

        //    // PKB inserts 976 statements
        //    std::vector<StmtIndex> stmts;
        //    for (int i = 0; i < 976; i++) {
        //        stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
        //    }
        //    for (int i = 0; i < 976 - 1; i++) {
        //        Parent::insert(stmts[i], stmts[i + 1]);
        //    }

        //    // 2. Main test:
        //    EvaluatedTable evTable = instruction->execute();

        //    // Test numRow:
        //    Assert::AreEqual(size_t(1), evTable.getNumRow());

        //    // Test Table: std::unordered_map<std::string, std::vector<int>>
        //    auto tableRef = evTable.getTableRef();
        //    Assert::AreEqual(true, tableRef.find("_") != tableRef.end());
        //    Assert::AreEqual(false, tableRef.find("stress") != tableRef.end());

        //    // Test Table size:
        //    Assert::AreEqual(size_t(1), tableRef.size()); // RHS wildcard will still have column (innerJoinMerge() will drop it during merge)

        //    // Test Entities: std::unordered_map<std::string, PqlEntityType>
        //    std::vector<int> s1values, wildcardValues;
        //    wildcardValues.emplace_back(27);
        //    auto actualWildcardValues = tableRef.at("_");
        //    std::sort(actualWildcardValues.begin(), actualWildcardValues.end());
        //    bool areVecEqual = std::equal(wildcardValues.begin(), wildcardValues.end(), actualWildcardValues.begin());
        //    Assert::AreEqual(true, areVecEqual); // actualWildcardValues == {27}

        //    auto actualEntities = evTable.getEntities();
        //    Assert::AreEqual(true, actualEntities.find("_") != actualEntities.end());
        //    Assert::AreEqual(false, actualEntities.find("28") != actualEntities.end());
        //    Assert::AreEqual(false, actualEntities.find("stress") != actualEntities.end());
        //    bool isPqlEntityType = PqlEntityType::Stmt == actualEntities.at("_");
        //    Assert::AreEqual(true, isPqlEntityType);

        //    // Test EvResult:
        //    bool actualEvResult = evTable.getEvResult();
        //    Assert::AreEqual(true, actualEvResult);

        //    // 3. Clean-up:
        //    Entity::performCleanUp();
        //    Parent::performCleanUp();
        //}

        TEST_METHOD(executeInstruction_parent_twoWildcards)
        {

            // 1. Setup:
            // Parent(_, _) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            rhsRef = std::make_pair(PqlReferenceType::wildcard, "_");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Parent, lhsRef, rhsRef);

            // PKB inserts 3 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 3; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }
            for (int i = 0; i < 2; i++) {
                Parent::insert(stmts[i], stmts[i + 1]);
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
            Assert::AreEqual(true, actualEvResult); // because Parent rs exist

            // 3. Clean-up:
            Entity::performCleanUp();
            Parent::performCleanUp();
        }
    };
}