#include "DesignExtractor.h"

static const int EXIT_NODE_IDX;

void DesignExtractor::insertFollows(SourceAST& ast) {
	FollowsMap followsMap = ast.getFollowsMap();
	for (auto& follows : followsMap) {
		StmtIndex prevIndex = follows.first;
		StmtIndex stmtIndex = follows.second;
		Follows::insert(prevIndex, stmtIndex);
	}
}

void DesignExtractor::insertModifies(SourceAST& ast) {
	ModifiesMap modifiesMap = ast.getModifiesMap();
	for (auto& modifies : modifiesMap) {
		StmtIndex stmtIndex = modifies.first;
		std::string varName = modifies.second;
		VarIndex varIndex = Entity::insertVar(varName);
		ModifiesS::insert(stmtIndex, varIndex);
	}
}

void DesignExtractor::insertUses(SourceAST& ast) {
	UsesMap usesMap = ast.getUsesMap();
	for (auto& uses : usesMap) {
		StmtIndex stmtIndex = uses.first;
		std::unordered_set<std::string> varNames = uses.second;
		for (const std::string& varName : varNames) {
			VarIndex varIndex = Entity::insertVar(varName);
			UsesS::insert(stmtIndex, varIndex);
		}
	}
}

void DesignExtractor::insertParent(SourceAST& ast) {
	ParentChildMap parentChildMap = ast.getParentChildMap();
	for (auto& parentChild : parentChildMap) {
		StmtIndex parentIndex = parentChild.first;
		std::vector<StmtIndex> childIndices = parentChild.second;
		for (StmtIndex childIndex : childIndices) {
			Parent::insert(parentIndex, childIndex);
			Container::insertStmtInContainer(parentIndex, childIndex);
		}
	}
}

void DesignExtractor::insertCalls(SourceAST& ast) {
	CallStmtProcCalledMap callStmtProcCalledMap = ast.getCallStmtProcCalledMap();
	StmtProcMap stmtProcMap = ast.getStmtProcMap();
	ProcNameIndexMap procNameIndexMap = ast.getProcNameIndexMap();

	/* For every call stmt, get its own procIndex and the procIndex of the proc it's calling */
	for (auto& callStmtProcCalled : callStmtProcCalledMap) {
		StmtIndex callStmtIndex = callStmtProcCalled.first;
		std::string calledProc = callStmtProcCalled.second;
		ProcIndex predIndex = stmtProcMap[callStmtIndex];
		ProcIndex succIndex = procNameIndexMap[calledProc];
		Calls::insert(predIndex, succIndex);
	}
}

void DesignExtractor::insertNext(PKBInserter* pkb) {
	for (auto cfgEntry : pkb->getCFGTable()) {
		int keyStmtIdx = cfgEntry.first;
		for (int nextStmtIdx : cfgEntry.second) {
			Next::insert(keyStmtIdx, nextStmtIdx);
		}
	}
}

void DesignExtractor::insertStmt(SourceAST& ast, std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap) {
	StmtNodes stmtNodes = ast.getStmtNodes();
	for (auto& stmtNode : stmtNodes) {
		StatementType stmtType = stmtNode->getStmtType();
		std::string nameValue = stmtNode->getNameValue();
		StmtIndex stmtIndex;
		if (nameValue.empty()) {
			stmtIndex = Entity::insertStmt(stmtType);
		} else {
			stmtIndex = Entity::insertStmt(stmtType, nameValue);
		}
		stmtNodeIndexMap[stmtNode] = stmtIndex;
	}
}

void DesignExtractor::insertPattern(SourceAST& ast) {
	PatternMap patternMap = ast.getPatternMap();
	StmtTypeMap stmtTypeMap = ast.getStmtTypeMap();

	for (auto& pattern : patternMap) {
		StmtIndex stmtIndex = pattern.first;
		std::unordered_set<std::string> varNames = pattern.second;
		for (const std::string& varName : varNames) {
			switch (stmtTypeMap[stmtIndex]) {
				VarIndex varIndex;
			case StatementType::IF_TYPE:
				varIndex = Entity::insertVar(varName);
				Pattern::insertIfInfo(stmtIndex, varIndex);
				break;
			case StatementType::WHILE_TYPE:
				varIndex = Entity::insertVar(varName);
				Pattern::insertWhileInfo(stmtIndex, varIndex);
				break;
			case StatementType::ASSIGN_TYPE:
				std::string rhs = varName;
				std::string lhs = ast.getModifiesMap()[stmtIndex];
				VarIndex lhsIndex = Entity::insertVar(lhs);
				Pattern::insertAssignInfo(lhsIndex, rhs, stmtIndex);
				break;
			}
		}
	}
}

void DesignExtractor::insertConst(SourceAST& ast) {
	ConstSet usesConsts = ast.getConstSet();
	for (const std::string& usesConst : usesConsts) {
		Entity::insertConst(stoi(usesConst));
	}
}

