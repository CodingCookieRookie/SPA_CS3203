#include "DesignExtractor.h"

std::unordered_map<StmtNode*, StmtIndex> DesignExtractor::stmtNodeIndexMap{};

void DesignExtractor::processProgramNode(
	ProgramNode* programNode,
	std::unordered_map<ProcIndex, std::vector<StmtIndex>>& procStmtMap,
	std::unordered_map<StmtIndex, std::string>& callsProcNameMap) {
	for (ProcedureNode* procedureNode : programNode->getProcedureNodes()) {
		processProcedureNode(procedureNode, procStmtMap, callsProcNameMap);
	}
}

void DesignExtractor::processProcedureNode(
	ProcedureNode* procedureNode,
	std::unordered_map<ProcIndex, std::vector<StmtIndex>>& procStmtMap,
	std::unordered_map<StmtIndex, std::string>& callsProcNameMap) {
	ProcIndex procIndex = Entity::insertProc(procedureNode->getProcName());
	std::vector<StmtIndex> stmtIndices =
		processStmtLstNode(procedureNode->getStmtLstNode(), callsProcNameMap);

	/* Populates procStmtMap with direct stmts, not including the nested stmts */
	procStmtMap[procIndex] = stmtIndices;
}

std::vector<StmtIndex> DesignExtractor::processStmtLstNode(
	StmtLstNode* stmtLstNode,
	std::unordered_map<StmtIndex, std::string>& callsProcNameMap) {
	std::vector<StmtIndex> stmtIndices;
	StmtIndex prevIndex = 0;
	for (StmtNode* stmtNode : stmtLstNode->getStmtNodes()) {
		StmtIndex stmtIndex = processStmtNode(stmtNode, prevIndex, callsProcNameMap);
		stmtIndices.push_back(stmtIndex);
		prevIndex = stmtIndex;
	}
	return stmtIndices;
}

