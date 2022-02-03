#include "stdafx.h"
#include "CppUnitTest.h"
#include "QPS-NEW/DesignEntity.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestDesignEntity)
	{
	public:

		TEST_METHOD(TestDesignEntityTrue1)
		{
			Assert::IsTrue(checkIfWordIsDesignEntity("stmt"));
		}

		TEST_METHOD(TestDesignEntityFalse1)
		{
			Assert::IsFalse(checkIfWordIsDesignEntity("stmt dsadas"));
		}

		TEST_METHOD(TestDesignEntityFalse2)
		{
			Assert::IsFalse(checkIfWordIsDesignEntity("#@#&#@&assign#^^$"));
		}
	};
}