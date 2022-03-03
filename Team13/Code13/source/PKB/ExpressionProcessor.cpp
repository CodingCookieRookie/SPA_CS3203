#include <stack>

#include "./ExpressionProcessor.h"

bool ExpressionProcessor::isOperand(char currentChar) {
	/* var_name and const_value consists of letters and digits. */
	return std::isalpha(currentChar) || std::isdigit(currentChar);
}

int ExpressionProcessor::evaluateOperatorPrecedence(char currentChar) {
	if (currentChar == '*' || currentChar == '/' || currentChar == '%') {
		return 2;
	} else if (currentChar == '+' || currentChar == '-') {
		return 1;
	} else {
		return -1;
	}
}

/* Shunting yard algorithm */
std::string ExpressionProcessor::convertInfixToPostFix(std::string s) {
	/* To track non-operands */
	std::stack<char> stk;
	std::string res;

	for (size_t i = 0; i < s.length(); i++) {
		char currentChar = s[i];

		/* Operand: Add to res */
		if (isOperand(currentChar)) {
			/* All variable names and constants are prepended by a space */
			if ((i > 0 && !isOperand(s[i - 1])) || i == 0) {
				res += ' ';
			}
			res += currentChar;
		} else if (currentChar == '(') {
			stk.push('(');
		} else if (currentChar == ')') {
			while (stk.top() != '(') {
				res += ' ';
				res += stk.top();
				stk.pop();
			}
			stk.pop();
		} else {
			/* If current operator is of lower/equal precedence than operator from stack top,
			then pop operator from stack and add to res.
			Ensure check is done only when stack is not empty. */
			while (!stk.empty() && evaluateOperatorPrecedence(s[i]) <= evaluateOperatorPrecedence(stk.top())) {
				res += ' ';
				res += stk.top();
				stk.pop();
			}
			stk.push(currentChar);
		}
	}

	/* Pop remaining items from stack */
	while (!stk.empty()) {
		res += ' ';
		res += stk.top();
		stk.pop();
	}

	return res + ' ';
}