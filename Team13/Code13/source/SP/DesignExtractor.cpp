#include "DesignExtractor.h"

std::unordered_map<StmtIndex, std::vector<StmtIndex>> DesignExtractor::stmtParentMap{};
std::unordered_map<StmtIndex, StmtIndex> DesignExtractor::stmtFollowsMap{};
std::unordered_map<StmtNode*, StmtIndex> DesignExtractor::stmtNodeIndexMap{};

void DesignExtractor::processProgramNode(ProgramNode* programNode) {
	for (ProcedureNode* procedureNode : programNode->getProcedureNodes()) {
		processProcedureNode(procedureNode);
	}
}

void DesignExtractor::processProcedureNode(ProcedureNode* procedureNode) {
	ProcIndex procIndex = Entity::insertProc(procedureNode->getProcName());
	std::vector<StmtIndex> stmtIndices =
		processStmtLstNode(procedureNode->getStmtLstNode());
}

std::vector<StmtIndex> DesignExtractor::processStmtLstNode(
	StmtLstNode* stmtLstNode) {
	std::vector<StmtIndex> stmtIndices;
	StmtIndex prevIndex = 0;
	for (StmtNode* stmtNode : stmtLstNode->getStmtNodes()) {
		StmtIndex stmtIndex = processStmtNode(stmtNode, prevIndex);
		stmtIndices.push_back(stmtIndex);
		prevIndex = stmtIndex;
	}
	return stmtIndices;
}

/* TODO: refactor to adhere to SLAP and remove long method */
StmtIndex DesignExtractor::processStmtNode(StmtNode* stmtNode, StmtIndex prevIndex) {
	StmtIndex stmtIndex = insertStmt(stmtNode);
	stmtNodeIndexMap[stmtNode] = stmtIndex;
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
		UsesS::insert(stmtIndex, varIndex);
		switch (stmtNode->getStmtType()) {
		case StatementType::ifType:
			Pattern::insertIfInfo(stmtIndex, varIndex);
			break;
		case StatementType::whileType:
			Pattern::insertWhileInfo(stmtIndex, varIndex);
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

StmtIndex DesignExtractor::insertStmt(StmtNode* stmtNode) {
	StmtIndex stmtIndex;
	std::string nameValue;
	StatementType stmtType = stmtNode->getStmtType();

	switch (stmtType) {
	case StatementType::readType:
		nameValue = *stmtNode->getModifiesVars().begin();
		stmtIndex = Entity::insertStmt(stmtType, nameValue);
		break;
	case StatementType::printType:
		nameValue = *stmtNode->getUsesVars().begin();
		stmtIndex = Entity::insertStmt(stmtType, nameValue);
		break;
	case StatementType::callType:
		nameValue = stmtNode->getProcCalled();
		stmtIndex = Entity::insertStmt(stmtType, nameValue);
		break;
	default:
		stmtIndex = Entity::insertStmt(stmtType);
		break;
	}

	return stmtIndex;
}

void DesignExtractor::appendNode(CFGNode*& head, CFGNode*& tail, CFGNode* next) {
	if (head == nullptr) {
		head = next;
		tail = next;
	} else {
		tail->addNext(next);
		tail = next;
	}
}

void DesignExtractor::processCFGStmtIndices(std::unordered_set<StmtIndex>& stmtIndices, CFGNode*& head, CFGNode*& tail) {
	if (!stmtIndices.empty()) {
		CFGNode* next = new CFGNode(stmtIndices);
		appendNode(head, tail, next);
	}
}

CFG DesignExtractor::generateCFG(StmtLstNode* stmtLstNode) {
	CFGNode* head = nullptr;
	CFGNode* tail = nullptr;
	std::unordered_set<StmtIndex> stmtIndices;
	for (StmtNode* stmtNode : stmtLstNode->getStmtNodes()) {
		if (stmtNode->getStmtType() == StatementType::ifType) {
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
		} else if (stmtNode->getStmtType() == StatementType::whileType) {
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

void DesignExtractor::extract(SourceAST& ast) {
	stmtParentMap.clear();
	stmtFollowsMap.clear();

	processProgramNode(ast.getRoot());

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

std::unordered_map<StmtIndex, std::vector<StmtIndex>> DesignExtractor::getStmtParentMap() {
	return stmtParentMap;
}

std::unordered_map<StmtIndex, StmtIndex> DesignExtractor::getStmtFollowsMap() {
	return stmtFollowsMap;
}
