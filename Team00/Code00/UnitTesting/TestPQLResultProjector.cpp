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
			std::unordered_map<std::string, PqlEntityType> entities;
			std::unordered_map<std::string, std::vector<int>> testTable;
			std::vector<int> vec;
			vec.push_back(1);
			vec.push_back(3);
			vec.push_back(5);
			entities["s"] = PqlEntityType::Stmt;
			testTable["s"] = vec;
			EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
			PQLResultProjector pqlResultProject = PQLResultProjector(evTestTable);
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
			std::vector<int> vec;
			vec.push_back(1);
			vec.push_back(2);
			vec.push_back(3);
			Entity::insertVar("a");
			Entity::insertVar("b");
			Entity::insertVar("c");
			entities["v"] = PqlEntityType::Variable;
			testTable["v"] = vec;
			EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
			PQLResultProjector pqlResultProject = PQLResultProjector(evTestTable);
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
			std::vector<int> vec;
			vec.push_back(1);
			vec.push_back(2);
			vec.push_back(3);
			Entity::insertProc("proc1");
			Entity::insertProc("proc2");
			Entity::insertProc("proc3");
			entities["p"] = PqlEntityType::Procedure;
			testTable["p"] = vec;
			EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
			PQLResultProjector pqlResultProject = PQLResultProjector(evTestTable);
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
	};
}