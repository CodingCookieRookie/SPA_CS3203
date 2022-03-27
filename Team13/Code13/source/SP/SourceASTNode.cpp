#include "SourceASTNode.h"

/* SourceASTNode */
SourceASTNode::SourceASTNode() {}
SourceASTNode::~SourceASTNode() {}

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

std::string StmtNode::getProcCalled() {
	return std::string();
}

std::string StmtNode::getPattern() {
	return std::string();
}

std::vector<StmtLstNode*> StmtNode::getChildStmtLst() {
	return std::vector<StmtLstNode*>();
}

std::unordered_set<std::string> StmtNode::getUses(ExprNode* expr, ExprNodeValueType valueType) {
	std::unordered_set<std::string> uses;
	std::stack<ExprNode*> stack;
	ExprNode* curr = expr;

	while (!stack.empty() || curr != nullptr) {
		/* Keeps traversing the left child as much as possible,
		else pops node from stack and traverses the right child. */
		if (curr != nullptr) {
			stack.push(curr);
			curr = curr->getLeftChild();
		} else {
			curr = stack.top();
			stack.pop();
			if (curr->getExprNodeValueType() == valueType) {
				uses.insert(curr->getValue());
			}
			curr = curr->getRightChild();
		}
	}

	return uses;
}

/* ReadNode */
ReadNode::ReadNode(std::string varName) : StmtNode(), varName(varName) {}

StatementType ReadNode::getStmtType() {
	return StatementType::READ_TYPE;
}

std::unordered_set<std::string> ReadNode::getModifiesVars() {
	return { varName };
}

/* PrintNode */
PrintNode::PrintNode(std::string varName) : StmtNode(), varName(varName) {}

StatementType PrintNode::getStmtType() {
	return StatementType::PRINT_TYPE;
}

std::unordered_set<std::string> PrintNode::getUsesVars() {
	return { varName };
}

/* AssignNode */
AssignNode::AssignNode(std::string varName, ExprNode* expr) : StmtNode(), varName(varName), expr(expr) {}

StatementType AssignNode::getStmtType() {
	return StatementType::ASSIGN_TYPE;
}

ExprNode* AssignNode::getExpr() {
	return expr;
}

std::unordered_set<std::string> AssignNode::getModifiesVars() {
	return { varName };
}

std::string AssignNode::getPattern() {
	return expr->getPattern();
}

std::unordered_set<std::string> AssignNode::getUsesVars() {
	return getUses(getExpr(), ExprNodeValueType::VAR_NAME);
}

std::unordered_set<std::string> AssignNode::getUsesConsts() {
	return getUses(getExpr(), ExprNodeValueType::CONST_VALUE);
}

/* ContainerNode */
ContainerNode::ContainerNode(ExprNode* condExpr, std::vector<StmtLstNode*> childStmtLst) : condExpr(condExpr), childStmtLst(childStmtLst) {}

ExprNode* ContainerNode::getCondExpr() {
	return condExpr;
}

std::vector<StmtLstNode*> ContainerNode::getChildStmtLst() {
	return childStmtLst;
}

std::unordered_set<std::string> ContainerNode::getUsesVars() {
	return getUses(condExpr, ExprNodeValueType::VAR_NAME);
}

std::unordered_set<std::string> ContainerNode::getUsesConsts() {
	return getUses(condExpr, ExprNodeValueType::CONST_VALUE);
}

/* WhileNode */
WhileNode::WhileNode(ExprNode* condExpr, StmtLstNode* stmtLst) : ContainerNode(condExpr, { stmtLst }) {}

StatementType WhileNode::getStmtType() {
	return StatementType::WHILE_TYPE;
}

/* IfNode */
IfNode::IfNode(ExprNode* condExpr, StmtLstNode* thenStmtLst, StmtLstNode* elseStmtLst) : ContainerNode(condExpr, { thenStmtLst, elseStmtLst }) {}

StatementType IfNode::getStmtType() {
	return StatementType::IF_TYPE;
}

/* CallNode */
CallNode::CallNode(std::string procName) : StmtNode(), procName(procName) {}

StatementType CallNode::getStmtType() {
	return StatementType::CALL_TYPE;
}

std::string CallNode::getProcCalled() {
	return procName;
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
