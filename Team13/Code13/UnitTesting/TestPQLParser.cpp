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

		std::unordered_set<std::string> selects = parsedQuery.getColumns();
		Assert::IsTrue(selects.find("s") != selects.end());
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
			PqlRelationshipType::USES_S
			== relationships[0].getRelationshipType());
		Assert::IsTrue(
			PqlReferenceType::SYNONYM
			== relationships[0].getLhs().first);
		Assert::AreEqual(std::string("s"),
			relationships[0].getLhs().second);
		Assert::IsTrue(
			PqlReferenceType::SYNONYM
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
			PqlRelationshipType::FOLLOWS_T
			== relationships[0].getRelationshipType());
		Assert::IsTrue(
			PqlReferenceType::INTEGER
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
			PqlRelationshipType::MODIFIES_S
			== relationships[0].getRelationshipType());
		Assert::IsTrue(
			PqlReferenceType::IDENT
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
			PqlReferenceType::WILDCARD
			== patterns[0].getEntRef().first);
		Assert::AreEqual(std::string(), patterns[0].getEntRef().second);
		Assert::IsTrue(
			PqlExpressionType::WILDCARD
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
			PqlReferenceType::SYNONYM
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
			PqlExpressionType::PARTIAL
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
			PqlRelationshipType::PARENT_T
			== relationships[0].getRelationshipType());

		std::vector<ParsedPattern> patterns =
			parsedQuery.getPatterns();
		Assert::AreEqual(size_t(1), patterns.size());
		Assert::IsTrue(
			PqlExpressionType::PARTIAL
			== patterns[0].getExpression().first);
		Assert::AreEqual(std::string(" x "), patterns[0].getExpression().second);
	}

	TEST_METHOD(parseQuery_patternClausePartialExpr_exprExtracted) {
		std::string query = "assign a; Select a pattern a(_, _\"(x + 1)       * 2 / 3		-4 %\n (5)\"_)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedPattern> patterns =
			parsedQuery.getPatterns();
		Assert::AreEqual(size_t(1), patterns.size());
		Assert::IsTrue(PqlExpressionType::PARTIAL == patterns[0].getExpression().first);
		Assert::AreEqual(std::string(" x 1 + 2 * 3 / 4 5 % - "), patterns[0].getExpression().second);
	}

	TEST_METHOD(parseQuery_patternClauseFullExpr_exprExtracted) {
		std::string query = "assign a; Select a pattern a(_, \"(x + 1)       * 2 / 3		-4 %\n (5)\")";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedPattern> patterns =
			parsedQuery.getPatterns();
		Assert::AreEqual(size_t(1), patterns.size());
		Assert::IsTrue(PqlExpressionType::FULL == patterns[0].getExpression().first);
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
		Assert::IsTrue(PqlRelationshipType::MODIFIES_P == relationships[0].getRelationshipType());
	}

	TEST_METHOD(parseQuery_usesFirstArgIdent_correctlyParsedAsUsesP) {
		std::string query = "stmt s; variable v; Select s such that Uses(\"x\", _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::USES_P == relationships[0].getRelationshipType());
	}

	TEST_METHOD(parseQuery_usesFirstArgProcSynonym_correctlyParsedAsModifiesP) {
		std::string query = "stmt s; variable v; procedure p; Select s such that Modifies(p, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::MODIFIES_P == relationships[0].getRelationshipType());
	}

	TEST_METHOD(parseQuery_usesFirstArgProcSynonym_correctlyParsedAsUsesP) {
		std::string query = "stmt s; variable v; procedure p; Select s such that Uses(p, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::USES_P == relationships[0].getRelationshipType());
	}

	TEST_METHOD(parseQuery_callsClauseTwoSynonyms_bothSynonymsExtracted) {
		std::string query = "procedure p1, p2; Select p1 such that Calls(p1, p2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::CALLS == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("p1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("p2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_callsClauseTwoWildcards_bothWildcardsExtracted) {
		std::string query = "procedure p1, p2; Select p1 such that Calls(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::CALLS == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::WILDCARD == relationships[0].getLhs().first);
		Assert::IsTrue(PqlReferenceType::WILDCARD == relationships[0].getRhs().first);
	}

	TEST_METHOD(parseQuery_callsClauseTwoIdent_bothIdentsExtracted) {
		std::string query = "procedure p1, p2; Select p1 such that Calls(\"proc1\", \"proc2\")";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::CALLS == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::IDENT == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("proc1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::IDENT == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("proc2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_callsTClauseTwoSynonyms_bothSynonymsExtracted) {
		std::string query = "procedure p1, p2; Select p1 such that Calls*(p1, p2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::CALLS_T == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("p1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("p2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_callsTClauseTwoWildcards_bothWildcardsExtracted) {
		std::string query = "procedure p1, p2; Select p1 such that Calls*(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::CALLS_T == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::WILDCARD == relationships[0].getLhs().first);
		Assert::IsTrue(PqlReferenceType::WILDCARD == relationships[0].getRhs().first);
	}

	TEST_METHOD(parseQuery_callsTClauseTwoIdent_bothIdentsExtracted) {
		std::string query = "procedure p1, p2; Select p1 such that Calls*(\"proc1\", \"proc2\")";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::CALLS_T == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::IDENT == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("proc1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::IDENT == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("proc2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_nextClauseTwoSynonyms_bothSynonymsExtracted) {
		std::string query = "stmt s1, s2; Select s1 such that Next(s1, s2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::NEXT == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("s1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("s2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_nextClauseTwoWildcards_bothWildcardsExtracted) {
		std::string query = "stmt s1, s2; Select s1 such that Next(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::NEXT == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::WILDCARD == relationships[0].getLhs().first);
		Assert::IsTrue(PqlReferenceType::WILDCARD == relationships[0].getRhs().first);
	}

	TEST_METHOD(parseQuery_nextClauseTwoIntegers_bothStmtIndexesExtracted) {
		std::string query = "stmt s1, s2; Select s1 such that Next(1, 2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::NEXT == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::INTEGER == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::INTEGER == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_nextTClauseTwoSynonyms_bothSynonymsExtracted) {
		std::string query = "stmt s1, s2; Select s1 such that Next*(s1, s2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::NEXT_T == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("s1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("s2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_nextTClauseTwoWildcards_bothWildcardsExtracted) {
		std::string query = "stmt s1, s2; Select s1 such that Next*(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::NEXT_T == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::WILDCARD == relationships[0].getLhs().first);
		Assert::IsTrue(PqlReferenceType::WILDCARD == relationships[0].getRhs().first);
	}

	TEST_METHOD(parseQuery_nextTClauseTwoIntegers_bothStmtIndexesExtracted) {
		std::string query = "stmt s1, s2; Select s1 such that Next*(1, 2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::NEXT_T == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::INTEGER == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::INTEGER == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_affectsClauseTwoAssigns_bothSynonymsExtracted) {
		std::string query = "assign a1, a2; Select a1 such that Affects(a1, a2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::AFFECTS == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("a1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("a2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_affectsClauseTwoStmts_bothSynonymsExtracted) {
		std::string query = "stmt s1, s2; Select s1 such that Affects(s1, s2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::AFFECTS == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("s1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("s2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_affectsClauseTwoWildcards_bothWildcardsExtracted) {
		std::string query = "assign a1, a2; Select a1 such that Affects(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::AFFECTS == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::WILDCARD == relationships[0].getLhs().first);
		Assert::IsTrue(PqlReferenceType::WILDCARD == relationships[0].getRhs().first);
	}

	TEST_METHOD(parseQuery_affectsClauseTwoIntegers_bothStmtIndexesExtracted) {
		std::string query = "assign a1, a2; Select a1 such that Affects(1, 2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::AFFECTS == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::INTEGER == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::INTEGER == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_affectsTClauseTwoAssigns_bothSynonymsExtracted) {
		std::string query = "assign a1, a2; Select a1 such that Affects*(a1, a2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::AFFECTS_T == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("a1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("a2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_affectsTClauseTwoStmts_bothSynonymsExtracted) {
		std::string query = "stmt s1, s2; Select s1 such that Affects*(s1, s2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::AFFECTS_T == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("s1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getRhs().first);
		Assert::AreEqual(std::string("s2"), relationships[0].getRhs().second);
	}

	TEST_METHOD(parseQuery_affectsTClauseTwoWildcards_bothWildcardsExtracted) {
		std::string query = "assign a1, a2; Select a1 such that Affects*(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::AFFECTS_T == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::WILDCARD == relationships[0].getLhs().first);
		Assert::IsTrue(PqlReferenceType::WILDCARD == relationships[0].getRhs().first);
	}

	TEST_METHOD(parseQuery_affectsTClauseTwoIntegers_bothStmtIndexesExtracted) {
		std::string query = "assign a1, a2; Select a1 such that Affects*(1, 2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::AFFECTS_T == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::INTEGER == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("1"), relationships[0].getLhs().second);
		Assert::IsTrue(PqlReferenceType::INTEGER == relationships[0].getRhs().first);
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

	TEST_METHOD(parseQuery_clausesSuchThatSuchThat_allClausesExtracted) {
		std::string query = "stmt s; variable v; Select s such that Uses(s, v) and Modifies(s, v)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(2), relationships.size());
		Assert::IsTrue(PqlRelationshipType::USES_S == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlRelationshipType::MODIFIES_S == relationships[1].getRelationshipType());
	}

	TEST_METHOD(parseQuery_clausesSuchThatPattern_allClausesExtracted) {
		std::string query = "assign a; stmt s; Select a such that Follows(s, s) pattern a(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		std::vector<ParsedPattern> patterns = parsedQuery.getPatterns();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::AreEqual(size_t(1), patterns.size());
		Assert::IsTrue(PqlRelationshipType::FOLLOWS == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlPatternType::PATTERN_A == patterns[0].getPatternType());
	}

	TEST_METHOD(parseQuery_clausesSuchThatWith_allClausesExtracted) {
		std::string query = "stmt s; constant c; Select s such that Follows*(s, s) with s.stmt# = c.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::AreEqual(size_t(1), withs.size());
		Assert::IsTrue(PqlRelationshipType::FOLLOWS_T == relationships[0].getRelationshipType());
	}

	TEST_METHOD(parseQuery_clausesPatternSuchThat_allClausesExtracted) {
		std::string query = "if ifs; Select ifs pattern ifs(_, _, _) such that Parent(ifs, ifs)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		std::vector<ParsedPattern> patterns = parsedQuery.getPatterns();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::AreEqual(size_t(1), patterns.size());
		Assert::IsTrue(PqlRelationshipType::PARENT == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlPatternType::PATTERN_I == patterns[0].getPatternType());
	}

	TEST_METHOD(parseQuery_clausesPatternPattern_allClausesExtracted) {
		std::string query = "assign a; while w; Select a pattern a(_, _) and w(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedPattern> patterns = parsedQuery.getPatterns();
		Assert::AreEqual(size_t(2), patterns.size());
		Assert::IsTrue(PqlPatternType::PATTERN_A == patterns[0].getPatternType());
		Assert::IsTrue(PqlPatternType::PATTERN_W == patterns[1].getPatternType());
	}

	TEST_METHOD(parseQuery_clausesPatternWith_allClausesExtracted) {
		std::string query = "if ifs; Select ifs pattern ifs(_, _, _) with ifs.stmt# = ifs.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedPattern> patterns = parsedQuery.getPatterns();
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), patterns.size());
		Assert::AreEqual(size_t(1), withs.size());
		Assert::IsTrue(PqlPatternType::PATTERN_I == patterns[0].getPatternType());
	}

	TEST_METHOD(parseQuery_clausesWithSuchThat_allClausesExtracted) {
		std::string query = "procedure p; Select p with \"proc\"= p.procName such that Calls(p, p)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::AreEqual(size_t(1), withs.size());
		Assert::IsTrue(PqlRelationshipType::CALLS == relationships[0].getRelationshipType());
	}

	TEST_METHOD(parseQuery_clausesWithPattern_allClausesExtracted) {
		std::string query = "assign a; Select a with a.stmt# = a.stmt# pattern a(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedPattern> patterns = parsedQuery.getPatterns();
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), patterns.size());
		Assert::AreEqual(size_t(1), withs.size());
		Assert::IsTrue(PqlPatternType::PATTERN_A == patterns[0].getPatternType());
	}

	TEST_METHOD(parseQuery_clausesWithWith_allClausesExtracted) {
		std::string query = "print pn; read r; procedure p; Select p with p.procName = pn.varName and r.varName = p.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(2), withs.size());
	}

	TEST_METHOD(parseQuery_patternAInvalidNumOfArgs_exceptionThrown) {
		std::string query = "assign a; Select a pattern a(_, _, _)";
		auto wrapperFunc =
			[&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_patternWInvalidNumOfArgs_exceptionThrown) {
		std::string query = "while w; Select w pattern w(_, _, _)";
		auto wrapperFunc =
			[&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_patternIInvalidNumOfArgs_exceptionThrown) {
		std::string query = "if ifs; Select ifs pattern ifs(_, _)";
		auto wrapperFunc =
			[&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_patternInvalidDesignEntities_exceptionThrown) {
		std::string queryStmtTwoArgs = "stmt s; Select s pattern s(_, _)";
		auto wrapperStmtTwoArgs =
			[&queryStmtTwoArgs] { PQLParser::parseQuery(queryStmtTwoArgs); };
		Assert::ExpectException<QPSException>(wrapperStmtTwoArgs);

		std::string queryStmtThreeArgs = "stmt s; Select s pattern s(_, _)";
		auto wrapperStmtThreeArgs =
			[&queryStmtThreeArgs] { PQLParser::parseQuery(queryStmtThreeArgs); };
		Assert::ExpectException<QPSException>(wrapperStmtThreeArgs);

		std::string queryReadTwoArgs = "read r; Select r pattern r(_, _)";
		auto wrapperReadTwoArgs =
			[&queryReadTwoArgs] { PQLParser::parseQuery(queryReadTwoArgs); };
		Assert::ExpectException<QPSException>(wrapperReadTwoArgs);

		std::string queryReadThreeArgs = "read r; Select r pattern r(_, _)";
		auto wrapperReadThreeArgs =
			[&queryReadThreeArgs] { PQLParser::parseQuery(queryReadThreeArgs); };
		Assert::ExpectException<QPSException>(wrapperReadThreeArgs);

		std::string queryPrintTwoArgs = "print p; Select p pattern p(_, _)";
		auto wrapperPrintTwoArgs =
			[&queryPrintTwoArgs] { PQLParser::parseQuery(queryPrintTwoArgs); };
		Assert::ExpectException<QPSException>(wrapperPrintTwoArgs);

		std::string queryPrintThreeArgs = "print p; Select p pattern p(_, _)";
		auto wrapperPrintThreeArgs =
			[&queryPrintThreeArgs] { PQLParser::parseQuery(queryPrintThreeArgs); };
		Assert::ExpectException<QPSException>(wrapperPrintThreeArgs);

		std::string queryCallTwoArgs = "call c; Select c pattern c(_, _)";
		auto wrapperCallTwoArgs =
			[&queryCallTwoArgs] { PQLParser::parseQuery(queryCallTwoArgs); };
		Assert::ExpectException<QPSException>(wrapperCallTwoArgs);

		std::string queryCallThreeArgs = "call c; Select c pattern c(_, _)";
		auto wrapperCallThreeArgs =
			[&queryCallThreeArgs] { PQLParser::parseQuery(queryCallThreeArgs); };
		Assert::ExpectException<QPSException>(wrapperCallThreeArgs);

		std::string queryVariableTwoArgs = "variable v; Select v pattern v(_, _)";
		auto wrapperVariableTwoArgs =
			[&queryVariableTwoArgs] { PQLParser::parseQuery(queryVariableTwoArgs); };
		Assert::ExpectException<QPSException>(wrapperVariableTwoArgs);

		std::string queryVariableThreeArgs = "variable v; Select v pattern v(_, _)";
		auto wrapperVariableThreeArgs =
			[&queryVariableThreeArgs] { PQLParser::parseQuery(queryVariableThreeArgs); };
		Assert::ExpectException<QPSException>(wrapperVariableThreeArgs);

		std::string queryConstantTwoArgs = "constant c; Select c pattern c(_, _)";
		auto wrapperConstantTwoArgs =
			[&queryConstantTwoArgs] { PQLParser::parseQuery(queryConstantTwoArgs); };
		Assert::ExpectException<QPSException>(wrapperConstantTwoArgs);

		std::string queryConstantThreeArgs = "constant c; Select c pattern c(_, _)";
		auto wrapperConstantThreeArgs =
			[&queryConstantThreeArgs] { PQLParser::parseQuery(queryConstantThreeArgs); };
		Assert::ExpectException<QPSException>(wrapperConstantThreeArgs);

		std::string queryProcedureTwoArgs = "procedure p; Select p pattern p(_, _)";
		auto wrapperProcedureTwoArgs =
			[&queryProcedureTwoArgs] { PQLParser::parseQuery(queryProcedureTwoArgs); };
		Assert::ExpectException<QPSException>(wrapperProcedureTwoArgs);

		std::string queryProcedureThreeArgs = "procedure p; Select p pattern p(_, _)";
		auto wrapperProcedureThreeArgs =
			[&queryProcedureThreeArgs] { PQLParser::parseQuery(queryProcedureThreeArgs); };
		Assert::ExpectException<QPSException>(wrapperProcedureThreeArgs);
	}

	TEST_METHOD(parseQuery_suchThatTrailingAnd_exceptionThrown) {
		std::string query = "stmt s; Select s such that Modifies(s, _) and";
		auto wrapperFunc =
			[&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_patternTrailingAnd_exceptionThrown) {
		std::string query = "assign a; Select a pattern a(_, _) and";
		auto wrapperFunc =
			[&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_withTrailingAnd_exceptionThrown) {
		std::string query = "procedure p; Select p with p.procName = \"hello\" and";
		auto wrapperFunc =
			[&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_suchThatLeadingAnd_exceptionThrown) {
		std::string query = "stmt s; Select s and Uses(s, _)";
		auto wrapperFunc =
			[&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_patternLeadingAnd_exceptionThrown) {
		std::string query = "if ifs; Select ifs and ifs(_, _)";
		auto wrapperFunc =
			[&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_withLeadingAnd_exceptionThrown) {
		std::string query = "print pn; Select pn and pn.stmt# = 10";
		auto wrapperFunc =
			[&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_withInvalidAttributes_exceptionThrown) {
		std::string queryStmtProcName = "stmt s; Select s with s.procName = s.procName";
		auto wrapperStmtProcName =
			[&queryStmtProcName] { PQLParser::parseQuery(queryStmtProcName); };
		Assert::ExpectException<QPSException>(wrapperStmtProcName);

		std::string queryStmtVarName = "stmt s; Select s with s.varName = s.varName";
		auto wrapperStmtVarName =
			[&queryStmtVarName] { PQLParser::parseQuery(queryStmtVarName); };
		Assert::ExpectException<QPSException>(wrapperStmtVarName);

		std::string queryStmtValue = "stmt s; Select s with s.value = s.value";
		auto wrapperStmtValue =
			[&queryStmtValue] { PQLParser::parseQuery(queryStmtValue); };
		Assert::ExpectException<QPSException>(wrapperStmtValue);

		std::string queryReadProcName = "read r; Select r with r.procName = r.procName";
		auto wrapperReadProcName =
			[&queryReadProcName] { PQLParser::parseQuery(queryReadProcName); };
		Assert::ExpectException<QPSException>(wrapperReadProcName);

		std::string queryReadValue = "read r; Select r with r.value = r.value";
		auto wrapperReadValue =
			[&queryReadValue] { PQLParser::parseQuery(queryReadValue); };
		Assert::ExpectException<QPSException>(wrapperReadValue);

		std::string queryPrintProcName = "print p; Select p with p.procName = p.procName";
		auto wrapperPrintProcName =
			[&queryPrintProcName] { PQLParser::parseQuery(queryPrintProcName); };
		Assert::ExpectException<QPSException>(wrapperPrintProcName);

		std::string queryPrintValue = "print p; Select p with p.value = p.value";
		auto wrapperPrintValue =
			[&queryPrintValue] { PQLParser::parseQuery(queryPrintValue); };
		Assert::ExpectException<QPSException>(wrapperPrintValue);

		std::string queryCallVarName = "call c; Select c with c.varName = c.varName";
		auto wrapperCallVarName =
			[&queryCallVarName] { PQLParser::parseQuery(queryCallVarName); };
		Assert::ExpectException<QPSException>(wrapperCallVarName);

		std::string queryCallValue = "call c; Select c with c.value = c.value";
		auto wrapperCallValue =
			[&queryCallValue] { PQLParser::parseQuery(queryCallValue); };
		Assert::ExpectException<QPSException>(wrapperCallValue);

		std::string queryVariableProcName = "variable v; Select v with v.procName = v.procName";
		auto wrapperVariableProcName =
			[&queryVariableProcName] { PQLParser::parseQuery(queryVariableProcName); };
		Assert::ExpectException<QPSException>(wrapperVariableProcName);

		std::string queryVariableValue = "variable v; Select v with v.value = v.value";
		auto wrapperVariableValue =
			[&queryVariableValue] { PQLParser::parseQuery(queryVariableValue); };
		Assert::ExpectException<QPSException>(wrapperVariableValue);

		std::string queryVariableStmt = "variable v; Select v with v.stmt# = v.stmt#";
		auto wrapperVariableStmt =
			[&queryVariableStmt] { PQLParser::parseQuery(queryVariableStmt); };
		Assert::ExpectException<QPSException>(wrapperVariableStmt);

		std::string queryConstantProcName = "constant c; Select c with c.procName = c.procName";
		auto wrapperConstantProcName =
			[&queryConstantProcName] { PQLParser::parseQuery(queryConstantProcName); };
		Assert::ExpectException<QPSException>(wrapperConstantProcName);

		std::string queryConstantVarName = "constant c; Select c with c.varName = c.varName";
		auto wrapperConstantVarName =
			[&queryConstantVarName] { PQLParser::parseQuery(queryConstantVarName); };
		Assert::ExpectException<QPSException>(wrapperConstantVarName);

		std::string queryConstantStmt = "constant c; Select c with c.stmt# = c.stmt#";
		auto wrapperConstantStmt =
			[&queryConstantStmt] { PQLParser::parseQuery(queryConstantStmt); };
		Assert::ExpectException<QPSException>(wrapperConstantStmt);

		std::string queryProcedureVarName = "procedure p; Select p with p.varName = p.varName";
		auto wrapperProcedureVarName =
			[&queryProcedureVarName] { PQLParser::parseQuery(queryProcedureVarName); };
		Assert::ExpectException<QPSException>(wrapperProcedureVarName);

		std::string queryProcedureValue = "procedure p; Select p with p.value = p.value";
		auto wrapperProcedureValue =
			[&queryProcedureValue] { PQLParser::parseQuery(queryProcedureValue); };
		Assert::ExpectException<QPSException>(wrapperProcedureValue);

		std::string queryProcedureStmt = "procedure p; Select p with p.stmt# = p.stmt#";
		auto wrapperProcedureStmt =
			[&queryProcedureStmt] { PQLParser::parseQuery(queryProcedureStmt); };
		Assert::ExpectException<QPSException>(wrapperProcedureStmt);

		std::string queryIfProcName = "if i; Select i with i.procName = i.procName";
		auto wrapperIfProcName =
			[&queryIfProcName] { PQLParser::parseQuery(queryIfProcName); };
		Assert::ExpectException<QPSException>(wrapperIfProcName);

		std::string queryIfVarName = "if i; Select i with i.varName = i.varName";
		auto wrapperIfVarName =
			[&queryIfVarName] { PQLParser::parseQuery(queryIfVarName); };
		Assert::ExpectException<QPSException>(wrapperIfVarName);

		std::string queryIfValue = "if i; Select i with i.value = i.value";
		auto wrapperIfValue =
			[&queryIfValue] { PQLParser::parseQuery(queryIfValue); };
		Assert::ExpectException<QPSException>(wrapperIfValue);

		std::string queryWhileProcName = "while w; Select w with w.procName = w.procName";
		auto wrapperWhileProcName =
			[&queryWhileProcName] { PQLParser::parseQuery(queryWhileProcName); };
		Assert::ExpectException<QPSException>(wrapperWhileProcName);

		std::string queryWhileVarName = "while w; Select w with w.varName = w.varName";
		auto wrapperWhileVarName =
			[&queryWhileVarName] { PQLParser::parseQuery(queryWhileVarName); };
		Assert::ExpectException<QPSException>(wrapperWhileVarName);

		std::string queryWhileValue = "while w; Select w with w.value = w.value";
		auto wrapperWhileValue =
			[&queryWhileValue] { PQLParser::parseQuery(queryWhileValue); };
		Assert::ExpectException<QPSException>(wrapperWhileValue);

		std::string queryAssignProcName = "assign a; Select a with a.procName = a.procName";
		auto wrapperAssignProcName =
			[&queryAssignProcName] { PQLParser::parseQuery(queryAssignProcName); };
		Assert::ExpectException<QPSException>(wrapperAssignProcName);

		std::string queryAssignVarName = "assign a; Select a with a.varName = a.varName";
		auto wrapperAssignVarName =
			[&queryAssignVarName] { PQLParser::parseQuery(queryAssignVarName); };
		Assert::ExpectException<QPSException>(wrapperAssignVarName);

		std::string queryAssignValue = "assign a; Select a with a.value = a.value";
		auto wrapperAssignValue =
			[&queryAssignValue] { PQLParser::parseQuery(queryAssignValue); };
		Assert::ExpectException<QPSException>(wrapperAssignValue);
	}

	TEST_METHOD(parseQuery_withIncompatibleAttributes_exceptionThrown) {
		std::string queryConstantValueProcedureProcName = "constant c1; procedure p2; Select c1 with c1.value = p2.procName";
		auto wrapperConstantValueProcedureProcName =
			[&queryConstantValueProcedureProcName] { PQLParser::parseQuery(queryConstantValueProcedureProcName); };
		Assert::ExpectException<QPSException>(wrapperConstantValueProcedureProcName);

		std::string queryProcedureProcNameConstantValue = "procedure p1; constant c2; Select p1 with p1.procName = c2.value";
		auto wrapperProcedureProcNameConstantValue =
			[&queryProcedureProcNameConstantValue] { PQLParser::parseQuery(queryProcedureProcNameConstantValue); };
		Assert::ExpectException<QPSException>(wrapperProcedureProcNameConstantValue);

		std::string queryConstantValueCallProcName = "constant c1; call c2; Select c1 with c1.value = c2.procName";
		auto wrapperConstantValueCallProcName =
			[&queryConstantValueCallProcName] { PQLParser::parseQuery(queryConstantValueCallProcName); };
		Assert::ExpectException<QPSException>(wrapperConstantValueCallProcName);

		std::string queryCallProcNameConstantValue = "call c1; constant c2; Select c1 with c1.procName = c2.value";
		auto wrapperCallProcNameConstantValue =
			[&queryCallProcNameConstantValue] { PQLParser::parseQuery(queryCallProcNameConstantValue); };
		Assert::ExpectException<QPSException>(wrapperCallProcNameConstantValue);

		std::string queryConstantValueVariableVarName = "constant c1; variable v2; Select c1 with c1.value = v2.varName";
		auto wrapperConstantValueVariableVarName =
			[&queryConstantValueVariableVarName] { PQLParser::parseQuery(queryConstantValueVariableVarName); };
		Assert::ExpectException<QPSException>(wrapperConstantValueVariableVarName);

		std::string queryVariableVarNameConstantValue = "variable v1; constant c2; Select v1 with v1.varName = c2.value";
		auto wrapperVariableVarNameConstantValue =
			[&queryVariableVarNameConstantValue] { PQLParser::parseQuery(queryVariableVarNameConstantValue); };
		Assert::ExpectException<QPSException>(wrapperVariableVarNameConstantValue);

		std::string queryConstantValueReadVarName = "constant c1; read r2; Select c1 with c1.value = r2.varName";
		auto wrapperConstantValueReadVarName =
			[&queryConstantValueReadVarName] { PQLParser::parseQuery(queryConstantValueReadVarName); };
		Assert::ExpectException<QPSException>(wrapperConstantValueReadVarName);

		std::string queryReadVarNameConstantValue = "read r1; constant c2; Select r1 with r1.varName = c2.value";
		auto wrapperReadVarNameConstantValue =
			[&queryReadVarNameConstantValue] { PQLParser::parseQuery(queryReadVarNameConstantValue); };
		Assert::ExpectException<QPSException>(wrapperReadVarNameConstantValue);

		std::string queryConstantValuePrintVarName = "constant c1; print p2; Select c1 with c1.value = p2.varName";
		auto wrapperConstantValuePrintVarName =
			[&queryConstantValuePrintVarName] { PQLParser::parseQuery(queryConstantValuePrintVarName); };
		Assert::ExpectException<QPSException>(wrapperConstantValuePrintVarName);

		std::string queryPrintVarNameConstantValue = "print p1; constant c2; Select p1 with p1.varName = c2.value";
		auto wrapperPrintVarNameConstantValue =
			[&queryPrintVarNameConstantValue] { PQLParser::parseQuery(queryPrintVarNameConstantValue); };
		Assert::ExpectException<QPSException>(wrapperPrintVarNameConstantValue);

		std::string queryStmtStmtProcedureProcName = "stmt s1; procedure p2; Select s1 with s1.stmt# = p2.procName";
		auto wrapperStmtStmtProcedureProcName =
			[&queryStmtStmtProcedureProcName] { PQLParser::parseQuery(queryStmtStmtProcedureProcName); };
		Assert::ExpectException<QPSException>(wrapperStmtStmtProcedureProcName);

		std::string queryProcedureProcNameStmtStmt = "procedure p1; stmt s2; Select p1 with p1.procName = s2.stmt#";
		auto wrapperProcedureProcNameStmtStmt =
			[&queryProcedureProcNameStmtStmt] { PQLParser::parseQuery(queryProcedureProcNameStmtStmt); };
		Assert::ExpectException<QPSException>(wrapperProcedureProcNameStmtStmt);

		std::string queryStmtStmtCallProcName = "stmt s1; call c2; Select s1 with s1.stmt# = c2.procName";
		auto wrapperStmtStmtCallProcName =
			[&queryStmtStmtCallProcName] { PQLParser::parseQuery(queryStmtStmtCallProcName); };
		Assert::ExpectException<QPSException>(wrapperStmtStmtCallProcName);

		std::string queryCallProcNameStmtStmt = "call c1; stmt s2; Select c1 with c1.procName = s2.stmt#";
		auto wrapperCallProcNameStmtStmt =
			[&queryCallProcNameStmtStmt] { PQLParser::parseQuery(queryCallProcNameStmtStmt); };
		Assert::ExpectException<QPSException>(wrapperCallProcNameStmtStmt);

		std::string queryStmtStmtVariableVarName = "stmt s1; variable v2; Select s1 with s1.stmt# = v2.varName";
		auto wrapperStmtStmtVariableVarName =
			[&queryStmtStmtVariableVarName] { PQLParser::parseQuery(queryStmtStmtVariableVarName); };
		Assert::ExpectException<QPSException>(wrapperStmtStmtVariableVarName);

		std::string queryVariableVarNameStmtStmt = "variable v1; stmt s2; Select v1 with v1.varName = s2.stmt#";
		auto wrapperVariableVarNameStmtStmt =
			[&queryVariableVarNameStmtStmt] { PQLParser::parseQuery(queryVariableVarNameStmtStmt); };
		Assert::ExpectException<QPSException>(wrapperVariableVarNameStmtStmt);

		std::string queryStmtStmtReadVarName = "stmt s1; read r2; Select s1 with s1.stmt# = r2.varName";
		auto wrapperStmtStmtReadVarName =
			[&queryStmtStmtReadVarName] { PQLParser::parseQuery(queryStmtStmtReadVarName); };
		Assert::ExpectException<QPSException>(wrapperStmtStmtReadVarName);

		std::string queryReadVarNameStmtStmt = "read r1; stmt s2; Select r1 with r1.varName = s2.stmt#";
		auto wrapperReadVarNameStmtStmt =
			[&queryReadVarNameStmtStmt] { PQLParser::parseQuery(queryReadVarNameStmtStmt); };
		Assert::ExpectException<QPSException>(wrapperReadVarNameStmtStmt);

		std::string queryStmtStmtPrintVarName = "stmt s1; print p2; Select s1 with s1.stmt# = p2.varName";
		auto wrapperStmtStmtPrintVarName =
			[&queryStmtStmtPrintVarName] { PQLParser::parseQuery(queryStmtStmtPrintVarName); };
		Assert::ExpectException<QPSException>(wrapperStmtStmtPrintVarName);

		std::string queryPrintVarNameStmtStmt = "print p1; stmt s2; Select p1 with p1.varName = s2.stmt#";
		auto wrapperPrintVarNameStmtStmt =
			[&queryPrintVarNameStmtStmt] { PQLParser::parseQuery(queryPrintVarNameStmtStmt); };
		Assert::ExpectException<QPSException>(wrapperPrintVarNameStmtStmt);

		std::string queryReadStmtProcedureProcName = "read r1; procedure p2; Select r1 with r1.stmt# = p2.procName";
		auto wrapperReadStmtProcedureProcName =
			[&queryReadStmtProcedureProcName] { PQLParser::parseQuery(queryReadStmtProcedureProcName); };
		Assert::ExpectException<QPSException>(wrapperReadStmtProcedureProcName);

		std::string queryProcedureProcNameReadStmt = "procedure p1; read r2; Select p1 with p1.procName = r2.stmt#";
		auto wrapperProcedureProcNameReadStmt =
			[&queryProcedureProcNameReadStmt] { PQLParser::parseQuery(queryProcedureProcNameReadStmt); };
		Assert::ExpectException<QPSException>(wrapperProcedureProcNameReadStmt);

		std::string queryReadStmtCallProcName = "read r1; call c2; Select r1 with r1.stmt# = c2.procName";
		auto wrapperReadStmtCallProcName =
			[&queryReadStmtCallProcName] { PQLParser::parseQuery(queryReadStmtCallProcName); };
		Assert::ExpectException<QPSException>(wrapperReadStmtCallProcName);

		std::string queryCallProcNameReadStmt = "call c1; read r2; Select c1 with c1.procName = r2.stmt#";
		auto wrapperCallProcNameReadStmt =
			[&queryCallProcNameReadStmt] { PQLParser::parseQuery(queryCallProcNameReadStmt); };
		Assert::ExpectException<QPSException>(wrapperCallProcNameReadStmt);

		std::string queryReadStmtVariableVarName = "read r1; variable v2; Select r1 with r1.stmt# = v2.varName";
		auto wrapperReadStmtVariableVarName =
			[&queryReadStmtVariableVarName] { PQLParser::parseQuery(queryReadStmtVariableVarName); };
		Assert::ExpectException<QPSException>(wrapperReadStmtVariableVarName);

		std::string queryVariableVarNameReadStmt = "variable v1; read r2; Select v1 with v1.varName = r2.stmt#";
		auto wrapperVariableVarNameReadStmt =
			[&queryVariableVarNameReadStmt] { PQLParser::parseQuery(queryVariableVarNameReadStmt); };
		Assert::ExpectException<QPSException>(wrapperVariableVarNameReadStmt);

		std::string queryReadStmtReadVarName = "read r1; read r2; Select r1 with r1.stmt# = r2.varName";
		auto wrapperReadStmtReadVarName =
			[&queryReadStmtReadVarName] { PQLParser::parseQuery(queryReadStmtReadVarName); };
		Assert::ExpectException<QPSException>(wrapperReadStmtReadVarName);

		std::string queryReadVarNameReadStmt = "read r1; read r2; Select r1 with r1.varName = r2.stmt#";
		auto wrapperReadVarNameReadStmt =
			[&queryReadVarNameReadStmt] { PQLParser::parseQuery(queryReadVarNameReadStmt); };
		Assert::ExpectException<QPSException>(wrapperReadVarNameReadStmt);

		std::string queryReadStmtPrintVarName = "read r1; print p2; Select r1 with r1.stmt# = p2.varName";
		auto wrapperReadStmtPrintVarName =
			[&queryReadStmtPrintVarName] { PQLParser::parseQuery(queryReadStmtPrintVarName); };
		Assert::ExpectException<QPSException>(wrapperReadStmtPrintVarName);

		std::string queryPrintVarNameReadStmt = "print p1; read r2; Select p1 with p1.varName = r2.stmt#";
		auto wrapperPrintVarNameReadStmt =
			[&queryPrintVarNameReadStmt] { PQLParser::parseQuery(queryPrintVarNameReadStmt); };
		Assert::ExpectException<QPSException>(wrapperPrintVarNameReadStmt);

		std::string queryPrintStmtProcedureProcName = "print p1; procedure p2; Select p1 with p1.stmt# = p2.procName";
		auto wrapperPrintStmtProcedureProcName =
			[&queryPrintStmtProcedureProcName] { PQLParser::parseQuery(queryPrintStmtProcedureProcName); };
		Assert::ExpectException<QPSException>(wrapperPrintStmtProcedureProcName);

		std::string queryProcedureProcNamePrintStmt = "procedure p1; print p2; Select p1 with p1.procName = p2.stmt#";
		auto wrapperProcedureProcNamePrintStmt =
			[&queryProcedureProcNamePrintStmt] { PQLParser::parseQuery(queryProcedureProcNamePrintStmt); };
		Assert::ExpectException<QPSException>(wrapperProcedureProcNamePrintStmt);

		std::string queryPrintStmtCallProcName = "print p1; call c2; Select p1 with p1.stmt# = c2.procName";
		auto wrapperPrintStmtCallProcName =
			[&queryPrintStmtCallProcName] { PQLParser::parseQuery(queryPrintStmtCallProcName); };
		Assert::ExpectException<QPSException>(wrapperPrintStmtCallProcName);

		std::string queryCallProcNamePrintStmt = "call c1; print p2; Select c1 with c1.procName = p2.stmt#";
		auto wrapperCallProcNamePrintStmt =
			[&queryCallProcNamePrintStmt] { PQLParser::parseQuery(queryCallProcNamePrintStmt); };
		Assert::ExpectException<QPSException>(wrapperCallProcNamePrintStmt);

		std::string queryPrintStmtVariableVarName = "print p1; variable v2; Select p1 with p1.stmt# = v2.varName";
		auto wrapperPrintStmtVariableVarName =
			[&queryPrintStmtVariableVarName] { PQLParser::parseQuery(queryPrintStmtVariableVarName); };
		Assert::ExpectException<QPSException>(wrapperPrintStmtVariableVarName);

		std::string queryVariableVarNamePrintStmt = "variable v1; print p2; Select v1 with v1.varName = p2.stmt#";
		auto wrapperVariableVarNamePrintStmt =
			[&queryVariableVarNamePrintStmt] { PQLParser::parseQuery(queryVariableVarNamePrintStmt); };
		Assert::ExpectException<QPSException>(wrapperVariableVarNamePrintStmt);

		std::string queryPrintStmtReadVarName = "print p1; read r2; Select p1 with p1.stmt# = r2.varName";
		auto wrapperPrintStmtReadVarName =
			[&queryPrintStmtReadVarName] { PQLParser::parseQuery(queryPrintStmtReadVarName); };
		Assert::ExpectException<QPSException>(wrapperPrintStmtReadVarName);

		std::string queryReadVarNamePrintStmt = "read r1; print p2; Select r1 with r1.varName = p2.stmt#";
		auto wrapperReadVarNamePrintStmt =
			[&queryReadVarNamePrintStmt] { PQLParser::parseQuery(queryReadVarNamePrintStmt); };
		Assert::ExpectException<QPSException>(wrapperReadVarNamePrintStmt);

		std::string queryPrintStmtPrintVarName = "print p1; print p2; Select p1 with p1.stmt# = p2.varName";
		auto wrapperPrintStmtPrintVarName =
			[&queryPrintStmtPrintVarName] { PQLParser::parseQuery(queryPrintStmtPrintVarName); };
		Assert::ExpectException<QPSException>(wrapperPrintStmtPrintVarName);

		std::string queryPrintVarNamePrintStmt = "print p1; print p2; Select p1 with p1.varName = p2.stmt#";
		auto wrapperPrintVarNamePrintStmt =
			[&queryPrintVarNamePrintStmt] { PQLParser::parseQuery(queryPrintVarNamePrintStmt); };
		Assert::ExpectException<QPSException>(wrapperPrintVarNamePrintStmt);

		std::string queryCallStmtProcedureProcName = "call c1; procedure p2; Select c1 with c1.stmt# = p2.procName";
		auto wrapperCallStmtProcedureProcName =
			[&queryCallStmtProcedureProcName] { PQLParser::parseQuery(queryCallStmtProcedureProcName); };
		Assert::ExpectException<QPSException>(wrapperCallStmtProcedureProcName);

		std::string queryProcedureProcNameCallStmt = "procedure p1; call c2; Select p1 with p1.procName = c2.stmt#";
		auto wrapperProcedureProcNameCallStmt =
			[&queryProcedureProcNameCallStmt] { PQLParser::parseQuery(queryProcedureProcNameCallStmt); };
		Assert::ExpectException<QPSException>(wrapperProcedureProcNameCallStmt);

		std::string queryCallStmtCallProcName = "call c1; call c2; Select c1 with c1.stmt# = c2.procName";
		auto wrapperCallStmtCallProcName =
			[&queryCallStmtCallProcName] { PQLParser::parseQuery(queryCallStmtCallProcName); };
		Assert::ExpectException<QPSException>(wrapperCallStmtCallProcName);

		std::string queryCallProcNameCallStmt = "call c1; call c2; Select c1 with c1.procName = c2.stmt#";
		auto wrapperCallProcNameCallStmt =
			[&queryCallProcNameCallStmt] { PQLParser::parseQuery(queryCallProcNameCallStmt); };
		Assert::ExpectException<QPSException>(wrapperCallProcNameCallStmt);

		std::string queryCallStmtVariableVarName = "call c1; variable v2; Select c1 with c1.stmt# = v2.varName";
		auto wrapperCallStmtVariableVarName =
			[&queryCallStmtVariableVarName] { PQLParser::parseQuery(queryCallStmtVariableVarName); };
		Assert::ExpectException<QPSException>(wrapperCallStmtVariableVarName);

		std::string queryVariableVarNameCallStmt = "variable v1; call c2; Select v1 with v1.varName = c2.stmt#";
		auto wrapperVariableVarNameCallStmt =
			[&queryVariableVarNameCallStmt] { PQLParser::parseQuery(queryVariableVarNameCallStmt); };
		Assert::ExpectException<QPSException>(wrapperVariableVarNameCallStmt);

		std::string queryCallStmtReadVarName = "call c1; read r2; Select c1 with c1.stmt# = r2.varName";
		auto wrapperCallStmtReadVarName =
			[&queryCallStmtReadVarName] { PQLParser::parseQuery(queryCallStmtReadVarName); };
		Assert::ExpectException<QPSException>(wrapperCallStmtReadVarName);

		std::string queryReadVarNameCallStmt = "read r1; call c2; Select r1 with r1.varName = c2.stmt#";
		auto wrapperReadVarNameCallStmt =
			[&queryReadVarNameCallStmt] { PQLParser::parseQuery(queryReadVarNameCallStmt); };
		Assert::ExpectException<QPSException>(wrapperReadVarNameCallStmt);

		std::string queryCallStmtPrintVarName = "call c1; print p2; Select c1 with c1.stmt# = p2.varName";
		auto wrapperCallStmtPrintVarName =
			[&queryCallStmtPrintVarName] { PQLParser::parseQuery(queryCallStmtPrintVarName); };
		Assert::ExpectException<QPSException>(wrapperCallStmtPrintVarName);

		std::string queryPrintVarNameCallStmt = "print p1; call c2; Select p1 with p1.varName = c2.stmt#";
		auto wrapperPrintVarNameCallStmt =
			[&queryPrintVarNameCallStmt] { PQLParser::parseQuery(queryPrintVarNameCallStmt); };
		Assert::ExpectException<QPSException>(wrapperPrintVarNameCallStmt);

		std::string queryWhileStmtProcedureProcName = "while w1; procedure p2; Select w1 with w1.stmt# = p2.procName";
		auto wrapperWhileStmtProcedureProcName =
			[&queryWhileStmtProcedureProcName] { PQLParser::parseQuery(queryWhileStmtProcedureProcName); };
		Assert::ExpectException<QPSException>(wrapperWhileStmtProcedureProcName);

		std::string queryProcedureProcNameWhileStmt = "procedure p1; while w2; Select p1 with p1.procName = w2.stmt#";
		auto wrapperProcedureProcNameWhileStmt =
			[&queryProcedureProcNameWhileStmt] { PQLParser::parseQuery(queryProcedureProcNameWhileStmt); };
		Assert::ExpectException<QPSException>(wrapperProcedureProcNameWhileStmt);

		std::string queryWhileStmtCallProcName = "while w1; call c2; Select w1 with w1.stmt# = c2.procName";
		auto wrapperWhileStmtCallProcName =
			[&queryWhileStmtCallProcName] { PQLParser::parseQuery(queryWhileStmtCallProcName); };
		Assert::ExpectException<QPSException>(wrapperWhileStmtCallProcName);

		std::string queryCallProcNameWhileStmt = "call c1; while w2; Select c1 with c1.procName = w2.stmt#";
		auto wrapperCallProcNameWhileStmt =
			[&queryCallProcNameWhileStmt] { PQLParser::parseQuery(queryCallProcNameWhileStmt); };
		Assert::ExpectException<QPSException>(wrapperCallProcNameWhileStmt);

		std::string queryWhileStmtVariableVarName = "while w1; variable v2; Select w1 with w1.stmt# = v2.varName";
		auto wrapperWhileStmtVariableVarName =
			[&queryWhileStmtVariableVarName] { PQLParser::parseQuery(queryWhileStmtVariableVarName); };
		Assert::ExpectException<QPSException>(wrapperWhileStmtVariableVarName);

		std::string queryVariableVarNameWhileStmt = "variable v1; while w2; Select v1 with v1.varName = w2.stmt#";
		auto wrapperVariableVarNameWhileStmt =
			[&queryVariableVarNameWhileStmt] { PQLParser::parseQuery(queryVariableVarNameWhileStmt); };
		Assert::ExpectException<QPSException>(wrapperVariableVarNameWhileStmt);

		std::string queryWhileStmtReadVarName = "while w1; read r2; Select w1 with w1.stmt# = r2.varName";
		auto wrapperWhileStmtReadVarName =
			[&queryWhileStmtReadVarName] { PQLParser::parseQuery(queryWhileStmtReadVarName); };
		Assert::ExpectException<QPSException>(wrapperWhileStmtReadVarName);

		std::string queryReadVarNameWhileStmt = "read r1; while w2; Select r1 with r1.varName = w2.stmt#";
		auto wrapperReadVarNameWhileStmt =
			[&queryReadVarNameWhileStmt] { PQLParser::parseQuery(queryReadVarNameWhileStmt); };
		Assert::ExpectException<QPSException>(wrapperReadVarNameWhileStmt);

		std::string queryWhileStmtPrintVarName = "while w1; print p2; Select w1 with w1.stmt# = p2.varName";
		auto wrapperWhileStmtPrintVarName =
			[&queryWhileStmtPrintVarName] { PQLParser::parseQuery(queryWhileStmtPrintVarName); };
		Assert::ExpectException<QPSException>(wrapperWhileStmtPrintVarName);

		std::string queryPrintVarNameWhileStmt = "print p1; while w2; Select p1 with p1.varName = w2.stmt#";
		auto wrapperPrintVarNameWhileStmt =
			[&queryPrintVarNameWhileStmt] { PQLParser::parseQuery(queryPrintVarNameWhileStmt); };
		Assert::ExpectException<QPSException>(wrapperPrintVarNameWhileStmt);

		std::string queryIfStmtProcedureProcName = "if i1; procedure p2; Select i1 with i1.stmt# = p2.procName";
		auto wrapperIfStmtProcedureProcName =
			[&queryIfStmtProcedureProcName] { PQLParser::parseQuery(queryIfStmtProcedureProcName); };
		Assert::ExpectException<QPSException>(wrapperIfStmtProcedureProcName);

		std::string queryProcedureProcNameIfStmt = "procedure p1; if i2; Select p1 with p1.procName = i2.stmt#";
		auto wrapperProcedureProcNameIfStmt =
			[&queryProcedureProcNameIfStmt] { PQLParser::parseQuery(queryProcedureProcNameIfStmt); };
		Assert::ExpectException<QPSException>(wrapperProcedureProcNameIfStmt);

		std::string queryIfStmtCallProcName = "if i1; call c2; Select i1 with i1.stmt# = c2.procName";
		auto wrapperIfStmtCallProcName =
			[&queryIfStmtCallProcName] { PQLParser::parseQuery(queryIfStmtCallProcName); };
		Assert::ExpectException<QPSException>(wrapperIfStmtCallProcName);

		std::string queryCallProcNameIfStmt = "call c1; if i2; Select c1 with c1.procName = i2.stmt#";
		auto wrapperCallProcNameIfStmt =
			[&queryCallProcNameIfStmt] { PQLParser::parseQuery(queryCallProcNameIfStmt); };
		Assert::ExpectException<QPSException>(wrapperCallProcNameIfStmt);

		std::string queryIfStmtVariableVarName = "if i1; variable v2; Select i1 with i1.stmt# = v2.varName";
		auto wrapperIfStmtVariableVarName =
			[&queryIfStmtVariableVarName] { PQLParser::parseQuery(queryIfStmtVariableVarName); };
		Assert::ExpectException<QPSException>(wrapperIfStmtVariableVarName);

		std::string queryVariableVarNameIfStmt = "variable v1; if i2; Select v1 with v1.varName = i2.stmt#";
		auto wrapperVariableVarNameIfStmt =
			[&queryVariableVarNameIfStmt] { PQLParser::parseQuery(queryVariableVarNameIfStmt); };
		Assert::ExpectException<QPSException>(wrapperVariableVarNameIfStmt);

		std::string queryIfStmtReadVarName = "if i1; read r2; Select i1 with i1.stmt# = r2.varName";
		auto wrapperIfStmtReadVarName =
			[&queryIfStmtReadVarName] { PQLParser::parseQuery(queryIfStmtReadVarName); };
		Assert::ExpectException<QPSException>(wrapperIfStmtReadVarName);

		std::string queryReadVarNameIfStmt = "read r1; if i2; Select r1 with r1.varName = i2.stmt#";
		auto wrapperReadVarNameIfStmt =
			[&queryReadVarNameIfStmt] { PQLParser::parseQuery(queryReadVarNameIfStmt); };
		Assert::ExpectException<QPSException>(wrapperReadVarNameIfStmt);

		std::string queryIfStmtPrintVarName = "if i1; print p2; Select i1 with i1.stmt# = p2.varName";
		auto wrapperIfStmtPrintVarName =
			[&queryIfStmtPrintVarName] { PQLParser::parseQuery(queryIfStmtPrintVarName); };
		Assert::ExpectException<QPSException>(wrapperIfStmtPrintVarName);

		std::string queryPrintVarNameIfStmt = "print p1; if i2; Select p1 with p1.varName = i2.stmt#";
		auto wrapperPrintVarNameIfStmt =
			[&queryPrintVarNameIfStmt] { PQLParser::parseQuery(queryPrintVarNameIfStmt); };
		Assert::ExpectException<QPSException>(wrapperPrintVarNameIfStmt);

		std::string queryAssignStmtProcedureProcName = "assign a1; procedure p2; Select a1 with a1.stmt# = p2.procName";
		auto wrapperAssignStmtProcedureProcName =
			[&queryAssignStmtProcedureProcName] { PQLParser::parseQuery(queryAssignStmtProcedureProcName); };
		Assert::ExpectException<QPSException>(wrapperAssignStmtProcedureProcName);

		std::string queryProcedureProcNameAssignStmt = "procedure p1; assign a2; Select p1 with p1.procName = a2.stmt#";
		auto wrapperProcedureProcNameAssignStmt =
			[&queryProcedureProcNameAssignStmt] { PQLParser::parseQuery(queryProcedureProcNameAssignStmt); };
		Assert::ExpectException<QPSException>(wrapperProcedureProcNameAssignStmt);

		std::string queryAssignStmtCallProcName = "assign a1; call c2; Select a1 with a1.stmt# = c2.procName";
		auto wrapperAssignStmtCallProcName =
			[&queryAssignStmtCallProcName] { PQLParser::parseQuery(queryAssignStmtCallProcName); };
		Assert::ExpectException<QPSException>(wrapperAssignStmtCallProcName);

		std::string queryCallProcNameAssignStmt = "call c1; assign a2; Select c1 with c1.procName = a2.stmt#";
		auto wrapperCallProcNameAssignStmt =
			[&queryCallProcNameAssignStmt] { PQLParser::parseQuery(queryCallProcNameAssignStmt); };
		Assert::ExpectException<QPSException>(wrapperCallProcNameAssignStmt);

		std::string queryAssignStmtVariableVarName = "assign a1; variable v2; Select a1 with a1.stmt# = v2.varName";
		auto wrapperAssignStmtVariableVarName =
			[&queryAssignStmtVariableVarName] { PQLParser::parseQuery(queryAssignStmtVariableVarName); };
		Assert::ExpectException<QPSException>(wrapperAssignStmtVariableVarName);

		std::string queryVariableVarNameAssignStmt = "variable v1; assign a2; Select v1 with v1.varName = a2.stmt#";
		auto wrapperVariableVarNameAssignStmt =
			[&queryVariableVarNameAssignStmt] { PQLParser::parseQuery(queryVariableVarNameAssignStmt); };
		Assert::ExpectException<QPSException>(wrapperVariableVarNameAssignStmt);

		std::string queryAssignStmtReadVarName = "assign a1; read r2; Select a1 with a1.stmt# = r2.varName";
		auto wrapperAssignStmtReadVarName =
			[&queryAssignStmtReadVarName] { PQLParser::parseQuery(queryAssignStmtReadVarName); };
		Assert::ExpectException<QPSException>(wrapperAssignStmtReadVarName);

		std::string queryReadVarNameAssignStmt = "read r1; assign a2; Select r1 with r1.varName = a2.stmt#";
		auto wrapperReadVarNameAssignStmt =
			[&queryReadVarNameAssignStmt] { PQLParser::parseQuery(queryReadVarNameAssignStmt); };
		Assert::ExpectException<QPSException>(wrapperReadVarNameAssignStmt);

		std::string queryAssignStmtPrintVarName = "assign a1; print p2; Select a1 with a1.stmt# = p2.varName";
		auto wrapperAssignStmtPrintVarName =
			[&queryAssignStmtPrintVarName] { PQLParser::parseQuery(queryAssignStmtPrintVarName); };
		Assert::ExpectException<QPSException>(wrapperAssignStmtPrintVarName);

		std::string queryPrintVarNameAssignStmt = "print p1; assign a2; Select p1 with p1.varName = a2.stmt#";
		auto wrapperPrintVarNameAssignStmt =
			[&queryPrintVarNameAssignStmt] { PQLParser::parseQuery(queryPrintVarNameAssignStmt); };
		Assert::ExpectException<QPSException>(wrapperPrintVarNameAssignStmt);
	}

	TEST_METHOD(parseQuery_withConstantValueConstantValue_withExtracted) {
		std::string query = "constant c1; constant c2; Select c1 with c1.value = c2.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withConstantValueStmtStmt_withExtracted) {
		std::string query = "constant c1; stmt s2; Select c1 with c1.value = s2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withConstantValueReadStmt_withExtracted) {
		std::string query = "constant c1; read r2; Select c1 with c1.value = r2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withConstantValuePrintStmt_withExtracted) {
		std::string query = "constant c1; print p2; Select c1 with c1.value = p2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withConstantValueCallStmt_withExtracted) {
		std::string query = "constant c1; call c2; Select c1 with c1.value = c2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withConstantValueWhileStmt_withExtracted) {
		std::string query = "constant c1; while w2; Select c1 with c1.value = w2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withConstantValueIfStmt_withExtracted) {
		std::string query = "constant c1; if i2; Select c1 with c1.value = i2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withConstantValueAssignStmt_withExtracted) {
		std::string query = "constant c1; assign a2; Select c1 with c1.value = a2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withStmtStmtConstantValue_withExtracted) {
		std::string query = "stmt s1; constant c2; Select s1 with s1.stmt# = c2.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withStmtStmtStmtStmt_withExtracted) {
		std::string query = "stmt s1; stmt s2; Select s1 with s1.stmt# = s2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withStmtStmtReadStmt_withExtracted) {
		std::string query = "stmt s1; read r2; Select s1 with s1.stmt# = r2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withStmtStmtPrintStmt_withExtracted) {
		std::string query = "stmt s1; print p2; Select s1 with s1.stmt# = p2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withStmtStmtCallStmt_withExtracted) {
		std::string query = "stmt s1; call c2; Select s1 with s1.stmt# = c2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withStmtStmtWhileStmt_withExtracted) {
		std::string query = "stmt s1; while w2; Select s1 with s1.stmt# = w2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withStmtStmtIfStmt_withExtracted) {
		std::string query = "stmt s1; if i2; Select s1 with s1.stmt# = i2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withStmtStmtAssignStmt_withExtracted) {
		std::string query = "stmt s1; assign a2; Select s1 with s1.stmt# = a2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withReadStmtConstantValue_withExtracted) {
		std::string query = "read r1; constant c2; Select r1 with r1.stmt# = c2.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withReadStmtStmtStmt_withExtracted) {
		std::string query = "read r1; stmt s2; Select r1 with r1.stmt# = s2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withReadStmtReadStmt_withExtracted) {
		std::string query = "read r1; read r2; Select r1 with r1.stmt# = r2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withReadStmtPrintStmt_withExtracted) {
		std::string query = "read r1; print p2; Select r1 with r1.stmt# = p2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withReadStmtCallStmt_withExtracted) {
		std::string query = "read r1; call c2; Select r1 with r1.stmt# = c2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withReadStmtWhileStmt_withExtracted) {
		std::string query = "read r1; while w2; Select r1 with r1.stmt# = w2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withReadStmtIfStmt_withExtracted) {
		std::string query = "read r1; if i2; Select r1 with r1.stmt# = i2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withReadStmtAssignStmt_withExtracted) {
		std::string query = "read r1; assign a2; Select r1 with r1.stmt# = a2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withPrintStmtConstantValue_withExtracted) {
		std::string query = "print p1; constant c2; Select p1 with p1.stmt# = c2.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withPrintStmtStmtStmt_withExtracted) {
		std::string query = "print p1; stmt s2; Select p1 with p1.stmt# = s2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withPrintStmtReadStmt_withExtracted) {
		std::string query = "print p1; read r2; Select p1 with p1.stmt# = r2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withPrintStmtPrintStmt_withExtracted) {
		std::string query = "print p1; print p2; Select p1 with p1.stmt# = p2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withPrintStmtCallStmt_withExtracted) {
		std::string query = "print p1; call c2; Select p1 with p1.stmt# = c2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withPrintStmtWhileStmt_withExtracted) {
		std::string query = "print p1; while w2; Select p1 with p1.stmt# = w2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withPrintStmtIfStmt_withExtracted) {
		std::string query = "print p1; if i2; Select p1 with p1.stmt# = i2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withPrintStmtAssignStmt_withExtracted) {
		std::string query = "print p1; assign a2; Select p1 with p1.stmt# = a2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withCallStmtConstantValue_withExtracted) {
		std::string query = "call c1; constant c2; Select c1 with c1.stmt# = c2.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withCallStmtStmtStmt_withExtracted) {
		std::string query = "call c1; stmt s2; Select c1 with c1.stmt# = s2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withCallStmtReadStmt_withExtracted) {
		std::string query = "call c1; read r2; Select c1 with c1.stmt# = r2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withCallStmtPrintStmt_withExtracted) {
		std::string query = "call c1; print p2; Select c1 with c1.stmt# = p2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withCallStmtCallStmt_withExtracted) {
		std::string query = "call c1; call c2; Select c1 with c1.stmt# = c2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withCallStmtWhileStmt_withExtracted) {
		std::string query = "call c1; while w2; Select c1 with c1.stmt# = w2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withCallStmtIfStmt_withExtracted) {
		std::string query = "call c1; if i2; Select c1 with c1.stmt# = i2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withCallStmtAssignStmt_withExtracted) {
		std::string query = "call c1; assign a2; Select c1 with c1.stmt# = a2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withWhileStmtConstantValue_withExtracted) {
		std::string query = "while w1; constant c2; Select w1 with w1.stmt# = c2.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withWhileStmtStmtStmt_withExtracted) {
		std::string query = "while w1; stmt s2; Select w1 with w1.stmt# = s2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withWhileStmtReadStmt_withExtracted) {
		std::string query = "while w1; read r2; Select w1 with w1.stmt# = r2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withWhileStmtPrintStmt_withExtracted) {
		std::string query = "while w1; print p2; Select w1 with w1.stmt# = p2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withWhileStmtCallStmt_withExtracted) {
		std::string query = "while w1; call c2; Select w1 with w1.stmt# = c2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withWhileStmtWhileStmt_withExtracted) {
		std::string query = "while w1; while w2; Select w1 with w1.stmt# = w2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withWhileStmtIfStmt_withExtracted) {
		std::string query = "while w1; if i2; Select w1 with w1.stmt# = i2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withWhileStmtAssignStmt_withExtracted) {
		std::string query = "while w1; assign a2; Select w1 with w1.stmt# = a2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withIfStmtConstantValue_withExtracted) {
		std::string query = "if i1; constant c2; Select i1 with i1.stmt# = c2.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withIfStmtStmtStmt_withExtracted) {
		std::string query = "if i1; stmt s2; Select i1 with i1.stmt# = s2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withIfStmtReadStmt_withExtracted) {
		std::string query = "if i1; read r2; Select i1 with i1.stmt# = r2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withIfStmtPrintStmt_withExtracted) {
		std::string query = "if i1; print p2; Select i1 with i1.stmt# = p2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withIfStmtCallStmt_withExtracted) {
		std::string query = "if i1; call c2; Select i1 with i1.stmt# = c2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withIfStmtWhileStmt_withExtracted) {
		std::string query = "if i1; while w2; Select i1 with i1.stmt# = w2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withIfStmtIfStmt_withExtracted) {
		std::string query = "if i1; if i2; Select i1 with i1.stmt# = i2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withIfStmtAssignStmt_withExtracted) {
		std::string query = "if i1; assign a2; Select i1 with i1.stmt# = a2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withAssignStmtConstantValue_withExtracted) {
		std::string query = "assign a1; constant c2; Select a1 with a1.stmt# = c2.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withAssignStmtStmtStmt_withExtracted) {
		std::string query = "assign a1; stmt s2; Select a1 with a1.stmt# = s2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withAssignStmtReadStmt_withExtracted) {
		std::string query = "assign a1; read r2; Select a1 with a1.stmt# = r2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withAssignStmtPrintStmt_withExtracted) {
		std::string query = "assign a1; print p2; Select a1 with a1.stmt# = p2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withAssignStmtCallStmt_withExtracted) {
		std::string query = "assign a1; call c2; Select a1 with a1.stmt# = c2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withAssignStmtWhileStmt_withExtracted) {
		std::string query = "assign a1; while w2; Select a1 with a1.stmt# = w2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withAssignStmtIfStmt_withExtracted) {
		std::string query = "assign a1; if i2; Select a1 with a1.stmt# = i2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withAssignStmtAssignStmt_withExtracted) {
		std::string query = "assign a1; assign a2; Select a1 with a1.stmt# = a2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withProcedureProcNameProcedureProcName_withExtracted) {
		std::string query = "procedure p1; procedure p2; Select p1 with p1.procName = p2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withProcedureProcNameCallProcName_withExtracted) {
		std::string query = "procedure p1; call c2; Select p1 with p1.procName = c2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withProcedureProcNameVariableVarName_withExtracted) {
		std::string query = "procedure p1; variable v2; Select p1 with p1.procName = v2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withProcedureProcNameReadVarName_withExtracted) {
		std::string query = "procedure p1; read r2; Select p1 with p1.procName = r2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withProcedureProcNamePrintVarName_withExtracted) {
		std::string query = "procedure p1; print p2; Select p1 with p1.procName = p2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withCallProcNameProcedureProcName_withExtracted) {
		std::string query = "call c1; procedure p2; Select c1 with c1.procName = p2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withCallProcNameCallProcName_withExtracted) {
		std::string query = "call c1; call c2; Select c1 with c1.procName = c2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withCallProcNameVariableVarName_withExtracted) {
		std::string query = "call c1; variable v2; Select c1 with c1.procName = v2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withCallProcNameReadVarName_withExtracted) {
		std::string query = "call c1; read r2; Select c1 with c1.procName = r2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withCallProcNamePrintVarName_withExtracted) {
		std::string query = "call c1; print p2; Select c1 with c1.procName = p2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withVariableVarNameProcedureProcName_withExtracted) {
		std::string query = "variable v1; procedure p2; Select v1 with v1.varName = p2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withVariableVarNameCallProcName_withExtracted) {
		std::string query = "variable v1; call c2; Select v1 with v1.varName = c2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withVariableVarNameVariableVarName_withExtracted) {
		std::string query = "variable v1; variable v2; Select v1 with v1.varName = v2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withVariableVarNameReadVarName_withExtracted) {
		std::string query = "variable v1; read r2; Select v1 with v1.varName = r2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withVariableVarNamePrintVarName_withExtracted) {
		std::string query = "variable v1; print p2; Select v1 with v1.varName = p2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withReadVarNameProcedureProcName_withExtracted) {
		std::string query = "read r1; procedure p2; Select r1 with r1.varName = p2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withReadVarNameCallProcName_withExtracted) {
		std::string query = "read r1; call c2; Select r1 with r1.varName = c2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withReadVarNameVariableVarName_withExtracted) {
		std::string query = "read r1; variable v2; Select r1 with r1.varName = v2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withReadVarNameReadVarName_withExtracted) {
		std::string query = "read r1; read r2; Select r1 with r1.varName = r2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withReadVarNamePrintVarName_withExtracted) {
		std::string query = "read r1; print p2; Select r1 with r1.varName = p2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withPrintVarNameProcedureProcName_withExtracted) {
		std::string query = "print p1; procedure p2; Select p1 with p1.varName = p2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withPrintVarNameCallProcName_withExtracted) {
		std::string query = "print p1; call c2; Select p1 with p1.varName = c2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withPrintVarNameVariableVarName_withExtracted) {
		std::string query = "print p1; variable v2; Select p1 with p1.varName = v2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withPrintVarNameReadVarName_withExtracted) {
		std::string query = "print p1; read r2; Select p1 with p1.varName = r2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_withPrintVarNamePrintVarName_withExtracted) {
		std::string query = "print p1; print p2; Select p1 with p1.varName = p2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<ParsedWith> withs = parsedQuery.getWiths();
		Assert::AreEqual(size_t(1), withs.size());
	}

	TEST_METHOD(parseQuery_attributeSpecified_attributeExtracted) {
		std::string query = "print pn; Select pn.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		std::vector<PqlReference> attributes = parsedQuery.getAttributes();
		Assert::AreEqual(size_t(1), attributes.size());
		PqlReference attribute = attributes[0];
		Assert::IsTrue(attribute.first == PqlReferenceType::VAR_NAME);
		Assert::AreEqual(std::string("pn"), attribute.second);
	}

	TEST_METHOD(parseQuery_booleanSpecified_emptyAttributes) {
		std::string query = "print pn; Select BOOLEAN";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		std::vector<PqlReference> attributes = parsedQuery.getAttributes();
		Assert::AreEqual(size_t(0), attributes.size());
	}

	TEST_METHOD(parseQuery_booleanSpecifiedAndDeclared_attributeExtracted) {
		std::string query = "print BOOLEAN; Select BOOLEAN";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		std::vector<PqlReference> attributes = parsedQuery.getAttributes();
		Assert::AreEqual(size_t(1), attributes.size());
		Assert::IsTrue(attributes[0].first == PqlReferenceType::SYNONYM);
		Assert::AreEqual(std::string("BOOLEAN"), attributes[0].second);
	}

	TEST_METHOD(parseQuery_tupleSpecified_attributeExtracted) {
		std::string query = "read r; Select <r.varName>";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		std::vector<PqlReference> attributes = parsedQuery.getAttributes();
		Assert::AreEqual(size_t(1), attributes.size());
		Assert::IsTrue(attributes[0].first == PqlReferenceType::VAR_NAME);
		Assert::AreEqual(std::string("r"), attributes[0].second);
	}

	TEST_METHOD(parseQuery_tupleSpecifiedMultipleElemsSameSynonym_allAttributesExtractedSingleSelectExtracted) {
		std::string query = "read r; Select <r, r.stmt#>";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		std::vector<PqlReference> attributes = parsedQuery.getAttributes();
		Assert::AreEqual(size_t(2), attributes.size());
		Assert::IsTrue(attributes[0].first == PqlReferenceType::SYNONYM);
		Assert::AreEqual(std::string("r"), attributes[0].second);
		Assert::IsTrue(attributes[1].first == PqlReferenceType::STMT_NUM);
		Assert::AreEqual(std::string("r"), attributes[1].second);

		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
	}

	TEST_METHOD(parseQuery_tupleSpecifiedMultipleElemsDifferentSynonym_allAttributesExtractedAllSelectsExtracted) {
		std::string query = "read r; print pn; Select <r.varName, pn.stmt#>";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		std::vector<PqlReference> attributes = parsedQuery.getAttributes();
		Assert::AreEqual(size_t(2), attributes.size());
		Assert::IsTrue(attributes[0].first == PqlReferenceType::VAR_NAME);
		Assert::AreEqual(std::string("r"), attributes[0].second);
		Assert::IsTrue(attributes[1].first == PqlReferenceType::STMT_NUM);
		Assert::AreEqual(std::string("pn"), attributes[1].second);

		Assert::AreEqual(size_t(2), parsedQuery.getColumns().size());
	}

	TEST_METHOD(parseQuery_multipleElemsNotTuple_exceptionThrown) {
		std::string query = "read r; print pn; Select r.varName, pn.stmt#";
		auto wrapperFunc =
			[&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_tupleNoClosingBracket_exceptionThrown) {
		std::string query = "read r; print pn; Select <r.varName, pn.stmt#";
		auto wrapperFunc =
			[&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_tupleNoComma_exceptionThrown) {
		std::string query = "read r; print pn; Select <r.varName pn.stmt#>";
		auto wrapperFunc =
			[&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_tupleExtraComma_exceptionThrown) {
		std::string query = "read r; print pn; Select <r.varName, , pn.stmt#>";
		auto wrapperFunc =
			[&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_emptyTuple_exceptionThrown) {
		std::string query = "read r; print pn; Select <>";
		auto wrapperFunc =
			[&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_tupleBooleanWithoutDeclaration_exceptionThrown) {
		std::string query = "read r; print pn; Select <BOOLEAN>";
		auto wrapperFunc =
			[&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_tupleBooleanWithDeclaration_allAttributesExtracted) {
		std::string query = "read BOOLEAN; Select <BOOLEAN>";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<PqlReference> attributes = parsedQuery.getAttributes();
		Assert::AreEqual(size_t(1), attributes.size());
		Assert::IsTrue(attributes[0].first == PqlReferenceType::SYNONYM);
		Assert::AreEqual(std::string("BOOLEAN"), attributes[0].second);
	}

	TEST_METHOD(parseQuery_selectBoolean_emptySelectExtracted) {
		std::string query = "Select BOOLEAN";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::unordered_set<std::string> columns = parsedQuery.getColumns();
		Assert::AreEqual(size_t(0), columns.size());
	}
	};
}
