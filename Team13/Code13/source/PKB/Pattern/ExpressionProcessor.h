#pragma once

#include <stack>
#include <string>

#include "../../Common/Types.h"

enum class OperatorPriority {
	LOW, MEDIUM, HIGH
};

class ExpressionProcessor {
private:
	static std::string processOperand(std::string& s, size_t index, char currentChar);
	static std::string processOperators(std::stack<char>& stk, char currentChar);
	static std::string processStack(std::stack<char>& stk, std::string& res);

public:
	static bool isOperand(char currentChar);
	static OperatorPriority evaluateOperatorPrecedence(char currentChar);
	static std::string convertInfixToPostFix(std::string s);
};
