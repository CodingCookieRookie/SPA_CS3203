#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include <algorithm>

#include "../source/QPS/PQLEvaluator.h"
#include "../source/QPS/PQLParser.h"
#include "../source/QPS/PQLResultProjector.h"
#include "../source/PKB/RS2.h"
#include "../source/PKB/Follows.h"
#include "../source/PKB/FollowsT.h"
#include "../source/PKB/Parent.h"
#include "../source/PKB/ParentT.h"
#include "../source/PKB/Pattern.h"
#include "../source/PKB/ModifiesP.h"
#include "../source/PKB/ModifiesS.h"
#include "../source/PKB/UsesP.h"
#include "../source/PKB/UsesS.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace IntegrationTesting {
	TEST_CLASS(TestQPSPKB) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Entity::performCleanUp();
		FollowsT::performCleanUp();
		Follows::performCleanUp();
		ParentT::performCleanUp();
		Parent::performCleanUp();
		ModifiesP::performCleanUp();
		ModifiesS::performCleanUp();
		UsesP::performCleanUp();
		UsesS::performCleanUp();
		Pattern::performCleanUp();
	}

	TEST_METHOD_INITIALIZE(initTables) {
		Entity::performCleanUp();
		ModifiesP::performCleanUp();
		ModifiesS::performCleanUp();
		UsesP::performCleanUp();
		UsesS::performCleanUp();
		Pattern::performCleanUp();
	}

