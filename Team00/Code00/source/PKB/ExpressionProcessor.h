#pragma once

#ifndef EXPRESSIONPROCESSOR_H
#define EXPRESSIONPROCESSOR_H

#include <unordered_map>

#include "../common/Types.h"

class ExpressionProcessor {
protected:

public:
	static bool isOperand(char currentChar);
	static int evaluateOperatorPrecedence(char currentChar);
	static std::string convertInfixToPostFix(std::string s);
};

#endif
