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
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		std::vector<int> vec;
		vec.push_back(1);
		vec.push_back(3);
		vec.push_back(5);
		entities["s"] = PqlEntityType::Stmt;
		testTable["s"] = vec;
		attributesProjected.emplace_back(PqlReferenceType::synonym, "s");

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected;
		expected.push_back("1");
		expected.push_back("3");
		expected.push_back("5");
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, attributesProjected, entities);
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
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		std::vector<int> vec;
		vec.push_back(1);
		vec.push_back(2);
		vec.push_back(3);
		Entity::insertVar("a");
		Entity::insertVar("b");
		Entity::insertVar("c");
		entities["v"] = PqlEntityType::Variable;
		testTable["v"] = vec;
		attributesProjected.emplace_back(PqlReferenceType::synonym, "v");

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected;
		expected.push_back("a");
		expected.push_back("b");
		expected.push_back("c");
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, attributesProjected, entities);
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
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		std::vector<int> vec;
		vec.push_back(1);
		vec.push_back(2);
		vec.push_back(3);
		Entity::insertProc("proc1");
		Entity::insertProc("proc2");
		Entity::insertProc("proc3");
		entities["p"] = PqlEntityType::Procedure;
		testTable["p"] = vec;
		attributesProjected.emplace_back(PqlReferenceType::synonym, "p");

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected;
		expected.push_back("proc1");
		expected.push_back("proc2");
		expected.push_back("proc3");
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, attributesProjected, entities);
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
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::synonym, "c1");
		entities["c1"] = PqlEntityType::Constant;
		testTable["c1"] = std::vector<int>{ 1, 1, 1, 2, 2, 3, 3, 4 };

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{ "1", "2", "3", "4" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, attributesProjected, entities);
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
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::synonym, "s1");
		entities["s1"] = PqlEntityType::Stmt;
		testTable["s1"] = std::vector<int>{ 1, 1, 1, 2, 2, 3, 3, 4 };

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{ "1", "2", "3", "4" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, attributesProjected, entities);
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
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::synonym, "s1");
		attributesProjected.emplace_back(PqlReferenceType::synonym, "s2");
		entities["s1"] = PqlEntityType::Stmt;
		entities["s2"] = PqlEntityType::Stmt;
		testTable["s1"] = std::vector<int>{ 1, 2, 3 };
		testTable["s2"] = std::vector<int>{ 4, 5, 6 };

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{ "1 4", "2 5", "3 6" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, attributesProjected, entities);
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
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::synonym, "s1");
		attributesProjected.emplace_back(PqlReferenceType::synonym, "s2");
		entities["s1"] = PqlEntityType::Stmt;
		entities["s2"] = PqlEntityType::Stmt;
		testTable["s1"] = std::vector<int>{ 1, 1, 2, 2, 3 };
		testTable["s2"] = std::vector<int>{ 4, 5, 5, 6, 6 };

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{ "1 4", "1 5", "2 5", "2 6", "3 6" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, attributesProjected, entities);
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
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::synonym, "s1");
		attributesProjected.emplace_back(PqlReferenceType::synonym, "s2");
		entities["s1"] = PqlEntityType::Stmt;
		entities["s2"] = PqlEntityType::Stmt;
		testTable["s1"] = std::vector<int>{ 1, 1, 2, 2, 3, 3, 4, 4, 4 };
		testTable["s2"] = std::vector<int>{ 4, 5, 5, 6, 6, 6, 7, 7, 7 };

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{ "1 4", "1 5", "2 5", "2 6", "3 6", "4 7" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, attributesProjected, entities);
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
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::synonym, "s1");
		entities["s1"] = PqlEntityType::Stmt;
		entities["s2"] = PqlEntityType::Stmt;
		testTable["s1"] = std::vector<int>{ 1, 2, 3 };
		testTable["s2"] = std::vector<int>{ 4, 5, 6 };

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{ "1", "2", "3" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, attributesProjected, entities);
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
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::synonym, "s1");
		attributesProjected.emplace_back(PqlReferenceType::synonym, "v1");
		attributesProjected.emplace_back(PqlReferenceType::synonym, "a1");
		attributesProjected.emplace_back(PqlReferenceType::synonym, "p1");
		entities["s1"] = PqlEntityType::Stmt;
		entities["s2"] = PqlEntityType::Stmt;
		entities["v1"] = PqlEntityType::Variable;
		entities["v2"] = PqlEntityType::Variable;
		entities["a2"] = PqlEntityType::Stmt;
		entities["p1"] = PqlEntityType::Procedure;
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

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{ "1 var1 13 proc1", "2 var2 14 proc2", "3 var3 15 proc3" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, attributesProjected, entities);
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
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::synonym, "s1");
		attributesProjected.emplace_back(PqlReferenceType::synonym, "v1");
		attributesProjected.emplace_back(PqlReferenceType::synonym, "a1");
		attributesProjected.emplace_back(PqlReferenceType::procName, "p1");
		entities["s1"] = PqlEntityType::Stmt;
		entities["s2"] = PqlEntityType::Stmt;
		entities["v1"] = PqlEntityType::Variable;
		entities["v2"] = PqlEntityType::Variable;
		entities["a2"] = PqlEntityType::Stmt;
		entities["p1"] = PqlEntityType::Procedure;
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

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{ "1 var1 13 proc1", "2 var2 14 proc2", "3 var3 15 proc3" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, attributesProjected, entities);
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
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::synonym, "cl");
		attributesProjected.emplace_back(PqlReferenceType::procName, "cl");
		attributesProjected.emplace_back(PqlReferenceType::synonym, "p");
		attributesProjected.emplace_back(PqlReferenceType::procName, "p");
		entities["cl"] = PqlEntityType::Call;
		entities["p"] = PqlEntityType::Procedure;
		testTable["cl"] = std::vector<int>{ 1, 2 };
		testTable["p"] = std::vector<int>{ 1, 2, 3 };
		StmtIndex CallIdx1 = Entity::insertStmt(StatementType::callType);
		StmtIndex CallIdx2 = Entity::insertStmt(StatementType::callType);
		std::string proc1 = "proc1";
		std::string proc2 = "proc2";
		std::string proc3 = "proc3";
		ProcIndex ProcIdx1 = Entity::insertProc("proc1");
		ProcIndex ProcIdx2 = Entity::insertProc("proc2");
		ProcIndex ProcIdx3 = Entity::insertProc("proc3");
		Attribute::insertStmtByName(CallIdx1, StatementType::callType, proc2);
		Attribute::insertStmtByName(CallIdx2, StatementType::callType, proc3);

		// Simulating stmt 1 in proc1 calls proc2, stmt2 in proc2 calls proc3
		// procedure p; call cl; Select <cl, cl.procName, p, p.procName>

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{
			"1 proc2 proc1 proc1", "2 proc3 proc2 proc2" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, attributesProjected, entities);
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
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::varName, "v");
		attributesProjected.emplace_back(PqlReferenceType::varName, "r");
		attributesProjected.emplace_back(PqlReferenceType::varName, "pn");
		attributesProjected.emplace_back(PqlReferenceType::synonym, "v");
		attributesProjected.emplace_back(PqlReferenceType::synonym, "r");
		attributesProjected.emplace_back(PqlReferenceType::synonym, "pn");
		entities["v"] = PqlEntityType::Variable;
		entities["r"] = PqlEntityType::Read;
		entities["pn"] = PqlEntityType::Print;
		testTable["v"] = std::vector<int>{ 1, 2, 3 };
		testTable["r"] = std::vector<int>{ 1, 2, 3 };
		testTable["pn"] = std::vector<int>{ 4, 5, 6 };
		std::string var1 = "var1";
		std::string var2 = "var2";
		std::string var3 = "var3";
		VarIndex varIndex1 = Entity::insertVar("var1");
		VarIndex varIndex2 = Entity::insertVar("var2");
		VarIndex varIndex3 = Entity::insertVar("var3");
		StmtIndex readStmt1 = Entity::insertStmt(StatementType::readType);
		StmtIndex readStmt2 = Entity::insertStmt(StatementType::readType);
		StmtIndex readStmt3 = Entity::insertStmt(StatementType::readType);
		StmtIndex printStmt4 = Entity::insertStmt(StatementType::printType);
		StmtIndex printStmt5 = Entity::insertStmt(StatementType::printType);
		StmtIndex printStmt6 = Entity::insertStmt(StatementType::printType);
		Attribute::insertStmtByName(readStmt1, StatementType::readType, var1); // simulate stmt 1 reading var1
		Attribute::insertStmtByName(readStmt2, StatementType::readType, var2); // simulate stmt 2 reading var2
		Attribute::insertStmtByName(readStmt3, StatementType::readType, var3); // simulate stmt 3 reading var3
		Attribute::insertStmtByName(printStmt4, StatementType::printType, var1); // simulate stmt 4 printing var1
		Attribute::insertStmtByName(printStmt5, StatementType::printType, var2); // simulate stmt 5 printing var2
		Attribute::insertStmtByName(printStmt6, StatementType::printType, var3); // simulate stmt 6 printing var3

		// print pn; read r; variable v; Select <v.varName, r.varName, pn.varName, v, r, pn>

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{
			"var1 var1 var1 var1 1 4",
			"var2 var2 var2 var2 2 5",
			"var3 var3 var3 var3 3 6" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, attributesProjected, entities);
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
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::synonym, "s1");
		attributesProjected.emplace_back(PqlReferenceType::synonym, "c1");
		attributesProjected.emplace_back(PqlReferenceType::synonym, "c2");
		attributesProjected.emplace_back(PqlReferenceType::value, "c2");
		entities["s1"] = PqlEntityType::Stmt;
		entities["c1"] = PqlEntityType::Constant;
		entities["c2"] = PqlEntityType::Constant;
		testTable["s1"] = std::vector<int>{ 1, 2 };
		testTable["c1"] = std::vector<int>{ 10, 20 };
		testTable["c2"] = std::vector<int>{ 99, 88 };

		// stmt s1; constant c1, c2; Select <s1, c1, c2, c2.value>

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{
			"1 10 99 99", "2 20 88 88" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, attributesProjected, entities);
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
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected;
		attributesProjected.emplace_back(PqlReferenceType::synonym, "s1");
		attributesProjected.emplace_back(PqlReferenceType::stmtNum, "s2");
		attributesProjected.emplace_back(PqlReferenceType::stmtNum, "r1");
		attributesProjected.emplace_back(PqlReferenceType::stmtNum, "pn1");
		attributesProjected.emplace_back(PqlReferenceType::stmtNum, "cl1");
		attributesProjected.emplace_back(PqlReferenceType::stmtNum, "w1");
		attributesProjected.emplace_back(PqlReferenceType::stmtNum, "if1");
		attributesProjected.emplace_back(PqlReferenceType::stmtNum, "a1");
		entities["s1"] = PqlEntityType::Stmt;
		entities["s2"] = PqlEntityType::Stmt;
		entities["r1"] = PqlEntityType::Read;
		entities["pn1"] = PqlEntityType::Print;
		entities["cl1"] = PqlEntityType::Call;
		entities["w1"] = PqlEntityType::While;
		entities["if1"] = PqlEntityType::If;
		entities["a1"] = PqlEntityType::Assign;
		testTable["s1"] = std::vector<int>{ 1, 2 };
		testTable["s2"] = std::vector<int>{ 3, 4 };
		testTable["r1"] = std::vector<int>{ 5, 6 };
		testTable["pn1"] = std::vector<int>{ 7, 8 };
		testTable["cl1"] = std::vector<int>{ 9, 10 };
		testTable["w1"] = std::vector<int>{ 11, 12 };
		testTable["if1"] = std::vector<int>{ 13, 14 };
		testTable["a1"] = std::vector<int>{ 15, 16 };

		// Select <s1, s2.stmt#, r1.stmt#, pn1.stmt#, cl1.stmt#, w1.stmt#, if1.stmt#, a1.stmt#>

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{
			"1 3 5 7 9 11 13 15",
			"2 4 6 8 10 12 14 16" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, attributesProjected, entities);
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
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected; // Empty attributesProjected table
		entities["s1"] = PqlEntityType::Stmt;
		entities["s2"] = PqlEntityType::Stmt;
		entities["r1"] = PqlEntityType::Read;
		entities["pn1"] = PqlEntityType::Print;
		entities["cl1"] = PqlEntityType::Call;
		entities["w1"] = PqlEntityType::While;
		entities["if1"] = PqlEntityType::If;
		entities["a1"] = PqlEntityType::Assign;
		testTable["s1"] = std::vector<int>{ 1, 2 };
		testTable["s2"] = std::vector<int>{ 3, 4 };
		testTable["r1"] = std::vector<int>{ 5, 6 };
		testTable["pn1"] = std::vector<int>{ 7, 8 };
		testTable["cl1"] = std::vector<int>{ 9, 10 };
		testTable["w1"] = std::vector<int>{ 11, 12 };
		testTable["if1"] = std::vector<int>{ 13, 14 };
		testTable["a1"] = std::vector<int>{ 15, 16 };

		// Select <s1, s2.stmt#, r1.stmt#, pn1.stmt#, cl1.stmt#, w1.stmt#, if1.stmt#, a1.stmt#>

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{ "TRUE" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, attributesProjected, entities);
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
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<PqlReference> attributesProjected; // Empty attributesProjected table

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(false);
		std::list<std::string> expected{ "FALSE" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, attributesProjected, entities);
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