public:
	TEST_METHOD(querying_declarationAndSelectStmtsOnly_success) {
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
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);
		// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(false, tableRef.find("s1") != tableRef.end());
		// Test Values: std::unordered_map<std::string, PqlEntityType>
		// No Test Values, empty table (merged in ResultProjector)

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "1", "2", "3", "4", "5", "6" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_declarationAndSelectStmtsOnlyStress_success) {
		// "stmt s; variable v1; Select s";
		// 1. Setup:
		std::string query = "stmt s1; if ifs; Select s1";
		// PKB inserts 99 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 99; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		}

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::IsFalse(parsedQuery.getColumns().empty());
		Assert::AreEqual(std::string("s1"), parsedQuery.getColumns()[0]);

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);
		// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(false, tableRef.find("s1") != tableRef.end());
		// Test Values:
		// No values, empty table, populated in ResultProjector only

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes;
		for (int i = 0; i < 99; i++) {
			int j = i + 1;
			expectedRes.emplace_back(std::to_string(j));
		}
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_selectStmtFollowsClauseTwoSynonyms_success) {
		// 1. Setup:
		std::string query = "stmt s1, s2; Select s1 such that Follows(s1, s2)";
		// PKB inserts 5 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 5; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		}
		for (int i = 0; i < 4; i++) {
			Follows::insert(stmts[i], stmts[i + 1]);
		}

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::IsFalse(parsedQuery.getColumns().empty());
		Assert::AreEqual(std::string("s1"), parsedQuery.getColumns()[0]);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::Follows == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("s1"), relationships[0].getLhs().second);

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);
		// Test numRow:
		Assert::AreEqual(size_t(4), evTable.getNumRow());
		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(size_t(2), tableRef.size()); // should contain s1 and s2 first (result projector filters down)
		// Test Values:
		std::vector<int> values;
		for (int i = 0; i < 4; i++) {
			values.emplace_back(i + 1);
		}
		auto actualValues = tableRef.at("s1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);
		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes;
		for (int i = 0; i < 4; i++) {
			int j = i + 1;
			expectedRes.emplace_back(std::to_string(j));
		}
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_selectStmtFollowsClauseTwoSynonymsSameStmtref_success) {
		// 1. Setup:
		std::string query = "stmt s1; Select s1 such that Follows(s1, s1)";
		// PKB inserts 5 statements
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 5; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		}
		for (int i = 0; i < 4; i++) {
			Follows::insert(stmts[i], stmts[i + 1]);
		}

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(1), parsedQuery.getDeclarations().size());
		Assert::IsFalse(parsedQuery.getColumns().empty());
		Assert::AreEqual(std::string("s1"), parsedQuery.getColumns()[0]);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::Follows == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("s1"), relationships[0].getLhs().second);

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);
		// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(size_t(1), tableRef.size()); // should contain s1 only
		// Test Values:
		std::vector<int> values;
		auto actualValues = tableRef.at("s1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);
		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes;
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin()); // no results printed out
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_selectStmtParentStarClauseLHSSynRHSWildcardStress_success) {
		// 1. Setup:
		std::string query = "stmt s1; Select s1 such that Parent*(s1, 92)";
		// PKB inserts 99 statements, 99 Parent relationships
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 99; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		}

		for (int i = 0; i < 99 - 1; i++) {
			Parent::insert(stmts[i], stmts[i + 1]);
		}

		ParentT::populate();

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(1), parsedQuery.getDeclarations().size());
		Assert::IsFalse(parsedQuery.getColumns().empty());
		Assert::AreEqual(std::string("s1"), parsedQuery.getColumns()[0]);
		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::ParentT == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::synonym == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("s1"), relationships[0].getLhs().second);

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);
		// Test numRow:
		Assert::AreEqual(size_t(91), evTable.getNumRow()); // 91 statements are parent* to stmt-92
		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(size_t(1), tableRef.size()); // should only contain s1 column
		// Test Values:
		std::vector<int> s1values;
		for (int i = 0; i < 91; i++) {
			s1values.emplace_back(i + 1);
		}
		auto actuals1Values = tableRef.at("s1");
		std::sort(actuals1Values.begin(), actuals1Values.end());
		bool areVecEqual = std::equal(s1values.begin(), s1values.end(), actuals1Values.begin());
		Assert::AreEqual(true, areVecEqual); // actualWildcardValues == {1, 2, ..., 91}
		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes;
		for (int i = 0; i < 91; i++) {
			int j = i + 1;
			expectedRes.emplace_back(std::to_string(j));
		}
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_declarationAndSelectModifiesStmtsOnly_success) {
		// 1. Setup:
		std::string query = "assign a1; variable v1; Select a1 such that Modifies(a1, v1)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts modifies
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("randomVar"); // insert dummy var
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		ModifiesS::insert(stmt, varIndex);
		ModifiesS::insert(stmt, varIndex2);

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(std::string("a1"), parsedQuery.getColumns()[0]);

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		// Test numRow:
		Assert::AreEqual(size_t(2), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a1") != tableRef.end());

		// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7, 7 };
		auto actualValues = tableRef.at("a1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		// bool actualEvResult = evTable.getEvResult();
		// Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_declarationAndSelectUsesStmtsOnly_success) {
		// 1. Setup:
		std::string query = "print p1; variable v1; Select p1 such that Uses(p1, v1)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts modifies
		StmtIndex stmt = Entity::insertStmt(StatementType::printType);
		Entity::insertVar("randomVar"); // insert dummy var
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		UsesS::insert(stmt, varIndex);
		UsesS::insert(stmt, varIndex2);

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(std::string("p1"), parsedQuery.getColumns()[0]);

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		// Test numRow:
		Assert::AreEqual(size_t(2), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p1") != tableRef.end());

		// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7, 7 };
		auto actualValues = tableRef.at("p1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		// bool actualEvResult = evTable.getEvResult();
		// Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_declarationAndPatternOnly_success) {
		// b = b + w
		// 1. Setup:
		std::string query = "assign a; variable v; Select a pattern a(v, _\"w\"_)";

		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+w");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("w");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		//// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end()); // "a" exists
		Assert::AreEqual(true, tableRef.find("v") != tableRef.end()); // "v" exists

		//// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesAndPatternOnly_success1) {
		/* Modifies(a / r / s / a1, v) pattern (v, "x")*/
		// b = b + x
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Modifies(a, v) pattern a(v, _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		VarIndex varIndex2 = Entity::insertVar("x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		ModifiesS::insert(stmt, varIndex);
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		//// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		////// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		//// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesAndPatternOnly_success2) {
		/* Modifies(a / r / s / a1, v) pattern ("b", "x")*/
		// b = b + x
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Modifies(a, v) pattern a(\"b\", _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		VarIndex varIndex2 = Entity::insertVar("x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		ModifiesS::insert(stmt, varIndex);
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		//// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		////// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		//// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesAndPatternOnly_success3) {
		/* Modifies(a / r / s / a1, "x") pattern (v, "x")*/
		// b = b + x
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Modifies(a, \"x\") pattern a(v, _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		VarIndex varIndex2 = Entity::insertVar("x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		ModifiesS::insert(stmt, varIndex);
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		//// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow());

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end()); //"a" exists

		////// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{};
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		//// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{};
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesAndPatternOnly_success4) {
		/* Modifies(a / r / s / a1, "x") pattern ("b", "x")*/
		// b = b + x
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Modifies(a, \"x\") pattern a(\"b\", _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		VarIndex varIndex2 = Entity::insertVar("x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		ModifiesS::insert(stmt, varIndex);
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		//// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow());

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		////// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{};
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		//// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{};
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesAndPatternOnly_success5) {
		/* Modifies(a / r / s / a1, _) pattern (v, "x")*/
		// b = b + x
		// 1. Setup
		std::string query = "assign a; variable v; Select a such that Modifies(a, _) pattern a(v, _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		VarIndex varIndex2 = Entity::insertVar("x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		ModifiesS::insert(stmt, varIndex);
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		//// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		////// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		//// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesAndPatternOnly_success6) {
		/* Modifies(a / r / s / a1, _) pattern ("b", "x")*/
		// b = b + x
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Modifies(a, _) pattern a(\"b\", _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		VarIndex varIndex2 = Entity::insertVar("x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		ModifiesS::insert(stmt, varIndex);
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		//// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		////// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		//// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesAndPatternOnly_success7) {
		/* Modifies(7, v) pattern (v, "x")*/
		// b = b + x
		// 1. Setup
		std::string query = "assign a; variable v; Select a such that Modifies(7, v) pattern a(v, _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		VarIndex varIndex2 = Entity::insertVar("x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		ModifiesS::insert(stmt, varIndex);
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		//// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		////// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		//// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesAndPatternOnly_success8) {
		/* Modifies(7, "b") pattern ("b", "x")*/
		// b = b + x
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Modifies(7, v) pattern a(\"b\", _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		VarIndex varIndex2 = Entity::insertVar("x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		ModifiesS::insert(stmt, varIndex);
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesAndPatternOnly_success9) {
		/* Modifies(7, _) pattern (v, "x")*/
		// b = b + x
		// 1. Setup
		std::string query = "assign a; variable v; Select a such that Modifies(7, _) pattern a(v, _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		VarIndex varIndex2 = Entity::insertVar("x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		ModifiesS::insert(stmt, varIndex);
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		//// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		////// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		//// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesAndPatternOnly_success10) {
		/* Modifies(7, "b") pattern ("b", "x")*/
		// b = b + x
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Modifies(7, _) pattern a(\"b\", _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		VarIndex varIndex2 = Entity::insertVar("x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		ModifiesS::insert(stmt, varIndex);
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	/* Uses variation generally similar to Modifies for testing */
	TEST_METHOD(querying_UsesAndPatternOnly_success1) {
		// b = b + y
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Uses(a, v) pattern a(v, _\"y\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		VarIndex varIndex2 = Entity::insertVar("y");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+y");
		UsesS::insert(stmt, varIndex);
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("y");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_UsesAndPatternOnly_success2) {
		// b = b + y
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Uses(7, v) pattern a(v, _\"y\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		VarIndex varIndex2 = Entity::insertVar("y");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+y");
		UsesS::insert(stmt, varIndex);
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("y");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_UsesAndPatternOnly_success3) {
		// b = b + y
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Uses(a, _) pattern a(v, _\"y\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+y");
		UsesS::insert(stmt, varIndex);
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("y");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_UsesAndPatternOnly_success4) {
		// b = b + y
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Uses(7, _) pattern a(v, _\"y\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		VarIndex varIndex2 = Entity::insertVar("y");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+y");
		UsesS::insert(stmt, varIndex);
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("y");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_UsesAndPatternOnly_success5) {
		// b = b + y
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Uses(a, _) pattern a(v, _\"y\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		VarIndex varIndex2 = Entity::insertVar("y");
		VarIndex varIndex3 = Entity::insertVar("w");
		VarIndex varIndex4 = Entity::insertVar("dd");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+y+w+yy+dd");
		UsesS::insert(stmt, varIndex);
		UsesS::insert(stmt, varIndex2);
		UsesS::insert(stmt, varIndex3);
		UsesS::insert(stmt, varIndex4);
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("y");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		// Test numRow:
		/*Modifies has 4 statements-var pair*/
		Assert::AreEqual(size_t(4), evTable.getNumRow());

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_FollowsAndPatternOnly_success) {
		// stmt 7. abc = abc + d;
		// stmt 8. d = abc + d;
		// 6 is followed by 7 (read follows assign), but 7 is not followed by 8
		// Results: {"a", {6}}
		// 1. Setup:
		std::string query = "read r; assign a; variable v; Select a such that Follows(r, a) pattern a(v, _\"d\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));

		StmtIndex readStmt = Entity::insertStmt(StatementType::readType);

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		StmtIndex stmt2 = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("def");
		VarIndex varIndex = Entity::insertVar("abc");
		VarIndex varIndex2 = Entity::insertVar("d");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix(" abc + d ");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);
		Pattern::insertAssignInfo(varIndex2, postFixExpression, stmt);
		Follows::insert(readStmt, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("d");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(2), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(3), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getRelationships().size());
		Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		// Test numRow:
		Assert::AreEqual(size_t(2), evTable.getNumRow()); // { {"a", { 7, 7 }}, {"r", { 6, 6 }}, {"v", { 2, 3 }}}

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ParentStarAndPatternOnlyStress99_success) {
		// stmt 99. abc = abc + d;
		// 6 is followed by 7 (read follows assign), but 7 is not followed by 8
		// Results: {"a", {6}}
		// 1. Setup:
		std::string query = "while w; assign a; variable v; Select a such that Parent*(w, a) pattern a(v, _\"d\"_)";
		// PKB inserts 99 statements and 98 Parent* relationships
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 98; i++) {
			stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		}
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType)); // 99th stmt is assignment

		for (int i = 0; i < 98; i++) {
			Parent::insert(stmts[i], stmts[i + 1]);
		}
		ParentT::populate();

		// PKB inserts pattern
		Entity::insertVar("def");
		VarIndex varIndex = Entity::insertVar("abc");
		VarIndex varIndex2 = Entity::insertVar("d");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix(" abc + d ");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmts[98]); // stmt 99

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("d");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(3), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getRelationships().size());
		Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		// Test numRow:
		Assert::AreEqual(size_t(98), evTable.getNumRow()); // { {"w", { 1, 2,..., 98 }}, {"a", { Ninety-eight 99s }} , {"v", {Ninety-eight 2s }}  }}

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());
		Assert::AreEqual(true, tableRef.find("w") != tableRef.end());
		Assert::AreEqual(true, tableRef.find("v") != tableRef.end());

		// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> wValues, aValues;
		for (int i = 0; i < 98; i++) {
			wValues.emplace_back(i + 1);
			aValues.emplace_back(99);
		}
		auto actualwValues = tableRef.at("w");
		std::sort(actualwValues.begin(), actualwValues.end());
		bool areVecEqual = std::equal(wValues.begin(), wValues.end(), actualwValues.begin());
		Assert::AreEqual(true, areVecEqual);
		auto actualaValues = tableRef.at("a");
		std::sort(actualaValues.begin(), actualaValues.end());
		bool areVecEqual2 = std::equal(aValues.begin(), aValues.end(), actualaValues.begin());
		Assert::AreEqual(true, areVecEqual2);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "99" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_declarationAndPatternExprOnly_success) {
		// b = (x + 1) * 2 / 3 - 4 % (5)
		// 1. Setup:
		std::string query = "assign a; variable v; Select a pattern a(v, _\"x + 1\"_)";

		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("(x+1)*2/3-4%(5)");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		//// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end()); // "a" exists
		Assert::AreEqual(true, tableRef.find("v") != tableRef.end()); // "v" exists

		//// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_substringButNotSubtree_emptyResult) {
		// b = (x + 1) * 2 / 3 - 4 % (5)
		// 1. Setup:
		std::string query = "assign a; variable v; Select a pattern a(v, _\"2 / 3\"_)";

		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("(x+1)*2/3-4%(5)");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		//// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow());

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end()); // "a" exists
		Assert::AreEqual(true, tableRef.find("v") != tableRef.end()); // "v" exists

		//// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values;
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes;
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_patternFullMatchButSubtree_emptyResult) {
		// b = (x + 1) * 2 / 3 - 4 % (5)
		// 1. Setup:
		std::string query = "assign a; variable v; Select a pattern a(v, \"x + 1\")";

		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("(x+1)*2/3-4%(5)");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		//// Test numRow:
		Assert::AreEqual(size_t(0), evTable.getNumRow());

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end()); // "a" exists
		Assert::AreEqual(true, tableRef.find("v") != tableRef.end()); // "v" exists

		//// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values;
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes;
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_patternFullMatchWithFullExpression_success) {
		// b = (x + 1) * 2 / 3 - 4 % (5)
		// 1. Setup:
		std::string query = "assign a; variable v; Select a pattern a(v, _\"(x + 1)       * 2 / 3		-4 %\n 5\"_)";

		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
		stmts.emplace_back(Entity::insertStmt(StatementType::printType));
		stmts.emplace_back(Entity::insertStmt(StatementType::callType));
		stmts.emplace_back(Entity::insertStmt(StatementType::ifType));
		stmts.emplace_back(Entity::insertStmt(StatementType::whileType));
		stmts.emplace_back(Entity::insertStmt(StatementType::readType));

		// PKB inserts pattern
		StmtIndex stmt = Entity::insertStmt(StatementType::assignType);
		Entity::insertVar("a");
		VarIndex varIndex = Entity::insertVar("b");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("(x+1)*2/3-4%(5)");
		Pattern::insertAssignInfo(varIndex, postFixExpression, stmt);

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

		// 3. Test QPS Evaluator:
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);

		//// Test numRow:
		Assert::AreEqual(size_t(1), evTable.getNumRow());

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end()); // "a" exists
		Assert::AreEqual(true, tableRef.find("v") != tableRef.end()); // "v" exists

		//// Test Values: std::unordered_map<std::string, PqlEntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		evTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}
	};
}
