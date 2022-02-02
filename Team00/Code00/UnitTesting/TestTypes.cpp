#include "stdafx.h"
#include "CppUnitTest.h"
//#include "TNode.h"

#include "../source/PKB/Types.h"
#include "../source/PKB/Entity.h"

#include <iostream>
#include <type_traits>
#include <cstdint>
using namespace std;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{

	TEST_CLASS(TestTypes)
	{
	public:

		TEST_METHOD(TestMethod1)
		{
			//TNode T;
			// TODO: Your test code here
			struct VAR_IDX idx = { 1 };
			struct PROC_IDX idx2 = { 1 };
			int i = Entity::insertNow(idx);
			int j = Entity::insertNow(idx2);
			Assert::AreEqual(i, 1);
			Assert::AreEqual(j, 2);
		}

		//TEST_METHOD(TestMethod2)
		//{
		//	//TNode T;
		//	// TODO: Your test code here
		//	struct VAR_IDX name;
		//	struct PROC_IDX name2;
		//	name.index = 1;
		//	name2.index = 1;

		//	Assert::AreEqual(name.index, name2.index);
		//}

		TEST_METHOD(TestMethod3)
		{
			//TNode T;
			// TODO: Your test code here
			Assert::IsFalse(typeid(VAR_IDX) == typeid(PROC_IDX));
			Assert::IsFalse(typeid(STMT_IDX) == typeid(PROC_IDX));
			Assert::IsFalse(typeid(STMT_IDX) == typeid(VAR_IDX));
		}

		TEST_METHOD(TestMethod4)
		{
			//TNode T;
			// TODO: Your test code here
			//STMT_IDX stmtIdx = { 1 };
			//STMT_IDX stmtIdx2 = { 5 };
			//VAR_IDX varIdx = { 2 };
			//VAR_IDX varIdx2 = { 4 };
			//PROC_IDX procIdx = { 3 };
			Entity::insertVar("future");
			Entity::insertVar("present");
			int res = Entity::getVarIdx("present");
			Assert::AreEqual(res, 2);
		}

		TEST_METHOD(TestMethod5)
		{
			//TNode T;
			// TODO: Your test code here
			STMT_IDX stmtIdx = { 1 };
			STMT_IDX stmtIdx2 = { 5 };
			VAR_IDX varIdx = { 2 };
			VAR_IDX varIdx2 = { 4 };
			PROC_IDX procIdx = { 3 };
			PROC_IDX procIdx2 = { 4 };
			Entity::insertStmtFromProc(procIdx.index, stmtIdx.index);
			Entity::insertStmtFromProc(procIdx2.index, stmtIdx2.index);
			unordered_set<int> res1 = Entity::getStmtsFromProc(procIdx.index);
			auto res2 = res1.begin();
			Assert::AreEqual(*res2, stmtIdx.index);
			Assert::AreEqual(*res2, 1);
			//Assert::AreEqual(*res3, stmtIdx2.index);
		}

		TEST_METHOD(TestMethod6) //"persistence"
		{
			//TNode T;
			// TODO: Your test code here
			Entity::insertVar("future");
			Entity::insertVar("present");
			int res = Entity::getVarIdx("present");
			Assert::AreEqual(res, 4);
		}

		TEST_METHOD(TestMethod7) 
		{
			//TNode T;
			// TODO: Your test code here
			Entity::insertVar("future");
			Entity::insertVar("present");
			vector<string> res = Entity::getAllVars();
			Assert::AreEqual(*res.begin(), Entity::getVarName(1));
		}
	};
}
