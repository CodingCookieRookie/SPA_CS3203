#include "DesignExtractor.h"

std::unordered_map<ProcIndex, std::vector<StmtIndex>> DesignExtractor::procStmtMap{};
std::unordered_map<StmtIndex, std::vector<StmtIndex>> DesignExtractor::stmtParentMap{};
std::unordered_map<StmtIndex, StmtIndex> DesignExtractor::stmtFollowsMap{};
std::unordered_map<ProcIndex, std::unordered_set<ProcIndex>> DesignExtractor::callsMap{};

void DesignExtractor::processProgramNode(ProgramNode* programNode) {
	for (ProcedureNode* procedureNode : programNode->getProcedureNodes()) {
		processProcedureNode(procedureNode);
	}
}

void DesignExtractor::processProcedureNode(ProcedureNode* procedureNode) {
	ProcIndex procIndex = Entity::getProcIdx(procedureNode->getProcName());
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
		ModifiesS::insert(stmtIndex, varIndex);
	}

	/* Container stmts use control variables for pattern */
	std::unordered_set<std::string> usesVars = stmtNode->getUsesVars();
	for (const std::string& varName : usesVars) {
		VarIndex varIndex = Entity::insertVar(varName);
		switch (stmtNode->getStmtType()) {
		case StatementType::ifType:
			Pattern::insertIfInfo(stmtIndex, varIndex);
			break;
		case StatementType::whileType:
			Pattern::insertWhileInfo(stmtIndex, varIndex);
			break;
		default:
			UsesS::insert(stmtIndex, varIndex);
			break;
		}
	}

	std::unordered_set<std::string> consts = stmtNode->getConsts();
	for (const std::string& constName : consts) {
		int constVal = stoi(constName);
		Entity::insertConst(constVal);
	}

	std::string pattern = stmtNode->getPattern();
	if (!pattern.empty() && modifies.size() == 1) {
		std::string varName = *(modifies.begin());
		VarIndex varIndex = Entity::getVarIdx(varName);
		Pattern::insertAssignInfo(varIndex, pattern, stmtIndex);
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

void DesignExtractor::extract(SourceAST& ast) {
	procStmtMap.clear();
	stmtParentMap.clear();
	stmtFollowsMap.clear();

	ProgramNode* progNode = ast.getRoot();

	validateAST(progNode);
	processProgramNode(progNode);

	/* Populate Parent and Follows Tables, and compute their transitive closures */
	for (const std::pair<StmtIndex, std::vector<StmtIndex>> parentPair : stmtParentMap) {
		StmtIndex predecessor = parentPair.first;
		std::vector<StmtIndex> successors = parentPair.second;
		for (StmtIndex& successor : successors) {
			Parent::insert(predecessor, successor);
			Container::insertStmtInContainer(predecessor, successor);
		}
	}
	for (const std::pair<StmtIndex, StmtIndex>& followsPair : stmtFollowsMap) {
		StmtIndex predecessor = followsPair.first;
		StmtIndex successor = followsPair.second;
		Follows::insert(predecessor, successor);
	}
	TransitivePopulator::populateRecursiveInfo();
}

void DesignExtractor::validateAST(ProgramNode* progNode) {
	validateNoDuplicateProc(progNode);
	populateCalls(progNode);
}

void DesignExtractor::validateNoDuplicateProc(ProgramNode* progNode) {
	for (ProcedureNode* procedureNode : progNode->getProcedureNodes()) {
		if (Entity::containsProc(procedureNode->getProcName())) {
			throw DEException(DEException::DUPLICATE_PROC_NAMES);
		}
		Entity::insertProc(procedureNode->getProcName());
	}
}

void DesignExtractor::populateCalls(ProgramNode* progNode) {
	for (ProcedureNode* procedureNode : progNode->getProcedureNodes()) {
		std::unordered_set<ProcIndex> calledProcs;
		processCallsStmtLstNode(procedureNode->getStmtLstNode(), calledProcs);
		callsMap[Entity::getProcIdx(procedureNode->getProcName())] = calledProcs;
	}
}

void DesignExtractor::processCallsStmtLstNode(StmtLstNode* stmtLstNode, std::unordered_set<ProcIndex>& calledProcs) {
	for (StmtNode* stmtNode : stmtLstNode->getStmtNodes()) {
		std::vector<StmtLstNode*> childStmtLsts = stmtNode->getChildStmtLst();
		for (StmtLstNode* stmtLstNode : childStmtLsts) {
			processCallsStmtLstNode(stmtLstNode, calledProcs);
		}

		if (stmtNode->getStmtType() == StatementType::callType) {
			processCallsStmtNode(stmtNode, calledProcs);
		}
	}
}

void DesignExtractor::processCallsStmtNode(StmtNode* stmtNode, std::unordered_set<ProcIndex>& calledProcs) {
	std::string procName = stmtNode->getProcCalled();
	if (!(Entity::containsProc(procName))) {
		throw DEException(DEException::CALL_NON_EXISTENT_PROC_NAME);
	}

	calledProcs.insert(Entity::getProcIdx(procName));
}

std::unordered_map<ProcIndex, std::vector<StmtIndex>> DesignExtractor::getProcStmtMap() {
	return procStmtMap;
}

std::unordered_map<StmtIndex, std::vector<StmtIndex>> DesignExtractor::getStmtParentMap() {
	return stmtParentMap;
}

std::unordered_map<StmtIndex, StmtIndex> DesignExtractor::getStmtFollowsMap() {
	return stmtFollowsMap;
}
