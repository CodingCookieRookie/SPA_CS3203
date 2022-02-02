#pragma once

#include "stdafx.h"
#include "CppUnitTest.h"
#include "QPS-NEW/PQLTokeniser.h"
#include "QPS-NEW/PQLDEToken.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace UnitTesting
{
	TEST_CLASS(TestPQLTokeniser)
	{
	public:

		TEST_METHOD(TestPQLTokeniserTrue1)
		{
			string designEntities = "stmt s; assign a;";
			string query = "Select s;";
			vector<string> queryVec;
			queryVec.push_back(designEntities);
			queryVec.push_back(query);
			tokeniseQuery(queryVec);
			vector<PQLDEToken> actualTokens = getPQLDETokens();
			vector<string> actualSynonyms = getSynonyms();
			PQLDEToken token1;
			token1.setDesignEntity("stmt");
			token1.setSynonym("s");
			PQLDEToken token2;
			token2.setDesignEntity("assign");
			token2.setSynonym("a");
			vector<PQLDEToken> expectedTokens;
			expectedTokens.push_back(token1);
			expectedTokens.push_back(token2);
			for (int i = 0; i < actualTokens.size(); i++) {
				
				Assert::AreEqual(expectedTokens.at(i).getPQLDETokenString(), actualTokens.at(i).getPQLDETokenString());
			}
			clearPQLDETokens();
			clearSynonyms();
		}

		TEST_METHOD(TestPQLTokeniserTrue2)
		{
			string designEntities = "assign s     ;    assign veryLongAssignmentName;";
			string query = "Select s;";
			vector<string> queryVec;
			queryVec.push_back(designEntities);
			queryVec.push_back(query);
			tokeniseQuery(queryVec);
			vector<PQLDEToken> actualTokens = getPQLDETokens();
			vector<string> actualSynonyms = getSynonyms();
			PQLDEToken token1;
			token1.setDesignEntity("assign");
			token1.setSynonym("s");
			PQLDEToken token2;
			token2.setDesignEntity("assign");
			token2.setSynonym("veryLongAssignmentName");
			vector<PQLDEToken> expectedTokens;
			expectedTokens.push_back(token1);
			expectedTokens.push_back(token2);
			for (int i = 0; i < actualTokens.size(); i++) {

				Assert::AreEqual(expectedTokens.at(i).getPQLDETokenString(), actualTokens.at(i).getPQLDETokenString());
			}
			clearPQLDETokens();
			clearSynonyms();
		}

		TEST_METHOD(TestPQLTokeniserFalse1)
		{
			string designEntities = "stmt s; assign;";
			string query = "Select s;";
			vector<string> queryVec;
			queryVec.push_back(designEntities);
			queryVec.push_back(query);
			tokeniseQuery(queryVec);
			vector<PQLDEToken> actualTokens = getPQLDETokens();
			vector<string> actualSynonyms = getSynonyms();
			PQLDEToken token1;
			token1.setDesignEntity("stmt");
			token1.setSynonym("s");
			PQLDEToken token2;
			vector<PQLDEToken> expectedTokens;
			expectedTokens.push_back(token1);
			expectedTokens.push_back(token2);
			for (int i = 0; i < actualTokens.size(); i++) {

				Assert::AreEqual(expectedTokens.at(i).getPQLDETokenString(), actualTokens.at(i).getPQLDETokenString());
			}
			clearPQLDETokens();
			clearSynonyms();
		}

		//TEST_METHOD(TestPQLTokeniserFalse2)
		//{
		//	string designEntities = "stmt s;; assign;";
		//	string query = "Select s;";
		//	vector<string> queryVec;
		//	queryVec.push_back(designEntities);
		//	queryVec.push_back(query);
		//	tokeniseQuery(queryVec);
		//	vector<PQLDEToken> actualTokens = getPQLDETokens();
		//	vector<string> actualSynonyms = getSynonyms();
		//	vector<PQLDEToken> expectedTokens;
		//	Assert::AreEqual(expectedTokens.size(), actualTokens.size());
		//	clearPQLDETokens();
		//	clearSynonyms();
		//}

	};
}