#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS/PQLParser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPQLParser) {
public:
	TEST_METHOD(parseQuery_declarationAndSelectOnly_designEntitiesExtracted) {
		std::string query = "stmt s; if ifs; Select s";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::IsFalse(parsedQuery.getColumns().empty());
		Assert::AreEqual(std::string("s"), parsedQuery.getColumns()[0]);
	}

	TEST_METHOD(parseQuery_repeatDeclaration_exceptionThrown) {
		std::string queryRepeat = "stmt s; if s; Select s";
		auto wrapperFuncRepeat = [&queryRepeat] { PQLParser::parseQuery(queryRepeat); };
		Assert::ExpectException<QPSException>(wrapperFuncRepeat);
	}

	TEST_METHOD(parseQuery_modifiesFirstArgNonStmtSynonym_exceptionThrown) {
		std::string queryVar = "stmt s; variable v; Select s such that Modifies(v, _)";
		auto wrapperVar =
			[&queryVar] { PQLParser::parseQuery(queryVar); };
		Assert::ExpectException<QPSException>(wrapperVar);

		std::string queryConst = "stmt s; variable v; constant c; Select s such that Modifies(c, _)";
		auto wrapperConst =
			[&queryConst] { PQLParser::parseQuery(queryConst); };
		Assert::ExpectException<QPSException>(wrapperConst);
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

		std::string queryConst = "stmt s; variable v; constant c; Select s such that Uses(c, _)";
		auto wrapperConst =
			[&queryConst] { PQLParser::parseQuery(queryConst); };
		Assert::ExpectException<QPSException>(wrapperConst);
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

		std::string queryRead = "assign a; read r; Select a pattern a(r, _)";
		auto wrapperRead =
			[&queryRead] { PQLParser::parseQuery(queryRead); };
		Assert::ExpectException<QPSException>(wrapperRead);
	}
	TEST_METHOD(parseQuery_syntaxError_exceptionThrown) {
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
		Assert::AreEqual(std::string(" x "), patterns[0].getExpression().second);
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
		Assert::AreEqual(std::string(" x "), patterns[0].getExpression().second);
	}

	TEST_METHOD(parseQuery_patternClausePartialExpr_exprExtracted) {
		std::string query = "assign a; Select a pattern a(_, _\"(x + 1)       * 2 / 3		-4 %\n (5)\"_)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedPattern> patterns =
			parsedQuery.getPatterns();
		Assert::AreEqual(size_t(1), patterns.size());
		Assert::IsTrue(PqlExpressionType::partial == patterns[0].getExpression().first);
		Assert::AreEqual(std::string(" x 1 + 2 * 3 / 4 5 % - "), patterns[0].getExpression().second);
	}

	TEST_METHOD(parseQuery_patternClauseFullExpr_exprExtracted) {
		std::string query = "assign a; Select a pattern a(_, \"(x + 1)       * 2 / 3		-4 %\n (5)\")";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedPattern> patterns =
			parsedQuery.getPatterns();
		Assert::AreEqual(size_t(1), patterns.size());
		Assert::IsTrue(PqlExpressionType::full == patterns[0].getExpression().first);
		Assert::AreEqual(std::string(" x 1 + 2 * 3 / 4 5 % - "), patterns[0].getExpression().second);
	}

	TEST_METHOD(parseQuery_patternClausePartialExprMissingOpeningQuotation_exceptionThrown) {
		std::string query = "assign a; Select a pattern a(_, _(x + 1)       * 2 / 3		-4 %\n (5)\"_)";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_patternClausePartialExprMissingClosingQuotation_exceptionThrown) {
		std::string query = "assign a; Select a pattern a(_, _\"(x + 1)       * 2 / 3		-4 %\n (5)_)";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_patternClausePartialExprMissingOpeningUnderscore_exceptionThrown) {
		std::string query = "assign a; Select a pattern a(_, \"(x + 1)       * 2 / 3		-4 %\n (5)\"_)";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_patternClausePartialExprMissingClosingUnderscore_exceptionThrown) {
		std::string query = "assign a; Select a pattern a(_, _\"(x + 1)       * 2 / 3		-4 %\n (5)\")";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_patternClauseFullExprMissingOpeningQuotation_exceptionThrown) {
		std::string query = "assign a; Select a pattern a(_, (x + 1)       * 2 / 3		-4 %\n (5)\")";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_patternClauseFullExprMissingClosingQuotation_exceptionThrown) {
		std::string query = "assign a; Select a pattern a(_, \"(x + 1)       * 2 / 3		-4 %\n (5))";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_invalidExpression_exceptionThrown) {
		std::string query = "assign a; Select a pattern a(_, \"\")";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<ExpressionException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_invalidExpressionOperatorOnly_exceptionThrown) {
		std::string query = "assign a; Select a pattern a(_, \" + \")";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<ExpressionException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_leadingZeroInSuchThat_lexerExceptionThrown) {
		std::string query =
			"variable v;"
			"Select v such that Uses(001, v)";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<LexerException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_leadingZeroInPattern_lexerExceptionThrown) {
		std::string query =
			"assign a;"
			"Select a pattern a (_, _\"02\"_)";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<LexerException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_modifiesFirstArgIdent_correctlyParsedAsModifiesP) {
		std::string query = "stmt s; variable v; Select s such that Modifies(\"x\", _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::ModifiesP == relationships[0].getRelationshipType());
	}

	TEST_METHOD(parseQuery_usesFirstArgIdent_correctlyParsedAsUsesP) {
		std::string query = "stmt s; variable v; Select s such that Uses(\"x\", _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::UsesP == relationships[0].getRelationshipType());
	}

	TEST_METHOD(parseQuery_usesFirstArgProcSynonym_correctlyParsedAsModifiesP) {
		std::string query = "stmt s; variable v; procedure p; Select s such that Modifies(p, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::ModifiesP == relationships[0].getRelationshipType());
	}

	TEST_METHOD(parseQuery_usesFirstArgProcSynonym_correctlyParsedAsUsesP) {
		std::string query = "stmt s; variable v; procedure p; Select s such that Uses(p, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::UsesP == relationships[0].getRelationshipType());
	}

	TEST_METHOD(parseQuery_callsClauseTwoSynonyms_bothSynonymsExtracted) {
		std::string query = "procedure p1, p2; Select p1 such that Calls(p1, p2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::Calls == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("p1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("p2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_callsClauseTwoWildcards_bothWildcardsExtracted) {
		std::string query = "procedure p1, p2; Select p1 such that Calls(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::Calls == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::wildcard == relationships[0].getLhs().first);
		Assert::IsTrue(PqlReferenceType::wildcard == relationships[0].getRhs().first);
	}

	TEST_METHOD(parseQuery_callsClauseTwoIdent_bothIdentsExtracted) {
		std::string query = "procedure p1, p2; Select p1 such that Calls(\"proc1\", \"proc2\")";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::Calls == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::ident == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("proc1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::ident == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("proc2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_callsTClauseTwoSynonyms_bothSynonymsExtracted) {
		std::string query = "procedure p1, p2; Select p1 such that Calls*(p1, p2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::CallsT == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("p1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("p2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_callsTClauseTwoWildcards_bothWildcardsExtracted) {
		std::string query = "procedure p1, p2; Select p1 such that Calls*(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::CallsT == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::wildcard == relationships[0].getLhs().first);
		Assert::IsTrue(PqlReferenceType::wildcard == relationships[0].getRhs().first);
	}

	TEST_METHOD(parseQuery_callsTClauseTwoIdent_bothIdentsExtracted) {
		std::string query = "procedure p1, p2; Select p1 such that Calls*(\"proc1\", \"proc2\")";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::CallsT == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::ident == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("proc1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::ident == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("proc2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_nextClauseTwoSynonyms_bothSynonymsExtracted) {
		std::string query = "stmt s1, s2; Select s1 such that Next(s1, s2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::Next == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("s1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("s2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_nextClauseTwoWildcards_bothWildcardsExtracted) {
		std::string query = "stmt s1, s2; Select s1 such that Next(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::Next == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::wildcard == relationships[0].getLhs().first);
		Assert::IsTrue(PqlReferenceType::wildcard == relationships[0].getRhs().first);
	}

	TEST_METHOD(parseQuery_nextClauseTwoIntegers_bothStmtIndexesExtracted) {
		std::string query = "stmt s1, s2; Select s1 such that Next(1, 2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::Next == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::integer == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::integer == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_nextTClauseTwoSynonyms_bothSynonymsExtracted) {
		std::string query = "stmt s1, s2; Select s1 such that Next*(s1, s2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::NextT == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("s1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("s2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_nextTClauseTwoWildcards_bothWildcardsExtracted) {
		std::string query = "stmt s1, s2; Select s1 such that Next*(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::NextT == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::wildcard == relationships[0].getLhs().first);
		Assert::IsTrue(PqlReferenceType::wildcard == relationships[0].getRhs().first);
	}

	TEST_METHOD(parseQuery_nextTClauseTwoIntegers_bothStmtIndexesExtracted) {
		std::string query = "stmt s1, s2; Select s1 such that Next*(1, 2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::NextT == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::integer == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::integer == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_affectsClauseTwoAssigns_bothSynonymsExtracted) {
		std::string query = "assign a1, a2; Select a1 such that Affects(a1, a2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::Affects == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("a1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("a2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_affectsClauseTwoStmts_bothSynonymsExtracted) {
		std::string query = "stmt s1, s2; Select s1 such that Affects(s1, s2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::Affects == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("s1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("s2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_affectsClauseTwoWildcards_bothWildcardsExtracted) {
		std::string query = "assign a1, a2; Select a1 such that Affects(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::Affects == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::wildcard == relationships[0].getLhs().first);
		Assert::IsTrue(PqlReferenceType::wildcard == relationships[0].getRhs().first);
	}

	TEST_METHOD(parseQuery_affectsClauseTwoIntegers_bothStmtIndexesExtracted) {
		std::string query = "assign a1, a2; Select a1 such that Affects(1, 2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::Affects == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::integer == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::integer == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_affectsTClauseTwoAssigns_bothSynonymsExtracted) {
		std::string query = "assign a1, a2; Select a1 such that Affects*(a1, a2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::AffectsT == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("a1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("a2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_affectsTClauseTwoStmts_bothSynonymsExtracted) {
		std::string query = "stmt s1, s2; Select s1 such that Affects*(s1, s2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::AffectsT == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("s1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("s2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_affectsTClauseTwoWildcards_bothWildcardsExtracted) {
		std::string query = "assign a1, a2; Select a1 such that Affects*(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::AffectsT == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::wildcard == relationships[0].getLhs().first);
		Assert::IsTrue(PqlReferenceType::wildcard == relationships[0].getRhs().first);
	}

	TEST_METHOD(parseQuery_affectsTClauseTwoIntegers_bothStmtIndexesExtracted) {
		std::string query = "assign a1, a2; Select a1 such that Affects*(1, 2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::AffectsT == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::integer == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::integer == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_callsFirstArgNonProcSynonym_exceptionThrown) {
		std::string queryCallsVariable = "procedure p; variable v; Select p such that Calls(v, p)";
		auto wrapperCallsVariable =
			[&queryCallsVariable] { PQLParser::parseQuery(queryCallsVariable); };
		Assert::ExpectException<QPSException>(wrapperCallsVariable);

		std::string queryCallsConst = "procedure p; constant c; Select p such that Calls(c, p)";
		auto wrapperCallsConst =
			[&queryCallsConst] { PQLParser::parseQuery(queryCallsConst); };
		Assert::ExpectException<QPSException>(wrapperCallsConst);

		std::string queryCallsStmt = "procedure p; stmt s; Select p such that Calls(s, p)";
		auto wrapperCallsStmt =
			[&queryCallsStmt] { PQLParser::parseQuery(queryCallsStmt); };
		Assert::ExpectException<QPSException>(wrapperCallsStmt);

		std::string queryCallsRead = "procedure p; read r; Select p such that Calls(r, p)";
		auto wrapperCallsRead =
			[&queryCallsRead] { PQLParser::parseQuery(queryCallsRead); };
		Assert::ExpectException<QPSException>(wrapperCallsRead);

		std::string queryCallsPrint = "procedure p; print pn; Select p such that Calls(pn, p)";
		auto wrapperCallsPrint =
			[&queryCallsPrint] { PQLParser::parseQuery(queryCallsPrint); };
		Assert::ExpectException<QPSException>(wrapperCallsPrint);

		std::string queryCallsCall = "procedure p; call c; Select p such that Calls(c, p)";
		auto wrapperCallsCall =
			[&queryCallsCall] { PQLParser::parseQuery(queryCallsCall); };
		Assert::ExpectException<QPSException>(wrapperCallsCall);

		std::string queryCallsWhile = "procedure p; while w; Select p such that Calls(w, p)";
		auto wrapperCallsWhile =
			[&queryCallsWhile] { PQLParser::parseQuery(queryCallsWhile); };
		Assert::ExpectException<QPSException>(wrapperCallsWhile);

		std::string queryCallsIf = "procedure p; if ifs; Select p such that Calls(ifs, p)";
		auto wrapperCallsIf =
			[&queryCallsIf] { PQLParser::parseQuery(queryCallsIf); };
		Assert::ExpectException<QPSException>(wrapperCallsIf);

		std::string queryCallsAssign = "procedure p; assign a; Select p such that Calls(a, p)";
		auto wrapperCallsAssign =
			[&queryCallsAssign] { PQLParser::parseQuery(queryCallsAssign); };
		Assert::ExpectException<QPSException>(wrapperCallsAssign);

		std::string queryCallsInteger = "procedure p; constant c; Select p such that Calls(1, p)";
		auto wrapperCallsInteger =
			[&queryCallsInteger] { PQLParser::parseQuery(queryCallsInteger); };
		Assert::ExpectException<QPSException>(wrapperCallsInteger);
	}

	TEST_METHOD(parseQuery_callsSecondArgNonProcSynonym_exceptionThrown) {
		std::string queryCallsVariable = "procedure p; variable v; Select p such that Calls(p, v)";
		auto wrapperCallsVariable =
			[&queryCallsVariable] { PQLParser::parseQuery(queryCallsVariable); };
		Assert::ExpectException<QPSException>(wrapperCallsVariable);

		std::string queryCallsConst = "procedure p; constant c; Select p such that Calls(p, c)";
		auto wrapperCallsConst =
			[&queryCallsConst] { PQLParser::parseQuery(queryCallsConst); };
		Assert::ExpectException<QPSException>(wrapperCallsConst);

		std::string queryCallsStmt = "procedure p; stmt s; Select p such that Calls(p, s)";
		auto wrapperCallsStmt =
			[&queryCallsStmt] { PQLParser::parseQuery(queryCallsStmt); };
		Assert::ExpectException<QPSException>(wrapperCallsStmt);

		std::string queryCallsRead = "procedure p; read r; Select p such that Calls(p, r)";
		auto wrapperCallsRead =
			[&queryCallsRead] { PQLParser::parseQuery(queryCallsRead); };
		Assert::ExpectException<QPSException>(wrapperCallsRead);

		std::string queryCallsPrint = "procedure p; print pn; Select p such that Calls(p, pn)";
		auto wrapperCallsPrint =
			[&queryCallsPrint] { PQLParser::parseQuery(queryCallsPrint); };
		Assert::ExpectException<QPSException>(wrapperCallsPrint);

		std::string queryCallsCall = "procedure p; call c; Select p such that Calls(p, c)";
		auto wrapperCallsCall =
			[&queryCallsCall] { PQLParser::parseQuery(queryCallsCall); };
		Assert::ExpectException<QPSException>(wrapperCallsCall);

		std::string queryCallsWhile = "procedure p; while w; Select p such that Calls(p, w)";
		auto wrapperCallsWhile =
			[&queryCallsWhile] { PQLParser::parseQuery(queryCallsWhile); };
		Assert::ExpectException<QPSException>(wrapperCallsWhile);

		std::string queryCallsIf = "procedure p; if ifs; Select p such that Calls(p, ifs)";
		auto wrapperCallsIf =
			[&queryCallsIf] { PQLParser::parseQuery(queryCallsIf); };
		Assert::ExpectException<QPSException>(wrapperCallsIf);

		std::string queryCallsAssign = "procedure p; assign a; Select p such that Calls(p, a)";
		auto wrapperCallsAssign =
			[&queryCallsAssign] { PQLParser::parseQuery(queryCallsAssign); };
		Assert::ExpectException<QPSException>(wrapperCallsAssign);

		std::string queryCallsInteger = "procedure p; constant c; Select p such that Calls(p, 1)";
		auto wrapperCallsInteger =
			[&queryCallsInteger] { PQLParser::parseQuery(queryCallsInteger); };
		Assert::ExpectException<QPSException>(wrapperCallsInteger);
	}


	TEST_METHOD(parseQuery_callsTFirstArgNonProcSynonym_exceptionThrown) {
		std::string queryCallsVariable = "procedure p; variable v; Select p such that Calls*(v, p)";
		auto wrapperCallsVariable =
			[&queryCallsVariable] { PQLParser::parseQuery(queryCallsVariable); };
		Assert::ExpectException<QPSException>(wrapperCallsVariable);

		std::string queryCallsConst = "procedure p; constant c; Select p such that Calls*(c, p)";
		auto wrapperCallsConst =
			[&queryCallsConst] { PQLParser::parseQuery(queryCallsConst); };
		Assert::ExpectException<QPSException>(wrapperCallsConst);

		std::string queryCallsStmt = "procedure p; stmt s; Select p such that Calls*(s, p)";
		auto wrapperCallsStmt =
			[&queryCallsStmt] { PQLParser::parseQuery(queryCallsStmt); };
		Assert::ExpectException<QPSException>(wrapperCallsStmt);

		std::string queryCallsRead = "procedure p; read r; Select p such that Calls*(r, p)";
		auto wrapperCallsRead =
			[&queryCallsRead] { PQLParser::parseQuery(queryCallsRead); };
		Assert::ExpectException<QPSException>(wrapperCallsRead);

		std::string queryCallsPrint = "procedure p; print pn; Select p such that Calls*(pn, p)";
		auto wrapperCallsPrint =
			[&queryCallsPrint] { PQLParser::parseQuery(queryCallsPrint); };
		Assert::ExpectException<QPSException>(wrapperCallsPrint);

		std::string queryCallsCall = "procedure p; call c; Select p such that Calls*(c, p)";
		auto wrapperCallsCall =
			[&queryCallsCall] { PQLParser::parseQuery(queryCallsCall); };
		Assert::ExpectException<QPSException>(wrapperCallsCall);

		std::string queryCallsWhile = "procedure p; while w; Select p such that Calls*(w, p)";
		auto wrapperCallsWhile =
			[&queryCallsWhile] { PQLParser::parseQuery(queryCallsWhile); };
		Assert::ExpectException<QPSException>(wrapperCallsWhile);

		std::string queryCallsIf = "procedure p; if ifs; Select p such that Calls*(ifs, p)";
		auto wrapperCallsIf =
			[&queryCallsIf] { PQLParser::parseQuery(queryCallsIf); };
		Assert::ExpectException<QPSException>(wrapperCallsIf);

		std::string queryCallsAssign = "procedure p; assign a; Select p such that Calls*(a, p)";
		auto wrapperCallsAssign =
			[&queryCallsAssign] { PQLParser::parseQuery(queryCallsAssign); };
		Assert::ExpectException<QPSException>(wrapperCallsAssign);

		std::string queryCallsInteger = "procedure p; constant c; Select p such that Calls*(1, p)";
		auto wrapperCallsInteger =
			[&queryCallsInteger] { PQLParser::parseQuery(queryCallsInteger); };
		Assert::ExpectException<QPSException>(wrapperCallsInteger);
	}

	TEST_METHOD(parseQuery_callsTSecondArgNonProcSynonym_exceptionThrown) {
		std::string queryCallsVariable = "procedure p; variable v; Select p such that Calls*(p, v)";
		auto wrapperCallsVariable =
			[&queryCallsVariable] { PQLParser::parseQuery(queryCallsVariable); };
		Assert::ExpectException<QPSException>(wrapperCallsVariable);

		std::string queryCallsConst = "procedure p; constant c; Select p such that Calls*(p, c)";
		auto wrapperCallsConst =
			[&queryCallsConst] { PQLParser::parseQuery(queryCallsConst); };
		Assert::ExpectException<QPSException>(wrapperCallsConst);

		std::string queryCallsStmt = "procedure p; stmt s; Select p such that Calls*(p, s)";
		auto wrapperCallsStmt =
			[&queryCallsStmt] { PQLParser::parseQuery(queryCallsStmt); };
		Assert::ExpectException<QPSException>(wrapperCallsStmt);

		std::string queryCallsRead = "procedure p; read r; Select p such that Calls*(p, r)";
		auto wrapperCallsRead =
			[&queryCallsRead] { PQLParser::parseQuery(queryCallsRead); };
		Assert::ExpectException<QPSException>(wrapperCallsRead);

		std::string queryCallsPrint = "procedure p; print pn; Select p such that Calls*(p, pn)";
		auto wrapperCallsPrint =
			[&queryCallsPrint] { PQLParser::parseQuery(queryCallsPrint); };
		Assert::ExpectException<QPSException>(wrapperCallsPrint);

		std::string queryCallsCall = "procedure p; call c; Select p such that Calls*(p, c)";
		auto wrapperCallsCall =
			[&queryCallsCall] { PQLParser::parseQuery(queryCallsCall); };
		Assert::ExpectException<QPSException>(wrapperCallsCall);

		std::string queryCallsWhile = "procedure p; while w; Select p such that Calls*(p, w)";
		auto wrapperCallsWhile =
			[&queryCallsWhile] { PQLParser::parseQuery(queryCallsWhile); };
		Assert::ExpectException<QPSException>(wrapperCallsWhile);

		std::string queryCallsIf = "procedure p; if ifs; Select p such that Calls*(p, ifs)";
		auto wrapperCallsIf =
			[&queryCallsIf] { PQLParser::parseQuery(queryCallsIf); };
		Assert::ExpectException<QPSException>(wrapperCallsIf);

		std::string queryCallsAssign = "procedure p; assign a; Select p such that Calls*(p, a)";
		auto wrapperCallsAssign =
			[&queryCallsAssign] { PQLParser::parseQuery(queryCallsAssign); };
		Assert::ExpectException<QPSException>(wrapperCallsAssign);

		std::string queryCallsInteger = "procedure p; constant c; Select p such that Calls*(p, 1)";
		auto wrapperCallsInteger =
			[&queryCallsInteger] { PQLParser::parseQuery(queryCallsInteger); };
		Assert::ExpectException<QPSException>(wrapperCallsInteger);
	}

	TEST_METHOD(parseQuery_nextFirstArgNonStmtSynonym_exceptionThrown) {
		std::string queryNextVariable = "assign a; variable v; Select a such that Next(a, v)";
		auto wrapperNextVariable =
			[&queryNextVariable] { PQLParser::parseQuery(queryNextVariable); };
		Assert::ExpectException<QPSException>(wrapperNextVariable);

		std::string queryNextProc = "assign a; procedure p; Select a such that Next(a, p)";
		auto wrapperNextProc =
			[&queryNextProc] { PQLParser::parseQuery(queryNextProc); };
		Assert::ExpectException<QPSException>(wrapperNextProc);

		std::string queryNextConst = "assign a; constant c; Select a such that Next(a, c)";
		auto wrapperNextConst =
			[&queryNextConst] { PQLParser::parseQuery(queryNextConst); };
		Assert::ExpectException<QPSException>(wrapperNextConst);

		std::string queryNextVarName = "assign a; constant c; Select a such that Next(a, \"x\")";
		auto wrapperNextVarName =
			[&queryNextVarName] { PQLParser::parseQuery(queryNextVarName); };
		Assert::ExpectException<QPSException>(wrapperNextVarName);
	}

	TEST_METHOD(parseQuery_nextSecondArgNonStmtSynonym_exceptionThrown) {
		std::string queryNextVariable = "assign a; variable v; Select a such that Next(v, a)";
		auto wrapperNextVariable =
			[&queryNextVariable] { PQLParser::parseQuery(queryNextVariable); };
		Assert::ExpectException<QPSException>(wrapperNextVariable);

		std::string queryNextProc = "assign a; procedure p; Select a such that Next(p, a)";
		auto wrapperNextProc =
			[&queryNextProc] { PQLParser::parseQuery(queryNextProc); };
		Assert::ExpectException<QPSException>(wrapperNextProc);

		std::string queryNextConst = "assign a; constant c; Select a such that Next(c, a)";
		auto wrapperNextConst =
			[&queryNextConst] { PQLParser::parseQuery(queryNextConst); };
		Assert::ExpectException<QPSException>(wrapperNextConst);

		std::string queryNextVarName = "assign a; constant c; Select a such that Next(\"x\", a)";
		auto wrapperNextVarName =
			[&queryNextVarName] { PQLParser::parseQuery(queryNextVarName); };
		Assert::ExpectException<QPSException>(wrapperNextVarName);
	}

	TEST_METHOD(parseQuery_nextTFirstArgNonStmtSynonym_exceptionThrown) {
		std::string queryNextVariable = "assign a; variable v; Select a such that Next*(a, v)";
		auto wrapperNextVariable =
			[&queryNextVariable] { PQLParser::parseQuery(queryNextVariable); };
		Assert::ExpectException<QPSException>(wrapperNextVariable);

		std::string queryNextProc = "assign a; procedure p; Select a such that Next*(a, p)";
		auto wrapperNextProc =
			[&queryNextProc] { PQLParser::parseQuery(queryNextProc); };
		Assert::ExpectException<QPSException>(wrapperNextProc);

		std::string queryNextConst = "assign a; constant c; Select a such that Next*(a, c)";
		auto wrapperNextConst =
			[&queryNextConst] { PQLParser::parseQuery(queryNextConst); };
		Assert::ExpectException<QPSException>(wrapperNextConst);

		std::string queryNextVarName = "assign a; constant c; Select a such that Next*(a, \"x\")";
		auto wrapperNextVarName =
			[&queryNextVarName] { PQLParser::parseQuery(queryNextVarName); };
		Assert::ExpectException<QPSException>(wrapperNextVarName);
	}

	TEST_METHOD(parseQuery_nextTSecondArgNonStmtSynonym_exceptionThrown) {
		std::string queryNextVariable = "assign a; variable v; Select a such that Next*(v, a)";
		auto wrapperNextVariable =
			[&queryNextVariable] { PQLParser::parseQuery(queryNextVariable); };
		Assert::ExpectException<QPSException>(wrapperNextVariable);

		std::string queryNextProc = "assign a; procedure p; Select a such that Next*(p, a)";
		auto wrapperNextProc =
			[&queryNextProc] { PQLParser::parseQuery(queryNextProc); };
		Assert::ExpectException<QPSException>(wrapperNextProc);

		std::string queryNextConst = "assign a; constant c; Select a such that Next*(c, a)";
		auto wrapperNextConst =
			[&queryNextConst] { PQLParser::parseQuery(queryNextConst); };
		Assert::ExpectException<QPSException>(wrapperNextConst);

		std::string queryNextVarName = "assign a; constant c; Select a such that Next*(\"x\", a)";
		auto wrapperNextVarName =
			[&queryNextVarName] { PQLParser::parseQuery(queryNextVarName); };
		Assert::ExpectException<QPSException>(wrapperNextVarName);
	}

	TEST_METHOD(parseQuery_affectsFirstArgNonAssignSynonym_exceptionThrown) {
		std::string queryAffectsVariable = "assign a; variable v; Select a such that Affects(v, a)";
		auto wrapperAffectsVariable =
			[&queryAffectsVariable] { PQLParser::parseQuery(queryAffectsVariable); };
		Assert::ExpectException<QPSException>(wrapperAffectsVariable);

		std::string queryAffectsConst = "assign a; constant c; Select a such that Affects(c, a)";
		auto wrapperAffectsConst =
			[&queryAffectsConst] { PQLParser::parseQuery(queryAffectsConst); };
		Assert::ExpectException<QPSException>(wrapperAffectsConst);

		std::string queryAffectsRead = "assign a; read r; Select a such that Affects(r, a)";
		auto wrapperAffectsRead =
			[&queryAffectsRead] { PQLParser::parseQuery(queryAffectsRead); };
		Assert::ExpectException<QPSException>(wrapperAffectsRead);

		std::string queryAffectsPrint = "assign a; print pn; Select a such that Affects(pn, a)";
		auto wrapperAffectsPrint =
			[&queryAffectsPrint] { PQLParser::parseQuery(queryAffectsPrint); };
		Assert::ExpectException<QPSException>(wrapperAffectsPrint);

		std::string queryAffectsCall = "assign a; call c; Select a such that Affects(c, a)";
		auto wrapperAffectsCall =
			[&queryAffectsCall] { PQLParser::parseQuery(queryAffectsCall); };
		Assert::ExpectException<QPSException>(wrapperAffectsCall);

		std::string queryAffectsWhile = "assign a; while w; Select a such that Affects(w, a)";
		auto wrapperAffectsWhile =
			[&queryAffectsWhile] { PQLParser::parseQuery(queryAffectsWhile); };
		Assert::ExpectException<QPSException>(wrapperAffectsWhile);

		std::string queryAffectsIf = "assign a; if ifs; Select a such that Affects(ifs, a)";
		auto wrapperAffectsIf =
			[&queryAffectsIf] { PQLParser::parseQuery(queryAffectsIf); };
		Assert::ExpectException<QPSException>(wrapperAffectsIf);

		std::string queryAffectsAssign = "assign a; procedure p; Select a such that Affects(p, a)";
		auto wrapperAffectsAssign =
			[&queryAffectsAssign] { PQLParser::parseQuery(queryAffectsAssign); };
		Assert::ExpectException<QPSException>(wrapperAffectsAssign);

		std::string queryAffectsIdent = "assign a; constant c; Select a such that Affects(\"abc\", a)";
		auto wrapperAffectsIdent =
			[&queryAffectsIdent] { PQLParser::parseQuery(queryAffectsIdent); };
		Assert::ExpectException<QPSException>(wrapperAffectsIdent);
	}

	TEST_METHOD(parseQuery_affectsSecondArgNonProcSynonym_exceptionThrown) {
		std::string queryAffectsVariable = "assign a; variable v; Select a such that Affects(a, v)";
		auto wrapperAffectsVariable =
			[&queryAffectsVariable] { PQLParser::parseQuery(queryAffectsVariable); };
		Assert::ExpectException<QPSException>(wrapperAffectsVariable);

		std::string queryAffectsConst = "assign a; constant c; Select a such that Affects(a, c)";
		auto wrapperAffectsConst =
			[&queryAffectsConst] { PQLParser::parseQuery(queryAffectsConst); };
		Assert::ExpectException<QPSException>(wrapperAffectsConst);

		std::string queryAffectsRead = "assign a; read r; Select a such that Affects(a, r)";
		auto wrapperAffectsRead =
			[&queryAffectsRead] { PQLParser::parseQuery(queryAffectsRead); };
		Assert::ExpectException<QPSException>(wrapperAffectsRead);

		std::string queryAffectsPrint = "assign a; print pn; Select a such that Affects(a, pn)";
		auto wrapperAffectsPrint =
			[&queryAffectsPrint] { PQLParser::parseQuery(queryAffectsPrint); };
		Assert::ExpectException<QPSException>(wrapperAffectsPrint);

		std::string queryAffectsCall = "assign a; call c; Select a such that Affects(a, c)";
		auto wrapperAffectsCall =
			[&queryAffectsCall] { PQLParser::parseQuery(queryAffectsCall); };
		Assert::ExpectException<QPSException>(wrapperAffectsCall);

		std::string queryAffectsWhile = "assign a; while w; Select a such that Affects(a, w)";
		auto wrapperAffectsWhile =
			[&queryAffectsWhile] { PQLParser::parseQuery(queryAffectsWhile); };
		Assert::ExpectException<QPSException>(wrapperAffectsWhile);

		std::string queryAffectsIf = "assign a; if ifs; Select a such that Affects(a, ifs)";
		auto wrapperAffectsIf =
			[&queryAffectsIf] { PQLParser::parseQuery(queryAffectsIf); };
		Assert::ExpectException<QPSException>(wrapperAffectsIf);

		std::string queryAffectsAssign = "assign a; procedure p; Select a such that Affects(a, p)";
		auto wrapperAffectsAssign =
			[&queryAffectsAssign] { PQLParser::parseQuery(queryAffectsAssign); };
		Assert::ExpectException<QPSException>(wrapperAffectsAssign);

		std::string queryAffectsIdent = "assign a; constant c; Select a such that Affects(a, \"abc\")";
		auto wrapperAffectsIdent =
			[&queryAffectsIdent] { PQLParser::parseQuery(queryAffectsIdent); };
		Assert::ExpectException<QPSException>(wrapperAffectsIdent);
	}


	TEST_METHOD(parseQuery_affectsTFirstArgNonProcSynonym_exceptionThrown) {
		std::string queryAffectsVariable = "assign a; variable v; Select a such that Affects*(v, a)";
		auto wrapperAffectsVariable =
			[&queryAffectsVariable] { PQLParser::parseQuery(queryAffectsVariable); };
		Assert::ExpectException<QPSException>(wrapperAffectsVariable);

		std::string queryAffectsConst = "assign a; constant c; Select a such that Affects*(c, a)";
		auto wrapperAffectsConst =
			[&queryAffectsConst] { PQLParser::parseQuery(queryAffectsConst); };
		Assert::ExpectException<QPSException>(wrapperAffectsConst);

		std::string queryAffectsRead = "assign a; read r; Select a such that Affects*(r, a)";
		auto wrapperAffectsRead =
			[&queryAffectsRead] { PQLParser::parseQuery(queryAffectsRead); };
		Assert::ExpectException<QPSException>(wrapperAffectsRead);

		std::string queryAffectsPrint = "assign a; print pn; Select a such that Affects*(pn, a)";
		auto wrapperAffectsPrint =
			[&queryAffectsPrint] { PQLParser::parseQuery(queryAffectsPrint); };
		Assert::ExpectException<QPSException>(wrapperAffectsPrint);

		std::string queryAffectsCall = "assign a; call c; Select a such that Affects*(c, a)";
		auto wrapperAffectsCall =
			[&queryAffectsCall] { PQLParser::parseQuery(queryAffectsCall); };
		Assert::ExpectException<QPSException>(wrapperAffectsCall);

		std::string queryAffectsWhile = "assign a; while w; Select a such that Affects*(w, a)";
		auto wrapperAffectsWhile =
			[&queryAffectsWhile] { PQLParser::parseQuery(queryAffectsWhile); };
		Assert::ExpectException<QPSException>(wrapperAffectsWhile);

		std::string queryAffectsIf = "assign a; if ifs; Select a such that Affects*(ifs, a)";
		auto wrapperAffectsIf =
			[&queryAffectsIf] { PQLParser::parseQuery(queryAffectsIf); };
		Assert::ExpectException<QPSException>(wrapperAffectsIf);

		std::string queryAffectsAssign = "assign a; procedure p; Select a such that Affects*(p, a)";
		auto wrapperAffectsAssign =
			[&queryAffectsAssign] { PQLParser::parseQuery(queryAffectsAssign); };
		Assert::ExpectException<QPSException>(wrapperAffectsAssign);

		std::string queryAffectsIdent = "assign a; constant c; Select a such that Affects*(\"abc\", a)";
		auto wrapperAffectsIdent =
			[&queryAffectsIdent] { PQLParser::parseQuery(queryAffectsIdent); };
		Assert::ExpectException<QPSException>(wrapperAffectsIdent);
	}

	TEST_METHOD(parseQuery_affectsTSecondArgNonProcSynonym_exceptionThrown) {
		std::string queryAffectsVariable = "assign a; variable v; Select a such that Affects*(a, v)";
		auto wrapperAffectsVariable =
			[&queryAffectsVariable] { PQLParser::parseQuery(queryAffectsVariable); };
		Assert::ExpectException<QPSException>(wrapperAffectsVariable);

		std::string queryAffectsConst = "assign a; constant c; Select a such that Affects*(a, c)";
		auto wrapperAffectsConst =
			[&queryAffectsConst] { PQLParser::parseQuery(queryAffectsConst); };
		Assert::ExpectException<QPSException>(wrapperAffectsConst);

		std::string queryAffectsRead = "assign a; read r; Select a such that Affects*(a, r)";
		auto wrapperAffectsRead =
			[&queryAffectsRead] { PQLParser::parseQuery(queryAffectsRead); };
		Assert::ExpectException<QPSException>(wrapperAffectsRead);

		std::string queryAffectsPrint = "assign a; print pn; Select a such that Affects*(a, pn)";
		auto wrapperAffectsPrint =
			[&queryAffectsPrint] { PQLParser::parseQuery(queryAffectsPrint); };
		Assert::ExpectException<QPSException>(wrapperAffectsPrint);

		std::string queryAffectsCall = "assign a; call c; Select a such that Affects*(a, c)";
		auto wrapperAffectsCall =
			[&queryAffectsCall] { PQLParser::parseQuery(queryAffectsCall); };
		Assert::ExpectException<QPSException>(wrapperAffectsCall);

		std::string queryAffectsWhile = "assign a; while w; Select a such that Affects*(a, w)";
		auto wrapperAffectsWhile =
			[&queryAffectsWhile] { PQLParser::parseQuery(queryAffectsWhile); };
		Assert::ExpectException<QPSException>(wrapperAffectsWhile);

		std::string queryAffectsIf = "assign a; if ifs; Select a such that Affects*(a, ifs)";
		auto wrapperAffectsIf =
			[&queryAffectsIf] { PQLParser::parseQuery(queryAffectsIf); };
		Assert::ExpectException<QPSException>(wrapperAffectsIf);

		std::string queryAffectsAssign = "assign a; procedure p; Select a such that Affects*(a, p)";
		auto wrapperAffectsAssign =
			[&queryAffectsAssign] { PQLParser::parseQuery(queryAffectsAssign); };
		Assert::ExpectException<QPSException>(wrapperAffectsAssign);

		std::string queryAffectsIdent = "assign a; constant c; Select a such that Affects*(a, \"abc\")";
		auto wrapperAffectsIdent =
			[&queryAffectsIdent] { PQLParser::parseQuery(queryAffectsIdent); };
		Assert::ExpectException<QPSException>(wrapperAffectsIdent);
	}
	};
}
