#include "SourceASTNode.h"

using namespace std;

/* SourceASTNode */
SourceASTNode::SourceASTNode() {}

/* StmtNode */
StmtNode::StmtNode(size_t stmtNum) : SourceASTNode(), stmtNum(stmtNum) { }

size_t StmtNode::getStmtNum() {
	return stmtNum;
}


void StmtNode::printNode(int depth) {
	printDashes(depth);
	std::cout << stmtNum << ":stmtNode" << std::endl;
}

/* ReadNode */
ReadNode::ReadNode(std::string varName, size_t stmtNum) : StmtNode(stmtNum), varName(varName) { }

std::string ReadNode::getVarName() {
	return varName;
}

void ReadNode::printNode(int depth) {
	printDashes(depth);
	std::cout << varName << ":read, stmtNum: " << getStmtNum() << std::endl;
}

/* PrintNode */
PrintNode::PrintNode(std::string varName, size_t stmtNum) : StmtNode(stmtNum) , varName(varName) { }

std::string PrintNode::getVarName() {
	return varName;
}

void PrintNode::printNode(int depth) {
	printDashes(depth);
	std::cout << varName << ":print, stmtNum: " << getStmtNum() << std::endl;
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

size_t ProgramNode::currStmtNum = 1;

void ProgramNode::addProcedure(ProcedureNode* procedureNode) {
	procedureNodes.push_back(procedureNode);
}

std::vector<ProcedureNode*> ProgramNode::getProcedureNodes() {
	return procedureNodes;
}

void ProgramNode::incrCurrStmtNum() {
	currStmtNum++;
}

void ProgramNode::printNode(int depth) {
	printDashes(depth);
	std::cout << ":program" << std::endl;
	for (ProcedureNode* procedureNode : procedureNodes) {
		procedureNode->printNode(depth + 1);
	}
}