StmtIndex DesignExtractor::processStmtNode(
	StmtNode* stmtNode,
	StmtIndex& prevIndex,
	std::unordered_map<StmtIndex, std::string>& callsProcNameMap) {
	StmtIndex stmtIndex = insertStmt(stmtNode);
	stmtNodeIndexMap[stmtNode] = stmtIndex;

	/* Populates the relevant Entity and Relationship */
	insertFollows(prevIndex, stmtIndex);
	insertModifies(stmtNode, stmtIndex);
	insertUses(stmtNode, stmtIndex);
	insertParent(stmtNode, stmtIndex, callsProcNameMap);
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
	std::unordered_set<std::string> consts = stmtNode->getConsts();
	for (const std::string& constName : consts) {
		Entity::insertConst(stoi(constName));
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
	std::unordered_map<StmtIndex, std::string>& callsProcNameMap) {
	std::vector<StmtLstNode*> childStmtLsts = stmtNode->getChildStmtLst();
	for (StmtLstNode* stmtLstNode : childStmtLsts) {
		std::vector<StmtIndex> stmtIndices = processStmtLstNode(stmtLstNode, callsProcNameMap);
		for (StmtIndex& childIndex : stmtIndices) {
			Parent::insert(stmtIndex, childIndex);
			Container::insertStmtInContainer(stmtIndex, childIndex);
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

void DesignExtractor::appendNode(CFGNode*& head, CFGNode*& tail, CFGNode* next) {
	if (head == nullptr) {
		head = next;
	} else {
		tail->addNext(next);
	}
	tail = next;
}

void DesignExtractor::processCFGStmtIndices(std::set<StmtIndex>& stmtIndices, CFGNode*& head, CFGNode*& tail) {
	if (!stmtIndices.empty()) {
		CFGNode* next = new CFGNode(stmtIndices);
		appendNode(head, tail, next);
	}
}

CFG DesignExtractor::generateCFG(StmtLstNode* stmtLstNode) {
	CFGNode* head = nullptr;
	CFGNode* tail = nullptr;
	std::set<StmtIndex> stmtIndices;
	for (StmtNode* stmtNode : stmtLstNode->getStmtNodes()) {
		if (stmtNode->getStmtType() == StatementType::IF_TYPE) {
			/* Add CFG node for previous non-container statements */
			processCFGStmtIndices(stmtIndices, head, tail);

			/* Add CFG node for if statement */
			stmtIndices.clear();
			stmtIndices.insert(stmtNodeIndexMap[stmtNode]);
			CFGNode* ifNode = new CFGNode(stmtIndices);
			appendNode(head, tail, ifNode);
			stmtIndices.clear();

			/* Link if statement node to CFG of its statement lists */
			CFG thenCFG = generateCFG(stmtNode->getChildStmtLst()[0]);
			CFG elseCFG = generateCFG(stmtNode->getChildStmtLst()[1]);
			CFGNode* thenCFGHead = thenCFG.getHead();
			CFGNode* elseCFGHead = elseCFG.getHead();
			tail->addNext(thenCFGHead);
			tail->addNext(elseCFGHead);

			/* Link to dummy node */
			CFGNode* dummy = new CFGNode();
			thenCFG.addToEnd(dummy);
			elseCFG.addToEnd(dummy);
			tail = dummy;
		} else if (stmtNode->getStmtType() == StatementType::WHILE_TYPE) {
			/* Add CFG node for previous non-container statements */
			processCFGStmtIndices(stmtIndices, head, tail);

			/* Add CFG node for while statement */
			stmtIndices.clear();
			stmtIndices.insert(stmtNodeIndexMap[stmtNode]);
			CFGNode* whileNode = new CFGNode(stmtIndices);
			appendNode(head, tail, whileNode);
			stmtIndices.clear();

			/* Link if statement node to CFG of its statement list */
			CFG whileCFG = generateCFG(stmtNode->getChildStmtLst()[0]);
			CFGNode* whileCFGHead = whileCFG.getHead();
			whileNode->addNext(whileCFGHead);
			whileCFG.addToEnd(whileNode);
		} else {
			/* Accumulate non-container statement indices in stmtIndices */
			stmtIndices.insert(stmtNodeIndexMap[stmtNode]);
		}
	}
	processCFGStmtIndices(stmtIndices, head, tail);

	return CFG(head, tail);
}

void DesignExtractor::processCFGs(ProgramNode* programNode) {
	for (ProcedureNode* procedureNode : programNode->getProcedureNodes()) {
		StmtLstNode* stmtLstNode = procedureNode->getStmtLstNode();
		CFG cfg = generateCFG(stmtLstNode);
		extractNextInfo(cfg.getHead());
		PKBCFG::insert(cfg);
	}
}

void DesignExtractor::extractNextInfo(CFGNode* node) {
	std::set<StmtIndex> predecessorStmtIndices = node->getStmtIndices();
	std::unordered_set<CFGNode*> successorNodes = node->getNextNodes();

	StmtIndex predecessor = *predecessorStmtIndices.begin();
	/* For more than 1 statement in a CFGNode */
	if (predecessorStmtIndices.size() > 1) {
		for (StmtIndex successor : predecessorStmtIndices) {
			if (successor != predecessor) {
				Next::insert(predecessor, successor);
				predecessor = successor;
			}
		}
	}

	for (auto& successorNode : successorNodes) {
		CFGNode* currNode = successorNode;
		/* Dummy node encountered */
		while (currNode->getStmtIndices().size() == 0 && currNode->getNextNodes().size() > 0) {
			/* Update currNode to dummy node's successor node */
			currNode = *currNode->getNextNodes().begin();
		}
		if (currNode->getStmtIndices().size() > 0) {
			StmtIndex successor = *currNode->getStmtIndices().begin();
			if (!Next::containsPredecessor(predecessor, successor)) {
				Next::insert(predecessor, successor);
				extractNextInfo(currNode);
			}
		}
	}
}

void DesignExtractor::extract(SourceAST& ast) {
	/* Maps procedure index to its direct stmt indices */
	std::unordered_map<ProcIndex, std::vector<StmtIndex>> procStmtMap;

	/* Maps stmt index of a call stmt to the procedure name called */
	std::unordered_map<StmtIndex, std::string> callsProcNameMap;

	ProgramNode* programNode = ast.getRoot();
	processProgramNode(programNode, procStmtMap, callsProcNameMap);
	processProcStmtMap(procStmtMap);
	processCallsProcNameMap(callsProcNameMap);
	TransitivePopulator::populateRecursiveInfo();

	processCFGs(programNode);
}
