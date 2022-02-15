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

std::unordered_set<std::string> StmtNode::getConsts() {
	return std::unordered_set<std::string>();
}

std::string StmtNode::getPattern() {
	return std::string();
}

std::vector<StmtLstNode*> StmtNode::getChildStmtLst() {
	return std::vector<StmtLstNode*>();
}

std::unordered_set<std::string> StmtNode::getUsesVarsInExpr(ExprNode* expr) {
	std::unordered_set<std::string> usesVars;
	std::queue<ExprNode*> queue;
	queue.push(expr);

	while (!queue.empty()) {
		ExprNode* currNode = queue.front();
		queue.pop();
		if (currNode->getExprNodeValueType() == ExprNodeValueType::varName) {
			usesVars.insert(currNode->getValue());
		}

		for (ExprNode* child : currNode->getChildren()) {
			queue.push(child);
		}
	}

	return usesVars;
}

std::unordered_set<std::string> StmtNode::getConstsInExpr(ExprNode* expr) {
	std::unordered_set<std::string> consts;

	std::queue<ExprNode*> queue;
	queue.push(expr);
	while (!queue.empty()) {
		ExprNode* currNode = queue.front();
		queue.pop();
		if (currNode->getExprNodeValueType() == ExprNodeValueType::constValue) {
			consts.insert(currNode->getValue());
		}

		for (ExprNode* child : currNode->getChildren()) {
			queue.push(child);
		}
	}

	return consts;
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
	return getUsesVarsInExpr(getExpr());
}

std::unordered_set<std::string> AssignNode::getConsts() {
	return getConstsInExpr(getExpr());
}

/* WhileNode */
WhileNode::WhileNode(ExprNode* condExpr, StmtLstNode* stmtLst) : StmtNode(), condExpr(condExpr), stmtLst(stmtLst) {}

ExprNode* WhileNode::getCondExpr() {
	return condExpr;
}

StatementType WhileNode::getStmtType() {
	return StatementType::whileType;
}

std::unordered_set<std::string> WhileNode::getUsesVars() {
	return getUsesVarsInExpr(condExpr);
}

std::unordered_set<std::string> WhileNode::getConsts() {
	return getConstsInExpr(condExpr);
}

std::vector<StmtLstNode*> WhileNode::getChildStmtLst() {
	return { stmtLst };
}

std::vector<StmtLstNode*> WhileNode::getChildStmtLst() {
	return { stmtLst };
}

/* IfNode */
IfNode::IfNode(ExprNode* condExpr, StmtLstNode* thenStmtLst, StmtLstNode* elseStmtLst) : StmtNode(), condExpr(condExpr), thenStmtLst(thenStmtLst), elseStmtLst(elseStmtLst) {}

ExprNode* IfNode::getCondExpr() {
	return condExpr;
}

StmtLstNode* IfNode::getThenStmtLst() {
	return thenStmtLst;
}

StmtLstNode* IfNode::getElseStmtLst() {
	return elseStmtLst;
}

StatementType IfNode::getStmtType() {
	return StatementType::ifType;
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
