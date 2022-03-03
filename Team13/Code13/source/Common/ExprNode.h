#pragma once

#include <string>
#include <vector>

#include "../Common/Types.h"

class ExprNode {
private:
	std::string value;
	ExprNodeValueType valueType;
	std::vector<ExprNode*> children;
	void populatePattern(std::vector<std::string>& tokens);
public:
	ExprNode(ExprNodeValueType valueType, std::string value);
	void addChild(ExprNode* child);
	std::vector<ExprNode*> getChildren();
	std::string getValue();
	ExprNodeValueType getExprNodeValueType();
	std::string getPattern();
};
