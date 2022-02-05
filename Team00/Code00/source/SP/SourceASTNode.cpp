#include "SourceASTNode.h"

using namespace std;

/* SourceASTNode */
SourceASTNode::SourceASTNode() {}

/* StmtNode */
StmtNode::StmtNode() : SourceASTNode() { }

void StmtNode::printNode(int depth) {
	printDashes(depth);
	std::cout << ":stmtNode" << std::endl;
}

/* ReadNode */
ReadNode::ReadNode(std::string varName) : StmtNode(), varName(varName) { }

std::string ReadNode::getVarName() {
	return varName;
}

StatementType ReadNode::getStmtType() {
	return StatementType::readType;
}

std::vector<std::string> ReadNode::getModifies() {
	return { varName };
}

std::vector<std::string> ReadNode::getUses() {
	return std::vector<std::string>();
}

void ReadNode::printNode(int depth) {
	printDashes(depth);
	std::cout << varName << ":read" << std::endl;
}

/* PrintNode */
PrintNode::PrintNode(std::string varName) : StmtNode() , varName(varName) { }

std::string PrintNode::getVarName() {
	return varName;
}

StatementType PrintNode::getStmtType() {
	return StatementType::printType;
}

std::vector<std::string> PrintNode::getModifies() {
	return std::vector<std::string>();
}

std::vector<std::string> PrintNode::getUses() {
	return { varName };
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
	for (StmtNode* stmtNode : stmtNodes) {
		stmtNode->printNode(depth + 1);
	}
}

/* ProcedureNode */
ProcedureNode::ProcedureNode(std::string procName) : SourceASTNode(), procName(procName) { 
	stmtListNode = new StmtListNode();
}

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
	stmtListNode->printNode(depth + 1);
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
	for (ProcedureNode* procedureNode : procedureNodes) {
		procedureNode->printNode(depth + 1);
	}
}
