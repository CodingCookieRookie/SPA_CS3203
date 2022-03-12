#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/QPS/EvaluatedTable.h"
#include "../source/QPS/PQLResultProjector.h"
#include "PKB/Entity.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPQLResultProjector) {
public:

	TEST_METHOD(resolveTableToResults_oneColumnStatement_projectOneColumn) {
		// 1. Set-up:
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<std::string> columnsProjected;
		std::vector<int> vec;
		vec.push_back(1);
		vec.push_back(3);
		vec.push_back(5);
		entities["s"] = PqlEntityType::Stmt;
		testTable["s"] = vec;
		columnsProjected.push_back("s");

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected;
		expected.push_back("1");
		expected.push_back("3");
		expected.push_back("5");
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, columnsProjected, entities);
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
		std::vector<std::string> columnsProjected;
		std::vector<int> vec;
		vec.push_back(1);
		vec.push_back(2);
		vec.push_back(3);
		Entity::insertVar("a");
		Entity::insertVar("b");
		Entity::insertVar("c");
		entities["v"] = PqlEntityType::Variable;
		testTable["v"] = vec;
		columnsProjected.push_back("v");

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected;
		expected.push_back("a");
		expected.push_back("b");
		expected.push_back("c");
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, columnsProjected, entities);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
		Entity::performCleanUp();
	}

	TEST_METHOD(resolveTableToResults_oneColumnProcedure_projectOneColumn) {
		// 1. Set-up:
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<std::string> columnsProjected;
		std::vector<int> vec;
		vec.push_back(1);
		vec.push_back(2);
		vec.push_back(3);
		Entity::insertProc("proc1");
		Entity::insertProc("proc2");
		Entity::insertProc("proc3");
		entities["p"] = PqlEntityType::Procedure;
		testTable["p"] = vec;
		columnsProjected.push_back("p");

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected;
		expected.push_back("proc1");
		expected.push_back("proc2");
		expected.push_back("proc3");
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, columnsProjected, entities);
		Assert::AreEqual(expected.size(), results.size());
		auto actualRes = results.begin();
		auto expectedRes = expected.begin();
		for (size_t i = 0; i < results.size(); i++) {
			Assert::AreEqual(*expectedRes, *actualRes);
			std::advance(actualRes, 1);
			std::advance(expectedRes, 1);
		}
		Entity::performCleanUp();
	}

	TEST_METHOD(resolveTableToResults_oneColumnConstantRepeated_projectOneColumnUnique) {
		// 1. Set-up:
		std::unordered_map<std::string, PqlEntityType> entities;
		std::unordered_map<std::string, std::vector<int>> testTable;
		std::vector<std::string> columnsProjected{ "c1" };
		entities["c1"] = PqlEntityType::Constant;
		testTable["c1"] = std::vector<int>{ 1, 1, 1, 2, 2, 3, 3, 4 };

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{ "1", "2", "3", "4" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, columnsProjected, entities);
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
		std::vector<std::string> columnsProjected{ "s1" };
		entities["s1"] = PqlEntityType::Stmt;
		testTable["s1"] = std::vector<int>{ 1, 1, 1, 2, 2, 3, 3, 4 };

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{ "1", "2", "3", "4" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, columnsProjected, entities);
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
		std::vector<std::string> columnsProjected{ "s1", "s2" };
		entities["s1"] = PqlEntityType::Stmt;
		entities["s2"] = PqlEntityType::Stmt;
		testTable["s1"] = std::vector<int>{ 1, 2, 3 };
		testTable["s2"] = std::vector<int>{ 4, 5, 6 };

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{ "1 4", "2 5", "3 6" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, columnsProjected, entities);
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
		std::vector<std::string> columnsProjected{ "s1", "s2" };
		entities["s1"] = PqlEntityType::Stmt;
		entities["s2"] = PqlEntityType::Stmt;
		testTable["s1"] = std::vector<int>{ 1, 1, 2, 2, 3 };
		testTable["s2"] = std::vector<int>{ 4, 5, 5, 6, 6 };

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{ "1 4", "1 5", "2 5", "2 6", "3 6" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, columnsProjected, entities);
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
		std::vector<std::string> columnsProjected{ "s1", "s2" };
		entities["s1"] = PqlEntityType::Stmt;
		entities["s2"] = PqlEntityType::Stmt;
		testTable["s1"] = std::vector<int>{ 1, 1, 2, 2, 3, 3, 4, 4, 4 };
		testTable["s2"] = std::vector<int>{ 4, 5, 5, 6, 6, 6, 7, 7, 7 };

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{ "1 4", "1 5", "2 5", "2 6", "3 6", "4 7" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, columnsProjected, entities);
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
		std::vector<std::string> columnsProjected{ "s1" };
		entities["s1"] = PqlEntityType::Stmt;
		entities["s2"] = PqlEntityType::Stmt;
		testTable["s1"] = std::vector<int>{ 1, 2, 3 };
		testTable["s2"] = std::vector<int>{ 4, 5, 6 };

		// 2. Main test:
		EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
		std::list<std::string> expected{ "1", "2", "3" };
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, columnsProjected, entities);
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
		std::vector<std::string> columnsProjected{ "s1", "v1", "a1", "p1" };
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
		std::list<std::string> results = PQLResultProjector::resolveTableToResults(evTestTable, columnsProjected, entities);
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
