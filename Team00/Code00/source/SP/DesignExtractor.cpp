#include "DesignExtractor.h"

std::unordered_map<ProcIndex, std::vector<StmtIndex>, ProcIndex::HashFunction> DesignExtractor::procStmtMap{};
std::unordered_map<StmtIndex, std::vector<StmtIndex>, StmtIndex::HashFunction> DesignExtractor::stmtParentMap{};
std::unordered_map<StmtIndex, StmtIndex, StmtIndex::HashFunction> DesignExtractor::stmtFollowsMap{};

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
	StmtIndex prevIndex;
	for (StmtNode* stmtNode : stmtLstNode->getStmtNodes()) {
		StmtIndex stmtIndex = processStmtNode(stmtNode, prevIndex);
		stmtIndices.push_back(stmtIndex);
		prevIndex = stmtIndex;
	}
	return stmtIndices;
}

StmtIndex DesignExtractor::processStmtNode(StmtNode* stmtNode, StmtIndex prevIndex) {
	StmtIndex stmtIndex = Entity::insertStmt(stmtNode->getStmtType());
	if (!(prevIndex == StmtIndex())) {
		stmtFollowsMap[prevIndex] = stmtIndex;
	}
	std::unordered_set<std::string> modifies = stmtNode->getModifiesVars();
	for (const std::string& varName : modifies) {
		VarIndex varIndex = Entity::insertVar(varName);
		Modifies::insert(stmtIndex, varIndex);
	}
	std::unordered_set<std::string> uses = stmtNode->getUsesVars();
	for (const std::string& varName : uses) {
		VarIndex varIndex = Entity::insertVar(varName);
		Uses::insert(stmtIndex, varIndex);
	}
	std::unordered_set<std::string> consts = stmtNode->getUsesConsts();
	for (const std::string& constName : consts) {
		int constVal = stoi(constName);
		Entity::insertConst(constVal);
	}
	std::string pattern = stmtNode->getPattern();
	if (!pattern.empty() && modifies.size() == 1) {
		std::string varName = *(modifies.begin());
		VarIndex varIndex = Entity::getVarIdx(varName);
		Pattern::insertPostFixInfo(varIndex, pattern, stmtIndex);
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
	stmtFollowsMap.clear();
	processProgramNode(ast.getRoot());

	/* Populate Parent and Follows Tables, and compute their transitive closures */
	for (const std::pair<StmtIndex, std::vector<StmtIndex>> parentPair : stmtParentMap) {
		StmtIndex predecessor = parentPair.first;
		std::vector<StmtIndex> successors = parentPair.second;
		for (const StmtIndex& successor : successors) {
			Parent::insert(predecessor, successor);
		}
	}
	ParentT::populate();
	for (const std::pair<StmtIndex, StmtIndex>& followsPair : stmtFollowsMap) {
		StmtIndex predecessor = followsPair.first;
		StmtIndex successor = followsPair.second;
		Follows::insert(predecessor, successor);
	}
	FollowsT::populate();
}

std::unordered_map<ProcIndex, std::vector<StmtIndex>, ProcIndex::HashFunction> DesignExtractor::getProcStmtMap() {
	return procStmtMap;
}

std::unordered_map<StmtIndex, std::vector<StmtIndex>, StmtIndex::HashFunction> DesignExtractor::getStmtParentMap() {
	return stmtParentMap;
}

std::unordered_map<StmtIndex, StmtIndex, StmtIndex::HashFunction> DesignExtractor::getStmtFollowsMap() {
	return stmtFollowsMap;
}
