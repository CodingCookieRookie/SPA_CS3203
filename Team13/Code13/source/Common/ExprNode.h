#pragma once

#include <string>
#include <vector>

#include "Types.h"

class ExprNode {
private:
	std::string value;
	ExprNodeValueType valueType;
	std::vector<ExprNode*> children;
	bool hasLeftChild();
	bool hasRightChild();
	void populatePattern(std::vector<std::string>& tokens);
public:
	ExprNode(ExprNodeValueType valueType, std::string value);
	~ExprNode();
	void addChild(ExprNode* child);
	std::vector<ExprNode*> getChildren();
	ExprNode* getLeftChild();
	ExprNode* getRightChild();
	std::string getValue();
	ExprNodeValueType getExprNodeValueType();
	std::string getPattern();
};
