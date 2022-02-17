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

        TEST_METHOD(parseQuery_repeatDeclaration_exceptionThrown)
        {
            std::string queryRepeat = "stmt s; if s; Select s";
            auto wrapperFuncRepeat = [&queryRepeat] { PQLParser::parseQuery(queryRepeat); };
            Assert::ExpectException<QPSException>(wrapperFuncRepeat);
        }

        TEST_METHOD(parseQuery_modifiesFirstArgNonStmtSynonym_exceptionThrown) {
            std::string queryVar = "stmt s; variable v; Select s such that Modifies(v, _)";
            auto wrapperVar =
                [&queryVar] { PQLParser::parseQuery(queryVar); };
            Assert::ExpectException<QPSException>(wrapperVar);

            std::string queryProc = "stmt s; variable v; procedure p; Select s such that Modifies(p, _)";
            auto wrapperProc =
                [&queryProc] { PQLParser::parseQuery(queryProc); };
            Assert::ExpectException<QPSException>(wrapperProc);

            std::string queryConst = "stmt s; variable v; constant c; Select s such that Modifies(c, _)";
            auto wrapperConst =
                [&queryConst] { PQLParser::parseQuery(queryConst); };
            Assert::ExpectException<QPSException>(wrapperConst);
        }

        TEST_METHOD(parseQuery_modifiesFirstArgVariable_exceptionThrown) {
            std::string queryModifiesEntRef = "stmt s; variable v; Select s such that Modifies(\"x\", _)";
            auto wrapperModifiesEntRef =
                [&queryModifiesEntRef] { PQLParser::parseQuery(queryModifiesEntRef); };
            Assert::ExpectException<QPSException>(wrapperModifiesEntRef);
        }

        TEST_METHOD(parseQuery_modifiesFirstArgWildcard_exceptionThrown) {
            std::string queryModifiesWildcard = "stmt s; variable v; Select s such that Modifies(_, v)";
            auto wrapperModifiesWildcard =
                [&queryModifiesWildcard] { PQLParser::parseQuery(queryModifiesWildcard); };
            Assert::ExpectException<QPSException>(wrapperModifiesWildcard);
        }

        TEST_METHOD(parseQuery_modifiesSecondArgSynonym_exceptionThrown) {
            std::string queryModifiesStmtRef = "stmt s; if ifs; Select s such that Modifies(if, s)";
            auto wrapperModifiesStmtRef =
                [&queryModifiesStmtRef] { PQLParser::parseQuery(queryModifiesStmtRef); };
            Assert::ExpectException<QPSException>(wrapperModifiesStmtRef);
        }

        TEST_METHOD(parseQuery_modifiesSecondArgStmtNumber_exceptionThrown) {
            std::string queryModifiesStmtRef = "print pn; variable v; Select pn such that Modifies(pn, 1)";
            auto wrapperModifiesStmtRef =
                [&queryModifiesStmtRef] { PQLParser::parseQuery(queryModifiesStmtRef); };
            Assert::ExpectException<QPSException>(wrapperModifiesStmtRef);
        }

        TEST_METHOD(parseQuery_usesFirstArgNonStmtSynonym_exceptionThrown) {
            std::string queryVar = "stmt s; variable v; Select s such that Uses(v, _)";
            auto wrapperVar =
                [&queryVar] { PQLParser::parseQuery(queryVar); };
            Assert::ExpectException<QPSException>(wrapperVar);

            std::string queryProc = "stmt s; variable v; procedure p; Select s such that Uses(p, _)";
            auto wrapperProc =
                [&queryProc] { PQLParser::parseQuery(queryProc); };
            Assert::ExpectException<QPSException>(wrapperProc);

            std::string queryConst = "stmt s; variable v; constant c; Select s such that Uses(c, _)";
            auto wrapperConst =
                [&queryConst] { PQLParser::parseQuery(queryConst); };
            Assert::ExpectException<QPSException>(wrapperConst);
        }

        TEST_METHOD(parseQuery_usesFirstArgVariable_exceptionThrown) {
            std::string queryUsesEntRef = "stmt s; variable v; Select s such that Uses(\"x\", _)";
            auto wrapperUsesEntRef =
                [&queryUsesEntRef] { PQLParser::parseQuery(queryUsesEntRef); };
            Assert::ExpectException<QPSException>(wrapperUsesEntRef);
        }

        TEST_METHOD(parseQuery_usesFirstArgWildcard_exceptionThrown) {
            std::string queryUsesWildcard = "stmt s; variable v; Select s such that Uses(_, _)";
            auto wrapperUsesWildcard =
                [&queryUsesWildcard] { PQLParser::parseQuery(queryUsesWildcard); };
            Assert::ExpectException<QPSException>(wrapperUsesWildcard);
        }

        TEST_METHOD(parseQuery_usesSecondArgSynonym_exceptionThrown) {
            std::string queryUsesStmtRef = "while w; call c; Select w such that Uses(c, w)";
            auto wrapperUsesStmtRef =
                [&queryUsesStmtRef] { PQLParser::parseQuery(queryUsesStmtRef); };
            Assert::ExpectException<QPSException>(wrapperUsesStmtRef);
        }

        TEST_METHOD(parseQuery_usesSecondArgStmtNumber_exceptionThrown) {
            std::string queryUsesStmtRef = "assign a; variable v; Select a such that Uses(a, 1)";
            auto wrapperUsesStmtRef =
                [&queryUsesStmtRef] { PQLParser::parseQuery(queryUsesStmtRef); };
            Assert::ExpectException<QPSException>(wrapperUsesStmtRef);
        }

        TEST_METHOD(parseQuery_followsFirstArgNonStmtSynonym_exceptionThrown) {
            std::string queryFollowsVariable = "assign a; variable v; Select a such that Follows(v, a)";
            auto wrapperFollowsVariable =
                [&queryFollowsVariable] { PQLParser::parseQuery(queryFollowsVariable); };
            Assert::ExpectException<QPSException>(wrapperFollowsVariable);

            std::string queryFollowsProc = "assign a; procedure p; Select a such that Follows(p, a)";
            auto wrapperFollowsProc =
                [&queryFollowsProc] { PQLParser::parseQuery(queryFollowsProc); };
            Assert::ExpectException<QPSException>(wrapperFollowsProc);

            std::string queryFollowsConst = "assign a; constant c; Select a such that Follows(c, a)";
            auto wrapperFollowsConst =
                [&queryFollowsConst] { PQLParser::parseQuery(queryFollowsConst); };
            Assert::ExpectException<QPSException>(wrapperFollowsConst);

            std::string queryFollowsVarName = "assign a; constant c; Select a such that Follows(\"x\", a)";
            auto wrapperFollowsVarName =
                [&queryFollowsVarName] { PQLParser::parseQuery(queryFollowsVarName); };
            Assert::ExpectException<QPSException>(wrapperFollowsVarName);
        }

        TEST_METHOD(parseQuery_followsSecondArgNonStmtSynonym_exceptionThrown) {
            std::string queryFollowsVariable = "assign a; variable v; Select a such that Follows(a, v)";
            auto wrapperFollowsVariable =
                [&queryFollowsVariable] { PQLParser::parseQuery(queryFollowsVariable); };
            Assert::ExpectException<QPSException>(wrapperFollowsVariable);

            std::string queryFollowsProc = "assign a; procedure p; Select a such that Follows(a, p)";
            auto wrapperFollowsProc =
                [&queryFollowsProc] { PQLParser::parseQuery(queryFollowsProc); };
            Assert::ExpectException<QPSException>(wrapperFollowsProc);

            std::string queryFollowsConst = "assign a; constant c; Select a such that Follows(a, c)";
            auto wrapperFollowsConst =
                [&queryFollowsConst] { PQLParser::parseQuery(queryFollowsConst); };
            Assert::ExpectException<QPSException>(wrapperFollowsConst);

            std::string queryFollowsVarName = "assign a; constant c; Select a such that Follows(a, \"x\")";
            auto wrapperFollowsVarName =
                [&queryFollowsVarName] { PQLParser::parseQuery(queryFollowsVarName); };
            Assert::ExpectException<QPSException>(wrapperFollowsVarName);
        }

        TEST_METHOD(parseQuery_followsTFirstArgNonStmtSynonym_exceptionThrown) {
            std::string queryFollowsVariable = "assign a; variable v; Select a such that Follows*(v, a)";
            auto wrapperFollowsVariable =
                [&queryFollowsVariable] { PQLParser::parseQuery(queryFollowsVariable); };
            Assert::ExpectException<QPSException>(wrapperFollowsVariable);

            std::string queryFollowsProc = "assign a; procedure p; Select a such that Follows*(p, a)";
            auto wrapperFollowsProc =
                [&queryFollowsProc] { PQLParser::parseQuery(queryFollowsProc); };
            Assert::ExpectException<QPSException>(wrapperFollowsProc);

            std::string queryFollowsConst = "assign a; constant c; Select a such that Follows*(c, a)";
            auto wrapperFollowsConst =
                [&queryFollowsConst] { PQLParser::parseQuery(queryFollowsConst); };
            Assert::ExpectException<QPSException>(wrapperFollowsConst);

            std::string queryFollowsVarName = "assign a; constant c; Select a such that Follows*(\"x\", a)";
            auto wrapperFollowsVarName =
                [&queryFollowsVarName] { PQLParser::parseQuery(queryFollowsVarName); };
            Assert::ExpectException<QPSException>(wrapperFollowsVarName);
        }

        TEST_METHOD(parseQuery_followsTSecondArgNonStmtSynonym_exceptionThrown) {
            std::string queryFollowsVariable = "assign a; variable v; Select a such that Follows*(a, v)";
            auto wrapperFollowsVariable =
                [&queryFollowsVariable] { PQLParser::parseQuery(queryFollowsVariable); };
            Assert::ExpectException<QPSException>(wrapperFollowsVariable);

            std::string queryFollowsProc = "assign a; procedure p; Select a such that Follows*(a, p)";
            auto wrapperFollowsProc =
                [&queryFollowsProc] { PQLParser::parseQuery(queryFollowsProc); };
            Assert::ExpectException<QPSException>(wrapperFollowsProc);

            std::string queryFollowsConst = "assign a; constant c; Select a such that Follows*(a, c)";
            auto wrapperFollowsConst =
                [&queryFollowsConst] { PQLParser::parseQuery(queryFollowsConst); };
            Assert::ExpectException<QPSException>(wrapperFollowsConst);

            std::string queryFollowsVarName = "assign a; constant c; Select a such that Follows*(a, \"x\")";
            auto wrapperFollowsVarName =
                [&queryFollowsVarName] { PQLParser::parseQuery(queryFollowsVarName); };
            Assert::ExpectException<QPSException>(wrapperFollowsVarName);
        }
        TEST_METHOD(parseQuery_parentFirstArgNonStmtSynonym_exceptionThrown) {
            std::string queryParentVariable = "assign a; variable v; Select a such that Parent(v, a)";
            auto wrapperParentVariable =
                [&queryParentVariable] { PQLParser::parseQuery(queryParentVariable); };
            Assert::ExpectException<QPSException>(wrapperParentVariable);

            std::string queryParentProc = "assign a; procedure p; Select a such that Parent(p, a)";
            auto wrapperParentProc =
                [&queryParentProc] { PQLParser::parseQuery(queryParentProc); };
            Assert::ExpectException<QPSException>(wrapperParentProc);

            std::string queryParentConst = "assign a; constant c; Select a such that Parent(c, a)";
            auto wrapperParentConst =
                [&queryParentConst] { PQLParser::parseQuery(queryParentConst); };
            Assert::ExpectException<QPSException>(wrapperParentConst);

            std::string queryParentVarName = "assign a; constant c; Select a such that Parent(\"x\", a)";
            auto wrapperParentVarName =
                [&queryParentVarName] { PQLParser::parseQuery(queryParentVarName); };
            Assert::ExpectException<QPSException>(wrapperParentVarName);
        }

        TEST_METHOD(parseQuery_parentSecondArgNonStmtSynonym_exceptionThrown) {
            std::string queryParentVariable = "assign a; variable v; Select a such that Parent(a, v)";
            auto wrapperParentVariable =
                [&queryParentVariable] { PQLParser::parseQuery(queryParentVariable); };
            Assert::ExpectException<QPSException>(wrapperParentVariable);

            std::string queryParentProc = "assign a; procedure p; Select a such that Parent(a, p)";
            auto wrapperParentProc =
                [&queryParentProc] { PQLParser::parseQuery(queryParentProc); };
            Assert::ExpectException<QPSException>(wrapperParentProc);

            std::string queryParentConst = "assign a; constant c; Select a such that Parent(a, c)";
            auto wrapperParentConst =
                [&queryParentConst] { PQLParser::parseQuery(queryParentConst); };
            Assert::ExpectException<QPSException>(wrapperParentConst);

            std::string queryParentVarName = "assign a; constant c; Select a such that Parent(a, \"x\")";
            auto wrapperParentVarName =
                [&queryParentVarName] { PQLParser::parseQuery(queryParentVarName); };
            Assert::ExpectException<QPSException>(wrapperParentVarName);
        }

        TEST_METHOD(parseQuery_parentTFirstArgNonStmtSynonym_exceptionThrown) {
            std::string queryParentVariable = "assign a; variable v; Select a such that Parent*(v, a)";
            auto wrapperParentVariable =
                [&queryParentVariable] { PQLParser::parseQuery(queryParentVariable); };
            Assert::ExpectException<QPSException>(wrapperParentVariable);

            std::string queryParentProc = "assign a; procedure p; Select a such that Parent*(p, a)";
            auto wrapperParentProc =
                [&queryParentProc] { PQLParser::parseQuery(queryParentProc); };
            Assert::ExpectException<QPSException>(wrapperParentProc);

            std::string queryParentConst = "assign a; constant c; Select a such that Parent*(c, a)";
            auto wrapperParentConst =
                [&queryParentConst] { PQLParser::parseQuery(queryParentConst); };
            Assert::ExpectException<QPSException>(wrapperParentConst);

            std::string queryParentVarName = "assign a; constant c; Select a such that Parent*(\"x\", a)";
            auto wrapperParentVarName =
                [&queryParentVarName] { PQLParser::parseQuery(queryParentVarName); };
            Assert::ExpectException<QPSException>(wrapperParentVarName);
        }

        TEST_METHOD(parseQuery_parentTSecondArgNonStmtSynonym_exceptionThrown) {
            std::string queryParentVariable = "assign a; variable v; Select a such that Parent*(a, v)";
            auto wrapperParentVariable =
                [&queryParentVariable] { PQLParser::parseQuery(queryParentVariable); };
            Assert::ExpectException<QPSException>(wrapperParentVariable);

            std::string queryParentProc = "assign a; procedure p; Select a such that Parent*(a, p)";
            auto wrapperParentProc =
                [&queryParentProc] { PQLParser::parseQuery(queryParentProc); };
            Assert::ExpectException<QPSException>(wrapperParentProc);

            std::string queryParentConst = "assign a; constant c; Select a such that Parent*(a, c)";
            auto wrapperParentConst =
                [&queryParentConst] { PQLParser::parseQuery(queryParentConst); };
            Assert::ExpectException<QPSException>(wrapperParentConst);

            std::string queryParentVarName = "assign a; constant c; Select a such that Parent*(a, \"x\")";
            auto wrapperParentVarName =
                [&queryParentVarName] { PQLParser::parseQuery(queryParentVarName); };
            Assert::ExpectException<QPSException>(wrapperParentVarName);
        }

        TEST_METHOD(parseQuery_patternSynAssignInvalid_exceptionThrown) {
            std::string queryVariable = "assign a; variable v; Select a pattern v(_, _)";
            auto wrapperVariable =
                [&queryVariable] { PQLParser::parseQuery(queryVariable); };
            Assert::ExpectException<QPSException>(wrapperVariable);

            std::string queryPrint = "assign a; print pn; Select a pattern pn(_, _)";
            auto wrapperPrint =
                [&queryPrint] { PQLParser::parseQuery(queryPrint); };
            Assert::ExpectException<QPSException>(wrapperPrint);
        }

        TEST_METHOD(parseQuery_patternEntRefInvalid_exceptionThrown) {
            std::string queryAssign = "assign a; variable v; Select a pattern a(a, _)";
            auto wrapperAssign =
                [&queryAssign] { PQLParser::parseQuery(queryAssign); };
            Assert::ExpectException<QPSException>(wrapperAssign);

            std::string queryConstant = "assign a; constant c; Select a pattern a(c, _)";
            auto wrapperConstant =
                [&queryConstant] { PQLParser::parseQuery(queryConstant); };
            Assert::ExpectException<QPSException>(wrapperConstant);

            std::string queryRead= "assign a; read r; Select a pattern a(r, _)";
            auto wrapperRead=
                [&queryRead] { PQLParser::parseQuery(queryRead); };
            Assert::ExpectException<QPSException>(wrapperRead);
        }
        TEST_METHOD(parseQuery_syntaxError_exceptionThrown)
        {
            /* No synonym Selected */
            std::string queryMissingSelect = "stmt s; if ifs; Select";
            auto wrapperFuncMissingSelect = [&queryMissingSelect] { PQLParser::parseQuery(queryMissingSelect); };
            Assert::ExpectException<QPSException>(wrapperFuncMissingSelect);

            /* Extra trailing characters */
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
