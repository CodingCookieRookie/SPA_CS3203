#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include <algorithm>

#include "../source/QPS/Evaluator/PQLEvaluator.h"
#include "../source/QPS/Parser/PQLParser.h"
#include "../source/QPS/ResultProjector/PQLResultProjector.h"
#include "../source/PKB/PKBInserter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace IntegrationTesting {
	TEST_CLASS(TestQPSPKB) {
private:
	PKB pkb;
	PKBGetter* pkbGetter;
	PKBInserter* pkbInserter;
	CacheStorage* cacheStorage;
	AffectsProcessor* affectsProcessor;
	AffectsTProcessor* affectsTProcessor;

	TEST_METHOD_INITIALIZE(initTables) {
		pkb = PKB();
		pkbGetter = new PKBGetter(pkb);
		pkbInserter = new PKBInserter(pkb);
		cacheStorage = new CacheStorage();
		affectsProcessor = new AffectsProcessor(cacheStorage);
		affectsTProcessor = new AffectsTProcessor(new AffectsTCache(), affectsProcessor);
	}

public:
	TEST_METHOD(querying_declarationAndSelectStmtsOnly_success) {
		// "stmt s; variable v1; Select s";
		// 1. Setup:
		std::string query = "stmt s1; if ifs; Select s1";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());

		std::unordered_set<std::string> selects = parsedQuery.getColumns();
		Assert::IsFalse(selects.empty());
		Assert::IsTrue(selects.find("s1") != selects.end());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(false, tableRef.find("s1") != tableRef.end());
		// Test Values: std::unordered_map<std::string, EntityType>
		// No Test Values, empty table (merged in ResultProjector)

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);

		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
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
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());

		std::unordered_set<std::string> selects = parsedQuery.getColumns();
		Assert::IsFalse(selects.empty());
		Assert::IsTrue(selects.find("s1") != selects.end());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(false, tableRef.find("s1") != tableRef.end());
		// Test Values:
		// No values, empty table, populated in ResultProjector only

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
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
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}
		for (int i = 0; i < 4; i++) {
			pkbInserter->insertRSInfo(RelationshipType::FOLLOWS, stmts[i], stmts[i + 1]);
		}

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());

		std::unordered_set<std::string> selects = parsedQuery.getColumns();
		Assert::IsFalse(selects.empty());
		Assert::IsTrue(selects.find("s1") != selects.end());

		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::FOLLOWS == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("s1"), relationships[0].getLhs().second);

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("s1") != tableRef.end());
		Assert::AreEqual(size_t(1), tableRef.size()); // optimized execute will project out only the relevant columns
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
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
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
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}
		for (int i = 0; i < 4; i++) {
			pkbInserter->insertRSInfo(RelationshipType::FOLLOWS, stmts[i], stmts[i + 1]);
		}

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(1), parsedQuery.getDeclarations().size());

		std::unordered_set<std::string> selects = parsedQuery.getColumns();
		Assert::IsFalse(selects.empty());
		Assert::IsTrue(selects.find("s1") != selects.end());

		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::FOLLOWS == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("s1"), relationships[0].getLhs().second);

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

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
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
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
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		}

		for (int i = 0; i < 99; i++) {
			for (int j = i + 1; j < 99; j++) {
				pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[i], stmts[j]); // i parents i + 1, simulate 99 nesting levels
			}
		}

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(1), parsedQuery.getDeclarations().size());

		std::unordered_set<std::string> selects = parsedQuery.getColumns();
		Assert::IsFalse(selects.empty());
		Assert::IsTrue(selects.find("s1") != selects.end());

		std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
		Assert::AreEqual(size_t(1), relationships.size());
		Assert::IsTrue(PqlRelationshipType::PARENT_T == relationships[0].getRelationshipType());
		Assert::IsTrue(PqlReferenceType::SYNONYM == relationships[0].getLhs().first);
		Assert::AreEqual(std::string("s1"), relationships[0].getLhs().second);

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

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
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes;
		for (int i = 0; i < 91; i++) {
			int j = i + 1;
			expectedRes.emplace_back(std::to_string(j));
		}
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_declarationAndSelectModifiesSStmtsOnly_success) {
		// 1. Setup:
		std::string query = "assign a1; variable v1; Select a1 such that Modifies(a1, v1)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts modifies
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex2);

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());

		std::unordered_set<std::string> selects = parsedQuery.getColumns();
		Assert::AreEqual(size_t(1), selects.size());
		Assert::IsTrue(selects.find("a1") != selects.end());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a1") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::unordered_set<int> values{ 7 };
		std::vector<int> actualCol = tableRef.at("a1");
		std::unordered_set<int> actualSet(actualCol.begin(), actualCol.end());
		Assert::IsTrue(values == actualSet);

		// Test EvResult:
		// bool actualEvResult = evTable.getEvResult();
		// Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_declarationAndSelectModifiesPStmtsOnly_success) {
		// 1. Setup:
		/* proc proc1 {...} */
		std::string query = "procedure p1; variable v1; Select p1 such that Modifies(p1, v1)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts modifies
		ProcIndex proc = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_P, proc, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_P, proc, varIndex2);

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());

		std::unordered_set<std::string> selects = parsedQuery.getColumns();
		// Assert::AreEqual(size_t(1), selects.size());
		//Assert::IsTrue(selects.find("p1") != selects.end());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p1") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 1 };
		auto actualValues = tableRef.at("p1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		//// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "proc1" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_declarationAndSelectUsesSStmtsOnly_success) {
		// 1. Setup:
		std::string query = "print p1; variable v1; Select p1 such that Uses(p1, v1)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts modifies
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex2);

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());

		std::unordered_set<std::string> selects = parsedQuery.getColumns();
		Assert::AreEqual(size_t(1), selects.size());
		Assert::IsTrue(selects.find("p1") != selects.end());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p1") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::unordered_set<int> values{ 7 };
		std::vector<int> actualCol = tableRef.at("p1");
		std::unordered_set<int> actualSet(actualCol.begin(), actualCol.end());
		Assert::IsTrue(values == actualSet);

		// Test EvResult:
		// bool actualEvResult = evTable.getEvResult();
		// Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_declarationAndSelectUsesPStmtsOnly_success) {
		// 1. Setup:
		std::string query = "procedure p1; variable v1; Select p1 such that Uses(p1, v1)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts modifies
		ProcIndex proc = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "randomVar"); // insert dummy var
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		pkbInserter->insertRSInfo(RelationshipType::USES_P, proc, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_P, proc, varIndex2);

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());

		std::unordered_set<std::string> selects = parsedQuery.getColumns();
		Assert::AreEqual(size_t(1), selects.size());
		Assert::IsTrue(selects.find("p1") != selects.end());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p1") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 1 };
		auto actualValues = tableRef.at("p1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		// bool actualEvResult = evTable.getEvResult();
		// Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "proc1" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}
	TEST_METHOD(querying_declarationAndPatternAOnly_success) {
		// b = b + w
		// 1. Setup:
		std::string query = "assign a; variable v; Select a pattern a(v, _\"w\"_)";

		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+w");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("w");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end()); // "a" exists
		Assert::AreEqual(true, tableRef.find("v") == tableRef.end()); // "v" exists

		//// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_declarationAndPatternIOnly_success) {
		// if (x > 0)
		// 1. Setup:
		std::string query = "if ifs; variable v; Select ifs pattern ifs(v, _, _)";

		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::IF_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertPatternContainer(StatementType::IF_TYPE, stmt, varIndex);

		// Check PKB population
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("ifs") != tableRef.end()); // "ifs" exists
		Assert::AreEqual(true, tableRef.find("v") == tableRef.end()); // "v" exists

		//// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("ifs");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_declarationAndPatternWOnly_success) {
		// while (x > 0)
		// 1. Setup:
		std::string query = "while w; variable v; Select w pattern w(v, _)";

		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "w");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertPatternContainer(StatementType::WHILE_TYPE, stmt, varIndex);

		// Check PKB population
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("w") != tableRef.end()); // "w" exists
		Assert::AreEqual(true, tableRef.find("v") == tableRef.end()); // "v" exists

		//// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("w");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesSAndPatternAOnly_success1) {
		/* Modifies(a / r / s / a1, v) pattern (v, "x")*/
		// b = b + x
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Modifies(a, v) pattern a(v, _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		////// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		//// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesPAndPatternIOnly_success1) {
		/* Modifies("p1", v) pattern ifs(v, _, _)*/
		// if (b > 0)
		// 1. Setup:
		std::string query = "procedure p1; variable v; if ifs; Select p1 such that Modifies(p1, v) pattern ifs(v, _, _)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		ProcIndex proc = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::IF_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "dummy");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertPatternContainer(StatementType::IF_TYPE, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_P, proc, varIndex);

		// Check PKB population
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(3), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p1") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 1 };
		auto actualValues = tableRef.at("p1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "proc1" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesPAndPatternWOnly_success1) {
		/* Modifies("p1", v) pattern while(v, _)*/
		// while (b > 0)
		// 1. Setup:
		std::string query = "procedure p1; variable v; while w; Select p1 such that Modifies(p1, v) pattern w(v, _)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		ProcIndex proc = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "dummy");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertPatternContainer(StatementType::WHILE_TYPE, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_P, proc, varIndex);

		// Check PKB population
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(3), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p1") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 1 };
		auto actualValues = tableRef.at("p1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "proc1" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}
	TEST_METHOD(querying_ModifiesSAndPatternAOnly_success2) {
		/* Modifies(a / r / s / a1, v) pattern ("b", "x")*/
		// b = b + x
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Modifies(a, v) pattern a(\"b\", _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		////// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		//// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesPAndPatternIOnly_success2) {
		/* Modifies("p1", v) pattern ifs("b", _, _)*/
		/* proc1 {
			if (b > 0)
			b = b + 1
		} */
		// 1. Setup:
		std::string query = "procedure p1; variable v; if ifs; Select p1 such that Modifies(p1, v) pattern ifs(\"b\", _, _)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));
		// PKB inserts pattern
		ProcIndex proc = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::IF_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "dummy");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertPatternContainer(StatementType::IF_TYPE, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_P, proc, varIndex);

		// Check PKB population
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(3), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p1") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 1 };
		auto actualValues = tableRef.at("p1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "proc1" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesPAndPatternWOnly_success2) {
		/* Modifies("p1", v) pattern w("proc1", _)*/
		// if (b > 0)
		// 1. Setup:
		std::string query = "procedure p1; variable v; while w; Select p1 such that Modifies(p1, v) pattern w(\"b\", _)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));
		// PKB inserts pattern
		ProcIndex proc = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "dummy");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertPatternContainer(StatementType::WHILE_TYPE, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_P, proc, varIndex);

		// Check PKB population
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(3), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p1") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 1 };
		auto actualValues = tableRef.at("p1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "proc1" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesSAndPatternAOnly_success3) {
		/* Modifies(a / r / s / a1, "x") pattern (v, "x")*/
		// b = b + x
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Modifies(a, \"x\") pattern a(v, _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB populated
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end()); //"a" exists

		////// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{};
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		//// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{};
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesSAndPatternAOnly_success4) {
		/* Modifies(a / r / s / a1, "x") pattern ("b", "x")*/
		// b = b + x
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Modifies(a, \"x\") pattern a(\"b\", _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		////// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{};
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		//// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{};
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesSAndPatternAOnly_success5) {
		/* Modifies(a / r / s / a1, _) pattern (v, "x")*/
		// b = b + x
		// 1. Setup
		std::string query = "assign a; variable v; Select a such that Modifies(a, _) pattern a(v, _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		////// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		//// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesSAndPatternAOnly_success6) {
		/* Modifies(a / r / s / a1, _) pattern ("b", "x")*/
		// b = b + x
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Modifies(a, _) pattern a(\"b\", _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		////// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		//// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesSAndPatternAOnly_success7) {
		/* Modifies(7, v) pattern (v, "x")*/
		// b = b + x
		// 1. Setup
		std::string query = "assign a; variable v; Select a such that Modifies(7, v) pattern a(v, _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		////// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		//// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesSAndPatternAOnly_success8) {
		/* Modifies(7, "b") pattern ("b", "x")*/
		// b = b + x
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Modifies(7, v) pattern a(\"b\", _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesSAndPatternAOnly_success9) {
		/* Modifies(7, _) pattern (v, "x")*/
		// b = b + x
		// 1. Setup
		std::string query = "assign a; variable v; Select a such that Modifies(7, _) pattern a(v, _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		////// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		//// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ModifiesSAndPatternAOnly_success10) {
		/* Modifies(7, "b") pattern ("b", "x")*/
		// b = b + x
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Modifies(7, _) pattern a(\"b\", _\"x\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+x");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("x");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	/* Uses variation generally similar to Modifies for testing */
	TEST_METHOD(querying_UsesSAndPatternAOnly_success1) {
		// b = b + y
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Uses(a, v) pattern a(v, _\"y\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+y");
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("y");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_UsesPAndPatternIOnly_success1) {
		/* proc1 {
			b = b + y

			if (b > 0)
		*/
		// 1. Setup:
		std::string query = "procedure p1; variable v; if ifs; Select p1 such that Uses(p1, v) pattern ifs(v, _, _)";

		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		ProcIndex proc = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::IF_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "dummy");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		pkbInserter->insertRSInfo(RelationshipType::USES_P, proc, varIndex);
		pkbInserter->insertPatternContainer(StatementType::IF_TYPE, stmt, varIndex);

		// Check PKB population
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(3), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p1") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 1 };
		auto actualValues = tableRef.at("p1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "proc1" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_UsesPAndPatternWOnly_success1) {
		/* proc1 {
			b = b + y

			if (b > 0)
		*/
		// 1. Setup:
		std::string query = "procedure p1; variable v; while w; Select p1 such that Uses(p1, v) pattern w(v, _)";

		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		ProcIndex proc = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "dummy");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		pkbInserter->insertRSInfo(RelationshipType::USES_P, proc, varIndex);
		pkbInserter->insertPatternContainer(StatementType::WHILE_TYPE, stmt, varIndex);

		// Check PKB population
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(3), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("p1") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 1 };
		auto actualValues = tableRef.at("p1");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "proc1" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_UsesSAndPatternAOnly_success2) {
		// b = b + y
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Uses(7, v) pattern a(v, _\"y\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+y");
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("y");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_UsesSAndPatternAOnly_success3) {
		// b = b + y
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Uses(a, _) pattern a(v, _\"y\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+y");
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("y");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_UsesSAndPatternAOnly_success4) {
		// b = b + y
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Uses(7, _) pattern a(v, _\"y\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+y");
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("y");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_UsesSAndPatternAOnly_success5) {
		// b = b + y
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Uses(a, _) pattern a(v, _\"y\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		VarIndex varIndex3 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "w");
		VarIndex varIndex4 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "dd");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("b+y+w+yy+dd");
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex3);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex4);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("y");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_AffectsAndPatternAOnly_success) {
		// a = b + c
		// c = a
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Affects(a, _) pattern a(v, _\"b\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "z");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex3 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "c");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("a+b");
		std::string postFixExpression2 = ExpressionProcessor::convertInfixToPostFix("a");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex3);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt2, varIndex3);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);
		pkbInserter->insertAssignInfo(varIndex3, postFixExpression2, stmt2);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("b");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable& evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable& projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_AffectsAndModifiesSOnly_success) {
		// a = b + c
		// c = a
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Affects(a, _) and Modifies(a, v)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "z");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex3 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "c");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("a+b");
		std::string postFixExpression2 = ExpressionProcessor::convertInfixToPostFix("a");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex3);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt2, varIndex3);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);
		pkbInserter->insertAssignInfo(varIndex3, postFixExpression2, stmt2);

		// Check PKB population
		std::vector<int> allAssignStmts = pkbGetter->getStmtIdxFromType(StatementType::ASSIGN_TYPE);
		Assert::AreEqual(size_t(3), allAssignStmts.size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable& evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable& projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_AffectsAndUsesSOnly_success) {
		// a = b + c
		// c = a
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Affects(a, _) and Uses(a, v)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "z");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex3 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "c");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("a+b");
		std::string postFixExpression2 = ExpressionProcessor::convertInfixToPostFix("a");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex3);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt2, varIndex3);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);
		pkbInserter->insertAssignInfo(varIndex3, postFixExpression2, stmt2);

		// Check PKB population
		std::vector<int> allAssignStmts = pkbGetter->getStmtIdxFromType(StatementType::ASSIGN_TYPE);
		Assert::AreEqual(size_t(3), allAssignStmts.size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable& evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable& projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_AffectsStarAndPatternAOnly_success) {
		// while (true) {
		//	a = b + c
		//	c = a
		// }
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Affects*(a, _) pattern a(v, _\"b\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "z");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex3 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "c");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("a+b");
		std::string postFixExpression2 = ExpressionProcessor::convertInfixToPostFix("a");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex3);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex3);

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex3);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt3, varIndex3);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt3, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt2, stmt3);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt3, stmt);

		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt2, stmt3);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt3, stmt);

		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt2);
		pkbInserter->insertAssignInfo(varIndex3, postFixExpression2, stmt3);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("b");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable& evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>

		std::vector<int> values{ 8 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable& projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "8" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_AffectandAffectsStar_success) {
		// while (true) {
		//	a = b + c
		//	c = a
		// }
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Affects(8, 9) and Affects*(a, _)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "z");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex3 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "c");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("a+b");
		std::string postFixExpression2 = ExpressionProcessor::convertInfixToPostFix("a");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex3);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex3);

		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex3);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt3, varIndex3);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt3, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt2, stmt3);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt3, stmt);

		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt2, stmt3);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt3, stmt);

		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt2);
		pkbInserter->insertAssignInfo(varIndex3, postFixExpression2, stmt3);

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable& evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>

		std::unordered_set<int> values{ 8, 9 };
		std::vector<int> actualValues = tableRef.at("a");
		std::unordered_set<int> actualValuesSet(actualValues.begin(), actualValues.end());

		Assert::IsTrue(values == actualValuesSet);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable& projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::unordered_set<std::string> resultSet(results.begin(), results.end());
		std::unordered_set<std::string> expectedRes{ "8", "9" };
		Assert::IsTrue(expectedRes == resultSet);
	}

	TEST_METHOD(querying_AffectsAndPatternWOnly_success) {
		// a = b + c
		// c = a
		// 1. Setup:
		std::string query = "assign a; variable v; Select a such that Affects(a, _) pattern a(v, _\"b\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "z");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		VarIndex varIndex3 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "c");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("a+b");
		std::string postFixExpression2 = ExpressionProcessor::convertInfixToPostFix("a");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex2);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt, varIndex3);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmt2, varIndex3);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmt2, varIndex);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmt, stmt2);
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);
		pkbInserter->insertAssignInfo(varIndex3, postFixExpression2, stmt2);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("b");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable& evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable& projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_FollowsAndPatternAOnly_success) {
		// stmt 7. abc = abc + d;
		// stmt 8. d = abc + d;
		// 6 is followed by 7 (read follows assign), but 7 is not followed by 8
		// Results: {"a", {6}}
		// 1. Setup:
		std::string query = "read r; assign a; variable v; Select a such that Follows(r, a) pattern a(v, _\"d\"_)";
		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));

		StmtIndex readStmt = pkbInserter->insertStmt(StatementType::READ_TYPE);

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "def");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "abc");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "d");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix(" abc + d ");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);
		pkbInserter->insertAssignInfo(varIndex2, postFixExpression, stmt2);
		pkbInserter->insertRSInfo(RelationshipType::FOLLOWS, readStmt, stmt);

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("d");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(2), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(3), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getRelationships().size());
		Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_ParentStarAndPatternAOnlyStress99_success) {
		// stmt 99. abc = abc + d;
		// 6 is followed by 7 (read follows assign), but 7 is not followed by 8
		// Results: {"a", {6}}
		// 1. Setup:
		std::string query = "while w; assign a; variable v; Select a such that Parent*(w, a) pattern a(v, _\"d\"_)";
		// PKB inserts 99 statements and 98 Parent* relationships
		std::vector<StmtIndex> stmts;
		for (int i = 0; i < 98; i++) {
			stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		}
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE)); // 99th stmt is assignment

		for (int i = 0; i < 99; i++) {
			for (int j = i + 1; j < 99; j++) {
				pkbInserter->insertRSInfo(RelationshipType::PARENT_T, stmts[i], stmts[j]); // i parents i + 1, simulate 99 nesting levels
			}
		}

		// PKB inserts pattern
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "def");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "abc");
		VarIndex varIndex2 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "d");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix(" abc + d ");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmts[98]); // stmt 99

		// Check PKB population
		std::string postFixExpressionQuery = ExpressionProcessor::convertInfixToPostFix("d");
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(postFixExpressionQuery);
		Assert::AreEqual(size_t(1), std::get<0>(allPatternStmtInfo).size());

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(3), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getRelationships().size());
		Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end());
		Assert::AreEqual(true, tableRef.find("w") == tableRef.end());
		Assert::AreEqual(true, tableRef.find("v") == tableRef.end());

		// Test Values: std::unordered_map<std::string, EntityType>
		std::unordered_set<int> aValues;
		for (int i = 0; i < 98; i++) {
			aValues.insert(99);
		}
		std::vector<int> actualCol = tableRef.at("a");
		std::unordered_set<int> actualSet(actualCol.begin(), actualCol.end());
		Assert::IsTrue(aValues == actualSet);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "99" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_declarationAndPatternAExprOnly_success) {
		// b = (x + 1) * 2 / 3 - 4 % (5)
		// 1. Setup:
		std::string query = "assign a; variable v; Select a pattern a(v, _\"x + 1\"_)";

		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("(x+1)*2/3-4%(5)");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end()); // "a" exists
		Assert::AreEqual(true, tableRef.find("v") == tableRef.end()); // "v" exists

		//// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
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
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("(x+1)*2/3-4%(5)");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end()); // "a" exists
		Assert::AreEqual(true, tableRef.find("v") == tableRef.end()); // "v" exists

		//// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values;
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes;
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_patternAFullMatchButSubtree_emptyResult) {
		// b = (x + 1) * 2 / 3 - 4 % (5)
		// 1. Setup:
		std::string query = "assign a; variable v; Select a pattern a(v, \"x + 1\")";

		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("(x+1)*2/3-4%(5)");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end()); // "a" exists
		Assert::AreEqual(true, tableRef.find("v") == tableRef.end()); // "v" exists

		//// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values;
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes;
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_patternAFullMatchWithFullExpression_success) {
		// b = (x + 1) * 2 / 3 - 4 % (5)
		// 1. Setup:
		std::string query = "assign a; variable v; Select a pattern a(v, _\"(x + 1)       * 2 / 3		-4 %\n 5\"_)";

		// PKB inserts 6 types of statements
		std::vector<StmtIndex> stmts;
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::ASSIGN_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::PRINT_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::CALL_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::IF_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::WHILE_TYPE));
		stmts.emplace_back(pkbInserter->insertStmt(StatementType::READ_TYPE));

		// PKB inserts pattern
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "a");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "b");
		std::string postFixExpression = ExpressionProcessor::convertInfixToPostFix("(x+1)*2/3-4%(5)");
		pkbInserter->insertAssignInfo(varIndex, postFixExpression, stmt);

		// 2. Test QPS Parser:
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getPatterns().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("a") != tableRef.end()); // "a" exists
		Assert::AreEqual(true, tableRef.find("v") == tableRef.end()); // "v" exists

		//// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 7 };
		auto actualValues = tableRef.at("a");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "7" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}
	TEST_METHOD(querying_withStringTwoAttributeComparison_successful) {
		/* Insert test program:
		* procedure proc1 {
		* read proc1;
		* print read;
		* read = proc1 + 2;
		* }
		*/
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmtIndex1 = pkbInserter->insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "proc1");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex1, varProc1);
		pkbInserter->insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "read");
		pkbInserter->insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex2, varRead);
		StmtIndex stmtIndex3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex3, varRead);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex3, varProc1);
		pkbInserter->insertConst(2);

		// 2. Test QPS Parser:
		std::string query = "read r; procedure p; Select r with r.varName = p.procName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getWiths().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("r") != tableRef.end()); // "r" exists

		//// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 1 };
		auto actualValues = tableRef.at("r");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "1" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_withStringAttributeIdentComparison_successful) {
		/* Insert test program:
		* procedure proc1 {
		* read proc1;
		* print read;
		* read = proc1 + 2;
		* }
		*/
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmtIndex1 = pkbInserter->insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "proc1");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex1, varProc1);
		pkbInserter->insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "read");
		pkbInserter->insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex2, varRead);
		StmtIndex stmtIndex3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex3, varRead);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex3, varProc1);
		pkbInserter->insertConst(2);

		// 2. Test QPS Parser:
		std::string query = "print pn; Select pn with \"read\" = pn.varName";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(1), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getWiths().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("pn") != tableRef.end()); // "pn" exists

		//// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 2 };
		auto actualValues = tableRef.at("pn");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "2" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_withStringTwoIdentComparison_successful) {
		/* Insert test program:
		* procedure proc1 {
		* read proc1;
		* print read;
		* read = proc1 + 2;
		* }
		*/
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmtIndex1 = pkbInserter->insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "proc1");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex1, varProc1);
		pkbInserter->insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "read");
		pkbInserter->insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex2, varRead);
		StmtIndex stmtIndex3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex3, varRead);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex3, varProc1);
		pkbInserter->insertConst(2);

		/* 1. Setup:
		* Clause: with "print" = "read"
		*/

		// 2. Test QPS Parser:
		std::string query = "stmt s; Select s with \"abc\" = \"abc\"";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(1), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getWiths().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test evResult:
		Assert::AreEqual(true, evTable.getEvResult());

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "1", "2", "3" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_withIntegerTwoAttributeComparison_successful) {
		/* Insert test program:
		* procedure proc1 {
		* read proc1;
		* print read;
		* read = proc1 + 2;
		* }
		*/
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmtIndex1 = pkbInserter->insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "proc1");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex1, varProc1);
		pkbInserter->insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "read");
		pkbInserter->insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex2, varRead);
		StmtIndex stmtIndex3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex3, varRead);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex3, varProc1);
		pkbInserter->insertConst(2);

		// 2. Test QPS Parser:
		std::string query = "constant c; stmt s; Select c with c.value = s.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getWiths().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("c") != tableRef.end()); // "c" exists
		Assert::AreEqual(true, tableRef.find("s") == tableRef.end()); // "s" exists

		//// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 2 };
		auto actualValues = tableRef.at("c");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "2" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_withIntegerAttributeIntegerComparison_successful) {
		/* Insert test program:
		* procedure proc1 {
		* read proc1;
		* print read;
		* read = proc1 + 2;
		* }
		*/
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmtIndex1 = pkbInserter->insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "proc1");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex1, varProc1);
		pkbInserter->insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "read");
		pkbInserter->insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex2, varRead);
		StmtIndex stmtIndex3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex3, varRead);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex3, varProc1);
		pkbInserter->insertConst(2);

		// 2. Test QPS Parser:
		std::string query = "read r; Select r with 1 = r.stmt#";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(1), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getWiths().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test Table:
		auto tableRef = evTable.getTableRef();
		Assert::AreEqual(true, tableRef.find("r") != tableRef.end()); // "r" exists

		//// Test Values: std::unordered_map<std::string, EntityType>
		std::vector<int> values{ 1 };
		auto actualValues = tableRef.at("r");
		bool areVecEqual = std::equal(values.begin(), values.end(), actualValues.begin());
		Assert::AreEqual(true, areVecEqual);

		// Test EvResult:
		bool actualEvResult = evTable.getEvResult();
		Assert::AreEqual(true, actualEvResult);

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "1" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}

	TEST_METHOD(querying_withIntegerTwoIntegerComparison_successful) {
		/* Insert test program:
		* procedure proc1 {
		* read proc1;
		* print read;
		* read = proc1 + 2;
		* }
		*/
		ProcIndex procIndex = pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, "proc1");
		StmtIndex stmtIndex1 = pkbInserter->insertStmt(StatementType::READ_TYPE);
		VarIndex varProc1 = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "proc1");
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex1, varProc1);
		pkbInserter->insertStmtByName(stmtIndex1, StatementType::READ_TYPE, std::string("proc1"));
		StmtIndex stmtIndex2 = pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		VarIndex varRead = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "read");
		pkbInserter->insertStmtByName(stmtIndex2, StatementType::PRINT_TYPE, std::string("read"));
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex2, varRead);
		StmtIndex stmtIndex3 = pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertRSInfo(RelationshipType::MODIFIES_S, stmtIndex3, varRead);
		pkbInserter->insertRSInfo(RelationshipType::USES_S, stmtIndex3, varProc1);
		pkbInserter->insertConst(2);

		// 2. Test QPS Parser:
		std::string query = "variable v; Select v with 100 = 100";
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		Assert::AreEqual(size_t(1), parsedQuery.getDeclarations().size());
		Assert::AreEqual(size_t(1), parsedQuery.getColumns().size());
		Assert::AreEqual(size_t(1), parsedQuery.getWiths().size());

		// 3. Test QPS Evaluator:
		PQLEvaluator pqlEvaluator = PQLEvaluator::PQLEvaluator(parsedQuery, pkbGetter);
		EvaluatedTable evTable = pqlEvaluator.evaluate();

		//// Test evResult:
		Assert::AreEqual(true, evTable.getEvResult());

		// 4. Test QPS Result Projector:
		EvaluatedTable projectedEvTable = pqlEvaluator.selectProjection(evTable);
		PQLResultProjector pqlResultProjector = PQLResultProjector(projectedEvTable, parsedQuery, pkbGetter);
		std::list<std::string> results = pqlResultProjector.resolveTableToResults();
		std::list<std::string> expectedRes{ "proc1", "read" };
		bool areListsEqual = std::equal(expectedRes.begin(), expectedRes.end(), results.begin());
		Assert::AreEqual(true, areListsEqual);
	}
	};
}
