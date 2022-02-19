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

	TEST_METHOD(evaluateOperatorPrecedence_comparePrecedence_differentOperator) {
		Assert::IsTrue(mulRes > addRes);
		Assert::IsTrue(divRes > addRes);
		Assert::IsTrue(modRes > addRes);
		Assert::IsTrue(mulRes > subRes);
		Assert::IsTrue(divRes > subRes);
		Assert::IsTrue(modRes > subRes);
	}

	TEST_METHOD(evaluateOperatorPrecedence_comparePrecedence_sameOperator) {
		Assert::AreEqual(mulRes, divRes);
		Assert::AreEqual(mulRes, modRes);
		Assert::AreEqual(divRes, modRes);
		Assert::AreEqual(addRes, subRes);
	}

	TEST_METHOD(convertInfixToPostFix_emptyString) {
		std::string input = "";
		std::string expectedRes = " ";
		std::string res = ExpressionProcessor::convertInfixToPostFix(input);
		Assert::AreEqual(expectedRes, res);
	}

	TEST_METHOD(convertInfixToPostFix_oneOperand) {
		std::string input = "ab";
		std::string expectedRes = " ab ";
		std::string res = ExpressionProcessor::convertInfixToPostFix(input);
		Assert::AreEqual(expectedRes, res);
		Assert::AreEqual(expectedRes, res);
	}

	TEST_METHOD(convertInfixToPostFix_oneOperator) {
		std::string input = "a+b";
		std::string expectedRes = " a b + ";
		std::string res = ExpressionProcessor::convertInfixToPostFix(input);
		Assert::AreEqual(expectedRes, res);
	}

	TEST_METHOD(convertInfixToPostFix_oneOperator_complexOperandNames) {
		std::string input1 = "ab+b0";
		std::string input2 = "a+bb0";
		// Ensures unique infix expressions remain unique after postfix conversion (with complex operand names)
		std::string expectedRes1 = " ab b0 + ";
		std::string expectedRes2 = " a bb0 + ";
		std::string res1 = ExpressionProcessor::convertInfixToPostFix(input1);
		std::string res2 = ExpressionProcessor::convertInfixToPostFix(input2);
		Assert::AreEqual(expectedRes1, res1);
		Assert::AreEqual(expectedRes2, res2);
	}

	TEST_METHOD(convertInfixToPostFix_oneOperator_complexOperandNames_mixOperandTypes) {
		std::string input1 = "ab+93238";
		std::string input2 = "a+b93238";
		// Ensures unique infix expressions remain unique after postfix conversion (with complex operand names)
		std::string expectedRes1 = " ab 93238 + ";
		std::string expectedRes2 = " a b93238 + ";
		std::string res1 = ExpressionProcessor::convertInfixToPostFix(input1);
		std::string res2 = ExpressionProcessor::convertInfixToPostFix(input2);
		Assert::AreEqual(expectedRes1, res1);
		Assert::AreEqual(expectedRes2, res2);
	}

	TEST_METHOD(convertInfixToPostFix_twoOperators_samePrecedence) {
		std::string input = "a+b-c";
		std::string expectedRes = " a b + c - ";
		std::string res = ExpressionProcessor::convertInfixToPostFix(input);
		Assert::AreEqual(expectedRes, res);
	}

	// Tests uniqueness of expressions despite associativity
	TEST_METHOD(convertInfixToPostFix_twoOperators_samePrecedence_associativity) {
		std::string input1 = "a+b-c";
		std::string input2 = "b-c+a";
		std::string input3 = "-c+b+a";
		std::string res1 = ExpressionProcessor::convertInfixToPostFix(input1);
		std::string res2 = ExpressionProcessor::convertInfixToPostFix(input2);
		std::string res3 = ExpressionProcessor::convertInfixToPostFix(input3);
		Assert::IsFalse(res1 == res2);
		Assert::IsFalse(res2 == res3);
		Assert::IsFalse(res1 == res3);
	}

	TEST_METHOD(convertInfixToPostFix_twoOperators_differentPrecedence) {
		std::string input = "a+b*c";
		std::string expectedRes = " a b c * + ";
		std::string res = ExpressionProcessor::convertInfixToPostFix(input);
		Assert::AreEqual(expectedRes, res);
	}

	TEST_METHOD(convertInfixToPostFix_manyOperators_samePrecedence) {
		std::string input = "a*b%c/d%deab";
		std::string expectedRes = " a b * c % d / deab % ";
		std::string res = ExpressionProcessor::convertInfixToPostFix(input);
		Assert::AreEqual(expectedRes, res);
	}

	TEST_METHOD(convertInfixToPostFix_manyOperators_samePrecedence_repeatedOperands) {
		std::string input = "a*deab%c/d%deab*deab";
		std::string expectedRes = " a deab * c % d / deab % deab * ";
		std::string res = ExpressionProcessor::convertInfixToPostFix(input);
		Assert::AreEqual(expectedRes, res);
	}

	// Tests uniqueness of expressions despite associativity
	TEST_METHOD(convertInfixToPostFix_manyOperators_samePrecedence_associativity) {
		std::string input1 = "a*b*c/deab";
		std::string input2 = "b*c*a/deab";
		std::string input3 = "a/deab*b*c";
		std::string res1 = ExpressionProcessor::convertInfixToPostFix(input1);
		std::string res2 = ExpressionProcessor::convertInfixToPostFix(input2);
		std::string res3 = ExpressionProcessor::convertInfixToPostFix(input3);
		Assert::IsFalse(res1 == res2);
		Assert::IsFalse(res2 == res3);
		Assert::IsFalse(res1 == res3);
	}

	TEST_METHOD(convertInfixToPostFix_manyOperators_differentPrecedence) {
		std::string input = "a+b/c%d/e";
		std::string expectedRes = " a b c / d % e / + ";
		std::string res = ExpressionProcessor::convertInfixToPostFix(input);
		Assert::AreEqual(expectedRes, res);
	}

	TEST_METHOD(convertInfixToPostFix_manyOperators_differentPrecedence_differentOperandTypes) {
		std::string input = "a+0/c%123/e";
		std::string expectedRes = " a 0 c / 123 % e / + ";
		std::string res = ExpressionProcessor::convertInfixToPostFix(input);
		Assert::AreEqual(expectedRes, res);
	}

	TEST_METHOD(convertInfixToPostFix_manyOperators_differentPrecedence_differentOperandTypes_complexOperandNames) {
		std::string input = "A123+0/print%1234567890/statement123";
		std::string expectedRes = " A123 0 print / 1234567890 % statement123 / + ";
		std::string res = ExpressionProcessor::convertInfixToPostFix(input);
		Assert::AreEqual(expectedRes, res);
	}

	TEST_METHOD(convertInfixToPostFix_manyOperators_differentPrecedence_differentOperandTypes_complexOperandNames_repeatedOperands) {
		std::string input = "print+print%print*printprint1234567890/statement123+print/1234567890";
		std::string expectedRes = " print print print % printprint1234567890 * statement123 / + print 1234567890 / + ";
		std::string res = ExpressionProcessor::convertInfixToPostFix(input);
		Assert::AreEqual(expectedRes, res);
	}

	TEST_METHOD(convertInfixToPostFix_twoSingleLevelBrackets) {
		std::string input = "((A+b)*c-d)*e";
		std::string expectedRes = " A b + c * d - e * ";
		std::string res = ExpressionProcessor::convertInfixToPostFix(input);
		Assert::AreEqual(expectedRes, res);
	}

	TEST_METHOD(convertInfixToPostFix_twoDifferentLevelBrackets) {
		std::string input = "((a+b))*c";
		std::string expectedRes = " a b + c * ";
		std::string res = ExpressionProcessor::convertInfixToPostFix(input);
		Assert::AreEqual(expectedRes, res);
	}

	TEST_METHOD(convertInfixToPostFix_manySingleLevelBrackets) {
		std::string input = "((H*((((A+((B+C)*D))*F)*G)*E))+J)";
		std::string expectedRes = " H A B C + D * + F * G * E * * J + ";
		std::string res = ExpressionProcessor::convertInfixToPostFix(input);
		Assert::AreEqual(expectedRes, res);
	}

	TEST_METHOD(convertInfixToPostFix_manyDifferentLevelBrackets) {
		std::string input = "(((H*(((((((A)))+(((B+(((C)))))*D))*F)*G)*E))+J))";
		std::string expectedRes = " H A B C + D * + F * G * E * * J + ";
		std::string res = ExpressionProcessor::convertInfixToPostFix(input);
		Assert::AreEqual(expectedRes, res);
	}

	TEST_METHOD(convertInfixToPostFix_manyOperators_differentPrecedence_differentOperandTypes_complexOperandNames_repeatedOperands_manyDifferentLevelBrackets) {
		std::string input = "((digit*((((A0123+(((statement+(((procedure*procedure1)))))*read/read))*F%123)*123%((((pattern)))))*44))-patterm9)";
		std::string expectedRes = " digit A0123 statement procedure procedure1 * + read * read / + F * 123 % 123 * pattern % 44 * * patterm9 - ";
		std::string res = ExpressionProcessor::convertInfixToPostFix(input);
		Assert::AreEqual(expectedRes, res);
	}
	};
}
