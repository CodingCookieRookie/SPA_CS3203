#include "SourceASTNode.h"

/* SourceASTNode */
SourceASTNode::SourceASTNode() : value() {}

SourceASTNode::SourceASTNode(std::string& value) :
	value(value) {}

void SourceASTNode::addChild(SourceASTNode* node) {
	m_nodes.push_back(node);
}

std::vector<SourceASTNode*> SourceASTNode::getChildren() {
	return m_nodes;
}

std::string SourceASTNode::getValue() {
	return value;
}

/* ProgramNode */
ProgramNode::ProgramNode() : SourceASTNode() { }

void ProgramNode::printNode(int depth) {
	printDashes(depth);
	std::cout << value << ":program" << std::endl;
}

/* ProcedureNode */
ProcedureNode::ProcedureNode(std::string& value) : SourceASTNode(value) { }

void ProcedureNode::printNode(int depth) {
	printDashes(depth);
	std::cout << value << ":procedure" << std::endl;
}

/* StmtListNode */
StmtListNode::StmtListNode() : SourceASTNode() { }

void StmtListNode::printNode(int depth) {
	printDashes(depth);
	std::cout << value << ":stmtLISTNode" << std::endl;
}

/* StmtNode */
StmtNode::StmtNode(std::string& value) : SourceASTNode(value) { }

void StmtNode::printNode(int depth) {
	printDashes(depth);
	std::cout << value << ":stmtNode" << std::endl;
}

/* ReadNode */
ReadNode::ReadNode(std::string& value) : StmtNode(value) { }

void ReadNode::printNode(int depth) {
	printDashes(depth);
	std::cout << value << ":read" << std::endl;
}
