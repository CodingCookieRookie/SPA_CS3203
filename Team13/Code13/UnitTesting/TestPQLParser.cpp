#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS/PQLParser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPQLParser) {
private:
	Processors processors = Processors(nullptr, nullptr, nullptr);
	PKBGetter* pkbGetter = nullptr;
public:
	TEST_METHOD(parseQuery_declarationAndSelectOnly_designEntitiesExtracted) {
		std::string query = "stmt s; if ifs; Select s";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		std::vector<PqlReference> attributes = parsedQuery.getAttributes();
		Assert::AreEqual(size_t(1), attributes.size());
		Assert::IsTrue(std::any_of(attributes.begin(), attributes.end(), [](PqlReference pqlRef) {
			return pqlRef == PqlReference(PqlReferenceType::SYNONYM, "s");
		}));
	}

	TEST_METHOD(parseQuery_repeatDeclaration_exceptionThrown) {
		std::string queryRepeat = "stmt s; if s; Select s";
		
		auto wrapperFuncRepeat = [&queryRepeat, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryRepeat);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFuncRepeat);
	}

	TEST_METHOD(parseQuery_modifiesFirstArgNonStmtSynonym_exceptionThrown) {
		
		std::string queryVar = "stmt s; variable v; Select s such that Modifies(v, _)";
		auto wrapperVar = [&queryVar, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryVar);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperVar);

		std::string queryConst = "stmt s; variable v; constant c; Select s such that Modifies(c, _)";
		auto wrapperConst = [&queryConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperConst);
	}

	TEST_METHOD(parseQuery_modifiesFirstArgWildcard_exceptionThrown) {
		
		std::string queryModifiesWildcard = "stmt s; variable v; Select s such that Modifies(_, v)";
		auto wrapperModifiesWildcard = [&queryModifiesWildcard, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryModifiesWildcard);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperModifiesWildcard);
	}

	TEST_METHOD(parseQuery_modifiesSecondArgSynonym_exceptionThrown) {
		
		std::string queryModifiesStmtRef = "stmt s; if ifs; Select s such that Modifies(if, s)";
		auto wrapperModifiesStmtRef = [&queryModifiesStmtRef, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryModifiesStmtRef);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperModifiesStmtRef);
	}

	TEST_METHOD(parseQuery_modifiesSecondArgStmtNumber_exceptionThrown) {
		
		std::string queryModifiesStmtRef = "print pn; variable v; Select pn such that Modifies(pn, 1)";
		auto wrapperModifiesStmtRef = [&queryModifiesStmtRef, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryModifiesStmtRef);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperModifiesStmtRef);
	}

	TEST_METHOD(parseQuery_usesFirstArgNonStmtSynonym_exceptionThrown) {
		
		std::string queryVar = "stmt s; variable v; Select s such that Uses(v, _)";
		auto wrapperVar = [&queryVar, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryVar);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperVar);

		std::string queryConst = "stmt s; variable v; constant c; Select s such that Uses(c, _)";
		auto wrapperConst = [&queryConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperConst);
	}

	TEST_METHOD(parseQuery_usesFirstArgWildcard_exceptionThrown) {
		
		std::string queryUsesWildcard = "stmt s; variable v; Select s such that Uses(_, _)";
		auto wrapperUsesWildcard = [&queryUsesWildcard, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryUsesWildcard);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperUsesWildcard);
	}

	TEST_METHOD(parseQuery_usesSecondArgSynonym_exceptionThrown) {
		
		std::string queryUsesStmtRef = "while w; call c; Select w such that Uses(c, w)";
		auto wrapperUsesStmtRef = [&queryUsesStmtRef, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryUsesStmtRef);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperUsesStmtRef);
	}

	TEST_METHOD(parseQuery_usesSecondArgStmtNumber_exceptionThrown) {
		
		std::string queryUsesStmtRef = "assign a; variable v; Select a such that Uses(a, 1)";
		auto wrapperUsesStmtRef = [&queryUsesStmtRef, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryUsesStmtRef);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperUsesStmtRef);
	}

	TEST_METHOD(parseQuery_followsFirstArgNonStmtSynonym_exceptionThrown) {
		
		std::string queryFollowsVariable = "assign a; variable v; Select a such that Follows(v, a)";
		auto wrapperFollowsVariable = [&queryFollowsVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryFollowsVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFollowsVariable);

		std::string queryFollowsProc = "assign a; procedure p; Select a such that Follows(p, a)";
		auto wrapperFollowsProc = [&queryFollowsProc, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryFollowsProc);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFollowsProc);

		std::string queryFollowsConst = "assign a; constant c; Select a such that Follows(c, a)";
		auto wrapperFollowsConst = [&queryFollowsConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryFollowsConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFollowsConst);

		std::string queryFollowsVarName = "assign a; constant c; Select a such that Follows(\"x\", a)";
		auto wrapperFollowsVarName = [&queryFollowsVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryFollowsVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFollowsVarName);
	}

	TEST_METHOD(parseQuery_followsSecondArgNonStmtSynonym_exceptionThrown) {
		
		std::string queryFollowsVariable = "assign a; variable v; Select a such that Follows(a, v)";
		auto wrapperFollowsVariable = [&queryFollowsVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryFollowsVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFollowsVariable);

		std::string queryFollowsProc = "assign a; procedure p; Select a such that Follows(a, p)";
		auto wrapperFollowsProc = [&queryFollowsProc, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryFollowsProc);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFollowsProc);

		std::string queryFollowsConst = "assign a; constant c; Select a such that Follows(a, c)";
		auto wrapperFollowsConst = [&queryFollowsConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryFollowsConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFollowsConst);

		std::string queryFollowsVarName = "assign a; constant c; Select a such that Follows(a, \"x\")";
		auto wrapperFollowsVarName = [&queryFollowsVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryFollowsVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFollowsVarName);
	}

	TEST_METHOD(parseQuery_followsTFirstArgNonStmtSynonym_exceptionThrown) {
		
		std::string queryFollowsVariable = "assign a; variable v; Select a such that Follows*(v, a)";
		auto wrapperFollowsVariable = [&queryFollowsVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryFollowsVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFollowsVariable);

		std::string queryFollowsProc = "assign a; procedure p; Select a such that Follows*(p, a)";
		auto wrapperFollowsProc = [&queryFollowsProc, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryFollowsProc);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFollowsProc);

		std::string queryFollowsConst = "assign a; constant c; Select a such that Follows*(c, a)";
		auto wrapperFollowsConst = [&queryFollowsConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryFollowsConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFollowsConst);

		std::string queryFollowsVarName = "assign a; constant c; Select a such that Follows*(\"x\", a)";
		auto wrapperFollowsVarName = [&queryFollowsVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryFollowsVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFollowsVarName);
	}

	TEST_METHOD(parseQuery_followsTSecondArgNonStmtSynonym_exceptionThrown) {
		
		std::string queryFollowsVariable = "assign a; variable v; Select a such that Follows*(a, v)";
		auto wrapperFollowsVariable = [&queryFollowsVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryFollowsVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFollowsVariable);

		std::string queryFollowsProc = "assign a; procedure p; Select a such that Follows*(a, p)";
		auto wrapperFollowsProc = [&queryFollowsProc, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryFollowsProc);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFollowsProc);

		std::string queryFollowsConst = "assign a; constant c; Select a such that Follows*(a, c)";
		auto wrapperFollowsConst = [&queryFollowsConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryFollowsConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFollowsConst);

		std::string queryFollowsVarName = "assign a; constant c; Select a such that Follows*(a, \"x\")";
		auto wrapperFollowsVarName = [&queryFollowsVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryFollowsVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFollowsVarName);
	}

	TEST_METHOD(parseQuery_parentFirstArgNonStmtSynonym_exceptionThrown) {
		
		std::string queryParentVariable = "assign a; variable v; Select a such that Parent(v, a)";
		auto wrapperParentVariable = [&queryParentVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryParentVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperParentVariable);

		std::string queryParentProc = "assign a; procedure p; Select a such that Parent(p, a)";
		auto wrapperParentProc = [&queryParentProc, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryParentProc);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperParentProc);

		std::string queryParentConst = "assign a; constant c; Select a such that Parent(c, a)";
		auto wrapperParentConst = [&queryParentConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryParentConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperParentConst);

		std::string queryParentVarName = "assign a; constant c; Select a such that Parent(\"x\", a)";
		auto wrapperParentVarName = [&queryParentVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryParentVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperParentVarName);
	}

	TEST_METHOD(parseQuery_parentSecondArgNonStmtSynonym_exceptionThrown) {
		
		std::string queryParentVariable = "assign a; variable v; Select a such that Parent(a, v)";
		auto wrapperParentVariable = [&queryParentVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryParentVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperParentVariable);

		std::string queryParentProc = "assign a; procedure p; Select a such that Parent(a, p)";
		auto wrapperParentProc = [&queryParentProc, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryParentProc);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperParentProc);

		std::string queryParentConst = "assign a; constant c; Select a such that Parent(a, c)";
		auto wrapperParentConst = [&queryParentConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryParentConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperParentConst);

		std::string queryParentVarName = "assign a; constant c; Select a such that Parent(a, \"x\")";
		auto wrapperParentVarName = [&queryParentVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryParentVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperParentVarName);
	}

	TEST_METHOD(parseQuery_parentTFirstArgNonStmtSynonym_exceptionThrown) {
		
		std::string queryParentVariable = "assign a; variable v; Select a such that Parent*(v, a)";
		auto wrapperParentVariable = [&queryParentVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryParentVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperParentVariable);

		std::string queryParentProc = "assign a; procedure p; Select a such that Parent*(p, a)";
		auto wrapperParentProc = [&queryParentProc, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryParentProc);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperParentProc);

		std::string queryParentConst = "assign a; constant c; Select a such that Parent*(c, a)";
		auto wrapperParentConst = [&queryParentConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryParentConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperParentConst);

		std::string queryParentVarName = "assign a; constant c; Select a such that Parent*(\"x\", a)";
		auto wrapperParentVarName = [&queryParentVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryParentVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperParentVarName);
	}

	TEST_METHOD(parseQuery_parentTSecondArgNonStmtSynonym_exceptionThrown) {
		
		std::string queryParentVariable = "assign a; variable v; Select a such that Parent*(a, v)";
		auto wrapperParentVariable = [&queryParentVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryParentVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperParentVariable);

		std::string queryParentProc = "assign a; procedure p; Select a such that Parent*(a, p)";
		auto wrapperParentProc = [&queryParentProc, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryParentProc);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperParentProc);

		std::string queryParentConst = "assign a; constant c; Select a such that Parent*(a, c)";
		auto wrapperParentConst = [&queryParentConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryParentConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperParentConst);

		std::string queryParentVarName = "assign a; constant c; Select a such that Parent*(a, \"x\")";
		auto wrapperParentVarName = [&queryParentVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryParentVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperParentVarName);
	}

	TEST_METHOD(parseQuery_patternSynAssignInvalid_exceptionThrown) {
		
		std::string queryVariable = "assign a; variable v; Select a pattern v(_, _)";
		auto wrapperVariable = [&queryVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperVariable);

		std::string queryPrint = "assign a; print pn; Select a pattern pn(_, _)";
		auto wrapperPrint = [&queryPrint, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrint);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperPrint);
	}

	TEST_METHOD(parseQuery_patternEntRefInvalid_exceptionThrown) {
		
		std::string queryAssign = "assign a; variable v; Select a pattern a(a, _)";
		auto wrapperAssign = [&queryAssign, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAssign);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAssign);

		std::string queryConstant = "assign a; constant c; Select a pattern a(c, _)";
		auto wrapperConstant = [&queryConstant, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryConstant);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperConstant);

		std::string queryRead = "assign a; read r; Select a pattern a(r, _)";
		auto wrapperRead = [&queryRead, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryRead);
			pq.getClauseInstructions(processors, pkbGetter);
		};
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
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(3), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(UsesSInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(GetAllStmtInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(GetAllVarInstruction); }));
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
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(2), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(ModifiesSInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(GetAllStmtInstruction); }));
	}

	TEST_METHOD(parseQuery_patternClause_patternExtracted) {
		
		std::string query = "assign a; Select a pattern a(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(PatternAInstruction); }));
	}

	TEST_METHOD(parseQuery_patternClauseSynonym_synonymExtracted) {
		
		std::string query = "assign a; variable v; Select a pattern a(v, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(PatternAInstruction); }));
	}

	TEST_METHOD(parseQuery_patternClauseVarExpr_varExtracted) {
		
		std::string query = "assign a; Select a pattern a(_, _\"x\"_)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(PatternAInstruction); }));
	}

	TEST_METHOD(parseQuery_suchThatAndPatternClauses_bothExtracted) {
		
		std::string query =
			"assign a; stmt s;"
			"Select a such that Parent*(s, a) pattern a(_, _\"x\"_)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(4), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(ParentStarInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(GetAllStmtInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(GetAllAssignInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(PatternAInstruction); }));
	}

	TEST_METHOD(parseQuery_patternClausePartialExpr_exprExtracted) {
		
		std::string query = "assign a; Select a pattern a(_, _\"(x + 1)       * 2 / 3		-4 %\n (5)\"_)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(PatternAInstruction); }));
	}

	TEST_METHOD(parseQuery_patternClauseFullExpr_exprExtracted) {
		
		std::string query = "assign a; Select a pattern a(_, \"(x + 1)       * 2 / 3		-4 %\n (5)\")";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(PatternAInstruction); }));
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
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(ModifiesPInstruction); }));
	}

	TEST_METHOD(parseQuery_usesFirstArgIdent_correctlyParsedAsUsesP) {
		
		std::string query = "stmt s; variable v; Select s such that Uses(\"x\", _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(UsesPInstruction); }));
	}

	TEST_METHOD(parseQuery_usesFirstArgProcSynonym_correctlyParsedAsModifiesP) {
				std::string query = "stmt s; variable v; procedure p; Select s such that Modifies(p, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(2), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(ModifiesPInstruction); }));
	}

	TEST_METHOD(parseQuery_usesFirstArgProcSynonym_correctlyParsedAsUsesP) {
		
		std::string query = "stmt s; variable v; procedure p; Select s such that Uses(p, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(2), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(UsesPInstruction); }));
	}

	TEST_METHOD(parseQuery_callsClauseTwoSynonyms_bothSynonymsExtracted) {
		
		std::string query = "procedure p1, p2; Select p1 such that Calls(p1, p2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(3), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(CallsInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(GetAllProcInstruction); }));
	}

	TEST_METHOD(parseQuery_callsClauseTwoWildcards_bothWildcardsExtracted) {
		
		std::string query = "procedure p1, p2; Select p1 such that Calls(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(CallsInstruction); }));
	}

	TEST_METHOD(parseQuery_callsClauseTwoIdent_bothIdentsExtracted) {
		
		std::string query = "procedure p1, p2; Select p1 such that Calls(\"proc1\", \"proc2\")";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(CallsInstruction); }));
	}

	TEST_METHOD(parseQuery_callsTClauseTwoSynonyms_bothSynonymsExtracted) {
		
		std::string query = "procedure p1, p2; Select p1 such that Calls*(p1, p2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(3), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(CallsStarInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(GetAllProcInstruction); }));
	}

	TEST_METHOD(parseQuery_callsTClauseTwoWildcards_bothWildcardsExtracted) {
		
		std::string query = "procedure p1, p2; Select p1 such that Calls*(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(CallsStarInstruction); }));
	}

	TEST_METHOD(parseQuery_callsTClauseTwoIdent_bothIdentsExtracted) {
		
		std::string query = "procedure p1, p2; Select p1 such that Calls*(\"proc1\", \"proc2\")";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(CallsStarInstruction); }));
	}

	TEST_METHOD(parseQuery_nextClauseTwoSynonyms_bothSynonymsExtracted) {
		
		std::string query = "stmt s1, s2; Select s1 such that Next(s1, s2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(3), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(NextInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(GetAllStmtInstruction); }));
	}

	TEST_METHOD(parseQuery_nextClauseTwoWildcards_bothWildcardsExtracted) {
		
		std::string query = "stmt s1, s2; Select s1 such that Next(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(NextInstruction); }));
	}

	TEST_METHOD(parseQuery_nextClauseTwoIntegers_bothStmtIndexesExtracted) {
		
		std::string query = "stmt s1, s2; Select s1 such that Next(1, 2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(NextInstruction); }));
	}

	TEST_METHOD(parseQuery_nextTClauseTwoSynonyms_bothSynonymsExtracted) {
		
		std::string query = "stmt s1, s2; Select s1 such that Next*(s1, s2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(3), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(NextStarInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(GetAllStmtInstruction); }));
	}

	TEST_METHOD(parseQuery_nextTClauseTwoWildcards_bothWildcardsExtracted) {
		
		std::string query = "stmt s1, s2; Select s1 such that Next*(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(NextStarInstruction); }));
	}

	TEST_METHOD(parseQuery_nextTClauseTwoIntegers_bothStmtIndexesExtracted) {
		
		std::string query = "stmt s1, s2; Select s1 such that Next*(1, 2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(NextStarInstruction); }));
	}

	TEST_METHOD(parseQuery_affectsClauseTwoAssigns_bothSynonymsExtracted) {
		
		std::string query = "assign a1, a2; Select a1 such that Affects(a1, a2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(3), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(AffectsInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(GetAllAssignInstruction); }));
	}

	TEST_METHOD(parseQuery_affectsClauseTwoStmts_bothSynonymsExtracted) {
		
		std::string query = "stmt s1, s2; Select s1 such that Affects(s1, s2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(3), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(AffectsInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(GetAllStmtInstruction); }));
	}

	TEST_METHOD(parseQuery_affectsClauseTwoWildcards_bothWildcardsExtracted) {
		
		std::string query = "assign a1, a2; Select a1 such that Affects(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(AffectsInstruction); }));
	}

	TEST_METHOD(parseQuery_affectsClauseTwoIntegers_bothStmtIndexesExtracted) {
		
		std::string query = "assign a1, a2; Select a1 such that Affects(1, 2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(AffectsInstruction); }));
	}

	TEST_METHOD(parseQuery_affectsTClauseTwoAssigns_bothSynonymsExtracted) {
		
		std::string query = "assign a1, a2; Select a1 such that Affects*(a1, a2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(3), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(AffectsStarInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(GetAllAssignInstruction); }));
	}

	TEST_METHOD(parseQuery_affectsTClauseTwoStmts_bothSynonymsExtracted) {
		
		std::string query = "stmt s1, s2; Select s1 such that Affects*(s1, s2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(3), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(AffectsStarInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(GetAllStmtInstruction); }));
	}

	TEST_METHOD(parseQuery_affectsTClauseTwoWildcards_bothWildcardsExtracted) {
		
		std::string query = "assign a1, a2; Select a1 such that Affects*(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(AffectsStarInstruction); }));
	}

	TEST_METHOD(parseQuery_affectsTClauseTwoIntegers_bothStmtIndexesExtracted) {
		
		std::string query = "assign a1, a2; Select a1 such that Affects*(1, 2)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(AffectsStarInstruction); }));
	}

	TEST_METHOD(parseQuery_callsFirstArgNonProcSynonym_exceptionThrown) {
		
		std::string queryCallsVariable = "procedure p; variable v; Select p such that Calls(v, p)";
		auto wrapperCallsVariable = [&queryCallsVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsVariable);

		std::string queryCallsConst = "procedure p; constant c; Select p such that Calls(c, p)";
		auto wrapperCallsConst = [&queryCallsConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsConst);

		std::string queryCallsStmt = "procedure p; stmt s; Select p such that Calls(s, p)";
		auto wrapperCallsStmt = [&queryCallsStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsStmt);

		std::string queryCallsRead = "procedure p; read r; Select p such that Calls(r, p)";
		auto wrapperCallsRead = [&queryCallsRead, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsRead);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsRead);

		std::string queryCallsPrint = "procedure p; print pn; Select p such that Calls(pn, p)";
		auto wrapperCallsPrint = [&queryCallsPrint, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsPrint);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsPrint);

		std::string queryCallsCall = "procedure p; call c; Select p such that Calls(c, p)";
		auto wrapperCallsCall = [&queryCallsCall, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsCall);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsCall);

		std::string queryCallsWhile = "procedure p; while w; Select p such that Calls(w, p)";
		auto wrapperCallsWhile = [&queryCallsWhile, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsWhile);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsWhile);

		std::string queryCallsIf = "procedure p; if ifs; Select p such that Calls(ifs, p)";
		auto wrapperCallsIf = [&queryCallsIf, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsIf);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsIf);

		std::string queryCallsAssign = "procedure p; assign a; Select p such that Calls(a, p)";
		auto wrapperCallsAssign = [&queryCallsAssign, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsAssign);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsAssign);

		std::string queryCallsInteger = "procedure p; constant c; Select p such that Calls(1, p)";
		auto wrapperCallsInteger = [&queryCallsInteger, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsInteger);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsInteger);
	}

	TEST_METHOD(parseQuery_callsSecondArgNonProcSynonym_exceptionThrown) {
		
		std::string queryCallsVariable = "procedure p; variable v; Select p such that Calls(p, v)";
		auto wrapperCallsVariable = [&queryCallsVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsVariable);

		std::string queryCallsConst = "procedure p; constant c; Select p such that Calls(p, c)";
		auto wrapperCallsConst = [&queryCallsConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsConst);

		std::string queryCallsStmt = "procedure p; stmt s; Select p such that Calls(p, s)";
		auto wrapperCallsStmt = [&queryCallsStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsStmt);

		std::string queryCallsRead = "procedure p; read r; Select p such that Calls(p, r)";
		auto wrapperCallsRead = [&queryCallsRead, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsRead);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsRead);

		std::string queryCallsPrint = "procedure p; print pn; Select p such that Calls(p, pn)";
		auto wrapperCallsPrint = [&queryCallsPrint, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsPrint);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsPrint);

		std::string queryCallsCall = "procedure p; call c; Select p such that Calls(p, c)";
		auto wrapperCallsCall = [&queryCallsCall, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsCall);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsCall);

		std::string queryCallsWhile = "procedure p; while w; Select p such that Calls(p, w)";
		auto wrapperCallsWhile = [&queryCallsWhile, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsWhile);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsWhile);

		std::string queryCallsIf = "procedure p; if ifs; Select p such that Calls(p, ifs)";
		auto wrapperCallsIf = [&queryCallsIf, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsIf);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsIf);

		std::string queryCallsAssign = "procedure p; assign a; Select p such that Calls(p, a)";
		auto wrapperCallsAssign = [&queryCallsAssign, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsAssign);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsAssign);

		std::string queryCallsInteger = "procedure p; constant c; Select p such that Calls(p, 1)";
		auto wrapperCallsInteger = [&queryCallsInteger, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsInteger);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsInteger);
	}

	TEST_METHOD(parseQuery_callsTFirstArgNonProcSynonym_exceptionThrown) {
		
		std::string queryCallsVariable = "procedure p; variable v; Select p such that Calls*(v, p)";
		auto wrapperCallsVariable = [&queryCallsVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsVariable);

		std::string queryCallsConst = "procedure p; constant c; Select p such that Calls*(c, p)";
		auto wrapperCallsConst = [&queryCallsConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsConst);

		std::string queryCallsStmt = "procedure p; stmt s; Select p such that Calls*(s, p)";
		auto wrapperCallsStmt = [&queryCallsStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsStmt);

		std::string queryCallsRead = "procedure p; read r; Select p such that Calls*(r, p)";
		auto wrapperCallsRead = [&queryCallsRead, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsRead);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsRead);

		std::string queryCallsPrint = "procedure p; print pn; Select p such that Calls*(pn, p)";
		auto wrapperCallsPrint = [&queryCallsPrint, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsPrint);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsPrint);

		std::string queryCallsCall = "procedure p; call c; Select p such that Calls*(c, p)";
		auto wrapperCallsCall = [&queryCallsCall, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsCall);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsCall);

		std::string queryCallsWhile = "procedure p; while w; Select p such that Calls*(w, p)";
		auto wrapperCallsWhile = [&queryCallsWhile, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsWhile);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsWhile);

		std::string queryCallsIf = "procedure p; if ifs; Select p such that Calls*(ifs, p)";
		auto wrapperCallsIf = [&queryCallsIf, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsIf);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsIf);

		std::string queryCallsAssign = "procedure p; assign a; Select p such that Calls*(a, p)";
		auto wrapperCallsAssign = [&queryCallsAssign, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsAssign);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsAssign);

		std::string queryCallsInteger = "procedure p; constant c; Select p such that Calls*(1, p)";
		auto wrapperCallsInteger = [&queryCallsInteger, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsInteger);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsInteger);
	}

	TEST_METHOD(parseQuery_callsTSecondArgNonProcSynonym_exceptionThrown) {
		
		std::string queryCallsVariable = "procedure p; variable v; Select p such that Calls*(p, v)";
		auto wrapperCallsVariable = [&queryCallsVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsVariable);

		std::string queryCallsConst = "procedure p; constant c; Select p such that Calls*(p, c)";
		auto wrapperCallsConst = [&queryCallsConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsConst);

		std::string queryCallsStmt = "procedure p; stmt s; Select p such that Calls*(p, s)";
		auto wrapperCallsStmt = [&queryCallsStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsStmt);

		std::string queryCallsRead = "procedure p; read r; Select p such that Calls*(p, r)";
		auto wrapperCallsRead = [&queryCallsRead, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsRead);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsRead);

		std::string queryCallsPrint = "procedure p; print pn; Select p such that Calls*(p, pn)";
		auto wrapperCallsPrint = [&queryCallsPrint, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsPrint);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsPrint);

		std::string queryCallsCall = "procedure p; call c; Select p such that Calls*(p, c)";
		auto wrapperCallsCall = [&queryCallsCall, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsCall);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsCall);

		std::string queryCallsWhile = "procedure p; while w; Select p such that Calls*(p, w)";
		auto wrapperCallsWhile = [&queryCallsWhile, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsWhile);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsWhile);

		std::string queryCallsIf = "procedure p; if ifs; Select p such that Calls*(p, ifs)";
		auto wrapperCallsIf = [&queryCallsIf, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsIf);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsIf);

		std::string queryCallsAssign = "procedure p; assign a; Select p such that Calls*(p, a)";
		auto wrapperCallsAssign = [&queryCallsAssign, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsAssign);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsAssign);

		std::string queryCallsInteger = "procedure p; constant c; Select p such that Calls*(p, 1)";
		auto wrapperCallsInteger = [&queryCallsInteger, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallsInteger);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallsInteger);
	}

	TEST_METHOD(parseQuery_nextFirstArgNonStmtSynonym_exceptionThrown) {
		
		std::string queryNextVariable = "assign a; variable v; Select a such that Next(a, v)";
		auto wrapperNextVariable = [&queryNextVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryNextVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperNextVariable);

		std::string queryNextProc = "assign a; procedure p; Select a such that Next(a, p)";
		auto wrapperNextProc = [&queryNextProc, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryNextProc);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperNextProc);

		std::string queryNextConst = "assign a; constant c; Select a such that Next(a, c)";
		auto wrapperNextConst = [&queryNextConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryNextConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperNextConst);

		std::string queryNextVarName = "assign a; constant c; Select a such that Next(a, \"x\")";
		auto wrapperNextVarName = [&queryNextVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryNextVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperNextVarName);
	}

	TEST_METHOD(parseQuery_nextSecondArgNonStmtSynonym_exceptionThrown) {
		
		std::string queryNextVariable = "assign a; variable v; Select a such that Next(v, a)";
		auto wrapperNextVariable = [&queryNextVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryNextVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperNextVariable);

		std::string queryNextProc = "assign a; procedure p; Select a such that Next(p, a)";
		auto wrapperNextProc = [&queryNextProc, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryNextProc);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperNextProc);

		std::string queryNextConst = "assign a; constant c; Select a such that Next(c, a)";
		auto wrapperNextConst = [&queryNextConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryNextConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperNextConst);

		std::string queryNextVarName = "assign a; constant c; Select a such that Next(\"x\", a)";
		auto wrapperNextVarName = [&queryNextVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryNextVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperNextVarName);
	}

	TEST_METHOD(parseQuery_nextTFirstArgNonStmtSynonym_exceptionThrown) {
		
		std::string queryNextVariable = "assign a; variable v; Select a such that Next*(a, v)";
		auto wrapperNextVariable = [&queryNextVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryNextVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperNextVariable);

		std::string queryNextProc = "assign a; procedure p; Select a such that Next*(a, p)";
		auto wrapperNextProc = [&queryNextProc, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryNextProc);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperNextProc);

		std::string queryNextConst = "assign a; constant c; Select a such that Next*(a, c)";
		auto wrapperNextConst = [&queryNextConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryNextConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperNextConst);

		std::string queryNextVarName = "assign a; constant c; Select a such that Next*(a, \"x\")";
		auto wrapperNextVarName = [&queryNextVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryNextVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperNextVarName);
	}

	TEST_METHOD(parseQuery_nextTSecondArgNonStmtSynonym_exceptionThrown) {
		
		std::string queryNextVariable = "assign a; variable v; Select a such that Next*(v, a)";
		auto wrapperNextVariable = [&queryNextVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryNextVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperNextVariable);

		std::string queryNextProc = "assign a; procedure p; Select a such that Next*(p, a)";
		auto wrapperNextProc = [&queryNextProc, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryNextProc);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperNextProc);

		std::string queryNextConst = "assign a; constant c; Select a such that Next*(c, a)";
		auto wrapperNextConst = [&queryNextConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryNextConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperNextConst);

		std::string queryNextVarName = "assign a; constant c; Select a such that Next*(\"x\", a)";
		auto wrapperNextVarName = [&queryNextVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryNextVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperNextVarName);
	}

	TEST_METHOD(parseQuery_affectsFirstArgNonAssignSynonym_exceptionThrown) {
		std::string queryAffectsVariable = "assign a; variable v; Select a such that Affects(v, a)";
		auto wrapperAffectsVariable = [&queryAffectsVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsVariable);

		std::string queryAffectsConst = "assign a; constant c; Select a such that Affects(c, a)";
		auto wrapperAffectsConst = [&queryAffectsConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsConst);

		std::string queryAffectsRead = "assign a; read r; Select a such that Affects(r, a)";
		auto wrapperAffectsRead = [&queryAffectsRead, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsRead);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsRead);

		std::string queryAffectsPrint = "assign a; print pn; Select a such that Affects(pn, a)";
		auto wrapperAffectsPrint = [&queryAffectsPrint, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsPrint);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsPrint);

		std::string queryAffectsCall = "assign a; call c; Select a such that Affects(c, a)";
		auto wrapperAffectsCall = [&queryAffectsCall, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsCall);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsCall);

		std::string queryAffectsWhile = "assign a; while w; Select a such that Affects(w, a)";
		auto wrapperAffectsWhile = [&queryAffectsWhile, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsWhile);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsWhile);

		std::string queryAffectsIf = "assign a; if ifs; Select a such that Affects(ifs, a)";
		auto wrapperAffectsIf = [&queryAffectsIf, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsIf);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsIf);

		std::string queryAffectsAssign = "assign a; procedure p; Select a such that Affects(p, a)";
		auto wrapperAffectsAssign = [&queryAffectsAssign, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsAssign);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsAssign);

		std::string queryAffectsIdent = "assign a; constant c; Select a such that Affects(\"abc\", a)";
		auto wrapperAffectsIdent = [&queryAffectsIdent, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsIdent);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsIdent);
	}

	TEST_METHOD(parseQuery_affectsSecondArgNonProcSynonym_exceptionThrown) {
		
		std::string queryAffectsVariable = "assign a; variable v; Select a such that Affects(a, v)";
		auto wrapperAffectsVariable = [&queryAffectsVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsVariable);

		std::string queryAffectsConst = "assign a; constant c; Select a such that Affects(a, c)";
		auto wrapperAffectsConst = [&queryAffectsConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsConst);

		std::string queryAffectsRead = "assign a; read r; Select a such that Affects(a, r)";
		auto wrapperAffectsRead = [&queryAffectsRead, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsRead);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsRead);

		std::string queryAffectsPrint = "assign a; print pn; Select a such that Affects(a, pn)";
		auto wrapperAffectsPrint = [&queryAffectsPrint, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsPrint);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsPrint);

		std::string queryAffectsCall = "assign a; call c; Select a such that Affects(a, c)";
		auto wrapperAffectsCall = [&queryAffectsCall, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsCall);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsCall);

		std::string queryAffectsWhile = "assign a; while w; Select a such that Affects(a, w)";
		auto wrapperAffectsWhile = [&queryAffectsWhile, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsWhile);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsWhile);

		std::string queryAffectsIf = "assign a; if ifs; Select a such that Affects(a, ifs)";
		auto wrapperAffectsIf = [&queryAffectsIf, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsIf);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsIf);

		std::string queryAffectsAssign = "assign a; procedure p; Select a such that Affects(a, p)";
		auto wrapperAffectsAssign = [&queryAffectsAssign, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsAssign);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsAssign);

		std::string queryAffectsIdent = "assign a; constant c; Select a such that Affects(a, \"abc\")";
		auto wrapperAffectsIdent = [&queryAffectsIdent, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsIdent);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsIdent);
	}

	TEST_METHOD(parseQuery_affectsTFirstArgNonAssignSynonym_exceptionThrown) {
		
		std::string queryAffectsVariable = "assign a; variable v; Select a such that Affects*(v, a)";
		auto wrapperAffectsVariable = [&queryAffectsVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsVariable);

		std::string queryAffectsConst = "assign a; constant c; Select a such that Affects*(c, a)";
		auto wrapperAffectsConst = [&queryAffectsConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsConst);

		std::string queryAffectsRead = "assign a; read r; Select a such that Affects*(r, a)";
		auto wrapperAffectsRead = [&queryAffectsRead, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsRead);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsRead);

		std::string queryAffectsPrint = "assign a; print pn; Select a such that Affects*(pn, a)";
		auto wrapperAffectsPrint = [&queryAffectsPrint, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsPrint);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsPrint);

		std::string queryAffectsCall = "assign a; call c; Select a such that Affects*(c, a)";
		auto wrapperAffectsCall = [&queryAffectsCall, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsCall);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsCall);

		std::string queryAffectsWhile = "assign a; while w; Select a such that Affects*(w, a)";
		auto wrapperAffectsWhile = [&queryAffectsWhile, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsWhile);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsWhile);

		std::string queryAffectsIf = "assign a; if ifs; Select a such that Affects*(ifs, a)";
		auto wrapperAffectsIf = [&queryAffectsIf, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsIf);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsIf);

		std::string queryAffectsAssign = "assign a; procedure p; Select a such that Affects*(p, a)";
		auto wrapperAffectsAssign = [&queryAffectsAssign, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsAssign);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsAssign);

		std::string queryAffectsIdent = "assign a; constant c; Select a such that Affects*(\"abc\", a)";
		auto wrapperAffectsIdent = [&queryAffectsIdent, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsIdent);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsIdent);
	}

	TEST_METHOD(parseQuery_affectsTSecondArgNonProcSynonym_exceptionThrown) {
		
		std::string queryAffectsVariable = "assign a; variable v; Select a such that Affects*(a, v)";
		auto wrapperAffectsVariable = [&queryAffectsVariable, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsVariable);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsVariable);

		std::string queryAffectsConst = "assign a; constant c; Select a such that Affects*(a, c)";
		auto wrapperAffectsConst = [&queryAffectsConst, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsConst);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsConst);

		std::string queryAffectsRead = "assign a; read r; Select a such that Affects*(a, r)";
		auto wrapperAffectsRead = [&queryAffectsRead, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsRead);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsRead);

		std::string queryAffectsPrint = "assign a; print pn; Select a such that Affects*(a, pn)";
		auto wrapperAffectsPrint = [&queryAffectsPrint, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsPrint);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsPrint);

		std::string queryAffectsCall = "assign a; call c; Select a such that Affects*(a, c)";
		auto wrapperAffectsCall = [&queryAffectsCall, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsCall);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsCall);

		std::string queryAffectsWhile = "assign a; while w; Select a such that Affects*(a, w)";
		auto wrapperAffectsWhile = [&queryAffectsWhile, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsWhile);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsWhile);

		std::string queryAffectsIf = "assign a; if ifs; Select a such that Affects*(a, ifs)";
		auto wrapperAffectsIf = [&queryAffectsIf, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsIf);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsIf);

		std::string queryAffectsAssign = "assign a; procedure p; Select a such that Affects*(a, p)";
		auto wrapperAffectsAssign = [&queryAffectsAssign, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsAssign);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsAssign);

		std::string queryAffectsIdent = "assign a; constant c; Select a such that Affects*(a, \"abc\")";
		auto wrapperAffectsIdent = [&queryAffectsIdent, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAffectsIdent);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAffectsIdent);
	}

	TEST_METHOD(parseQuery_clausesSuchThatSuchThat_allClausesExtracted) {
		
		std::string query = "stmt s; variable v; Select s such that Uses(s, v) and Modifies(s, v)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(6), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(UsesSInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(ModifiesSInstruction); }));
	}

	TEST_METHOD(parseQuery_clausesSuchThatPattern_allClausesExtracted) {
		
		std::string query = "assign a; stmt s; Select a such that Follows(s, s) pattern a(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(4), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(FollowsInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(PatternAInstruction); }));
	}

	TEST_METHOD(parseQuery_clausesSuchThatWith_allClausesExtracted) {
		
		std::string query = "stmt s; constant c; Select s such that Follows*(s, s) with s.stmt# = c.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(4), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(FollowsStarInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_clausesPatternSuchThat_allClausesExtracted) {
		
		std::string query = "if ifs; Select ifs pattern ifs(_, _, _) such that Parent(ifs, ifs)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(4), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(PatternIInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(ParentInstruction); }));
	}

	TEST_METHOD(parseQuery_clausesPatternPattern_allClausesExtracted) {
		
		std::string query = "assign a; while w; Select a pattern a(_, _) and w(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(2), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(PatternAInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(PatternWInstruction); }));
	}

	TEST_METHOD(parseQuery_clausesPatternWith_allClausesExtracted) {
		
		std::string query = "if ifs; Select ifs pattern ifs(_, _, _) with ifs.stmt# = ifs.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(2), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(PatternIInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_clausesWithSuchThat_allClausesExtracted) {
		
		std::string query = "procedure p; Select p with \"proc\"= p.procName such that Calls(p, p)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(4), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(CallsInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_clausesWithPattern_allClausesExtracted) {
		
		std::string query = "assign a; Select a with a.stmt# = a.stmt# pattern a(_, _)";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(2), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(PatternAInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_clausesWithWith_allClausesExtracted) {
		
		std::string query = "print pn; read r; procedure p; Select p with p.procName = pn.varName and r.stmt# = pn.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(2), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_patternAInvalidNumOfArgs_exceptionThrown) {
		
		std::string query = "assign a; Select a pattern a(_, _, _)";
		auto wrapperFunc = [&query, this] {
			ParsedQuery pq = PQLParser::parseQuery(query);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_patternWInvalidNumOfArgs_exceptionThrown) {
		
		std::string query = "while w; Select w pattern w(_, _, _)";
		auto wrapperFunc = [&query, this] {
			ParsedQuery pq = PQLParser::parseQuery(query);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_patternIInvalidNumOfArgs_exceptionThrown) {
		
		std::string query = "if ifs; Select ifs pattern ifs(_, _)";
		auto wrapperFunc = [&query, this] {
			ParsedQuery pq = PQLParser::parseQuery(query);
			pq.getClauseInstructions(processors, pkbGetter); 
		};
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_patternInvalidDesignEntities_exceptionThrown) {
		
		std::string queryStmtTwoArgs = "stmt s; Select s pattern s(_, _)";
		auto wrapperStmtTwoArgs = [&queryStmtTwoArgs, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryStmtTwoArgs);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperStmtTwoArgs);

		std::string queryStmtThreeArgs = "stmt s; Select s pattern s(_, _)";
		auto wrapperStmtThreeArgs = [&queryStmtThreeArgs, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryStmtThreeArgs);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperStmtThreeArgs);

		std::string queryReadTwoArgs = "read r; Select r pattern r(_, _)";
		auto wrapperReadTwoArgs = [&queryReadTwoArgs, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryReadTwoArgs);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperReadTwoArgs);

		std::string queryReadThreeArgs = "read r; Select r pattern r(_, _)";
		auto wrapperReadThreeArgs = [&queryReadThreeArgs, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryReadThreeArgs);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperReadThreeArgs);

		std::string queryPrintTwoArgs = "print p; Select p pattern p(_, _)";
		auto wrapperPrintTwoArgs = [&queryPrintTwoArgs, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrintTwoArgs);
			pq.getClauseInstructions(processors, pkbGetter); 
		};
		Assert::ExpectException<QPSException>(wrapperPrintTwoArgs);

		std::string queryPrintThreeArgs = "print p; Select p pattern p(_, _)";
		auto wrapperPrintThreeArgs = [&queryPrintThreeArgs, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrintThreeArgs);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperPrintThreeArgs);

		std::string queryCallTwoArgs = "call c; Select c pattern c(_, _)";
		auto wrapperCallTwoArgs = [&queryCallTwoArgs, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallTwoArgs);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallTwoArgs);

		std::string queryCallThreeArgs = "call c; Select c pattern c(_, _)";
		auto wrapperCallThreeArgs = [&queryCallThreeArgs, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallThreeArgs);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallThreeArgs);

		std::string queryVariableTwoArgs = "variable v; Select v pattern v(_, _)";
		auto wrapperVariableTwoArgs = [&queryVariableTwoArgs, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryVariableTwoArgs);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperVariableTwoArgs);

		std::string queryVariableThreeArgs = "variable v; Select v pattern v(_, _)";
		auto wrapperVariableThreeArgs = [&queryVariableThreeArgs, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryVariableThreeArgs);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperVariableThreeArgs);

		std::string queryConstantTwoArgs = "constant c; Select c pattern c(_, _)";
		auto wrapperConstantTwoArgs = [&queryConstantTwoArgs, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryConstantTwoArgs);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperConstantTwoArgs);

		std::string queryConstantThreeArgs = "constant c; Select c pattern c(_, _)";
		auto wrapperConstantThreeArgs = [&queryConstantThreeArgs, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryConstantThreeArgs);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperConstantThreeArgs);

		std::string queryProcedureTwoArgs = "procedure p; Select p pattern p(_, _)";
		auto wrapperProcedureTwoArgs = [&queryProcedureTwoArgs, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryProcedureTwoArgs);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperProcedureTwoArgs);

		std::string queryProcedureThreeArgs = "procedure p; Select p pattern p(_, _)";
		auto wrapperProcedureThreeArgs = [&queryProcedureThreeArgs, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryProcedureThreeArgs);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperProcedureThreeArgs);
	}

	TEST_METHOD(parseQuery_suchThatTrailingAnd_exceptionThrown) {
		std::string query = "stmt s; Select s such that Modifies(s, _) and";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_patternTrailingAnd_exceptionThrown) {
		std::string query = "assign a; Select a pattern a(_, _) and";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_withTrailingAnd_exceptionThrown) {
		std::string query = "procedure p; Select p with p.procName = \"hello\" and";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_suchThatLeadingAnd_exceptionThrown) {
		std::string query = "stmt s; Select s and Uses(s, _)";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_patternLeadingAnd_exceptionThrown) {
		std::string query = "if ifs; Select ifs and ifs(_, _)";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_withLeadingAnd_exceptionThrown) {
		std::string query = "print pn; Select pn and pn.stmt# = 10";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_withInvalidAttributes_exceptionThrown) {
		
		std::string queryStmtProcName = "stmt s; Select s with s.procName = s.procName";
		auto wrapperStmtProcName = [&queryStmtProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryStmtProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperStmtProcName);

		std::string queryStmtVarName = "stmt s; Select s with s.varName = s.varName";
		auto wrapperStmtVarName = [&queryStmtVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryStmtVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperStmtVarName);

		std::string queryStmtValue = "stmt s; Select s with s.value = s.value";
		auto wrapperStmtValue = [&queryStmtValue, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryStmtValue);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperStmtValue);

		std::string queryReadProcName = "read r; Select r with r.procName = r.procName";
		auto wrapperReadProcName = [&queryReadProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryReadProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperReadProcName);

		std::string queryReadValue = "read r; Select r with r.value = r.value";
		auto wrapperReadValue = [&queryReadValue, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryReadValue);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperReadValue);

		std::string queryPrintProcName = "print p; Select p with p.procName = p.procName";
		auto wrapperPrintProcName = [&queryPrintProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrintProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperPrintProcName);

		std::string queryPrintValue = "print p; Select p with p.value = p.value";
		auto wrapperPrintValue = [&queryPrintValue, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrintValue);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperPrintValue);

		std::string queryCallVarName = "call c; Select c with c.varName = c.varName";
		auto wrapperCallVarName = [&queryCallVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallVarName);

		std::string queryCallValue = "call c; Select c with c.value = c.value";
		auto wrapperCallValue = [&queryCallValue, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallValue);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallValue);

		std::string queryVariableProcName = "variable v; Select v with v.procName = v.procName";
		auto wrapperVariableProcName = [&queryVariableProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryVariableProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperVariableProcName);

		std::string queryVariableValue = "variable v; Select v with v.value = v.value";
		auto wrapperVariableValue = [&queryVariableValue, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryVariableValue);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperVariableValue);

		std::string queryVariableStmt = "variable v; Select v with v.stmt# = v.stmt#";
		auto wrapperVariableStmt = [&queryVariableStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryVariableStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperVariableStmt);

		std::string queryConstantProcName = "constant c; Select c with c.procName = c.procName";
		auto wrapperConstantProcName = [&queryConstantProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryConstantProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperConstantProcName);

		std::string queryConstantVarName = "constant c; Select c with c.varName = c.varName";
		auto wrapperConstantVarName = [&queryConstantVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryConstantVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperConstantVarName);

		std::string queryConstantStmt = "constant c; Select c with c.stmt# = c.stmt#";
		auto wrapperConstantStmt = [&queryConstantStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryConstantStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperConstantStmt);

		std::string queryProcedureVarName = "procedure p; Select p with p.varName = p.varName";
		auto wrapperProcedureVarName = [&queryProcedureVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryProcedureVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperProcedureVarName);

		std::string queryProcedureValue = "procedure p; Select p with p.value = p.value";
		auto wrapperProcedureValue = [&queryProcedureValue, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryProcedureValue);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperProcedureValue);

		std::string queryProcedureStmt = "procedure p; Select p with p.stmt# = p.stmt#";
		auto wrapperProcedureStmt = [&queryProcedureStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryProcedureStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperProcedureStmt);

		std::string queryIfProcName = "if i; Select i with i.procName = i.procName";
		auto wrapperIfProcName = [&queryIfProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryIfProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperIfProcName);

		std::string queryIfVarName = "if i; Select i with i.varName = i.varName";
		auto wrapperIfVarName = [&queryIfVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryIfVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperIfVarName);

		std::string queryIfValue = "if i; Select i with i.value = i.value";
		auto wrapperIfValue = [&queryIfValue, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryIfValue);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperIfValue);

		std::string queryWhileProcName = "while w; Select w with w.procName = w.procName";
		auto wrapperWhileProcName = [&queryWhileProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryWhileProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperWhileProcName);

		std::string queryWhileVarName = "while w; Select w with w.varName = w.varName";
		auto wrapperWhileVarName = [&queryWhileVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryWhileVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperWhileVarName);

		std::string queryWhileValue = "while w; Select w with w.value = w.value";
		auto wrapperWhileValue = [&queryWhileValue, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryWhileValue);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperWhileValue);

		std::string queryAssignProcName = "assign a; Select a with a.procName = a.procName";
		auto wrapperAssignProcName = [&queryAssignProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAssignProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAssignProcName);

		std::string queryAssignVarName = "assign a; Select a with a.varName = a.varName";
		auto wrapperAssignVarName = [&queryAssignVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAssignVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAssignVarName);

		std::string queryAssignValue = "assign a; Select a with a.value = a.value";
		auto wrapperAssignValue = [&queryAssignValue, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAssignValue);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAssignValue);
	}

	TEST_METHOD(parseQuery_withIncompatibleAttributes_exceptionThrown) {
		
		std::string queryConstantValueProcedureProcName = "constant c1; procedure p2; Select c1 with c1.value = p2.procName";
		auto wrapperConstantValueProcedureProcName = [&queryConstantValueProcedureProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryConstantValueProcedureProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperConstantValueProcedureProcName);

		std::string queryProcedureProcNameConstantValue = "procedure p1; constant c2; Select p1 with p1.procName = c2.value";
		auto wrapperProcedureProcNameConstantValue = [&queryProcedureProcNameConstantValue, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryProcedureProcNameConstantValue);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperProcedureProcNameConstantValue);

		std::string queryConstantValueCallProcName = "constant c1; call c2; Select c1 with c1.value = c2.procName";
		auto wrapperConstantValueCallProcName = [&queryConstantValueCallProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryConstantValueCallProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperConstantValueCallProcName);

		std::string queryCallProcNameConstantValue = "call c1; constant c2; Select c1 with c1.procName = c2.value";
		auto wrapperCallProcNameConstantValue = [&queryCallProcNameConstantValue, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallProcNameConstantValue);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallProcNameConstantValue);

		std::string queryConstantValueVariableVarName = "constant c1; variable v2; Select c1 with c1.value = v2.varName";
		auto wrapperConstantValueVariableVarName = [&queryConstantValueVariableVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryConstantValueVariableVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperConstantValueVariableVarName);

		std::string queryVariableVarNameConstantValue = "variable v1; constant c2; Select v1 with v1.varName = c2.value";
		auto wrapperVariableVarNameConstantValue = [&queryVariableVarNameConstantValue, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryVariableVarNameConstantValue);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperVariableVarNameConstantValue);

		std::string queryConstantValueReadVarName = "constant c1; read r2; Select c1 with c1.value = r2.varName";
		auto wrapperConstantValueReadVarName = [&queryConstantValueReadVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryConstantValueReadVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperConstantValueReadVarName);

		std::string queryReadVarNameConstantValue = "read r1; constant c2; Select r1 with r1.varName = c2.value";
		auto wrapperReadVarNameConstantValue = [&queryReadVarNameConstantValue, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryReadVarNameConstantValue);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperReadVarNameConstantValue);

		std::string queryConstantValuePrintVarName = "constant c1; print p2; Select c1 with c1.value = p2.varName";
		auto wrapperConstantValuePrintVarName = [&queryConstantValuePrintVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryConstantValuePrintVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperConstantValuePrintVarName);

		std::string queryPrintVarNameConstantValue = "print p1; constant c2; Select p1 with p1.varName = c2.value";
		auto wrapperPrintVarNameConstantValue = [&queryPrintVarNameConstantValue, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrintVarNameConstantValue);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperPrintVarNameConstantValue);

		std::string queryStmtStmtProcedureProcName = "stmt s1; procedure p2; Select s1 with s1.stmt# = p2.procName";
		auto wrapperStmtStmtProcedureProcName = [&queryStmtStmtProcedureProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryStmtStmtProcedureProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperStmtStmtProcedureProcName);

		std::string queryProcedureProcNameStmtStmt = "procedure p1; stmt s2; Select p1 with p1.procName = s2.stmt#";
		auto wrapperProcedureProcNameStmtStmt = [&queryProcedureProcNameStmtStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryProcedureProcNameStmtStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperProcedureProcNameStmtStmt);

		std::string queryStmtStmtCallProcName = "stmt s1; call c2; Select s1 with s1.stmt# = c2.procName";
		auto wrapperStmtStmtCallProcName = [&queryStmtStmtCallProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryStmtStmtCallProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperStmtStmtCallProcName);

		std::string queryCallProcNameStmtStmt = "call c1; stmt s2; Select c1 with c1.procName = s2.stmt#";
		auto wrapperCallProcNameStmtStmt = [&queryCallProcNameStmtStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallProcNameStmtStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallProcNameStmtStmt);

		std::string queryStmtStmtVariableVarName = "stmt s1; variable v2; Select s1 with s1.stmt# = v2.varName";
		auto wrapperStmtStmtVariableVarName = [&queryStmtStmtVariableVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryStmtStmtVariableVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperStmtStmtVariableVarName);

		std::string queryVariableVarNameStmtStmt = "variable v1; stmt s2; Select v1 with v1.varName = s2.stmt#";
		auto wrapperVariableVarNameStmtStmt = [&queryVariableVarNameStmtStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryVariableVarNameStmtStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperVariableVarNameStmtStmt);

		std::string queryStmtStmtReadVarName = "stmt s1; read r2; Select s1 with s1.stmt# = r2.varName";
		auto wrapperStmtStmtReadVarName = [&queryStmtStmtReadVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryStmtStmtReadVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperStmtStmtReadVarName);

		std::string queryReadVarNameStmtStmt = "read r1; stmt s2; Select r1 with r1.varName = s2.stmt#";
		auto wrapperReadVarNameStmtStmt = [&queryReadVarNameStmtStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryReadVarNameStmtStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperReadVarNameStmtStmt);

		std::string queryStmtStmtPrintVarName = "stmt s1; print p2; Select s1 with s1.stmt# = p2.varName";
		auto wrapperStmtStmtPrintVarName = [&queryStmtStmtPrintVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryStmtStmtPrintVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperStmtStmtPrintVarName);

		std::string queryPrintVarNameStmtStmt = "print p1; stmt s2; Select p1 with p1.varName = s2.stmt#";
		auto wrapperPrintVarNameStmtStmt = [&queryPrintVarNameStmtStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrintVarNameStmtStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperPrintVarNameStmtStmt);

		std::string queryReadStmtProcedureProcName = "read r1; procedure p2; Select r1 with r1.stmt# = p2.procName";
		auto wrapperReadStmtProcedureProcName = [&queryReadStmtProcedureProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryReadStmtProcedureProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperReadStmtProcedureProcName);

		std::string queryProcedureProcNameReadStmt = "procedure p1; read r2; Select p1 with p1.procName = r2.stmt#";
		auto wrapperProcedureProcNameReadStmt = [&queryProcedureProcNameReadStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryProcedureProcNameReadStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperProcedureProcNameReadStmt);

		std::string queryReadStmtCallProcName = "read r1; call c2; Select r1 with r1.stmt# = c2.procName";
		auto wrapperReadStmtCallProcName = [&queryReadStmtCallProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryReadStmtCallProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperReadStmtCallProcName);

		std::string queryCallProcNameReadStmt = "call c1; read r2; Select c1 with c1.procName = r2.stmt#";
		auto wrapperCallProcNameReadStmt = [&queryCallProcNameReadStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallProcNameReadStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallProcNameReadStmt);

		std::string queryReadStmtVariableVarName = "read r1; variable v2; Select r1 with r1.stmt# = v2.varName";
		auto wrapperReadStmtVariableVarName = [&queryReadStmtVariableVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryReadStmtVariableVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperReadStmtVariableVarName);

		std::string queryVariableVarNameReadStmt = "variable v1; read r2; Select v1 with v1.varName = r2.stmt#";
		auto wrapperVariableVarNameReadStmt = [&queryVariableVarNameReadStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryVariableVarNameReadStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperVariableVarNameReadStmt);

		std::string queryReadStmtReadVarName = "read r1; read r2; Select r1 with r1.stmt# = r2.varName";
		auto wrapperReadStmtReadVarName = [&queryReadStmtReadVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryReadStmtReadVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperReadStmtReadVarName);

		std::string queryReadVarNameReadStmt = "read r1; read r2; Select r1 with r1.varName = r2.stmt#";
		auto wrapperReadVarNameReadStmt = [&queryReadVarNameReadStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryReadVarNameReadStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperReadVarNameReadStmt);

		std::string queryReadStmtPrintVarName = "read r1; print p2; Select r1 with r1.stmt# = p2.varName";
		auto wrapperReadStmtPrintVarName = [&queryReadStmtPrintVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryReadStmtPrintVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperReadStmtPrintVarName);

		std::string queryPrintVarNameReadStmt = "print p1; read r2; Select p1 with p1.varName = r2.stmt#";
		auto wrapperPrintVarNameReadStmt = [&queryPrintVarNameReadStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrintVarNameReadStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperPrintVarNameReadStmt);

		std::string queryPrintStmtProcedureProcName = "print p1; procedure p2; Select p1 with p1.stmt# = p2.procName";
		auto wrapperPrintStmtProcedureProcName = [&queryPrintStmtProcedureProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrintStmtProcedureProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperPrintStmtProcedureProcName);

		std::string queryProcedureProcNamePrintStmt = "procedure p1; print p2; Select p1 with p1.procName = p2.stmt#";
		auto wrapperProcedureProcNamePrintStmt = [&queryProcedureProcNamePrintStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryProcedureProcNamePrintStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperProcedureProcNamePrintStmt);

		std::string queryPrintStmtCallProcName = "print p1; call c2; Select p1 with p1.stmt# = c2.procName";
		auto wrapperPrintStmtCallProcName = [&queryPrintStmtCallProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrintStmtCallProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperPrintStmtCallProcName);

		std::string queryCallProcNamePrintStmt = "call c1; print p2; Select c1 with c1.procName = p2.stmt#";
		auto wrapperCallProcNamePrintStmt = [&queryCallProcNamePrintStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallProcNamePrintStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallProcNamePrintStmt);

		std::string queryPrintStmtVariableVarName = "print p1; variable v2; Select p1 with p1.stmt# = v2.varName";
		auto wrapperPrintStmtVariableVarName = [&queryPrintStmtVariableVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrintStmtVariableVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperPrintStmtVariableVarName);

		std::string queryVariableVarNamePrintStmt = "variable v1; print p2; Select v1 with v1.varName = p2.stmt#";
		auto wrapperVariableVarNamePrintStmt = [&queryVariableVarNamePrintStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryVariableVarNamePrintStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperVariableVarNamePrintStmt);

		std::string queryPrintStmtReadVarName = "print p1; read r2; Select p1 with p1.stmt# = r2.varName";
		auto wrapperPrintStmtReadVarName = [&queryPrintStmtReadVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrintStmtReadVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperPrintStmtReadVarName);

		std::string queryReadVarNamePrintStmt = "read r1; print p2; Select r1 with r1.varName = p2.stmt#";
		auto wrapperReadVarNamePrintStmt = [&queryReadVarNamePrintStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryReadVarNamePrintStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperReadVarNamePrintStmt);

		std::string queryPrintStmtPrintVarName = "print p1; print p2; Select p1 with p1.stmt# = p2.varName";
		auto wrapperPrintStmtPrintVarName = [&queryPrintStmtPrintVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrintStmtPrintVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperPrintStmtPrintVarName);

		std::string queryPrintVarNamePrintStmt = "print p1; print p2; Select p1 with p1.varName = p2.stmt#";
		auto wrapperPrintVarNamePrintStmt = [&queryPrintVarNamePrintStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrintVarNamePrintStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperPrintVarNamePrintStmt);

		std::string queryCallStmtProcedureProcName = "call c1; procedure p2; Select c1 with c1.stmt# = p2.procName";
		auto wrapperCallStmtProcedureProcName = [&queryCallStmtProcedureProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallStmtProcedureProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallStmtProcedureProcName);

		std::string queryProcedureProcNameCallStmt = "procedure p1; call c2; Select p1 with p1.procName = c2.stmt#";
		auto wrapperProcedureProcNameCallStmt = [&queryProcedureProcNameCallStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryProcedureProcNameCallStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperProcedureProcNameCallStmt);

		std::string queryCallStmtCallProcName = "call c1; call c2; Select c1 with c1.stmt# = c2.procName";
		auto wrapperCallStmtCallProcName = [&queryCallStmtCallProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallStmtCallProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallStmtCallProcName);

		std::string queryCallProcNameCallStmt = "call c1; call c2; Select c1 with c1.procName = c2.stmt#";
		auto wrapperCallProcNameCallStmt = [&queryCallProcNameCallStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallProcNameCallStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallProcNameCallStmt);

		std::string queryCallStmtVariableVarName = "call c1; variable v2; Select c1 with c1.stmt# = v2.varName";
		auto wrapperCallStmtVariableVarName = [&queryCallStmtVariableVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallStmtVariableVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallStmtVariableVarName);

		std::string queryVariableVarNameCallStmt = "variable v1; call c2; Select v1 with v1.varName = c2.stmt#";
		auto wrapperVariableVarNameCallStmt = [&queryVariableVarNameCallStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryVariableVarNameCallStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperVariableVarNameCallStmt);

		std::string queryCallStmtReadVarName = "call c1; read r2; Select c1 with c1.stmt# = r2.varName";
		auto wrapperCallStmtReadVarName = [&queryCallStmtReadVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallStmtReadVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallStmtReadVarName);

		std::string queryReadVarNameCallStmt = "read r1; call c2; Select r1 with r1.varName = c2.stmt#";
		auto wrapperReadVarNameCallStmt = [&queryReadVarNameCallStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryReadVarNameCallStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperReadVarNameCallStmt);

		std::string queryCallStmtPrintVarName = "call c1; print p2; Select c1 with c1.stmt# = p2.varName";
		auto wrapperCallStmtPrintVarName = [&queryCallStmtPrintVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallStmtPrintVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallStmtPrintVarName);

		std::string queryPrintVarNameCallStmt = "print p1; call c2; Select p1 with p1.varName = c2.stmt#";
		auto wrapperPrintVarNameCallStmt = [&queryPrintVarNameCallStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrintVarNameCallStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperPrintVarNameCallStmt);

		std::string queryWhileStmtProcedureProcName = "while w1; procedure p2; Select w1 with w1.stmt# = p2.procName";
		auto wrapperWhileStmtProcedureProcName = [&queryWhileStmtProcedureProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryWhileStmtProcedureProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperWhileStmtProcedureProcName);

		std::string queryProcedureProcNameWhileStmt = "procedure p1; while w2; Select p1 with p1.procName = w2.stmt#";
		auto wrapperProcedureProcNameWhileStmt = [&queryProcedureProcNameWhileStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryProcedureProcNameWhileStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperProcedureProcNameWhileStmt);

		std::string queryWhileStmtCallProcName = "while w1; call c2; Select w1 with w1.stmt# = c2.procName";
		auto wrapperWhileStmtCallProcName = [&queryWhileStmtCallProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryWhileStmtCallProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperWhileStmtCallProcName);

		std::string queryCallProcNameWhileStmt = "call c1; while w2; Select c1 with c1.procName = w2.stmt#";
		auto wrapperCallProcNameWhileStmt = [&queryCallProcNameWhileStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallProcNameWhileStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallProcNameWhileStmt);

		std::string queryWhileStmtVariableVarName = "while w1; variable v2; Select w1 with w1.stmt# = v2.varName";
		auto wrapperWhileStmtVariableVarName = [&queryWhileStmtVariableVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryWhileStmtVariableVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperWhileStmtVariableVarName);

		std::string queryVariableVarNameWhileStmt = "variable v1; while w2; Select v1 with v1.varName = w2.stmt#";
		auto wrapperVariableVarNameWhileStmt = [&queryVariableVarNameWhileStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryVariableVarNameWhileStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperVariableVarNameWhileStmt);

		std::string queryWhileStmtReadVarName = "while w1; read r2; Select w1 with w1.stmt# = r2.varName";
		auto wrapperWhileStmtReadVarName = [&queryWhileStmtReadVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryWhileStmtReadVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperWhileStmtReadVarName);

		std::string queryReadVarNameWhileStmt = "read r1; while w2; Select r1 with r1.varName = w2.stmt#";
		auto wrapperReadVarNameWhileStmt = [&queryReadVarNameWhileStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryReadVarNameWhileStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperReadVarNameWhileStmt);

		std::string queryWhileStmtPrintVarName = "while w1; print p2; Select w1 with w1.stmt# = p2.varName";
		auto wrapperWhileStmtPrintVarName = [&queryWhileStmtPrintVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryWhileStmtPrintVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperWhileStmtPrintVarName);

		std::string queryPrintVarNameWhileStmt = "print p1; while w2; Select p1 with p1.varName = w2.stmt#";
		auto wrapperPrintVarNameWhileStmt = [&queryPrintVarNameWhileStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrintVarNameWhileStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperPrintVarNameWhileStmt);

		std::string queryIfStmtProcedureProcName = "if i1; procedure p2; Select i1 with i1.stmt# = p2.procName";
		auto wrapperIfStmtProcedureProcName = [&queryIfStmtProcedureProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryIfStmtProcedureProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperIfStmtProcedureProcName);

		std::string queryProcedureProcNameIfStmt = "procedure p1; if i2; Select p1 with p1.procName = i2.stmt#";
		auto wrapperProcedureProcNameIfStmt = [&queryProcedureProcNameIfStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryProcedureProcNameIfStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperProcedureProcNameIfStmt);

		std::string queryIfStmtCallProcName = "if i1; call c2; Select i1 with i1.stmt# = c2.procName";
		auto wrapperIfStmtCallProcName = [&queryIfStmtCallProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryIfStmtCallProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperIfStmtCallProcName);

		std::string queryCallProcNameIfStmt = "call c1; if i2; Select c1 with c1.procName = i2.stmt#";
		auto wrapperCallProcNameIfStmt = [&queryCallProcNameIfStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallProcNameIfStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallProcNameIfStmt);

		std::string queryIfStmtVariableVarName = "if i1; variable v2; Select i1 with i1.stmt# = v2.varName";
		auto wrapperIfStmtVariableVarName = [&queryIfStmtVariableVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryIfStmtVariableVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperIfStmtVariableVarName);

		std::string queryVariableVarNameIfStmt = "variable v1; if i2; Select v1 with v1.varName = i2.stmt#";
		auto wrapperVariableVarNameIfStmt = [&queryVariableVarNameIfStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryVariableVarNameIfStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperVariableVarNameIfStmt);

		std::string queryIfStmtReadVarName = "if i1; read r2; Select i1 with i1.stmt# = r2.varName";
		auto wrapperIfStmtReadVarName = [&queryIfStmtReadVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryIfStmtReadVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperIfStmtReadVarName);

		std::string queryReadVarNameIfStmt = "read r1; if i2; Select r1 with r1.varName = i2.stmt#";
		auto wrapperReadVarNameIfStmt = [&queryReadVarNameIfStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryReadVarNameIfStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperReadVarNameIfStmt);

		std::string queryIfStmtPrintVarName = "if i1; print p2; Select i1 with i1.stmt# = p2.varName";
		auto wrapperIfStmtPrintVarName = [&queryIfStmtPrintVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryIfStmtPrintVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperIfStmtPrintVarName);

		std::string queryPrintVarNameIfStmt = "print p1; if i2; Select p1 with p1.varName = i2.stmt#";
		auto wrapperPrintVarNameIfStmt = [&queryPrintVarNameIfStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrintVarNameIfStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperPrintVarNameIfStmt);

		std::string queryAssignStmtProcedureProcName = "assign a1; procedure p2; Select a1 with a1.stmt# = p2.procName";
		auto wrapperAssignStmtProcedureProcName = [&queryAssignStmtProcedureProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAssignStmtProcedureProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAssignStmtProcedureProcName);

		std::string queryProcedureProcNameAssignStmt = "procedure p1; assign a2; Select p1 with p1.procName = a2.stmt#";
		auto wrapperProcedureProcNameAssignStmt = [&queryProcedureProcNameAssignStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryProcedureProcNameAssignStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperProcedureProcNameAssignStmt);

		std::string queryAssignStmtCallProcName = "assign a1; call c2; Select a1 with a1.stmt# = c2.procName";
		auto wrapperAssignStmtCallProcName = [&queryAssignStmtCallProcName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAssignStmtCallProcName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAssignStmtCallProcName);

		std::string queryCallProcNameAssignStmt = "call c1; assign a2; Select c1 with c1.procName = a2.stmt#";
		auto wrapperCallProcNameAssignStmt = [&queryCallProcNameAssignStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryCallProcNameAssignStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperCallProcNameAssignStmt);

		std::string queryAssignStmtVariableVarName = "assign a1; variable v2; Select a1 with a1.stmt# = v2.varName";
		auto wrapperAssignStmtVariableVarName = [&queryAssignStmtVariableVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAssignStmtVariableVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAssignStmtVariableVarName);

		std::string queryVariableVarNameAssignStmt = "variable v1; assign a2; Select v1 with v1.varName = a2.stmt#";
		auto wrapperVariableVarNameAssignStmt = [&queryVariableVarNameAssignStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryVariableVarNameAssignStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperVariableVarNameAssignStmt);

		std::string queryAssignStmtReadVarName = "assign a1; read r2; Select a1 with a1.stmt# = r2.varName";
		auto wrapperAssignStmtReadVarName = [&queryAssignStmtReadVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAssignStmtReadVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAssignStmtReadVarName);

		std::string queryReadVarNameAssignStmt = "read r1; assign a2; Select r1 with r1.varName = a2.stmt#";
		auto wrapperReadVarNameAssignStmt = [&queryReadVarNameAssignStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryReadVarNameAssignStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperReadVarNameAssignStmt);

		std::string queryAssignStmtPrintVarName = "assign a1; print p2; Select a1 with a1.stmt# = p2.varName";
		auto wrapperAssignStmtPrintVarName = [&queryAssignStmtPrintVarName, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryAssignStmtPrintVarName);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperAssignStmtPrintVarName);

		std::string queryPrintVarNameAssignStmt = "print p1; assign a2; Select p1 with p1.varName = a2.stmt#";
		auto wrapperPrintVarNameAssignStmt = [&queryPrintVarNameAssignStmt, this] {
			ParsedQuery pq = PQLParser::parseQuery(queryPrintVarNameAssignStmt);
			pq.getClauseInstructions(processors, pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperPrintVarNameAssignStmt);
	}

	TEST_METHOD(parseQuery_withConstantValueConstantValue_withExtracted) {
		
		std::string query = "constant c1; constant c2; Select c1 with c1.value = c2.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withConstantValueStmtStmt_withExtracted) {
		
		std::string query = "constant c1; stmt s2; Select c1 with c1.value = s2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withConstantValueReadStmt_withExtracted) {
		
		std::string query = "constant c1; read r2; Select c1 with c1.value = r2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withConstantValuePrintStmt_withExtracted) {
		
		std::string query = "constant c1; print p2; Select c1 with c1.value = p2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withConstantValueCallStmt_withExtracted) {
		
		std::string query = "constant c1; call c2; Select c1 with c1.value = c2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withConstantValueWhileStmt_withExtracted) {
		
		std::string query = "constant c1; while w2; Select c1 with c1.value = w2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withConstantValueIfStmt_withExtracted) {
		
		std::string query = "constant c1; if i2; Select c1 with c1.value = i2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withConstantValueAssignStmt_withExtracted) {
		
		std::string query = "constant c1; assign a2; Select c1 with c1.value = a2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withStmtStmtConstantValue_withExtracted) {
		
		std::string query = "stmt s1; constant c2; Select s1 with s1.stmt# = c2.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withStmtStmtStmtStmt_withExtracted) {
		
		std::string query = "stmt s1; stmt s2; Select s1 with s1.stmt# = s2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withStmtStmtReadStmt_withExtracted) {
		
		std::string query = "stmt s1; read r2; Select s1 with s1.stmt# = r2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withStmtStmtPrintStmt_withExtracted) {
		
		std::string query = "stmt s1; print p2; Select s1 with s1.stmt# = p2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withStmtStmtCallStmt_withExtracted) {
		
		std::string query = "stmt s1; call c2; Select s1 with s1.stmt# = c2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withStmtStmtWhileStmt_withExtracted) {
		
		std::string query = "stmt s1; while w2; Select s1 with s1.stmt# = w2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withStmtStmtIfStmt_withExtracted) {
		
		std::string query = "stmt s1; if i2; Select s1 with s1.stmt# = i2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withStmtStmtAssignStmt_withExtracted) {
		
		std::string query = "stmt s1; assign a2; Select s1 with s1.stmt# = a2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());

		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withReadStmtConstantValue_withExtracted) {
		
		std::string query = "read r1; constant c2; Select r1 with r1.stmt# = c2.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withReadStmtStmtStmt_withExtracted) {
		
		std::string query = "read r1; stmt s2; Select r1 with r1.stmt# = s2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withReadStmtReadStmt_withExtracted) {
		
		std::string query = "read r1; read r2; Select r1 with r1.stmt# = r2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withReadStmtPrintStmt_withExtracted) {
		
		std::string query = "read r1; print p2; Select r1 with r1.stmt# = p2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withReadStmtCallStmt_withExtracted) {
		
		std::string query = "read r1; call c2; Select r1 with r1.stmt# = c2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withReadStmtWhileStmt_withExtracted) {
		
		std::string query = "read r1; while w2; Select r1 with r1.stmt# = w2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withReadStmtIfStmt_withExtracted) {
		
		std::string query = "read r1; if i2; Select r1 with r1.stmt# = i2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withReadStmtAssignStmt_withExtracted) {
		
		std::string query = "read r1; assign a2; Select r1 with r1.stmt# = a2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withPrintStmtConstantValue_withExtracted) {
		
		std::string query = "print p1; constant c2; Select p1 with p1.stmt# = c2.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withPrintStmtStmtStmt_withExtracted) {
		
		std::string query = "print p1; stmt s2; Select p1 with p1.stmt# = s2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withPrintStmtReadStmt_withExtracted) {
		
		std::string query = "print p1; read r2; Select p1 with p1.stmt# = r2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withPrintStmtPrintStmt_withExtracted) {
		
		std::string query = "print p1; print p2; Select p1 with p1.stmt# = p2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withPrintStmtCallStmt_withExtracted) {
		
		std::string query = "print p1; call c2; Select p1 with p1.stmt# = c2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withPrintStmtWhileStmt_withExtracted) {
		
		std::string query = "print p1; while w2; Select p1 with p1.stmt# = w2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withPrintStmtIfStmt_withExtracted) {
		
		std::string query = "print p1; if i2; Select p1 with p1.stmt# = i2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withPrintStmtAssignStmt_withExtracted) {
		
		std::string query = "print p1; assign a2; Select p1 with p1.stmt# = a2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withCallStmtConstantValue_withExtracted) {
		
		std::string query = "call c1; constant c2; Select c1 with c1.stmt# = c2.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withCallStmtStmtStmt_withExtracted) {
		
		std::string query = "call c1; stmt s2; Select c1 with c1.stmt# = s2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withCallStmtReadStmt_withExtracted) {
		
		std::string query = "call c1; read r2; Select c1 with c1.stmt# = r2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withCallStmtPrintStmt_withExtracted) {
		
		std::string query = "call c1; print p2; Select c1 with c1.stmt# = p2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withCallStmtCallStmt_withExtracted) {
		
		std::string query = "call c1; call c2; Select c1 with c1.stmt# = c2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withCallStmtWhileStmt_withExtracted) {
		
		std::string query = "call c1; while w2; Select c1 with c1.stmt# = w2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withCallStmtIfStmt_withExtracted) {
		
		std::string query = "call c1; if i2; Select c1 with c1.stmt# = i2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withCallStmtAssignStmt_withExtracted) {
		
		std::string query = "call c1; assign a2; Select c1 with c1.stmt# = a2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withWhileStmtConstantValue_withExtracted) {
		
		std::string query = "while w1; constant c2; Select w1 with w1.stmt# = c2.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withWhileStmtStmtStmt_withExtracted) {
		
		std::string query = "while w1; stmt s2; Select w1 with w1.stmt# = s2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withWhileStmtReadStmt_withExtracted) {
		
		std::string query = "while w1; read r2; Select w1 with w1.stmt# = r2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withWhileStmtPrintStmt_withExtracted) {
		
		std::string query = "while w1; print p2; Select w1 with w1.stmt# = p2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withWhileStmtCallStmt_withExtracted) {
		
		std::string query = "while w1; call c2; Select w1 with w1.stmt# = c2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withWhileStmtWhileStmt_withExtracted) {
		
		std::string query = "while w1; while w2; Select w1 with w1.stmt# = w2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withWhileStmtIfStmt_withExtracted) {
		
		std::string query = "while w1; if i2; Select w1 with w1.stmt# = i2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withWhileStmtAssignStmt_withExtracted) {
		
		std::string query = "while w1; assign a2; Select w1 with w1.stmt# = a2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withIfStmtConstantValue_withExtracted) {
		
		std::string query = "if i1; constant c2; Select i1 with i1.stmt# = c2.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withIfStmtStmtStmt_withExtracted) {
		
		std::string query = "if i1; stmt s2; Select i1 with i1.stmt# = s2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withIfStmtReadStmt_withExtracted) {
		
		std::string query = "if i1; read r2; Select i1 with i1.stmt# = r2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withIfStmtPrintStmt_withExtracted) {
		
		std::string query = "if i1; print p2; Select i1 with i1.stmt# = p2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withIfStmtCallStmt_withExtracted) {
		
		std::string query = "if i1; call c2; Select i1 with i1.stmt# = c2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withIfStmtWhileStmt_withExtracted) {
		
		std::string query = "if i1; while w2; Select i1 with i1.stmt# = w2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withIfStmtIfStmt_withExtracted) {
		
		std::string query = "if i1; if i2; Select i1 with i1.stmt# = i2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withIfStmtAssignStmt_withExtracted) {
		
		std::string query = "if i1; assign a2; Select i1 with i1.stmt# = a2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withAssignStmtConstantValue_withExtracted) {
		
		std::string query = "assign a1; constant c2; Select a1 with a1.stmt# = c2.value";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withAssignStmtStmtStmt_withExtracted) {
		
		std::string query = "assign a1; stmt s2; Select a1 with a1.stmt# = s2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withAssignStmtReadStmt_withExtracted) {
		
		std::string query = "assign a1; read r2; Select a1 with a1.stmt# = r2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withAssignStmtPrintStmt_withExtracted) {
		
		std::string query = "assign a1; print p2; Select a1 with a1.stmt# = p2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withAssignStmtCallStmt_withExtracted) {
		
		std::string query = "assign a1; call c2; Select a1 with a1.stmt# = c2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withAssignStmtWhileStmt_withExtracted) {
		
		std::string query = "assign a1; while w2; Select a1 with a1.stmt# = w2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withAssignStmtIfStmt_withExtracted) {
		
		std::string query = "assign a1; if i2; Select a1 with a1.stmt# = i2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withAssignStmtAssignStmt_withExtracted) {
		
		std::string query = "assign a1; assign a2; Select a1 with a1.stmt# = a2.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size());
		Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {
			return typeid(*instruction) == typeid(WithIntegerInstruction); }));
	}

	TEST_METHOD(parseQuery_withProcedureProcNameProcedureProcName_withExtracted) {
				std::string query = "procedure p1; procedure p2; Select p1 with p1.procName = p2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withProcedureProcNameCallProcName_withExtracted) {
				std::string query = "procedure p1; call c2; Select p1 with p1.procName = c2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withProcedureProcNameVariableVarName_withExtracted) {
				std::string query = "procedure p1; variable v2; Select p1 with p1.procName = v2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withProcedureProcNameReadVarName_withExtracted) {
				std::string query = "procedure p1; read r2; Select p1 with p1.procName = r2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withProcedureProcNamePrintVarName_withExtracted) {
				std::string query = "procedure p1; print p2; Select p1 with p1.procName = p2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withCallProcNameProcedureProcName_withExtracted) {
				std::string query = "call c1; procedure p2; Select c1 with c1.procName = p2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withCallProcNameCallProcName_withExtracted) {
				std::string query = "call c1; call c2; Select c1 with c1.procName = c2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withCallProcNameVariableVarName_withExtracted) {
				std::string query = "call c1; variable v2; Select c1 with c1.procName = v2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withCallProcNameReadVarName_withExtracted) {
				std::string query = "call c1; read r2; Select c1 with c1.procName = r2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withCallProcNamePrintVarName_withExtracted) {
				std::string query = "call c1; print p2; Select c1 with c1.procName = p2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withVariableVarNameProcedureProcName_withExtracted) {
				std::string query = "variable v1; procedure p2; Select v1 with v1.varName = p2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withVariableVarNameCallProcName_withExtracted) {
				std::string query = "variable v1; call c2; Select v1 with v1.varName = c2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withVariableVarNameVariableVarName_withExtracted) {
				std::string query = "variable v1; variable v2; Select v1 with v1.varName = v2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withVariableVarNameReadVarName_withExtracted) {
				std::string query = "variable v1; read r2; Select v1 with v1.varName = r2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withVariableVarNamePrintVarName_withExtracted) {
				std::string query = "variable v1; print p2; Select v1 with v1.varName = p2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withReadVarNameProcedureProcName_withExtracted) {
				std::string query = "read r1; procedure p2; Select r1 with r1.varName = p2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withReadVarNameCallProcName_withExtracted) {
				std::string query = "read r1; call c2; Select r1 with r1.varName = c2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withReadVarNameVariableVarName_withExtracted) {
				std::string query = "read r1; variable v2; Select r1 with r1.varName = v2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withReadVarNameReadVarName_withExtracted) {
				std::string query = "read r1; read r2; Select r1 with r1.varName = r2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withReadVarNamePrintVarName_withExtracted) {
				std::string query = "read r1; print p2; Select r1 with r1.varName = p2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withPrintVarNameProcedureProcName_withExtracted) {
				std::string query = "print p1; procedure p2; Select p1 with p1.varName = p2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withPrintVarNameCallProcName_withExtracted) {
				std::string query = "print p1; call c2; Select p1 with p1.varName = c2.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withPrintVarNameVariableVarName_withExtracted) {
				std::string query = "print p1; variable v2; Select p1 with p1.varName = v2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withPrintVarNameReadVarName_withExtracted) {
				std::string query = "print p1; read r2; Select p1 with p1.varName = r2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
	}

	TEST_METHOD(parseQuery_withPrintVarNamePrintVarName_withExtracted) {
				std::string query = "print p1; print p2; Select p1 with p1.varName = p2.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		std::vector<Instruction*> instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
		Assert::AreEqual(size_t(1), instructions.size()); Assert::IsTrue(std::any_of(instructions.begin(), instructions.end(), [](Instruction* instruction) {return typeid(*instruction) == typeid(WithStringInstruction); }));
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
			[&query, this] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_tupleNoClosingBracket_exceptionThrown) {
		
		std::string query = "read r; print pn; Select <r.varName, pn.stmt#";
		auto wrapperFunc =
			[&query, this] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_tupleNoComma_exceptionThrown) {
		std::string query = "read r; print pn; Select <r.varName pn.stmt#>";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_tupleExtraComma_exceptionThrown) {
		std::string query = "read r; print pn; Select <r.varName, , pn.stmt#>";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_emptyTuple_exceptionThrown) {
		std::string query = "read r; print pn; Select <>";
		auto wrapperFunc = [&query] { PQLParser::parseQuery(query); };
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_tupleBooleanWithoutDeclaration_exceptionThrown) {
		std::string query = "read r; print pn; Select <BOOLEAN>";
		auto wrapperFunc = [&query, this] {
			ParsedQuery pq = PQLParser::parseQuery(query);
			pq.getSelectInstructions(pkbGetter);
		};
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
		std::vector<PqlReference> columns = parsedQuery.getAttributes();
		Assert::AreEqual(size_t(0), columns.size());
	}

	TEST_METHOD(parseQuery_selectBoolean_duplicateSynonymDeclaration) {
		std::string query = "stmt s; variable s; Select BOOLEAN";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		// Throws exception during query execution
		auto wrapperFunc = [&query, this] {
			ParsedQuery pq = PQLParser::parseQuery(query);
			pq.getSelectInstructions(pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFunc);

		// But projection can occur as per normal
		std::vector<PqlReference> columns = parsedQuery.getAttributes();
		Assert::AreEqual(size_t(0), columns.size());
	}

	TEST_METHOD(parseQuery_selectBoolean_duplicateBooleanSynonymDeclaration) {
		std::string query = "stmt BOOLEAN; variable BOOLEAN; Select BOOLEAN";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		// Throws exception during query execution
		auto wrapperFunc = [&query, this] {
			ParsedQuery pq = PQLParser::parseQuery(query);
			pq.getSelectInstructions(pkbGetter);
		};
		Assert::ExpectException<QPSException>(wrapperFunc);

		// But projection can occur as per normal - projects empty list
		std::vector<PqlReference> columns = parsedQuery.getAttributes();
		Assert::AreEqual(size_t(1), columns.size());
	}

	TEST_METHOD(parseQuery_selectBooleanAttribute_noDeclarations) {
		std::string query = "Select BOOLEAN.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		// Throws exception during projection
		auto wrapperFunc = [&parsedQuery] {
			parsedQuery.getAttributes();
		};
		Assert::ExpectException<QPSException>(wrapperFunc);
	}

	TEST_METHOD(parseQuery_selectBooleanAttribute_booleanDeclaration) {
		std::string query = "stmt BOOLEAN; Select BOOLEAN.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);

		// Projection occurs as per normal
		std::vector<PqlReference> columns = parsedQuery.getAttributes();
		Assert::AreEqual(size_t(1), columns.size());
	}
	};
}
