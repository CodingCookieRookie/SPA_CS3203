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
