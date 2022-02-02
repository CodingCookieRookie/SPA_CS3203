#include "stdafx.h"
#include "CppUnitTest.h"
#include "QPS-NEW/PQLDEToken.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace UnitTesting
{
	TEST_CLASS(TestPQLDEToken)
	{
	public:

		TEST_METHOD(TestPQLDETokenTrue)
		{
			PQLDEToken::getPQLDEToken("stmt s");
		}
	};
}