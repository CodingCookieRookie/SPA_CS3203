#include <stack>

#include "./ExpressionProcessor.h"

bool ExpressionProcessor::isOperand(char currentChar) {
	// var_name and const_value consists of letters and digits.
	return (currentChar >= 'a' && currentChar <= 'z') || (currentChar >= 'A' && currentChar <= 'Z') || (currentChar >= '0' && currentChar <= '9');
}

int ExpressionProcessor::evaluateOperatorPrecedence(char currentChar) {
	// Multiplication, division and modulus
	if (currentChar == '*' || currentChar == '/' || currentChar == '%')
		return 2;
	// Addition and subtraction
	else if (currentChar == '+' || currentChar == '-')
		return 1;
	else
		return -1;
}

// Shunting yard algorithm
std::string ExpressionProcessor::convertInfixToPostFix(std::string s) {
	// To track non-operands
	std::stack<char> stk;
	std::string res;

	for (size_t i = 0; i < s.length(); i++) {
		char currentChar = s[i];

		// Operand: Add to res
		if (isOperand(currentChar)) {
			res += currentChar;
		}

		// '(': Push to stack
		else if (currentChar == '(')
			stk.push('(');

		// ')': Pop from stack and add to res, until '(' is found.
		// i.e. Pop all operators until opening parenthesis is found.
		else if (currentChar == ')') {
			while (stk.top() != '(')
			{
				res += stk.top();
				stk.pop();
			}
			stk.pop();
		}

		// Operator
		else {
			// If current operator is of lower/equal precedence than operator from stack top,
			// then pop operator from stack and add to res.
			// Ensure check is done only when stack is not empty.
			while (!stk.empty() && evaluateOperatorPrecedence(s[i]) <= evaluateOperatorPrecedence(stk.top())) {
				res += stk.top();
				stk.pop();
			}
			stk.push(currentChar);
		}
	}

	// Pop remaining items from stack
	while (!stk.empty()) {
		res += stk.top();
		stk.pop();
	}

	return res;
}
