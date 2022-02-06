#include "stdafx.h"
#include "CppUnitTest.h"
#include "QPS-NEW/DesignEntity.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestDesignEntity)
	{
	public:

		TEST_METHOD(testDesignEntity_success)
		{
			Assert::IsTrue(checkIfWordIsDesignEntity("stmt"));
		}

		TEST_METHOD(testDesignEntity_failure1)
		{
			Assert::IsFalse(checkIfWordIsDesignEntity("stmt dsadas"));
		}

		TEST_METHOD(testDesignEntity_failure2)
		{
			Assert::IsFalse(checkIfWordIsDesignEntity("#@#&#@&assign#^^$"));
		}
	};
}