void DesignExtractor::insertProc(SourceAST& ast) {
	ProcNames procNames = ast.getProcNames();
	for (auto& procName : procNames) {
		Entity::insertProc(procName);
	}
}

void DesignExtractor::insertStmtFromProc(SourceAST& ast) {
	ProcStmtMap procStmtMap = ast.getProcStmtMap();
	for (auto& procStmt : procStmtMap) {
		StmtIndex procIndex = procStmt.first;
		std::vector<StmtIndex> stmtIndices = procStmt.second;
		for (StmtIndex stmtIndex : stmtIndices) {
			Entity::insertStmtFromProc(procIndex, stmtIndex);
		}
	}
}

void DesignExtractor::generateCFG(
	StmtLstNode* stmtLstNode,
	PKBInserter* pkb,
	std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap) {
	std::vector<StmtNode*> stmtNodes = stmtLstNode->getStmtNodes();
	for (size_t i = 0; i < stmtNodes.size(); i++) {
		StmtNode* currNode = stmtNodes.at(i);
		int currStmtIdx = stmtNodeIndexMap[currNode];
		int nextStmtIdx = i == stmtNodes.size() - 1 ? EXIT_NODE_IDX : stmtNodeIndexMap[stmtNodes.at(i + 1)];
		generateCFGFromStmt(currNode, pkb, stmtNodeIndexMap, currStmtIdx, nextStmtIdx);
	}
}

void DesignExtractor::generateCFGFromStmt(
	StmtNode* currNode,
	PKBInserter* pkb,
	std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap,
	int currStmtIdx,
	int nextStmtIdx) {
	if (currNode->getStmtType() == StatementType::IF_TYPE) {
		/* Process for then statement list and else statement list respectively */
		for (size_t i = 0; i <= 1; i++) {
			StmtLstNode* childStmtLst = currNode->getChildStmtLst()[i];
			StmtNode* firstChildStmtNode = childStmtLst->getStmtNodes().at(0);
			int firstChildStmtIdx = stmtNodeIndexMap[firstChildStmtNode];

			pkb->insertToCFG(currStmtIdx, firstChildStmtIdx);

			generateCFG(childStmtLst, pkb, stmtNodeIndexMap);

			StmtNode* lastChildStmtNode = childStmtLst->getStmtNodes().at(childStmtLst->getStmtNodes().size() - 1);
			int lastChildStmtIdx = stmtNodeIndexMap[lastChildStmtNode];

			generateCFGFromStmt(lastChildStmtNode, pkb, stmtNodeIndexMap, lastChildStmtIdx, nextStmtIdx);
		}
	} else if (currNode->getStmtType() == StatementType::WHILE_TYPE) {
		if (nextStmtIdx != EXIT_NODE_IDX) {
			pkb->insertToCFG(currStmtIdx, nextStmtIdx);
		}

		StmtLstNode* childStmtLst = currNode->getChildStmtLst()[0];
		StmtNode* firstChildStmtNode = childStmtLst->getStmtNodes().at(0);
		int firstChildStmtIdx = stmtNodeIndexMap[firstChildStmtNode];

		pkb->insertToCFG(currStmtIdx, firstChildStmtIdx);

		generateCFG(childStmtLst, pkb, stmtNodeIndexMap);

		StmtNode* lastChildStmtNode = childStmtLst->getStmtNodes().at(childStmtLst->getStmtNodes().size() - 1);
		int lastChildStmtIdx = stmtNodeIndexMap[lastChildStmtNode];

		generateCFGFromStmt(lastChildStmtNode, pkb, stmtNodeIndexMap, lastChildStmtIdx, currStmtIdx);
	} else {
		if (nextStmtIdx != EXIT_NODE_IDX) {
			pkb->insertToCFG(currStmtIdx, nextStmtIdx);
		}
	}
}

void DesignExtractor::processCFGs(
	ProgramNode* programNode,
	PKBInserter* pkb,
	std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap) {
	for (ProcedureNode* procedureNode : programNode->getProcedureNodes()) {
		StmtLstNode* stmtLstNode = procedureNode->getStmtLstNode();
		generateCFG(stmtLstNode, pkb, stmtNodeIndexMap);
	}
	insertNext(pkb);
}

void DesignExtractor::extract(SourceAST& ast, PKBInserter* pkb) {
	ast.process();

	/* Populates relationships */
	insertFollows(ast);
	insertModifies(ast);
	insertUses(ast);
	insertParent(ast);
	insertCalls(ast);

	/* Maps a StmtNode to the StmtIndex of the statement */
	std::unordered_map<StmtNode*, StmtIndex> stmtNodeIndexMap;

	/* Populates entities */
	insertStmt(ast, stmtNodeIndexMap);
	insertPattern(ast);
	insertConst(ast);
	insertProc(ast);
	insertStmtFromProc(ast);

	TransitivePopulator::populateRecursiveInfo();

	/* Construct CFGs and populates Next relationship */
	processCFGs(ast.getRoot(), pkb, stmtNodeIndexMap);
}
