#include "DesignExtractor.h"

static const int EXIT_NODE_IDX;

void DesignExtractor::processProgramNode(
	ProgramNode* programNode,
	std::unordered_map<ProcIndex, std::vector<StmtIndex>>& procStmtMap,
	std::unordered_map<StmtIndex, std::string>& callsProcNameMap,
	std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap) {
	for (ProcedureNode* procedureNode : programNode->getProcedureNodes()) {
		processProcedureNode(procedureNode, procStmtMap, callsProcNameMap, stmtNodeIndexMap);
	}
}

void DesignExtractor::processProcedureNode(
	ProcedureNode* procedureNode,
	std::unordered_map<ProcIndex, std::vector<StmtIndex>>& procStmtMap,
	std::unordered_map<StmtIndex, std::string>& callsProcNameMap,
	std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap) {
	ProcIndex procIndex = Entity::insertProc(procedureNode->getProcName());
	std::vector<StmtIndex> stmtIndices =
		processStmtLstNode(procedureNode->getStmtLstNode(), callsProcNameMap, stmtNodeIndexMap);

	/* Populates procStmtMap with direct stmts, not including the nested stmts */
	procStmtMap[procIndex] = stmtIndices;
}

std::vector<StmtIndex> DesignExtractor::processStmtLstNode(
	StmtLstNode* stmtLstNode,
	std::unordered_map<StmtIndex, std::string>& callsProcNameMap,
	std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap) {
	std::vector<StmtIndex> stmtIndices;
	StmtIndex prevIndex = 0;
	for (StmtNode* stmtNode : stmtLstNode->getStmtNodes()) {
		StmtIndex stmtIndex = processStmtNode(stmtNode, prevIndex, callsProcNameMap, stmtNodeIndexMap);
		stmtIndices.push_back(stmtIndex);
		prevIndex = stmtIndex;
	}
	return stmtIndices;
}

StmtIndex DesignExtractor::processStmtNode(
	StmtNode* stmtNode,
	StmtIndex& prevIndex,
	std::unordered_map<StmtIndex, std::string>& callsProcNameMap,
	std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap) {
	StmtIndex stmtIndex = insertStmt(stmtNode);
	stmtNodeIndexMap[stmtNode] = stmtIndex;

	/* Populates the relevant Entity and Relationship */
	insertFollows(prevIndex, stmtIndex);
	insertModifies(stmtNode, stmtIndex);
	insertUses(stmtNode, stmtIndex);
	insertParent(stmtNode, stmtIndex, callsProcNameMap, stmtNodeIndexMap);
	insertConst(stmtNode);
	insertPattern(stmtNode, stmtIndex);

	/* Populates callsProcNameMap only if current stmt is a call stmt */
	if (stmtNode->getStmtType() == StatementType::CALL_TYPE) {
		callsProcNameMap[stmtIndex] = stmtNode->getProcCalled();
	}

	return stmtIndex;
}

StmtIndex DesignExtractor::insertStmt(StmtNode* stmtNode) {
	StmtIndex stmtIndex;
	std::string nameValue;
	StatementType stmtType = stmtNode->getStmtType();

	switch (stmtType) {
	case StatementType::READ_TYPE:
		nameValue = *stmtNode->getModifiesVars().begin();
		stmtIndex = Entity::insertStmt(stmtType, nameValue);
		break;
	case StatementType::PRINT_TYPE:
		nameValue = *stmtNode->getUsesVars().begin();
		stmtIndex = Entity::insertStmt(stmtType, nameValue);
		break;
	case StatementType::CALL_TYPE:
		nameValue = stmtNode->getProcCalled();
		stmtIndex = Entity::insertStmt(stmtType, nameValue);
		break;
	default:
		stmtIndex = Entity::insertStmt(stmtType);
		break;
	}

	return stmtIndex;
}

void DesignExtractor::insertConst(StmtNode* stmtNode) {
	std::unordered_set<std::string> usesConsts = stmtNode->getUsesConsts();
	for (const std::string& usesConst : usesConsts) {
		Entity::insertConst(stoi(usesConst));
	}
}

void DesignExtractor::insertPattern(StmtNode* stmtNode, StmtIndex& stmtIndex) {
	std::unordered_set<std::string> modifies = stmtNode->getModifiesVars();
	std::string pattern = stmtNode->getPattern();
	if (!pattern.empty() && modifies.size() == 1) {
		std::string varName = *(modifies.begin());
		VarIndex varIndex = Entity::getVarIdx(varName);
		Pattern::insertAssignInfo(varIndex, pattern, stmtIndex);
	}
}

