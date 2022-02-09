#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/ExpressionProcessor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestExpressionProcessor) {
private:
	std::string test1 = "((A+b)*c-d)*e";
	std::string res1 = "Ab+c*d-e*";

public:
	TEST_METHOD(insertVar_getV222arIdx_differentVariables) {
		std::string res = ExpressionProcessor::convertInfixToPostFix(test1);
		Assert::AreEqual(res, res1);
	}
	};
}
