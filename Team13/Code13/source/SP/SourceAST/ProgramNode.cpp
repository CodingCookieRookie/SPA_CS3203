#pragma once

#include "ProgramNode.h"

ProgramNode::ProgramNode() : SourceASTNode() {}

ProgramNode::~ProgramNode() {
	for (ProcedureNode*& procNode : procedureNodes) {
		delete procNode;
		procNode = nullptr;
	}
}

void ProgramNode::addProcedure(ProcedureNode* procedureNode) {
	procedureNodes.push_back(procedureNode);
}

void ProgramNode::process() {
	for (ProcedureNode* procedureNode : getProcedureNodes()) {
		procedureNode->process(relationshipMaps, entityMaps);
	}

	/* For every call stmt, converts the name of the proc called to its index. Note that stmtIndex starts from 1 */
	for (const auto& [stmtIndex, procNameCalled] : relationshipMaps.callStmtToProcNameCalledMap) {
		StmtNode* stmtNode = entityMaps.stmtNodes[stmtIndex - 1];
		ProcIndex procIndexCalled = entityMaps.procNameToIndexMap[procNameCalled];
		stmtNode->populateSameSynonymsRS(relationshipMaps.callStmtToProcIndexCalledMap, stmtIndex, { procIndexCalled });

		ProcIndex procIndexCaller = entityMaps.stmtProcMap[stmtNode->getStmtIdx()];
		stmtNode->populateSameSynonymsRS(relationshipMaps.procIndexCallerToProcIndexCalledMap, procIndexCaller, { procIndexCalled });
	}
}

std::vector<ProcedureNode*> ProgramNode::getProcedureNodes() {
	return procedureNodes;
}

RelationshipMaps ProgramNode::getRelationshipMaps() {
	return relationshipMaps;
}

EntityMaps ProgramNode::getEntityMaps() {
	return entityMaps;
}
