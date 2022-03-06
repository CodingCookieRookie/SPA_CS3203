#pragma once

#include <string>

#include "../Common/Types.h"

enum class OperatorPriority {
	low, medium, high
};

class ExpressionProcessor {
public:
	static bool isOperand(char currentChar);
	static OperatorPriority evaluateOperatorPrecedence(char currentChar);
	static std::string convertInfixToPostFix(std::string s);
};
