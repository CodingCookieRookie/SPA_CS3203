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
#include "../source/PKB/Pattern.h"

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
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns());
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
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "7" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}

		TEST_METHOD(querying_declarationAndPatternOnly_success)
		{
			// b = b + x
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
			std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix(" b + w ");
			Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);

			// Check PKB populated
			std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern("w", true);
			Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

			// 2. Test QPS Parser:
			ParsedQuery parsedQuery = PQLParser::parseQuery(query);
			Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
			Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

			// 3. Test QPS Evaluator:
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();

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
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "7" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}

		TEST_METHOD(querying_ModifiesAndPatternOnly_success)
		{
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
			std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix(" b + x ");
			Modifies::insert(stmt, varIndex);
			Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);
			Pattern::insertPostFixInfo(varIndex2, postFixExpression, stmt);

			// Check PBK populated
			std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern("x", true);
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

			////// Test Values: std::unordered_map<std::string, PqlEntityType>
			std::vector<int> values{ 7 };
			auto actualValues = tableRef.at("a");
			bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
			Assert::AreEqual(true, areVecEqual);

			//// Test EvResult:
			bool actualEvResult = evTable.getEvResult();
			Assert::AreEqual(true, actualEvResult);

			//// 4. Test QPS Result Projector:
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns());
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
			std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix(" b + y ");
			Uses::insert(stmt, varIndex);
			Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);
			Pattern::insertPostFixInfo(varIndex2, postFixExpression, stmt);

			//// Check PBK populated
			std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern("y", true);
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
			bool actualEvResult = evTable.getEvResult();
			Assert::AreEqual(true, actualEvResult);

			// 4. Test QPS Result Projector:
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "7" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}

		TEST_METHOD(querying_FollowsAndPatternOnly_success)
		{
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
			Pattern::insertPostFixInfo(varIndex, postFixExpression, stmt);
			Pattern::insertPostFixInfo(varIndex2, postFixExpression, stmt);
			Follows::insert(readStmt, stmt);

			// Check PKB populated
			std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern("d", true);
			Assert::AreEqual(size_t(2), std::get<0>(allPatternStmtInfo).size());

			// 2. Test QPS Parser:
			ParsedQuery parsedQuery = PQLParser::parseQuery(query);
			Assert::AreEqual(size_t(3), parsedQuery.getDeclarations().size());
			Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
			Assert::AreEqual(size_t(1), parsedQuery.getRelationships().size());
			Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

			// 3. Test QPS Evaluator:
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();

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
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "7" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}

		TEST_METHOD(querying_ParentStarAndPatternOnlyStress99_success)
		{
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
			std::unordered_map<StmtIndex,
				std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
			for (int i = 0; i < 98; i++) {
				uPredSucTable[stmts[i]] = { stmts[i + 1] }; // i parents i + 1, up till 97 parents 98th stmt
			}
			ParentT::populate(uPredSucTable);

			// PKB inserts pattern
			Entity::insertVar("def");
			VarIndex varIndex = Entity::insertVar("abc");
			VarIndex varIndex2 = Entity::insertVar("d");
			std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix(" abc + d ");
			Pattern::insertPostFixInfo(varIndex, postFixExpression, stmts[98]); //stmt 99

			// Check PKB populated
			std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = Pattern::getStmtsFromPattern("d", true);
			Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

			// 2. Test QPS Parser:
			ParsedQuery parsedQuery = PQLParser::parseQuery(query);
			Assert::AreEqual(size_t(3), parsedQuery.getDeclarations().size());
			Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
			Assert::AreEqual(size_t(1), parsedQuery.getRelationships().size());
			Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

			// 3. Test QPS Evaluator:
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();

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
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "99" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}
		//TODO: 2-cl, stress
	};
}
