#pragma once

#ifndef EXPRESSIONPROCESSOR_H
#define EXPRESSIONPROCESSOR_H

#include <string>

#include "../common/Types.h"

class ExpressionProcessor {
public:
	static bool isOperand(char currentChar);
	static int evaluateOperatorPrecedence(char currentChar);
	static std::string convertInfixToPostFix(std::string s);
};

#endif
