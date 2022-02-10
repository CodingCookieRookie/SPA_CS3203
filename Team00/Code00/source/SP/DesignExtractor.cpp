#include "DesignExtractor.h"

std::unordered_map<ProcIndex, std::vector<StmtIndex>, ProcIndex::HashFunction> DesignExtractor::procStmtMap{};
std::unordered_map<StmtIndex, std::vector<StmtIndex>, StmtIndex::HashFunction> DesignExtractor::stmtParentMap{};

void DesignExtractor::processProgramNode(ProgramNode* programNode) {
	for (ProcedureNode* procedureNode : programNode->getProcedureNodes()) {
		processProcedureNode(procedureNode);
	}
}

void DesignExtractor::processProcedureNode(ProcedureNode* procedureNode) {
	ProcIndex procIndex = Entity::insertProc(procedureNode->getProcName());
	std::vector<StmtIndex> stmtIndices =
		processStmtLstNode(procedureNode->getStmtLstNode());
	procStmtMap[procIndex] = stmtIndices;
}

std::vector<StmtIndex> DesignExtractor::processStmtLstNode(
	StmtLstNode* stmtLstNode) {
	std::vector<StmtIndex> stmtIndices;
	for (StmtNode* stmtNode : stmtLstNode->getStmtNodes()) {
		StmtIndex stmtIndex = processStmtNode(stmtNode);
		stmtIndices.push_back(stmtIndex);
	}
	return stmtIndices;
}

StmtIndex DesignExtractor::processStmtNode(StmtNode* stmtNode) {
	StmtIndex stmtIndex = Entity::insertStmt(stmtNode->getStmtType());
	std::unordered_set<std::string> modifies = stmtNode->getModifiesVars();
	for (const std::string& varName : modifies) {
		Entity::insertVar(varName);
	}
	std::unordered_set<std::string> uses = stmtNode->getUsesVars();
	for (const std::string& varName : uses) {
		Entity::insertVar(varName);
	}
	std::vector<StmtLstNode*> childStmtLsts = stmtNode->getChildStmtLst();
	for (StmtLstNode* stmtLstNode : childStmtLsts) {
		std::vector<StmtIndex> stmtIndices = processStmtLstNode(stmtLstNode);
		for (StmtIndex& childIndex : stmtIndices) {
			stmtParentMap[stmtIndex].push_back(childIndex);
		}
	}
	return stmtIndex;
}

void DesignExtractor::Extract(SourceAST& ast) {
	procStmtMap.clear();
	stmtParentMap.clear();
	processProgramNode(ast.getRoot());
}

std::unordered_map<ProcIndex, std::vector<StmtIndex>, ProcIndex::HashFunction> DesignExtractor::getProcStmtMap() {
	return procStmtMap;
}

std::unordered_map<StmtIndex, std::vector<StmtIndex>, StmtIndex::HashFunction> DesignExtractor::getStmtParentMap() {
	return stmtParentMap;
}
