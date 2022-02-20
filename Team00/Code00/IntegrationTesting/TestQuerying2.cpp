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
			Modifies::performCleanUp();
			Uses::performCleanUp();
			Pattern::performCleanUp();
		}

		TEST_METHOD_INITIALIZE(initTables) {
			Entity::performCleanUp();
			Modifies::performCleanUp();
			Uses::performCleanUp();
			Pattern::performCleanUp();
		}
	public:

		TEST_METHOD(querying_declarationAndSelectModifiesStmtsOnly_success)
		{
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
			Modifies::insert(stmt, varIndex);
			Modifies::insert(stmt, varIndex2);

			// 2. Test QPS Parser:
			ParsedQuery parsedQuery = PQLParser::parseQuery(query);
			Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
			Assert::AreEqual(std::string("a1"), parsedQuery.getColumns()[0]);

			// 3. Test QPS Evaluator:
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();

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
			//bool actualEvResult = evTable.getEvResult();
			//Assert::AreEqual(true, actualEvResult);

			// 4. Test QPS Result Projector:
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "7" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
			Pattern::performCleanUp();
		}

		TEST_METHOD(querying_declarationAndSelectUsesStmtsOnly_success)
		{
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
			Uses::insert(stmt, varIndex);
			Uses::insert(stmt, varIndex2);

			// 2. Test QPS Parser:
			ParsedQuery parsedQuery = PQLParser::parseQuery(query);
			Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
			Assert::AreEqual(std::string("p1"), parsedQuery.getColumns()[0]);

			// 3. Test QPS Evaluator:
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();

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
			//bool actualEvResult = evTable.getEvResult();
			//Assert::AreEqual(true, actualEvResult);

			// 4. Test QPS Result Projector:
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "7" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}

		TEST_METHOD(querying_declarationAndPatternOnly_success)
		{
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
			Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

			// Check PBK populated
			std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern(" w ", true);
			Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

			// 2. Test QPS Parser:
			ParsedQuery parsedQuery = PQLParser::parseQuery(query);
			Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

			// 3. Test QPS Evaluator:
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();

			//// Test numRow:
			Assert::AreEqual(size_t(1), evTable.getNumRow());

			//// Test Table:
			auto tableRef = evTable.getTableRef();
			Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

			//// Test Values: std::unordered_map<std::string, PqlEntityType>
			std::vector<int> values{ 7 };
			auto actualValues = tableRef.at("a");
			bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
			Assert::AreEqual(true, areVecEqual);

			// 4. Test QPS Result Projector:
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "7" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}

		TEST_METHOD(querying_ModifiesAndPatternOnly_success1)
		{
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
			Modifies::insert(stmt, varIndex);
			Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

			// Check PBK populated
			std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern(" x ", true);
			Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

			// 2. Test QPS Parser:
			ParsedQuery parsedQuery = PQLParser::parseQuery(query);
			Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

			// 3. Test QPS Evaluator:
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();

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
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "7" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}

		TEST_METHOD(querying_ModifiesAndPatternOnly_success2)
		{
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
			Modifies::insert(stmt, varIndex);
			Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

			// Check PBK populated
			std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern(" x ", true);
			Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

			// 2. Test QPS Parser:
			ParsedQuery parsedQuery = PQLParser::parseQuery(query);
			Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

			// 3. Test QPS Evaluator:
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();

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
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "7" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}
		TEST_METHOD(querying_ModifiesAndPatternOnly_success3)
		{
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
			Modifies::insert(stmt, varIndex);
			Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

			// Check PBK populated
			std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern(" x ", true);
			Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

			// 2. Test QPS Parser:
			ParsedQuery parsedQuery = PQLParser::parseQuery(query);
			Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

			// 3. Test QPS Evaluator:
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();

			//// Test numRow:
			Assert::AreEqual(size_t(0), evTable.getNumRow());

			//// Test Table:
			auto tableRef = evTable.getTableRef();
			Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

			////// Test Values: std::unordered_map<std::string, PqlEntityType>
			std::vector<int> values{ };
			auto actualValues = tableRef.at("a");
			bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
			Assert::AreEqual(true, areVecEqual);

			//// 4. Test QPS Result Projector:
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}

		TEST_METHOD(querying_ModifiesAndPatternOnly_success4)
		{
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
			Modifies::insert(stmt, varIndex);
			Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

			// Check PBK populated
			std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern(" x ", true);
			Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

			// 2. Test QPS Parser:
			ParsedQuery parsedQuery = PQLParser::parseQuery(query);
			Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

			// 3. Test QPS Evaluator:
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();

			//// Test numRow:
			Assert::AreEqual(size_t(0), evTable.getNumRow());

			//// Test Table:
			auto tableRef = evTable.getTableRef();
			Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

			////// Test Values: std::unordered_map<std::string, PqlEntityType>
			std::vector<int> values{ };
			auto actualValues = tableRef.at("a");
			bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
			Assert::AreEqual(true, areVecEqual);

			//// 4. Test QPS Result Projector:
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}

		TEST_METHOD(querying_ModifiesAndPatternOnly_success5)
		{
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
			Modifies::insert(stmt, varIndex);
			Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

			// Check PBK populated
			std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern(" x ", true);
			Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

			// 2. Test QPS Parser:
			ParsedQuery parsedQuery = PQLParser::parseQuery(query);
			Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

			// 3. Test QPS Evaluator:
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();

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
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "7" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}

		TEST_METHOD(querying_ModifiesAndPatternOnly_success6)
		{
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
			Modifies::insert(stmt, varIndex);
			Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

			// Check PBK populated
			std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern(" x ", true);
			Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

			// 2. Test QPS Parser:
			ParsedQuery parsedQuery = PQLParser::parseQuery(query);
			Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

			// 3. Test QPS Evaluator:
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();

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
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "7" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}
		TEST_METHOD(querying_ModifiesAndPatternOnly_success7)
		{
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
			Modifies::insert(stmt, varIndex);
			Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

			// Check PBK populated
			std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern(" x ", true);
			Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

			// 2. Test QPS Parser:
			ParsedQuery parsedQuery = PQLParser::parseQuery(query);
			Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

			// 3. Test QPS Evaluator:
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();

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
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "7" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}
		TEST_METHOD(querying_ModifiesAndPatternOnly_success8)
		{
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
			Modifies::insert(stmt, varIndex);
			Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

			// Check PBK populated
			std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern(" x ", true);
			Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

			// 2. Test QPS Parser:
			ParsedQuery parsedQuery = PQLParser::parseQuery(query);
			Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

			// 3. Test QPS Evaluator:
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();

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
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "7" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}

		TEST_METHOD(querying_ModifiesAndPatternOnly_success9)
		{
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
			Modifies::insert(stmt, varIndex);
			Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

			// Check PBK populated
			std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern(" x ", true);
			Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

			// 2. Test QPS Parser:
			ParsedQuery parsedQuery = PQLParser::parseQuery(query);
			Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

			// 3. Test QPS Evaluator:
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();

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
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "7" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}
		TEST_METHOD(querying_ModifiesAndPatternOnly_success10)
		{
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
			Modifies::insert(stmt, varIndex);
			Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

			// Check PBK populated
			std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern(" x ", true);
			Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

			// 2. Test QPS Parser:
			ParsedQuery parsedQuery = PQLParser::parseQuery(query);
			Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

			// 3. Test QPS Evaluator:
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();

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
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "7" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}

		TEST_METHOD(querying_UsesAndPatternOnly_success)
		{
			// b = b + x

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
			Uses::insert(stmt, varIndex);
			Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);
			Pattern::insertPostFixInfo(varIndex2, postFixExpression, stmt);

			//// Check PBK populated
			std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern(" y ", true);
			Assert::AreEqual(size_t(2), std::get<0>(allPatternStmtInfo).size());

			// 2. Test QPS Parser:
			ParsedQuery parsedQuery = PQLParser::parseQuery(query);
			Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

			// 3. Test QPS Evaluator:
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();

			//// Test numRow:
			Assert::AreEqual(size_t(1), evTable.getNumRow());

			//// Test Table:
			auto tableRef = evTable.getTableRef();
			Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

			//////// Test Values: std::unordered_map<std::string, PqlEntityType>
			std::vector<int> values{ 7 };
			auto actualValues = tableRef.at("a");
			bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
			Assert::AreEqual(true, areVecEqual);

			// Test EvResult:
			//bool actualEvResult = evTable.getEvResult();
			//Assert::AreEqual(true, actualEvResult);

			// 4. Test QPS Result Projector:
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "7" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}
	};
}
