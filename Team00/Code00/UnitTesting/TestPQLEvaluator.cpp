#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS-NEW/PQLEvaluator.h"
#include "../source/QPS-NEW/PQLParser.h"
#include "../source/PKB/RS2.h"
#include "../source/PKB/Follows.h"
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

        TEST_METHOD(executeInstruction_follows_twoConstants)
        {
 
            // 1. Setup:
            // Follows(1, 2) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "1");
            rhsRef = std::make_pair(PqlReferenceType::integer, "2");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Follows, lhsRef, rhsRef);

            // PKB insert statements
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

            // PKB insert statements
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

            // PKB insert statements
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

            // PKB insert 4 statements
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



        // Parent Relationship Tests

        TEST_METHOD(executeInstruction_parent_twoConstants) {

            // 1. Setup:
            // Parent (1, 2) RelationshipInstruction
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "1");
            rhsRef = std::make_pair(PqlReferenceType::integer, "2");
            Instruction* instruction = new RelationshipInstruction(PqlRelationshipType::Parent, lhsRef, rhsRef);

            // PKB insert statements
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

            // PKB insert statements
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

            // PKB insert statements
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

    };
}