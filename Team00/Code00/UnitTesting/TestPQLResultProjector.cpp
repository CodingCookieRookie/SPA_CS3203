#pragma once

#include "stdafx.h"
#include "CppUnitTest.h"
#include "../source/QPS-NEW/EvaluatedTable.h"
#include "../source/QPS-NEW/PQLResultProjector.h"
#include <iostream>
#include <PKB/Entity.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace UnitTesting
{
	TEST_CLASS(TestPQLResultProjector)
	{
	public:

		TEST_METHOD(resolveTableToResults_oneColumnStatement_success)
		{
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
			PQLResultProjector pqlResultProject = PQLResultProjector(evTestTable, columnsProjected);
			std::list<std::string> expected;
			expected.push_back("1");
			expected.push_back("3");
			expected.push_back("5");
			std::list<std::string> results = pqlResultProject.resolveTableToResults();
			Assert::AreEqual(expected.size(), results.size());
			auto actualRes = results.begin();
			auto expectedRes = expected.begin();
			for (size_t i = 0; i < results.size(); i++) {
				Assert::AreEqual(*expectedRes, *actualRes);
				std::advance(actualRes, 1);
				std::advance(expectedRes, 1);
			}
		}

		TEST_METHOD(resolveTableToResults_oneColumnVariable_success)
		{
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
			EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
			PQLResultProjector pqlResultProject = PQLResultProjector(evTestTable, columnsProjected);
			std::list<std::string> expected;
			expected.push_back("a");
			expected.push_back("b");
			expected.push_back("c");
			std::list<std::string> results = pqlResultProject.resolveTableToResults();
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

		TEST_METHOD(resolveTableToResults_oneColumnProcedure_success)
		{
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
			EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
			PQLResultProjector pqlResultProject = PQLResultProjector(evTestTable, columnsProjected);
			std::list<std::string> expected;
			expected.push_back("proc1");
			expected.push_back("proc2");
			expected.push_back("proc3");
			std::list<std::string> results = pqlResultProject.resolveTableToResults();
			Assert::AreEqual(expected.size(), results.size());
			auto actualRes = results.begin();
			auto expectedRes = expected.begin();
			for (size_t i = 0; i < results.size(); i++) {
				Assert::AreEqual(*expectedRes , *actualRes);
				std::advance(actualRes, 1);
				std::advance(expectedRes, 1);
			}
			Entity::performCleanUp();
		}

		TEST_METHOD(resolveTableToResults_twoColumnStatement_success)
		{
			// 1. Set-up:
			std::unordered_map<std::string, PqlEntityType> entities;
			std::unordered_map<std::string, std::vector<int>> testTable;
			std::vector<std::string> columnsProjected{"s1", "s2"};
			entities["s1"] = PqlEntityType::Stmt;
			entities["s2"] = PqlEntityType::Stmt;
			testTable["s1"] = std::vector<int>{ 1, 2, 3 };
			testTable["s2"] = std::vector<int>{ 4, 5, 6 };

			// 2. Main test:
			EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
			PQLResultProjector pqlResultProject = PQLResultProjector(evTestTable, columnsProjected);
			std::list<std::string> expected{"1", "4", "2", "5", "3", "6"};
			std::list<std::string> results = pqlResultProject.resolveTableToResults();
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