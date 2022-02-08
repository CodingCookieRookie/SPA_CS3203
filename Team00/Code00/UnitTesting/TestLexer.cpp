#include "CppUnitTest.h"
#include "stdafx.h"

#include "common/Lexer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestLexer) {
public:
    TEST_METHOD(nextInteger_integerNotStartingWithZero_integerReturned) {
        const char* source = " 103 integer ";
        Lexer lexer(source);    
        Assert::AreEqual(std::string("103"), lexer.nextInteger());
    }

    TEST_METHOD(nextInteger_integerIsZeroAndNotEndOfString_zeroReturned) {
        const char* source = "      0   ";
        Lexer lexer(source);
        Assert::AreEqual(std::string("0"), lexer.nextInteger());
    }

    TEST_METHOD(nextInteger_integerIsZeroAtEndOfString_zeroReturned) {
        const char* source = "      0";
        Lexer lexer(source);
        Assert::AreEqual(std::string("0"), lexer.nextInteger());
    }

    TEST_METHOD(nextInteger_integerStartingWithZero_emptyStringReturned) {
        const char* source = "      01234 integer";
        Lexer lexer(source);
        Assert::AreEqual(std::string(), lexer.nextInteger());
    }

    TEST_METHOD(nextInteger_isNotDigit_emptyStringReturned) {
        const char* source = "   s0meString   01 ";
        Lexer lexer(source);
        Assert::AreEqual(std::string(), lexer.nextInteger());
    }

    TEST_METHOD(nextInteger_isNotDigitButStartingWithZero_emptyStringReturned) {
        const char* source = "   0meString   01 ";
        Lexer lexer(source);
        Assert::AreEqual(std::string(), lexer.nextInteger());
    }
    };
}
