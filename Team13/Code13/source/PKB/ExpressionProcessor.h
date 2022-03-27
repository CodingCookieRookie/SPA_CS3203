#pragma once

#include <stack>
#include <string>

#include "../Common/Types.h"

enum class OperatorPriority {
	LOW, MEDIUM, HIGH
};

class ExpressionProcessor {
public:
	static bool isOperand(char currentChar);
	static OperatorPriority evaluateOperatorPrecedence(char currentChar);
	static std::string convertInfixToPostFix(std::string s);
};
