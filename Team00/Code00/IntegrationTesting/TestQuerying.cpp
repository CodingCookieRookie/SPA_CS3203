#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include <algorithm>

#include "../source/QPS-NEW/PQLEvaluator.h"
#include "../source/QPS-NEW/PQLParser.h"
#include "../source/QPS-NEW/PQLResultProjector.h"
#include "../source/PKB/RS2.h"
#include "../source/PKB/Follows.h"
#include "../source/PKB/FollowsT.h"
#include "../source/PKB/Parent.h"
#include "../source/PKB/ParentT.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace IntegrationTesting
{
    TEST_CLASS(TestQPSPKB)
    {
    private:
        TEST_METHOD_CLEANUP(cleanUpTables) {
            Entity::performCleanUp();
            FollowsT::performCleanUp();
            Follows::performCleanUp();
            ParentT::performCleanUp();
            Parent::performCleanUp();
        }
    public:

        TEST_METHOD(querying_declarationAndSelectStmtsOnly_success)
        {
            // "stmt s; variable v1; Select s";
            // 1. Setup:
            std::string query = "stmt s1; if ifs; Select s1";
            // PKB inserts 6 types of statements
            std::vector<StmtIndex> stmts;
            stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
            stmts.emplace_back(Entity::insertStmt(StatementType::printType));
            stmts.emplace_back(Entity::insertStmt(StatementType::callType));
            stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
            stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
            stmts.emplace_back(Entity::insertStmt(StatementType::readType));

            // 2. Test QPS Parser:
            ParsedQuery parsedQuery = PQLParser::parseQuery(query);
            Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
            Assert::IsFalse(parsedQuery.getColumns().empty());
            Assert::AreEqual(std::string("s1"), parsedQuery.getColumns()[0]);

            // 3. Test QPS Evaluator:
            PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
            EvaluatedTable evTable = pqlEvaluator.evaluate();
            // Test numRow:
            Assert::AreEqual(size_t(6), evTable.getNumRow());
            // Test Table:
            auto tableRef = evTable.getTableRef();
            Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
            // Test Entities: std::unordered_map<std::string, PqlEntityType>
            std::vector<int> values{ 1, 2, 3, 4, 5, 6 };
            auto actualValues = tableRef.at("s1");
            bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
            Assert::AreEqual(true, areVecEqual);
            // Test EvResult:
            bool actualEvResult = evTable.getEvResult();
            Assert::AreEqual(true, actualEvResult);


            // 4. Test QPS Result Projector:
            PQLResultProjector resultProjector = PQLResultProjector(evTable);
            std::list<std::string> results = resultProjector.resolveTableToResults();
            std::list<std::string> expectedRes{ "1", "2", "3", "4", "5", "6" };
            bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
            Assert::AreEqual(true, areListsEqual);
        }

    };
}