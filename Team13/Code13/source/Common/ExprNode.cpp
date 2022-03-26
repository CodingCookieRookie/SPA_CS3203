#include "ExprNode.h"

/* ExprNode */
ExprNode::ExprNode(ExprNodeValueType valueType, std::string value) : valueType(valueType), value(value) {}

void ExprNode::addChild(ExprNode* child) {
	children.push_back(child);
}

bool ExprNode::hasLeftChild() {
	return !children.empty();
}

bool ExprNode::hasRightChild() {
	return children.size() == 2;
}

ExprNode* ExprNode::getLeftChild() {
	return hasLeftChild() ? children[0] : nullptr;
}

ExprNode* ExprNode::getRightChild() {
	return hasRightChild() ? children[1] : nullptr;
}

std::vector<ExprNode*> ExprNode::getChildren() {
	return children;
}

std::string ExprNode::getValue() {
	return value;
}

ExprNodeValueType ExprNode::getExprNodeValueType() {
	return valueType;
}

std::string ExprNode::getPattern() {
	std::string pattern;
	std::vector<std::string> tokens;
	populatePattern(tokens);
	pattern.push_back(' ');
	for (std::string& token : tokens) {
		for (char c : token) {
			pattern.push_back(c);
		}
		pattern.push_back(' ');
	}
	return pattern;
}

void ExprNode::populatePattern(std::vector<std::string>& tokens) {
	for (ExprNode* child : children) {
		child->populatePattern(tokens);
	}
	tokens.push_back(value);
}
