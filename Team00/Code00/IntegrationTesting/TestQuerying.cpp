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
			// Test Values: std::unordered_map<std::string, PqlEntityType>
			std::vector<int> values{ 1, 2, 3, 4, 5, 6 };
			auto actualValues = tableRef.at("s1");
			bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
			Assert::AreEqual(true, areVecEqual);
			// Test EvResult:
			bool actualEvResult = evTable.getEvResult();
			Assert::AreEqual(true, actualEvResult);

			// 4. Test QPS Result Projector:
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes{ "1", "2", "3", "4", "5", "6" };
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}

		TEST_METHOD(querying_declarationAndSelectStmtsOnlyStress_success)
		{
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
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();
			// Test numRow:
			Assert::AreEqual(size_t(99), evTable.getNumRow());
			// Test Table:
			auto tableRef = evTable.getTableRef();
			Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
			// Test Values:
			std::vector<int> values;
			for (int i = 0; i < 99; i++) {
				values.emplace_back(i + 1);
			}
			auto actualValues = tableRef.at("s1");
			bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
			Assert::AreEqual(true, areVecEqual);
			// Test EvResult:
			bool actualEvResult = evTable.getEvResult();
			Assert::AreEqual(true, actualEvResult);

			// 4. Test QPS Result Projector:
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes;
			for (int i = 0; i < 99; i++) {
				int j = i + 1;
				expectedRes.emplace_back(std::to_string(j));
			}
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}

		TEST_METHOD(querying_selectStmtFollowsClauseTwoSynonyms_success)
		{
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
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();
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
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes;
			for (int i = 0; i < 4; i++) {
				int j = i + 1;
				expectedRes.emplace_back(std::to_string(j));
			}
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}

		TEST_METHOD(querying_selectStmtParentStarClauseLHSSynRHSWildcardStress_success)
		{
			// 1. Setup:
			std::string query = "stmt s1; Select s1 such that Parent*(s1, 92)";
			// PKB inserts 99 statements, 99 Parent relationships
			std::vector<StmtIndex> stmts;
			for (int i = 0; i < 99; i++) {
				stmts.emplace_back(Entity::insertStmt(StatementType::assignType));
			}
			std::unordered_map<StmtIndex,
				std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable;
			for (int i = 0; i < 99 - 1; i++) {
				uPredSucTable[stmts[i]] = { stmts[i + 1] }; // i parents i + 1
			}
			ParentT::populate(uPredSucTable);

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
			PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
			EvaluatedTable evTable = pqlEvaluator.evaluate();
			// Test numRow:
			Assert::AreEqual(size_t(91), evTable.getNumRow()); //91 statements are parent* to stmt-92
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
			PQLResultProjector resultProjector = PQLResultProjector(evTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
			std::list<std::string> results = resultProjector.resolveTableToResults();
			std::list<std::string> expectedRes;
			for (int i = 0; i < 91; i++) {
				int j = i + 1;
				expectedRes.emplace_back(std::to_string(j));
			}
			bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
			Assert::AreEqual(true, areListsEqual);
		}
	};
}
