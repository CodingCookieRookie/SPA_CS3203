#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include <algorithm>

#include "../source/QPS-NEW/PQLEvaluator.h"
#include "../source/PKB/RS2.h"
#include "../source/PKB/Entity.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
    TEST_CLASS(TestGetAllInstructions)
    {
    private:
        TEST_METHOD_CLEANUP(cleanUpTables) {
            Entity::performCleanUp();
        }
    public:

        TEST_METHOD(executeGetAllInstruction_getAllStmt_evaluatedTableFormed) {

            // 1. Setup:
            // The 'Select s1' portion of the query
            Instruction* instruction = new GetAllInstruction(PqlEntityType::Stmt, "s1");

            // PKB inserts 5 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 5; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            }

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(5), evTable.getNumRow());

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> values{ 1, 2, 3, 4, 5 };
            auto actualValues = tableRef.at("s1");
            bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
            Assert::AreEqual(true, areVecEqual);

            // Test EvResult:
            bool actualEvResult = evTable.getEvResult();
            Assert::AreEqual(true, actualEvResult);

            // 3. Clean-up:
            Entity::performCleanUp();
            Parent::performCleanUp();
        }

        TEST_METHOD(executeGetAllInstruction_getAllPrintsStress_evaluatedTableFormed) {

            // 1. Setup:
            // The 'Select pn1' portion of the query
            Instruction* instruction = new GetAllInstruction(PqlEntityType::Print, "pn1");

            // PKB inserts 98 statements
            std::vector<StmtIndex> stmts;
            for (int i = 0; i < 49; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::printType));
            }
            for (int i = 0; i < 50; i++) {
                stmts.emplace_back(Entity::insertStmt(StatementType::assignType)); // extra
            }

            // 2. Main test:
            EvaluatedTable evTable = instruction->execute();

            // Test numRow:
            Assert::AreEqual(size_t(49), evTable.getNumRow());

            // Test Table: std::unordered_map<std::string, std::vector<int>>
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("pn1") != tableRef.end());

            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> pn1values;
            for (int i = 0; i < 49; i++) {
                pn1values.emplace_back(i + 1);
            }
            auto actualValues = tableRef.at("pn1");
            // Cannot check equality due to unordering (actuals1Values is not {1, 2, ..., 49}), this is PKB's optimisation.
            // Instead sort, and check for equality
            std::sort(actualValues.begin(), actualValues.end());
            bool areVecEqual = std::equal(pn1values.begin(), pn1values.end(), actualValues.begin());
            Assert::AreEqual(true, areVecEqual);

            // Test EvResult:
            bool actualEvResult = evTable.getEvResult();
            Assert::AreEqual(true, actualEvResult);

            // 3. Clean-up:
            Entity::performCleanUp();
            Parent::performCleanUp();
        }

    };
}