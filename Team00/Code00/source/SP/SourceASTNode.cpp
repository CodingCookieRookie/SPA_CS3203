#include "SourceASTNode.h"

using namespace std;

/* SourceASTNode */
SourceASTNode::SourceASTNode() {}
//SourceASTNode::SourceASTNode() : value() {}
//
//SourceASTNode::SourceASTNode(std::string& value) :
//	value(value) {}
//
//void SourceASTNode::addChild(SourceASTNode* node) {
//	children.push_back(node);
//}
//
//std::vector<SourceASTNode*> SourceASTNode::getChildren() {
//	return children;
//}
//
//std::string SourceASTNode::getValue() {
//	return value;
//}

/* StmtNode */
StmtNode::StmtNode(int stmtNum) : SourceASTNode(), stmtNum(stmtNum) { }

int StmtNode::getStmtNum() {
	return stmtNum;
}

void StmtNode::printNode(int depth) {
	printDashes(depth);
	std::cout << stmtNum << ":stmtNode" << std::endl;
}

/* ReadNode */
ReadNode::ReadNode(std::string varName) : varName(varName) { }

std::string ReadNode::getVarName() {
	return varName;
}

void ReadNode::printNode(int depth) {
	printDashes(depth);
	std::cout << varName << ":read" << std::endl;
}

/* PrintNode */
PrintNode::PrintNode(std::string varName) : varName(varName) { }

std::string PrintNode::getVarName() {
	return varName;
}

void PrintNode::printNode(int depth) {
	printDashes(depth);
	std::cout << varName << ":print" << std::endl;
}

/* StmtListNode */
StmtListNode::StmtListNode() : SourceASTNode() { }

void StmtListNode::addStmtNode(StmtNode* stmtNode) {
	stmtNodes.push_back(stmtNode);
}

std::vector<StmtNode*> StmtListNode::getStmtNodes() {
	return stmtNodes;
}

void StmtListNode::printNode(int depth) {
	printDashes(depth);
	std::cout << ":stmtLISTNode" << std::endl;
}

/* ProcedureNode */
ProcedureNode::ProcedureNode(std::string procName) : SourceASTNode(), procName(procName) { }

void ProcedureNode::addStmtList(StmtListNode* stmtListNode) {
	this->stmtListNode = stmtListNode;
}

StmtListNode* ProcedureNode::getStmtListNode() {
	return stmtListNode;
}

std::string ProcedureNode::getProcName() {
	return procName;
}

void ProcedureNode::printNode(int depth) {
	printDashes(depth);
	std::cout << procName << ":procedure" << std::endl;
}

/* ProgramNode */
ProgramNode::ProgramNode() : SourceASTNode() { }

void ProgramNode::addProcedure(ProcedureNode* procedureNode) {
	procedureNodes.push_back(procedureNode);
}

std::vector<ProcedureNode*> ProgramNode::getProcedureNodes() {
	return procedureNodes;
}

void ProgramNode::printNode(int depth) {
	printDashes(depth);
	std::cout << ":program" << std::endl;
}
