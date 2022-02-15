#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS-NEW/PQLParser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
    TEST_CLASS(TestPQLParser)
    {
    public:
        TEST_METHOD(parseQuery_declarationAndSelectOnly_designEntitiesExtracted)
        {
            std::string query = "stmt s; if ifs; Select s";
            ParsedQuery parsedQuery = PQLParser::parseQuery(query);
            Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
            Assert::IsFalse(parsedQuery.getColumns().empty());
            Assert::AreEqual(std::string("s"), parsedQuery.getColumns()[0]);
        }

        TEST_METHOD(parseQuery_invalidSemantics_exceptionThrown)
        {
            /* Repeat declaration */
            std::string queryRepeat = "stmt s; if s; Select s";
            auto wrapperFuncRepeat = [&queryRepeat] { PQLParser::parseQuery(queryRepeat); };
            Assert::ExpectException<QPSException>(wrapperFuncRepeat);

            /* Undeclared "Select" synonym */
            std::string queryUndeclared = "stmt s; if ifs; Select s1";
            auto wrapperFuncUndeclared = [&queryUndeclared] { PQLParser::parseQuery(queryUndeclared); };
            Assert::ExpectException<QPSException>(wrapperFuncUndeclared);
        }

        TEST_METHOD(parseQuery_syntaxError_exceptionThrown)
        {
            /* No synonym Selected */
            std::string queryMissingSelect = "stmt s; if ifs; Select";
            auto wrapperFuncMissingSelect = [&queryMissingSelect] { PQLParser::parseQuery(queryMissingSelect); };
            Assert::ExpectException<QPSException>(wrapperFuncMissingSelect);

            ///* Extra trailing characters */
            std::string queryExtra = "stmt s; if ifs; Select s test";
            auto wrapperFuncExtra = [&queryExtra] { PQLParser::parseQuery(queryExtra); };
            Assert::ExpectException<QPSException>(wrapperFuncExtra);
        }

        TEST_METHOD(parseQuery_usesClause_suchThatExtracted) {
            std::string query = "stmt s; variable v; Select s such that Uses(s, v)";
            ParsedQuery parsedQuery = PQLParser::parseQuery(query);
            std::vector<ParsedRelationship> relationships =
                parsedQuery.getRelationships();
            Assert::AreEqual(size_t(1), relationships.size());
            Assert::IsTrue(
                PqlRelationshipType::UsesS
                == relationships[0].getRelationshipType());
            Assert::IsTrue(
                PqlReferenceType::synonym
                == relationships[0].getLhs().first);
            Assert::AreEqual(std::string("s"),
                relationships[0].getLhs().second);
            Assert::IsTrue(
                PqlReferenceType::synonym
                == relationships[0].getRhs().first);
            Assert::AreEqual(std::string("v"),
                relationships[0].getRhs().second);
        }

        TEST_METHOD(parseQuery_followsClauseStmtIndex_stmtIndexExtracted) {
            std::string query = "stmt s1, s2; Select s1 such that Follows*(1, s1)";
            ParsedQuery parsedQuery = PQLParser::parseQuery(query);
            std::vector<ParsedRelationship> relationships =
                parsedQuery.getRelationships();
            Assert::AreEqual(size_t(1), relationships.size());
            Assert::IsTrue(
                PqlRelationshipType::FollowsT
                == relationships[0].getRelationshipType());
            Assert::IsTrue(
                PqlReferenceType::integer
                == relationships[0].getLhs().first);
            Assert::AreEqual(std::string("1"),
                relationships[0].getLhs().second);
        }
        TEST_METHOD(parseQuery_modifiesClauseVarName_varNameExtracted) {
            std::string query = "stmt s; variable v; Select s such that Modifies(s, \"x\")";
            ParsedQuery parsedQuery = PQLParser::parseQuery(query);
            std::vector<ParsedRelationship> relationships =
                parsedQuery.getRelationships();
            Assert::AreEqual(size_t(1), relationships.size());
            Assert::IsTrue(
                PqlRelationshipType::ModifiesS
                == relationships[0].getRelationshipType());
            Assert::IsTrue(
                PqlReferenceType::ident
                == relationships[0].getRhs().first);
            Assert::AreEqual(std::string("x"),
                relationships[0].getRhs().second);
        }

        TEST_METHOD(parseQuery_patternClause_patternExtracted) {
            std::string query = "assign a; Select a pattern a(_, _)";
            ParsedQuery parsedQuery = PQLParser::parseQuery(query);
            std::vector<ParsedPattern> patterns =
                parsedQuery.getPatterns();
            Assert::AreEqual(size_t(1), patterns.size());
            Assert::AreEqual(std::string("a"), patterns[0].getSynonym());
            Assert::IsTrue(
                PqlReferenceType::wildcard
                == patterns[0].getEntRef().first);
            Assert::AreEqual(std::string(), patterns[0].getEntRef().second);
            Assert::IsTrue(
                PqlExpressionType::wildcard
                == patterns[0].getExpression().first);
            Assert::AreEqual(std::string(), patterns[0].getExpression().second);
        }

        TEST_METHOD(parseQuery_patternClauseSynonym_synonymExtracted) {
            std::string query = "assign a; variable v; Select a pattern a(v, _)";
            ParsedQuery parsedQuery = PQLParser::parseQuery(query);
            std::vector<ParsedPattern> patterns =
                parsedQuery.getPatterns();
            Assert::AreEqual(size_t(1), patterns.size());
            Assert::AreEqual(std::string("a"), patterns[0].getSynonym());
            Assert::IsTrue(
                PqlReferenceType::synonym
                == patterns[0].getEntRef().first);
            Assert::AreEqual(std::string("v"), patterns[0].getEntRef().second);
        }

        TEST_METHOD(parseQuery_patternClauseVarExpr_varExtracted) {
            std::string query = "assign a; Select a pattern a(_, _\"x\"_)";
            ParsedQuery parsedQuery = PQLParser::parseQuery(query);
            std::vector<ParsedPattern> patterns =
                parsedQuery.getPatterns();
            Assert::AreEqual(size_t(1), patterns.size());
            Assert::IsTrue(
                PqlExpressionType::partial
                == patterns[0].getExpression().first);
            Assert::AreEqual(std::string("x"), patterns[0].getExpression().second);
        }

        TEST_METHOD(parseQuery_suchThatAndPatternClauses_bothExtracted) {
            std::string query =
                "assign a; stmt s;"
                "Select a such that Parent*(s, a) pattern a(_, _\"x\"_)";
            ParsedQuery parsedQuery = PQLParser::parseQuery(query);

            std::vector<ParsedRelationship> relationships =
                parsedQuery.getRelationships();
            Assert::AreEqual(size_t(1), relationships.size());
            Assert::IsTrue(
                PqlRelationshipType::ParentT
                == relationships[0].getRelationshipType());

            std::vector<ParsedPattern> patterns =
                parsedQuery.getPatterns();
            Assert::AreEqual(size_t(1), patterns.size());
            Assert::IsTrue(
                PqlExpressionType::partial
                == patterns[0].getExpression().first);
            Assert::AreEqual(std::string("x"), patterns[0].getExpression().second);
        }
    };
}
