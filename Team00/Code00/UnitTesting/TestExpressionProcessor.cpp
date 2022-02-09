#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/ExpressionProcessor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestExpressionProcessor) {
private:
	int mulRes = ExpressionProcessor::evaluateOperatorPrecedence('*');
	int divRes = ExpressionProcessor::evaluateOperatorPrecedence('/');
	int modRes = ExpressionProcessor::evaluateOperatorPrecedence('%');
	int addRes = ExpressionProcessor::evaluateOperatorPrecedence('+');
	int subRes = ExpressionProcessor::evaluateOperatorPrecedence('-');

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

	TEST_METHOD(evaluateOperatorPrecedence_validOperator) {
		Assert::AreEqual(2, mulRes);
		Assert::AreEqual(2, divRes);
		Assert::AreEqual(2, modRes);
		Assert::AreEqual(1, addRes);
		Assert::AreEqual(1, subRes);
	}
	TEST_METHOD(evaluateOperatorPrecedence_invalidOperator) {
		int res = ExpressionProcessor::evaluateOperatorPrecedence('^');
		Assert::AreEqual(-1, res);
	}

	TEST_METHOD(evaluateOperatorPrecedence_comparePrecedenceDifferentOperator) {
		Assert::IsTrue(mulRes > addRes);
		Assert::IsTrue(divRes > addRes);
		Assert::IsTrue(modRes > addRes);
		Assert::IsTrue(mulRes > subRes);
		Assert::IsTrue(divRes > subRes);
		Assert::IsTrue(modRes > subRes);
	}

	TEST_METHOD(evaluateOperatorPrecedence_comparePrecedenceSameOperator) {
		Assert::AreEqual(mulRes, divRes);
		Assert::AreEqual(mulRes, modRes);
		Assert::AreEqual(divRes, modRes);
		Assert::AreEqual(addRes, subRes);
	}

	TEST_METHOD(convertInfixToPostFix_brackets) {
		std::string test1 = "((A+b)*c-d)*e";
		std::string res1 = "Ab+c*d-e*";
		std::string res = ExpressionProcessor::convertInfixToPostFix(test1);
		Assert::AreEqual(res, res1);
	}
	};
}
