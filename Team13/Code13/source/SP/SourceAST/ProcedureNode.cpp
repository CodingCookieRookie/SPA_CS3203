#include "ProcedureNode.h"

ProcedureNode::ProcedureNode(std::string procName) : SourceASTNode(), procName(procName), procIndex(0) {
	stmtLstNode = new StmtLstNode();
}

ProcedureNode::~ProcedureNode() {
	delete stmtLstNode;
	stmtLstNode = nullptr;
}

void ProcedureNode::bidirectionalPopulateProcAndStmt(ProcStmtMap& procStmtMap, StmtProcMap& stmtProcMap,
	std::unordered_set<StmtIndex>& stmtIndices) {
	for (StmtIndex stmtIndex : stmtIndices) {
		procStmtMap[procIndex].push_back(stmtIndex);
		stmtProcMap[stmtIndex] = procIndex;
	}
}

void ProcedureNode::bidirectionalPopulateProcNameAndIndex(ProcNameToIndexMap& procNameToIndexMap,
	SortedProcIndexToNameMap& sortedProcIndexToNameMap) {
	procNameToIndexMap[procName] = procIndex;
	sortedProcIndexToNameMap[procIndex] = procName;
}

void ProcedureNode::setProcIndex(ProcIndex procIndex) {
	this->procIndex = procIndex;
}

void ProcedureNode::addStmtLst(StmtLstNode* stmtLstNode) {
	this->stmtLstNode = stmtLstNode;
}

void ProcedureNode::process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) {
	getStmtLstNode()->process(relationshipMaps, entityMaps);

	/* Sets procIndex. Note that procIndex starts with 1. */
	ProcIndex procIndex = entityMaps.procStmtMap.size() + 1;
	setProcIndex(procIndex);

	/* Bidirectional population of ProcStmtMap and StmtProcMap.*/
	std::unordered_set<StmtIndex> stmtIndices = stmtLstNode->getAllStmtNodeIndices();
	bidirectionalPopulateProcAndStmt(entityMaps.procStmtMap, entityMaps.stmtProcMap, stmtIndices);

	/* Bidirectional population of ProcNameToIndex and ProcIndexToName */
	bidirectionalPopulateProcNameAndIndex(entityMaps.procNameToIndexMap, entityMaps.sortedProcIndexToNameMap);
}

StmtLstNode* ProcedureNode::getStmtLstNode() {
	return stmtLstNode;
}

std::string ProcedureNode::getProcName() {
	return procName;
}
