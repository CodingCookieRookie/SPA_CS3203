#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/QPS/EvaluatedTable.h"
#include "../source/QPS/PQLResultProjector.h"
#include "PKB/Entity.h"
#include "PKB/Calls.h"
#include "PKB/ModifiesS.h"
#include "PKB/UsesS.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPQLResultProjector) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Attribute::performCleanUp();
		Entity::performCleanUp();
		Calls::performCleanUp();
		UsesS::performCleanUp();
		ModifiesS::performCleanUp();
	}
public:

	TEST_METHOD(resolveTableToResults_oneColumnStatement_projectOneColumn) {
		// 1. Set-up:
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		std::vector<int> vec;
		vec.push_back(1);
		vec.push_back(3);
		vec.push_back(5);
		testTable["s"] = vec;
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "s");

		std::vector <std::pair<EntityType, std::string>> declarations;
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s"));
		std::vector<ParsedRelationship> noRelationships;
		std::vector<ParsedPattern> noPatterns;
		std::vector<ParsedWith> noWiths;
		ParsedQuery parsedQuery = ParsedQuery(declarations, attributesProjected,
			noRelationships, noPatterns, noWiths);

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(testTable);
		std::list<std::string> expected;
		expected.push_back("1");
		expected.push_back("3");
		expected.push_back("5");
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, parsedQuery);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
	}

	TEST_METHOD(resolveTableToResults_oneColumnVariable_projectOneColumn) {
		// 1. Set-up:
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		std::vector<int> vec;
		vec.push_back(1);
		vec.push_back(2);
		vec.push_back(3);
		Entity::insertVar("a");
		Entity::insertVar("b");
		Entity::insertVar("c");
		testTable["v"] = vec;
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "v");

		std::vector <std::pair<EntityType, std::string>> declarations;
		declarations.emplace_back(std::make_pair(EntityType::VARIABLE, "v"));
		std::vector<ParsedRelationship> noRelationships;
		std::vector<ParsedPattern> noPatterns;
		std::vector<ParsedWith> noWiths;
		ParsedQuery parsedQuery = ParsedQuery(declarations, attributesProjected,
			noRelationships, noPatterns, noWiths);

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(testTable);
		std::list<std::string> expected;
		expected.push_back("a");
		expected.push_back("b");
		expected.push_back("c");
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, parsedQuery);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
	}

	TEST_METHOD(resolveTableToResults_oneColumnProcedure_projectOneColumn) {
		// 1. Set-up:
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		std::vector<int> vec;
		vec.push_back(1);
		vec.push_back(2);
		vec.push_back(3);
		Entity::insertProc("proc1");
		Entity::insertProc("proc2");
		Entity::insertProc("proc3");
		testTable["p"] = vec;
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "p");

		std::vector <std::pair<EntityType, std::string>> declarations;
		declarations.emplace_back(std::make_pair(EntityType::PROCEDURE, "p"));
		std::vector<ParsedRelationship> noRelationships;
		std::vector<ParsedPattern> noPatterns;
		std::vector<ParsedWith> noWiths;
		ParsedQuery parsedQuery = ParsedQuery(declarations, attributesProjected,
			noRelationships, noPatterns, noWiths);

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(testTable);
		std::list<std::string> expected;
		expected.push_back("proc1");
		expected.push_back("proc2");
		expected.push_back("proc3");
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, parsedQuery);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
	}

	TEST_METHOD(resolveTableToResults_oneColumnConstantRepeated_projectOneColumnUnique) {
		// 1. Set-up:
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "c1");
		testTable["c1"] = std::vector<int>{ 1, 1, 1, 2, 2, 3, 3, 4 };

		std::vector <std::pair<EntityType, std::string>> declarations;
		declarations.emplace_back(std::make_pair(EntityType::CONSTANT, "c1"));
		std::vector<ParsedRelationship> noRelationships;
		std::vector<ParsedPattern> noPatterns;
		std::vector<ParsedWith> noWiths;
		ParsedQuery parsedQuery = ParsedQuery(declarations, attributesProjected,
			noRelationships, noPatterns, noWiths);

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(testTable);
		std::list<std::string> expected{ "1", "2", "3", "4" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, parsedQuery);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
	}

	TEST_METHOD(resolveTableToResults_oneColumnStatementRepeated_projectOneColumnUnique) {
		// 1. Set-up:
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "s1");
		testTable["s1"] = std::vector<int>{ 1, 1, 1, 2, 2, 3, 3, 4 };

		std::vector <std::pair<EntityType, std::string>> declarations;
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s1"));
		std::vector<ParsedRelationship> noRelationships;
		std::vector<ParsedPattern> noPatterns;
		std::vector<ParsedWith> noWiths;
		ParsedQuery parsedQuery = ParsedQuery(declarations, attributesProjected,
			noRelationships, noPatterns, noWiths);

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(testTable);
		std::list<std::string> expected{ "1", "2", "3", "4" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, parsedQuery);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
	}

	TEST_METHOD(resolveTableToResults_twoColumnStatement_projectTwoColumns) {
		// 1. Set-up:
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "s1");
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "s2");
		testTable["s1"] = std::vector<int>{ 1, 2, 3 };
		testTable["s2"] = std::vector<int>{ 4, 5, 6 };

		std::vector <std::pair<EntityType, std::string>> declarations;
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s1"));
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s2"));
		std::vector<ParsedRelationship> noRelationships;
		std::vector<ParsedPattern> noPatterns;
		std::vector<ParsedWith> noWiths;
		ParsedQuery parsedQuery = ParsedQuery(declarations, attributesProjected,
			noRelationships, noPatterns, noWiths);

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(testTable);
		std::list<std::string> expected{ "1 4", "2 5", "3 6" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, parsedQuery);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
	}

	TEST_METHOD(resolveTableToResults_twoColumnStatementEachColWithRepeats_projectTwoColumns) {
		// 1. Set-up:
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "s1");
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "s2");
		testTable["s1"] = std::vector<int>{ 1, 1, 2, 2, 3 };
		testTable["s2"] = std::vector<int>{ 4, 5, 5, 6, 6 };

		std::vector <std::pair<EntityType, std::string>> declarations;
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s1"));
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s2"));
		std::vector<ParsedRelationship> noRelationships;
		std::vector<ParsedPattern> noPatterns;
		std::vector<ParsedWith> noWiths;
		ParsedQuery parsedQuery = ParsedQuery(declarations, attributesProjected,
			noRelationships, noPatterns, noWiths);

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(testTable);
		std::list<std::string> expected{ "1 4", "1 5", "2 5", "2 6", "3 6" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, parsedQuery);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
	}

	TEST_METHOD(resolveTableToResults_twoColumnStatementWithRepeatsInBoth_projectTwoColumnsUnique) {
		// 1. Set-up:
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "s1");
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "s2");
		testTable["s1"] = std::vector<int>{ 1, 1, 2, 2, 3, 3, 4, 4, 4 };
		testTable["s2"] = std::vector<int>{ 4, 5, 5, 6, 6, 6, 7, 7, 7 };

		std::vector <std::pair<EntityType, std::string>> declarations;
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s1"));
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s2"));
		std::vector<ParsedRelationship> noRelationships;
		std::vector<ParsedPattern> noPatterns;
		std::vector<ParsedWith> noWiths;
		ParsedQuery parsedQuery = ParsedQuery(declarations, attributesProjected,
			noRelationships, noPatterns, noWiths);

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(testTable);
		std::list<std::string> expected{ "1 4", "1 5", "2 5", "2 6", "3 6", "4 7" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, parsedQuery);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
	}

	TEST_METHOD(resolveTableToResults_twoColumnStatement_projectOneColumn) {
		// 1. Set-up:
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "s1");
		testTable["s1"] = std::vector<int>{ 1, 2, 3 };
		testTable["s2"] = std::vector<int>{ 4, 5, 6 };

		std::vector <std::pair<EntityType, std::string>> declarations;
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s1"));
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s2"));
		std::vector<ParsedRelationship> noRelationships;
		std::vector<ParsedPattern> noPatterns;
		std::vector<ParsedWith> noWiths;
		ParsedQuery parsedQuery = ParsedQuery(declarations, attributesProjected,
			noRelationships, noPatterns, noWiths);

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(testTable);
		std::list<std::string> expected{ "1", "2", "3" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, parsedQuery);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
	}

	TEST_METHOD(resolveTableToResults_sixColumnsMixed_projectFourColumns) {
		// 1. Set-up:
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "s1");
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "v1");
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "a1");
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "p1");
		testTable["s1"] = std::vector<int>{ 1, 2, 3 };
		testTable["s2"] = std::vector<int>{ 4, 5, 6 };
		testTable["v1"] = std::vector<int>{ 1, 2, 3 };
		testTable["v2"] = std::vector<int>{ 4, 5, 6 };
		testTable["a1"] = std::vector<int>{ 13, 14, 15 };
		testTable["p1"] = std::vector<int>{ 1, 2, 3 };
		for (int i = 0; i < 6; i++) {
			Entity::insertVar("var" + std::to_string(i + 1));
		}
		for (int i = 0; i < 3; i++) {
			Entity::insertProc("proc" + std::to_string(i + 1));
		}

		std::vector <std::pair<EntityType, std::string>> declarations;
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s1"));
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s2"));
		declarations.emplace_back(std::make_pair(EntityType::VARIABLE, "v1"));
		declarations.emplace_back(std::make_pair(EntityType::VARIABLE, "v2"));
		declarations.emplace_back(std::make_pair(EntityType::ASSIGN, "a1"));
		declarations.emplace_back(std::make_pair(EntityType::PROCEDURE, "p1"));
		std::vector<ParsedRelationship> noRelationships;
		std::vector<ParsedPattern> noPatterns;
		std::vector<ParsedWith> noWiths;
		ParsedQuery parsedQuery = ParsedQuery(declarations, attributesProjected,
			noRelationships, noPatterns, noWiths);

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(testTable);
		std::list<std::string> expected{ "1 var1 13 proc1", "2 var2 14 proc2", "3 var3 15 proc3" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, parsedQuery);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
	}

	TEST_METHOD(resolveTableToResults_sixColumnsMixed_projectFourWithProcName) {
		// 1. Set-up:
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "s1");
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "v1");
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "a1");
		attributesProjected.emplace_back(PqlReferenceType::PROC_NAME, "p1");
		testTable["s1"] = std::vector<int>{ 1, 2, 3 };
		testTable["s2"] = std::vector<int>{ 4, 5, 6 };
		testTable["v1"] = std::vector<int>{ 1, 2, 3 };
		testTable["v2"] = std::vector<int>{ 4, 5, 6 };
		testTable["a1"] = std::vector<int>{ 13, 14, 15 };
		testTable["p1"] = std::vector<int>{ 1, 2, 3 };
		for (int i = 0; i < 6; i++) {
			Entity::insertVar("var" + std::to_string(i + 1));
		}
		for (int i = 0; i < 3; i++) {
			Entity::insertProc("proc" + std::to_string(i + 1));
		}
		std::vector <std::pair<EntityType, std::string>> declarations;
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s1"));
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s2"));
		declarations.emplace_back(std::make_pair(EntityType::VARIABLE, "v1"));
		declarations.emplace_back(std::make_pair(EntityType::VARIABLE, "v2"));
		declarations.emplace_back(std::make_pair(EntityType::ASSIGN, "a1"));
		declarations.emplace_back(std::make_pair(EntityType::PROCEDURE, "p1"));
		std::vector<ParsedRelationship> noRelationships;
		std::vector<ParsedPattern> noPatterns;
		std::vector<ParsedWith> noWiths;
		ParsedQuery parsedQuery = ParsedQuery(declarations, attributesProjected,
			noRelationships, noPatterns, noWiths);

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(testTable);
		std::list<std::string> expected{ "1 var1 13 proc1", "2 var2 14 proc2", "3 var3 15 proc3" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, parsedQuery);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
	}

	TEST_METHOD(resolveTableToResults_sixColumnsMixed_projectMultipleProcName) {
		// 1. Set-up:
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "cl");
		attributesProjected.emplace_back(PqlReferenceType::PROC_NAME, "cl");
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "p");
		attributesProjected.emplace_back(PqlReferenceType::PROC_NAME, "p");
		testTable["cl"] = std::vector<int>{ 1, 2 };
		testTable["p"] = std::vector<int>{ 1, 2, 3 };
		StmtIndex CallIdx1 = Entity::insertStmt(StatementType::CALL_TYPE);
		StmtIndex CallIdx2 = Entity::insertStmt(StatementType::CALL_TYPE);
		std::string proc1 = "proc1";
		std::string proc2 = "proc2";
		std::string proc3 = "proc3";
		ProcIndex ProcIdx1 = Entity::insertProc("proc1");
		ProcIndex ProcIdx2 = Entity::insertProc("proc2");
		ProcIndex ProcIdx3 = Entity::insertProc("proc3");
		Attribute::insertStmtByName(CallIdx1, StatementType::CALL_TYPE, proc2);
		Attribute::insertStmtByName(CallIdx2, StatementType::CALL_TYPE, proc3);

		std::vector <std::pair<EntityType, std::string>> declarations;
		declarations.emplace_back(std::make_pair(EntityType::CALL, "cl"));
		declarations.emplace_back(std::make_pair(EntityType::PROCEDURE, "p"));
		std::vector<ParsedRelationship> noRelationships;
		std::vector<ParsedPattern> noPatterns;
		std::vector<ParsedWith> noWiths;
		ParsedQuery parsedQuery = ParsedQuery(declarations, attributesProjected,
			noRelationships, noPatterns, noWiths);

		// Simulating stmt 1 in proc1 calls proc2, stmt2 in proc2 calls proc3
		// procedure p; call cl; Select <cl, cl.procName, p, p.procName>

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(testTable);
		std::list<std::string> expected{
			"1 proc2 proc1 proc1", "2 proc3 proc2 proc2" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, parsedQuery);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
	}

	TEST_METHOD(resolveTableToResults_sixColumnsMixed_projectMultipleVarName) {
		// 1. Set-up:
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::VAR_NAME, "v");
		attributesProjected.emplace_back(PqlReferenceType::VAR_NAME, "r");
		attributesProjected.emplace_back(PqlReferenceType::VAR_NAME, "pn");
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "v");
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "r");
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "pn");
		testTable["v"] = std::vector<int>{ 1, 2, 3 };
		testTable["r"] = std::vector<int>{ 1, 2, 3 };
		testTable["pn"] = std::vector<int>{ 4, 5, 6 };
		std::string var1 = "var1";
		std::string var2 = "var2";
		std::string var3 = "var3";
		VarIndex varIndex1 = Entity::insertVar("var1");
		VarIndex varIndex2 = Entity::insertVar("var2");
		VarIndex varIndex3 = Entity::insertVar("var3");
		StmtIndex readStmt1 = Entity::insertStmt(StatementType::READ_TYPE);
		StmtIndex readStmt2 = Entity::insertStmt(StatementType::READ_TYPE);
		StmtIndex readStmt3 = Entity::insertStmt(StatementType::READ_TYPE);
		StmtIndex printStmt4 = Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex printStmt5 = Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex printStmt6 = Entity::insertStmt(StatementType::PRINT_TYPE);
		Attribute::insertStmtByName(readStmt1, StatementType::READ_TYPE, var1); // simulate stmt 1 reading var1
		Attribute::insertStmtByName(readStmt2, StatementType::READ_TYPE, var2); // simulate stmt 2 reading var2
		Attribute::insertStmtByName(readStmt3, StatementType::READ_TYPE, var3); // simulate stmt 3 reading var3
		Attribute::insertStmtByName(printStmt4, StatementType::PRINT_TYPE, var1); // simulate stmt 4 printing var1
		Attribute::insertStmtByName(printStmt5, StatementType::PRINT_TYPE, var2); // simulate stmt 5 printing var2
		Attribute::insertStmtByName(printStmt6, StatementType::PRINT_TYPE, var3); // simulate stmt 6 printing var3

		std::vector <std::pair<EntityType, std::string>> declarations;
		declarations.emplace_back(std::make_pair(EntityType::VARIABLE, "v"));
		declarations.emplace_back(std::make_pair(EntityType::READ, "r"));
		declarations.emplace_back(std::make_pair(EntityType::PRINT, "pn"));
		std::vector<ParsedRelationship> noRelationships;
		std::vector<ParsedPattern> noPatterns;
		std::vector<ParsedWith> noWiths;
		ParsedQuery parsedQuery = ParsedQuery(declarations, attributesProjected,
			noRelationships, noPatterns, noWiths);

		// print pn; read r; variable v; Select <v.varName, r.varName, pn.varName, v, r, pn>

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(testTable);
		std::list<std::string> expected{
			"var1 var1 var1 var1 1 4",
			"var2 var2 var2 var2 2 5",
			"var3 var3 var3 var3 3 6" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, parsedQuery);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
	}

	TEST_METHOD(resolveTableToResults_sixColumnsMixed_projectConstantValue) {
		// 1. Set-up:
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "s1");
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "c1");
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "c2");
		attributesProjected.emplace_back(PqlReferenceType::VALUE, "c2");
		testTable["s1"] = std::vector<int>{ 1, 2 };
		testTable["c1"] = std::vector<int>{ 10, 20 };
		testTable["c2"] = std::vector<int>{ 99, 88 };

		std::vector <std::pair<EntityType, std::string>> declarations;
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s1"));
		declarations.emplace_back(std::make_pair(EntityType::CONSTANT, "c1"));
		declarations.emplace_back(std::make_pair(EntityType::CONSTANT, "c2"));
		std::vector<ParsedRelationship> noRelationships;
		std::vector<ParsedPattern> noPatterns;
		std::vector<ParsedWith> noWiths;
		ParsedQuery parsedQuery = ParsedQuery(declarations, attributesProjected,
			noRelationships, noPatterns, noWiths);

		// stmt s1; constant c1, c2; Select <s1, c1, c2, c2.value>

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(testTable);
		std::list<std::string> expected{
			"1 10 99 99", "2 20 88 88" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, parsedQuery);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
	}

	TEST_METHOD(resolveTableToResults_sixColumnsMixed_projectAllStmtNumbers) {
		// 1. Set-up:
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::SYNONYM, "s1");
		attributesProjected.emplace_back(PqlReferenceType::STMT_NUM, "s2");
		attributesProjected.emplace_back(PqlReferenceType::STMT_NUM, "r1");
		attributesProjected.emplace_back(PqlReferenceType::STMT_NUM, "pn1");
		attributesProjected.emplace_back(PqlReferenceType::STMT_NUM, "cl1");
		attributesProjected.emplace_back(PqlReferenceType::STMT_NUM, "w1");
		attributesProjected.emplace_back(PqlReferenceType::STMT_NUM, "if1");
		attributesProjected.emplace_back(PqlReferenceType::STMT_NUM, "a1");
		testTable["s1"] = std::vector<int>{ 1, 2 };
		testTable["s2"] = std::vector<int>{ 3, 4 };
		testTable["r1"] = std::vector<int>{ 5, 6 };
		testTable["pn1"] = std::vector<int>{ 7, 8 };
		testTable["cl1"] = std::vector<int>{ 9, 10 };
		testTable["w1"] = std::vector<int>{ 11, 12 };
		testTable["if1"] = std::vector<int>{ 13, 14 };
		testTable["a1"] = std::vector<int>{ 15, 16 };

		std::vector <std::pair<EntityType, std::string>> declarations;
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s1"));
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s2"));
		declarations.emplace_back(std::make_pair(EntityType::READ, "r1"));
		declarations.emplace_back(std::make_pair(EntityType::PRINT, "pn1"));
		declarations.emplace_back(std::make_pair(EntityType::CALL, "cl1"));
		declarations.emplace_back(std::make_pair(EntityType::WHILE, "w1"));
		declarations.emplace_back(std::make_pair(EntityType::IF, "if1"));
		declarations.emplace_back(std::make_pair(EntityType::ASSIGN, "a1"));
		std::vector<ParsedRelationship> noRelationships;
		std::vector<ParsedPattern> noPatterns;
		std::vector<ParsedWith> noWiths;
		ParsedQuery parsedQuery = ParsedQuery(declarations, attributesProjected,
			noRelationships, noPatterns, noWiths);

		// Select <s1, s2.stmt#, r1.stmt#, pn1.stmt#, cl1.stmt#, w1.stmt#, if1.stmt#, a1.stmt#>

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(testTable);
		std::list<std::string> expected{
			"1 3 5 7 9 11 13 15",
			"2 4 6 8 10 12 14 16" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, parsedQuery);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
	}

	TEST_METHOD(resolveTableToResults_sixColumnsMixed_projectBooleanTrue) {
		// 1. Set-up:
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected; // Empty attributesProjected table
		testTable["s1"] = std::vector<int>{ 1, 2 };
		testTable["s2"] = std::vector<int>{ 3, 4 };
		testTable["r1"] = std::vector<int>{ 5, 6 };
		testTable["pn1"] = std::vector<int>{ 7, 8 };
		testTable["cl1"] = std::vector<int>{ 9, 10 };
		testTable["w1"] = std::vector<int>{ 11, 12 };
		testTable["if1"] = std::vector<int>{ 13, 14 };
		testTable["a1"] = std::vector<int>{ 15, 16 };

		std::vector <std::pair<EntityType, std::string>> declarations;
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s1"));
		declarations.emplace_back(std::make_pair(EntityType::STMT, "s2"));
		declarations.emplace_back(std::make_pair(EntityType::READ, "r1"));
		declarations.emplace_back(std::make_pair(EntityType::PRINT, "pn1"));
		declarations.emplace_back(std::make_pair(EntityType::CALL, "cl1"));
		declarations.emplace_back(std::make_pair(EntityType::WHILE, "w1"));
		declarations.emplace_back(std::make_pair(EntityType::IF, "if1"));
		declarations.emplace_back(std::make_pair(EntityType::ASSIGN, "a1"));
		std::vector<ParsedRelationship> noRelationships;
		std::vector<ParsedPattern> noPatterns;
		std::vector<ParsedWith> noWiths;
		ParsedQuery parsedQuery = ParsedQuery(declarations, attributesProjected,
			noRelationships, noPatterns, noWiths);

		// Select <s1, s2.stmt#, r1.stmt#, pn1.stmt#, cl1.stmt#, w1.stmt#, if1.stmt#, a1.stmt#>

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(testTable);
		std::list<std::string> expected{ "TRUE" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, parsedQuery);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
	}

	TEST_METHOD(resolveTableToResults_sixColumnsMixed_projectBooleanFalse) {
		// 1. Set-up:
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected; // Empty attributesProjected table

		std::vector <std::pair<EntityType, std::string>> declarations;
		std::vector<ParsedRelationship> noRelationships;
		std::vector<ParsedPattern> noPatterns;
		std::vector<ParsedWith> noWiths;
		ParsedQuery parsedQuery = ParsedQuery(declarations, attributesProjected,
			noRelationships, noPatterns, noWiths);

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(false);
		std::list<std::string> expected{ "FALSE" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, parsedQuery);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
	}

	};
}
