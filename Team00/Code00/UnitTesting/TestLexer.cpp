#include "CppUnitTest.h"
#include "stdafx.h"

#include "common/Lexer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestLexer) {
public:
	TEST_METHOD(nextInteger_validIntegerNotStartingWithZero_attachedToNonDigit_integerReturned) {
		const char* source = " 103;  ";
		Lexer lexer(source);
		Assert::AreEqual(std::string("103"), lexer.nextInteger());
	}

	TEST_METHOD(nextInteger_zeroAndNotEndOfString_attachedToNonDigit_zeroReturned) {
		const char* source = "      0+ ;   ";
		Lexer lexer(source);
		Assert::AreEqual(std::string("0"), lexer.nextInteger());
	}

	TEST_METHOD(nextInteger_zeroAtEndOfString_zeroReturned) {
		const char* source = "      0";
		Lexer lexer(source);
		Assert::AreEqual(std::string("0"), lexer.nextInteger());
	}

	TEST_METHOD(nextInteger_zeroAttachedToNonDigit_zeroReturned) {
		const char* source = "   0meString   01 ";
		Lexer lexer(source);
		Assert::AreEqual(std::string("0"), lexer.nextInteger());
	}

	TEST_METHOD(nextInteger_invalidIntegerIsNotDigit_emptyStringReturned) {
		const char* source = "   s0meString   01 ";
		Lexer lexer(source);
		Assert::AreEqual(std::string(), lexer.nextInteger());
	}

	TEST_METHOD(nextInteger_invalidIntegerStartingWithZero_lexerExceptionThrown) {
		const char* source = "      01234 + 456";
		Lexer lexer(source);
		auto wrapperFunc1 = [&lexer] { lexer.nextInteger(); };
		Assert::ExpectException<LexerException>(wrapperFunc1);

		Lexer lexer1(source);
		try {
			lexer1.nextInteger();
		} catch (LexerException& ex) {
			Assert::AreEqual(LexerException::INVALID_INT.c_str(), ex.what());
		}
	}
	};
}