void DesignExtractor::insertFollows(StmtIndex& prevIndex, StmtIndex& stmtIndex) {
	if (!(prevIndex == StmtIndex())) {
		Follows::insert(prevIndex, stmtIndex);
	}
}

void DesignExtractor::insertModifies(StmtNode* stmtNode, StmtIndex& stmtIndex) {
	std::unordered_set<std::string> modifies = stmtNode->getModifiesVars();
	for (const std::string& varName : modifies) {
		VarIndex varIndex = Entity::insertVar(varName);
		ModifiesS::insert(stmtIndex, varIndex);
	}
}

void DesignExtractor::insertUses(StmtNode* stmtNode, StmtIndex& stmtIndex) {
	/* TODO (iter3): refactor container stmt's getUsesVars to getPattern */
	/* Container stmts use control variables for pattern */
	std::unordered_set<std::string> usesVars = stmtNode->getUsesVars();
	for (const std::string& varName : usesVars) {
		VarIndex varIndex = Entity::insertVar(varName);
		UsesS::insert(stmtIndex, varIndex);
		switch (stmtNode->getStmtType()) {
		case StatementType::IF_TYPE:
			Pattern::insertIfInfo(stmtIndex, varIndex);
			break;
		case StatementType::WHILE_TYPE:
			Pattern::insertWhileInfo(stmtIndex, varIndex);
			break;
		}
	}
}

void DesignExtractor::insertParent(
	StmtNode* stmtNode,
	StmtIndex& stmtIndex,
	std::unordered_map<StmtIndex, std::string>& callsProcNameMap,
	std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap) {
	std::vector<StmtLstNode*> childStmtLsts = stmtNode->getChildStmtLst();
	for (StmtLstNode* stmtLstNode : childStmtLsts) {
		std::vector<StmtIndex> stmtIndices = processStmtLstNode(stmtLstNode, callsProcNameMap, stmtNodeIndexMap);
		for (StmtIndex& childIndex : stmtIndices) {
			Parent::insert(stmtIndex, childIndex);
			Container::insertStmtInContainer(stmtIndex, childIndex);
		}
	}
}

void DesignExtractor::insertNext(CFG* cfg) {
	for (auto cfgEntry : cfg->getCFGTable()) {
		int keyStmtIdx = cfgEntry.first;
		for (int nextStmtIdx : cfgEntry.second) {
			Next::insert(keyStmtIdx, nextStmtIdx);
		}
	}
}

void DesignExtractor::processProcStmtMap(std::unordered_map<ProcIndex, std::vector<StmtIndex>>& procStmtMap) {
	for (auto& procStmt : procStmtMap) {
		ProcIndex procIndex = procStmt.first;
		std::vector<StmtIndex> stmtIndices = procStmt.second;
		for (StmtIndex stmtIndex : stmtIndices) {
			insertStmtFromProc(procIndex, stmtIndex);
		}
	}
}

void DesignExtractor::insertStmtFromProc(ProcIndex& procIndex, StmtIndex& stmtIndex) {
	Entity::insertStmtFromProc(procIndex, stmtIndex);

	/* Recursively insert statements in container stmts */
	std::vector<StmtIndex> nestedStmtIndices = Parent::getSuccessors(stmtIndex);
	for (StmtIndex nestedStmtIndex : nestedStmtIndices) {
		insertStmtFromProc(procIndex, nestedStmtIndex);
	}
}

void DesignExtractor::processCallsProcNameMap(std::unordered_map<StmtIndex, std::string>& callsProcNameMap) {
	/* TODO (iter3?): would be better if we have stmt-proc mapping? */
	for (ProcIndex& predecessor : Entity::getAllProcs()) {
		std::unordered_set<StmtIndex> stmtIndices = Entity::getStmtsFromProc(predecessor);

		/* Only populate Calls relationship if it is a call stmt*/
		for (auto& stmtIndex : stmtIndices) {
			if (callsProcNameMap.find(stmtIndex) != callsProcNameMap.end()) {
				ProcIndex successor = Entity::getProcIdx(callsProcNameMap[stmtIndex]);
				Calls::insert(predecessor, successor);
			}
		}
	}
}

