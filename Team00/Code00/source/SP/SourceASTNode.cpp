#include <queue>

#include "SourceASTNode.h"

/* SourceASTNode */
SourceASTNode::SourceASTNode() {}

/* StmtNode */
StmtNode::StmtNode() : SourceASTNode() {}

std::unordered_set<std::string> StmtNode::getModifiesVars() {
	return std::unordered_set<std::string>();
}

std::unordered_set<std::string> StmtNode::getUsesVars() {
	return std::unordered_set<std::string>();
}

std::unordered_set<std::string> StmtNode::getUsesConsts() {
	return std::unordered_set<std::string>();
}

std::string StmtNode::getPattern() {
	return std::string();
}

std::vector<StmtLstNode*> StmtNode::getChildStmtLst() {
	return std::vector<StmtLstNode*>();
}

/* ReadNode */
ReadNode::ReadNode(std::string varName) : StmtNode(), varName(varName) {}
std::string ReadNode::getVarName() {
	return varName;
}

StatementType ReadNode::getStmtType() {
	return StatementType::readType;
}

std::unordered_set<std::string> ReadNode::getModifiesVars() {
	return { varName };
}

/* PrintNode */
PrintNode::PrintNode(std::string varName) : StmtNode(), varName(varName) {}

std::string PrintNode::getVarName() {
	return varName;
}

StatementType PrintNode::getStmtType() {
	return StatementType::printType;
}

std::unordered_set<std::string> PrintNode::getUsesVars() {
	return { varName };
}

/* ExprNode */
ExprNode::ExprNode(ExprNodeValueType valueType, std::string value) : SourceASTNode(), valueType(valueType), value(value) {}

void ExprNode::addChild(ExprNode* child) {
	children.push_back(child);
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

void ExprNode::populatePattern(std::vector<std::string>& tokens) {
	for (ExprNode* child : children) {
		child->populatePattern(tokens);
	}
	tokens.push_back(value);
}

/* AssignNode */
AssignNode::AssignNode(std::string varName, ExprNode* expr) : StmtNode(), varName(varName), expr(expr) {
	populateUsesSet();
	populatePattern();
}

std::string AssignNode::getVarName() {
	return varName;
}

StatementType AssignNode::getStmtType() {
	return StatementType::assignType;
}

ExprNode* AssignNode::getExpr() {
	return expr;
}

std::unordered_set<std::string> AssignNode::getModifiesVars() {
	return { varName };
}

void AssignNode::populateUsesSet() {
	std::queue<ExprNode*> queue;
	queue.push(this->getExpr());
	while (!queue.empty()) {
		ExprNode* currNode = queue.front();
		queue.pop();
		if (currNode->getExprNodeValueType() == ExprNodeValueType::varName) {
			usesVars.insert(currNode->getValue());
		} else if (currNode->getExprNodeValueType() == ExprNodeValueType::constValue) {
			usesConsts.insert(currNode->getValue());
		}

		for (ExprNode* child : currNode->getChildren()) {
			queue.push(child);
		}
	}
}

void AssignNode::populatePattern() {
	pattern.clear();
	std::vector<std::string> tokens;
	expr->populatePattern(tokens);
	pattern.push_back(' ');
	for (std::string& token : tokens) {
		for (char c : token) {
			pattern.push_back(c);
		}
		pattern.push_back(' ');
	}
}

std::string AssignNode::getPattern() {
	return pattern;
}

std::unordered_set<std::string> AssignNode::getUsesVars() {
	return usesVars;
}

std::unordered_set<std::string> AssignNode::getUsesConsts() {
	return usesConsts;
}

/* WhileNode */
WhileNode::WhileNode(ExprNode* condExpr, StmtLstNode* stmtLst) : StmtNode(), condExpr(condExpr), stmtLst(stmtLst) {}

ExprNode* WhileNode::getCondExpr() {
	return condExpr;
}

StmtLstNode* WhileNode::getStmtLst() {
	return stmtLst;
}

StatementType WhileNode::getStmtType() {
	return StatementType::whileType;
}

/* StmtLstNode */
StmtLstNode::StmtLstNode() : SourceASTNode() {}

void StmtLstNode::addStmtNode(StmtNode* stmtNode) {
	stmtNodes.push_back(stmtNode);
}

std::vector<StmtNode*> StmtLstNode::getStmtNodes() {
	return stmtNodes;
}

/* ProcedureNode */
ProcedureNode::ProcedureNode(std::string procName) : SourceASTNode(), procName(procName) {
	stmtLstNode = new StmtLstNode();
}

void ProcedureNode::addStmtLst(StmtLstNode* stmtLstNode) {
	this->stmtLstNode = stmtLstNode;
}

StmtLstNode* ProcedureNode::getStmtLstNode() {
	return stmtLstNode;
}

std::string ProcedureNode::getProcName() {
	return procName;
}

/* ProgramNode */
ProgramNode::ProgramNode() : SourceASTNode() {}

void ProgramNode::addProcedure(ProcedureNode* procedureNode) {
	procedureNodes.push_back(procedureNode);
}

std::vector<ProcedureNode*> ProgramNode::getProcedureNodes() {
	return procedureNodes;
}
