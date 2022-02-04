#include "stdafx.h"
#include "CppUnitTest.h"
//#include "TNode.h"

#include "../source/common/Types.h"
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
			struct VarIndex idx = { 1 };
			struct ProcIndex idx2 = { 1 };
			int i = Entity::insertNow(idx);
			int j = Entity::insertNow(idx2);
			Assert::AreEqual(i, 1);
			Assert::AreEqual(j, 2);
		}

		//TEST_METHOD(TestMethod2)
		//{
		//	//TNode T;
		//	// TODO: Your test code here
		//	struct VarIndex name;
		//	struct ProcIndex name2;
		//	name.index = 1;
		//	name2.index = 1;

		//	Assert::AreEqual(name.index, name2.index);
		//}

		TEST_METHOD(TestMethod3)
		{
			//TNode T;
			// TODO: Your test code here
			Assert::IsFalse(StatementType::assignType == StatementType::whileType);
			Assert::IsFalse(typeid(VarIndex) == typeid(ProcIndex));
			Assert::IsFalse(typeid(StmtIndex) == typeid(ProcIndex));
			Assert::IsFalse(typeid(StmtIndex) == typeid(VarIndex));
		}

		TEST_METHOD(TestMethod4)
		{
			//TNode T;
			// TODO: Your test code here
			//StmtIndex stmtIdx = { 1 };
			//StmtIndex stmtIdx2 = { 5 };
			VarIndex varIdx = { 2 };
			VarIndex varIdx2 = VarIndex(2);
			//VarIndex varIdx2 = { 4 };
			//ProcIndex procIdx = { 3 };
			Entity::insertVar("future");
			Entity::insertVar("present");
			VarIndex res = Entity::getVarIdx("present");
			Assert::IsTrue(res == varIdx);
			Assert::IsTrue(res == varIdx2);
		}

		//TEST_METHOD(TestMethod5)
		//{
		//	//TNode T;
		//	// TODO: Your test code here
		//	StmtIndex stmtIdx = { 1 };
		//	StmtIndex stmtIdx2 = { 5 };
		//	VarIndex varIdx = { 2 };
		//	VarIndex varIdx2 = { 4 };
		//	ProcIndex procIdx = { 3 };
		//	ProcIndex procIdx2 = { 4 };
		//	Entity::insertStmtFromProc(procIdx.index, stmtIdx.index);
		//	Entity::insertStmtFromProc(procIdx2.index, stmtIdx2.index);
		//	unordered_set<int> res1 = Entity::getStmtsFromProc(procIdx.index);
		//	auto res2 = res1.begin();
		//	Assert::AreEqual(*res2, stmtIdx.index);
		//	Assert::AreEqual(*res2, 1);
		//	//Assert::AreEqual(*res3, stmtIdx2.index);
		//}

		TEST_METHOD(TestMethod6) //"persistence"
		{
			//TNode T;
			// TODO: Your test code here
			Entity::insertVar("future");
			Entity::insertVar("present");
			VarIndex res = Entity::getVarIdx("present");
			//Assert::AreEqual(res.index, 4);
		}

		//TEST_METHOD(TestMethod7) 
		//{
		//	//TNode T;
		//	// TODO: Your test code here
		//	Entity::insertVar("future");
		//	Entity::insertVar("present");
		//	vector<string> res = Entity::getAllVars();
		//	Assert::AreEqual(*res.begin(), Entity::getVarName(1));
		//}

		TEST_METHOD(TestMethod8) 
		{
			//TNode T;
			// TODO: Your test code here
			Entity::insertVar("future");
			Entity::insertVar("present");
			string res = Entity::getVarName(VarIndex(2));
			Assert::AreEqual(res, string("present"));
		}
	};
}
