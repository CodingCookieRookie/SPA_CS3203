#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/ExpressionProcessor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestExpressionProcessor) {
private:

public:
	TEST_METHOD(isOperand_digit) {
		// Boundary value analysis
		Assert::IsTrue(ExpressionProcessor::isOperand('0'));
		Assert::IsTrue(ExpressionProcessor::isOperand('5'));
		Assert::IsTrue(ExpressionProcessor::isOperand('9'));
	}

	TEST_METHOD(isOperand_letter) {
		// Boundary value analysis
		Assert::IsTrue(ExpressionProcessor::isOperand('a'));
		Assert::IsTrue(ExpressionProcessor::isOperand('m'));
		Assert::IsTrue(ExpressionProcessor::isOperand('z'));
		Assert::IsTrue(ExpressionProcessor::isOperand('A'));
		Assert::IsTrue(ExpressionProcessor::isOperand('M'));
		Assert::IsTrue(ExpressionProcessor::isOperand('Z'));
	}

	//TEST_METHOD(evaluateOperatorPrecedence_sameOperator) {
	//	std::string res = ExpressionProcessor::convertInfixToPostFix(test1);
	//	Assert::AreEqual(res, res1);
	//}

	//TEST_METHOD(evaluateOperatorPrecedence_differentOperatorSamePrecedence) {
	//	std::string res = ExpressionProcessor::convertInfixToPostFix(test1);
	//	Assert::AreEqual(res, res1);
	//}

	//TEST_METHOD(evaluateOperatorPrecedence_differentOperatorDifferentPrecedence) {
	//	std::string res = ExpressionProcessor::convertInfixToPostFix(test1);
	//	Assert::AreEqual(res, res1);
	//}

	//TEST_METHOD(evaluateOperatorPrecedence_invalidOperator) {
	//	std::string res = ExpressionProcessor::convertInfixToPostFix(test1);
	//	Assert::AreEqual(res, res1);
	//}

	TEST_METHOD(convertInfixToPostFix_brackets) {
		std::string test1 = "((A+b)*c-d)*e";
		std::string res1 = "Ab+c*d-e*";
		std::string res = ExpressionProcessor::convertInfixToPostFix(test1);
		Assert::AreEqual(res, res1);
	}
	};
}
