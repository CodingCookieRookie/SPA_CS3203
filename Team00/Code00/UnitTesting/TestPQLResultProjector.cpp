#pragma once

#include "stdafx.h"
#include "CppUnitTest.h"
#include "../source/QPS-NEW/EvaluatedTable.h"
#include "../source/QPS-NEW/PQLResultProjector.h"
#include <iostream>

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
			auto expectedRes = results.begin();
			for (size_t i = 0; i < results.size(); i++) {
				Assert::AreEqual(*actualRes, *expectedRes);
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
			entities["v"] = PqlEntityType::Variable;
			testTable["v"] = vec;
			EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
			PQLResultProjector pqlResultProject = PQLResultProjector(evTestTable);
			std::list<std::string> expected;
			expected.push_back("1");
			expected.push_back("2");
			expected.push_back("3");
			std::list<std::string> results = pqlResultProject.resolveTableToResults();
			Assert::AreEqual(expected.size(), results.size());
			auto actualRes = results.begin();
			auto expectedRes = results.begin();
			for (size_t i = 0; i < results.size(); i++) {
				Assert::AreEqual(*actualRes, *expectedRes);
				std::advance(actualRes, 1);
				std::advance(expectedRes, 1);
			}
		}

		TEST_METHOD(resolveTableToResults_oneColumnProcedure_success)
		{
			std::unordered_map<std::string, PqlEntityType> entities;
			std::unordered_map<std::string, std::vector<int>> testTable;
			std::vector<int> vec;
			vec.push_back(4);
			vec.push_back(5);
			vec.push_back(6);
			entities["p"] = PqlEntityType::Procedure;
			testTable["p"] = vec;
			EvaluatedTable evTestTable = EvaluatedTable(entities, testTable);
			PQLResultProjector pqlResultProject = PQLResultProjector(evTestTable);
			std::list<std::string> expected;
			expected.push_back("4");
			expected.push_back("5");
			expected.push_back("6");
			std::list<std::string> results = pqlResultProject.resolveTableToResults();
			Assert::AreEqual(expected.size(), results.size());
			auto actualRes = results.begin();
			auto expectedRes = results.begin();
			for (size_t i = 0; i < results.size(); i++) {
				Assert::AreEqual(*actualRes, *expectedRes);
				std::advance(actualRes, 1);
				std::advance(expectedRes, 1);
			}
		}
	};
}