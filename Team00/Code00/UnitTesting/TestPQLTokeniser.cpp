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

		TEST_METHOD(tokeniseQuery_selectAllStmt_success)
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
			for (size_t i = 0; i < actualTokens.size(); i++) {
				
				Assert::AreEqual(expectedTokens.at(i).getPQLDETokenString(), actualTokens.at(i).getPQLDETokenString());
			}
			clearPQLDETokens();
			clearSynonyms();
		}

		TEST_METHOD(tokeniseQuery_weirdspacing_success)
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
			for (size_t i = 0; i < actualTokens.size(); i++) {

				Assert::AreEqual(expectedTokens.at(i).getPQLDETokenString(), actualTokens.at(i).getPQLDETokenString());
			}
			clearPQLDETokens();
			clearSynonyms();
		}

		TEST_METHOD(tokeniseQuery_noSynonym_noSynonymAdded)
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
			token2.setDesignEntity("assign");
			vector<PQLDEToken> expectedTokens;
			expectedTokens.push_back(token1);
			expectedTokens.push_back(token2);
			for (size_t i = 0; i < actualTokens.size(); i++) {

				Assert::AreEqual(expectedTokens.at(i).getPQLDETokenString(), actualTokens.at(i).getPQLDETokenString());
			}
			clearPQLDETokens();
			clearSynonyms();
		}

		/*TEST_METHOD(TestPQLTokeniserFalse2)	// for parsing to take note, and why we need tokenise + parsing at same time -> shall be combined soon
		{
			string designEntities = "stmt s;; assign a;";
			string query = "Select s;";
			vector<string> queryVec;
			queryVec.push_back(designEntities);
			queryVec.push_back(query);
			tokeniseQuery(queryVec);
			vector<PQLDEToken> actualTokens = getPQLDETokens();
			vector<string> actualSynonyms = getSynonyms();
			vector<PQLDEToken> expectedTokens;
			Assert::AreEqual(expectedTokens.size(), actualTokens.size());
			clearPQLDETokens();
			clearSynonyms();
		}*/

	};
}