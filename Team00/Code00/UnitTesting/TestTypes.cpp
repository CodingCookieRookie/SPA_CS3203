#include "stdafx.h"
#include "CppUnitTest.h"
//#include "TNode.h"

#include "../SPA/Types.h"
#include "../SPA/Entity.h"
#include "../SPA/Entity.cpp"

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

		TEST_METHOD(TestMethod2)
		{
			//TNode T;
			// TODO: Your test code here
			struct VAR_IDX name;
			struct PROC_IDX name2;
			name.index = 1;
			name2.index = 1;

			Assert::AreEqual(name.index, name2.index);
		}

		TEST_METHOD(TestMethod3)
		{
			//TNode T;
			// TODO: Your test code here
			Assert::IsFalse(typeid(VAR_IDX) == typeid(PROC_IDX));

			//Assert::IsTrue(typeid(char) != typeid(double));
		}

	};
}
