#include "stdafx.h"
#include "CppUnitTest.h"
#include "QPS-NEW/PQLDEToken.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestPQLDEToken)
	{
	public:

		TEST_METHOD(testPQLDEToken_success)
		{
			PQLDEToken token;
			token.setDesignEntity("stmt");
			token.setSynonym("s");
			string expected = "Entity: stmt synonym: s";
			Assert::AreEqual(token.getPQLDETokenString(), expected);
		}

	};
}