void DesignExtractor::generateCFG(
	StmtLstNode* stmtLstNode,
	CFG* cfg,
	std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap) {
	std::vector<StmtNode*> stmtNodes = stmtLstNode->getStmtNodes();
	for (size_t i = 0; i < stmtNodes.size(); i++) {
		StmtNode* currNode = stmtNodes.at(i);
		int currStmtIdx = stmtNodeIndexMap[currNode];
		int nextStmtIdx = i == stmtNodes.size() - 1 ? EXIT_NODE_IDX : stmtNodeIndexMap[stmtNodes.at(i + 1)];
		generateCFGFromStmt(currNode, cfg, stmtNodeIndexMap, currStmtIdx, nextStmtIdx);
	}
}

void DesignExtractor::generateCFGFromStmt(
	StmtNode* currNode,
	CFG* cfg,
	std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap,
	int currStmtIdx,
	int nextStmtIdx) {
	if (currNode->getStmtType() == StatementType::IF_TYPE) {
		/* Process for then statement list and else statement list respectively */
		for (size_t i = 0; i <= 1; i++) {
			StmtLstNode* childStmtLst = currNode->getChildStmtLst()[i];
			StmtNode* firstChildStmtNode = childStmtLst->getStmtNodes().at(0);
			int firstChildStmtIdx = stmtNodeIndexMap[firstChildStmtNode];

			cfg->insert(currStmtIdx, firstChildStmtIdx);

			generateCFG(childStmtLst, cfg, stmtNodeIndexMap);

			StmtNode* lastChildStmtNode = childStmtLst->getStmtNodes().at(childStmtLst->getStmtNodes().size() - 1);
			int lastChildStmtIdx = stmtNodeIndexMap[lastChildStmtNode];

			generateCFGFromStmt(lastChildStmtNode, cfg, stmtNodeIndexMap, lastChildStmtIdx, nextStmtIdx);
		}
	} else if (currNode->getStmtType() == StatementType::WHILE_TYPE) {
		if (nextStmtIdx != EXIT_NODE_IDX) {
			cfg->insert(currStmtIdx, nextStmtIdx);
		}

		StmtLstNode* childStmtLst = currNode->getChildStmtLst()[0];
		StmtNode* firstChildStmtNode = childStmtLst->getStmtNodes().at(0);
		int firstChildStmtIdx = stmtNodeIndexMap[firstChildStmtNode];

		cfg->insert(currStmtIdx, firstChildStmtIdx);

		generateCFG(childStmtLst, cfg, stmtNodeIndexMap);

		StmtNode* lastChildStmtNode = childStmtLst->getStmtNodes().at(childStmtLst->getStmtNodes().size() - 1);
		int lastChildStmtIdx = stmtNodeIndexMap[lastChildStmtNode];

		generateCFGFromStmt(lastChildStmtNode, cfg, stmtNodeIndexMap, lastChildStmtIdx, currStmtIdx);
	} else {
		if (nextStmtIdx != EXIT_NODE_IDX) {
			cfg->insert(currStmtIdx, nextStmtIdx);
		}
	}
}

void DesignExtractor::processCFGs(
	ProgramNode* programNode,
	CFG* cfg,
	std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap) {
	for (ProcedureNode* procedureNode : programNode->getProcedureNodes()) {
		StmtLstNode* stmtLstNode = procedureNode->getStmtLstNode();
		generateCFG(stmtLstNode, cfg, stmtNodeIndexMap);
	}
	insertNext(cfg);
}

void DesignExtractor::extract(SourceAST& ast, CFG* cfg) {
	/* Maps procedure index to its direct stmt indices */
	std::unordered_map<ProcIndex, std::vector<StmtIndex>> procStmtMap;

	/* Maps stmt index of a call stmt to the procedure name called */
	std::unordered_map<StmtIndex, std::string> callsProcNameMap;

	/* Maps a StmtNode to the StmtIndex of the statement */
	std::unordered_map<StmtNode*, StmtIndex> stmtNodeIndexMap;

	ProgramNode* programNode = ast.getRoot();
	processProgramNode(programNode, procStmtMap, callsProcNameMap, stmtNodeIndexMap);
	processProcStmtMap(procStmtMap);
	processCallsProcNameMap(callsProcNameMap);
	TransitivePopulator::populateRecursiveInfo();

	processCFGs(programNode, cfg, stmtNodeIndexMap);
}
