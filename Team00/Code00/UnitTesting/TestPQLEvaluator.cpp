#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS-NEW/PQLEvaluator.h"
#include "../source/QPS-NEW/PQLParser.h"
#include "../source/PKB/RS2.h"
#include "../source/PKB/Follows.h"

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
            auto wrapperFunc = [&pqlEvaluator] { pqlEvaluator.evaluate(); };
            
            Assert::AreEqual(size_t(2), pq1.getDeclarations().size());
            Assert::IsFalse(pq1.getColumns().empty());
            Assert::AreEqual(std::string("s"), pq1.getColumns()[0]);
        }

        TEST_METHOD(evaluateQuery_followsOnly_noMerge_designEntitiesExtracted)
        {
            // Follows(1, 2)
            std::vector<PQL_VARIABLE> declarations;
            std::vector<std::string> columns;
            std::vector<ParsedRelationship> relationships;
            PqlReference lhsRef, rhsRef;
            lhsRef = std::make_pair(PqlReferenceType::integer, "1");
            rhsRef = std::make_pair(PqlReferenceType::integer, "2");
            relationships.push_back(ParsedRelationship(PqlRelationshipType::Follows, lhsRef, rhsRef));
            std::vector<ParsedPattern> patterns;
            ParsedQuery pq1 = ParsedQuery(declarations, columns, relationships, patterns);

            // PKB insert statements
            StmtIndex s1 = Entity::insertStmt(StatementType::assignType);
            StmtIndex s2 = Entity::insertStmt(StatementType::assignType);
            Follows::insert(s1, s2);

            PQLEvaluator pqlEvaluator(pq1);
            EvaluatedTable evTable = pqlEvaluator.evaluate();

            Assert::AreEqual(size_t(0), evTable.getNumRow());
            Assert::AreEqual(true, evTable.getEvResult());
        }
    };
}