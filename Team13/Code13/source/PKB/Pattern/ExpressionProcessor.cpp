#include "./ExpressionProcessor.h"

bool ExpressionProcessor::isOperand(char currentChar) {
	/* var_name and const_value consists of letters and digits. */
	return std::isalpha(currentChar) || std::isdigit(currentChar);
}

OperatorPriority ExpressionProcessor::evaluateOperatorPrecedence(char currentChar) {
	if (currentChar == '*' || currentChar == '/' || currentChar == '%') {
		return OperatorPriority::HIGH;
	} else if (currentChar == '+' || currentChar == '-') {
		return OperatorPriority::MEDIUM;
	} else {
		return OperatorPriority::LOW;
	}
}

std::string ExpressionProcessor::processOperand(std::string& s, size_t index, char currentChar) {
	std::string res;
	/* All variable names and constants are prepended by a space */
	if ((index > 0 && !isOperand(s[index - 1])) || index == 0) {
		res += ' ';
	}
	res += currentChar;
	return res;
}

std::string ExpressionProcessor::processOperators(std::stack<char>& stk, char currentChar) {
	std::string res;
	/* If current operator is of lower/equal precedence than operator from stack top,
		then pop operator from stack and add to res.
		Ensure check is done only when stack is not empty. */
	while (!stk.empty() && evaluateOperatorPrecedence(currentChar) <= evaluateOperatorPrecedence(stk.top())) {
		res += ' ';
		res += stk.top();
		stk.pop();
	}
	stk.push(currentChar);
	return res;
}

std::string ExpressionProcessor::processStack(std::stack<char>& stk, std::string& res) {
	/* Pop remaining items from stack */
	while (!stk.empty()) {
		res += ' ';
		res += stk.top();
		stk.pop();
	}
	return res;
}

/* Shunting yard algorithm */
std::string ExpressionProcessor::convertInfixToPostFix(std::string s) {
	std::stack<char> stk;
	std::string res;

	for (size_t i = 0; i < s.length(); i++) {
		char currentChar = s[i];
		if (isOperand(currentChar)) {
			res += processOperand(s, i, currentChar);
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
			res += processOperators(stk, currentChar);
		}
	}
	return processStack(stk, res) + ' ';
}
