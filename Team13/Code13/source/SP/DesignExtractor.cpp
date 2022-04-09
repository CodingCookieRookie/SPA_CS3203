#include "DesignExtractor.h"

static const int EXIT_NODE_IDX;

DesignExtractor::DesignExtractor(SourceAST& ast, PKBInserter* pkbInserter) : ast(ast), pkbInserter(pkbInserter) {
	this->cfg = new CFG();

	ast.process();

	rsToASTMap = {
		{RelationshipType::USES_S, ast.getUsesMap()},
		{RelationshipType::MODIFIES_S, ast.getModifiesMap()},
		{RelationshipType::FOLLOWS, ast.getFollowsMap()},
		{RelationshipType::PARENT, ast.getParentChildMap()},
		{RelationshipType::CALLS, ast.getProcIndexCallerToProcIndexCalledMap()}
	};
}

DesignExtractor::~DesignExtractor() {}

std::unordered_map<RelationshipType, RelationshipType> DesignExtractor::rsStoPMap = {
		{RelationshipType::USES_S, RelationshipType::USES_P},
		{RelationshipType::MODIFIES_S, RelationshipType::MODIFIES_P},
};

std::unordered_map<RelationshipType, RelationshipType> DesignExtractor::rsPtoSMap = {
	{RelationshipType::USES_P, RelationshipType::USES_S},
	{RelationshipType::MODIFIES_P, RelationshipType::MODIFIES_S},
};

std::unordered_map<RelationshipType, RelationshipType> DesignExtractor::rsToRsTMap = {
	{RelationshipType::FOLLOWS, RelationshipType::FOLLOWS_T},
	{RelationshipType::PARENT, RelationshipType::PARENT_T},
	{RelationshipType::CALLS, RelationshipType::CALLS_T},
};

void DesignExtractor::insertNext(PKBInserter* pkbInserter) {
	for (auto cfgEntry : cfg->getCFGTable()) {
		int keyStmtIdx = cfgEntry.first;
		for (int nextStmtIdx : cfgEntry.second) {
			pkbInserter->insertRSInfo(RelationshipType::NEXT, keyStmtIdx, nextStmtIdx);
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
			stmtIndex = pkbInserter->insertStmt(stmtType);
		} else {
			stmtIndex = pkbInserter->insertStmt(stmtType, nameValue);
		}
		stmtNodeIndexMap[stmtNode] = stmtIndex;
	}
}

void DesignExtractor::insertVar(SourceAST& ast) {
	SortedVarIndexToNameMap sortedVarIndexToNameMap = ast.getSortedVarIndexToNameMap();
	for (const auto& [varIndex, varName] : sortedVarIndexToNameMap) {
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, varName);
	}
}

void DesignExtractor::insertPattern(SourceAST& ast) {
	PatternMap patternMap = ast.getPatternMap();
	StmtTypeMap stmtTypeMap = ast.getStmtTypeMap();
	VarNameToIndexMap varNameToIndexMap = ast.getVarNameToIndexMap();

	for (auto& pattern : patternMap) {
		StmtIndex stmtIndex = pattern.first;
		std::unordered_set<std::string> varNames = pattern.second;
		for (const std::string& varName : varNames) {
			StatementType stmtType = stmtTypeMap[stmtIndex];
			switch (stmtType) {
				VarIndex varIndex;
			case StatementType::IF_TYPE:
			case StatementType::WHILE_TYPE:
				varIndex = varNameToIndexMap[varName];
				pkbInserter->insertPatternContainer(stmtType, stmtIndex, varIndex);
				break;
			case StatementType::ASSIGN_TYPE:
				std::string rhs = varName;
				VarIndex lhsIndex = *ast.getModifiesMap()[stmtIndex].begin();
				pkbInserter->insertAssignInfo(lhsIndex, rhs, stmtIndex);
				break;
			}
		}
	}
}

void DesignExtractor::insertConst(SourceAST& ast) {
	ConstSet usesConsts = ast.getConstSet();
	for (const std::string& usesConst : usesConsts) {
		pkbInserter->insertConst(stoi(usesConst));
	}
}

void DesignExtractor::insertProc(SourceAST& ast) {
	SortedProcIndexToNameMap sortedProcIndexToName = ast.getSortedProcIndexToNameMap();
	for (auto& [procIndex, procName] : sortedProcIndexToName) {
		pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, procName);
	}
}

void DesignExtractor::insertStmtFromProc(SourceAST& ast) {
	ProcStmtMap procStmtMap = ast.getProcStmtMap();
	for (auto& procStmt : procStmtMap) {
		StmtIndex procIndex = procStmt.first;
		std::vector<StmtIndex> stmtIndices = procStmt.second;
		for (StmtIndex stmtIndex : stmtIndices) {
			pkbInserter->insertStmtFromProc(procIndex, stmtIndex);
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
	StatementType stmtType = currNode->getStmtType();
	switch (stmtType) {
	case StatementType::IF_TYPE:
		generateCFGfromIfStmt(currNode, pkb, stmtNodeIndexMap, currStmtIdx, nextStmtIdx);
		break;
	case StatementType::WHILE_TYPE:
		generateCFGfromWhileStmt(currNode, pkb, stmtNodeIndexMap, currStmtIdx, nextStmtIdx);
		break;
	default:
		if (nextStmtIdx != EXIT_NODE_IDX) {
			cfg->insert(currStmtIdx, nextStmtIdx);
		}
		break;
	}
}

void DesignExtractor::generateCFGfromIfStmt(
	StmtNode* currNode,
	PKBInserter* pkb,
	std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap,
	int currStmtIdx,
	int nextStmtIdx) {
	/* Process for then statement list and else statement list respectively */
	for (size_t i = 0; i <= 1; i++) {
		StmtLstNode* childStmtLst = currNode->getChildStmtLst()[i];
		StmtNode* firstChildStmtNode = childStmtLst->getStmtNodes().at(0);
		int firstChildStmtIdx = stmtNodeIndexMap[firstChildStmtNode];
		cfg->insert(currStmtIdx, firstChildStmtIdx);
		generateCFG(childStmtLst, pkb, stmtNodeIndexMap);
		StmtNode* lastChildStmtNode = childStmtLst->getStmtNodes().at(childStmtLst->getStmtNodes().size() - 1);
		int lastChildStmtIdx = stmtNodeIndexMap[lastChildStmtNode];
		generateCFGFromStmt(lastChildStmtNode, pkb, stmtNodeIndexMap, lastChildStmtIdx, nextStmtIdx);
	}
}

void DesignExtractor::generateCFGfromWhileStmt(
	StmtNode* currNode,
	PKBInserter* pkb,
	std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap,
	int currStmtIdx,
	int nextStmtIdx) {
	if (nextStmtIdx != EXIT_NODE_IDX) {
		cfg->insert(currStmtIdx, nextStmtIdx);
	}
	StmtLstNode* childStmtLst = currNode->getChildStmtLst()[0];
	StmtNode* firstChildStmtNode = childStmtLst->getStmtNodes().at(0);
	int firstChildStmtIdx = stmtNodeIndexMap[firstChildStmtNode];
	cfg->insert(currStmtIdx, firstChildStmtIdx);
	generateCFG(childStmtLst, pkb, stmtNodeIndexMap);
	StmtNode* lastChildStmtNode = childStmtLst->getStmtNodes().at(childStmtLst->getStmtNodes().size() - 1);
	int lastChildStmtIdx = stmtNodeIndexMap[lastChildStmtNode];
	generateCFGFromStmt(lastChildStmtNode, pkb, stmtNodeIndexMap, lastChildStmtIdx, currStmtIdx);
}

void DesignExtractor::processCFGs(
	ProgramNode* programNode,
	PKBInserter* pkbInserter,
	std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap) {
	for (ProcedureNode* procedureNode : programNode->getProcedureNodes()) {
		StmtLstNode* stmtLstNode = procedureNode->getStmtLstNode();
		generateCFG(stmtLstNode, pkbInserter, stmtNodeIndexMap);
	}
	insertNext(pkbInserter);
}

std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> DesignExtractor::getCFG() {
	return cfg->getCFGTable();
}

void DesignExtractor::populateDiffSynonymsRSInfo() {
	for (const auto& [rsTypeS, rsTypeP] : rsStoPMap) {
		for (const auto& [procIndex, stmtIndex] : ast.getProcStmtMap()) {
			populateDiffSynonymsRSProcInfo(rsTypeP, procIndex);
		}
	}
}

void DesignExtractor::populateDiffSynonymsRSProcInfo(RelationshipType rsTypeP, ProcIndex procIndex) {
	/* Terminates early if the info for curr procIndex has been added to PKB */
	std::unordered_set<ProcIndex>& populatedIndices = rsToPopulatedIndicesMap[rsTypeP];
	if (populatedIndices.find(procIndex) != populatedIndices.end()) {
		return;
	}

	/* Marks curr procIndex as populated */
	populatedIndices.insert(procIndex);

	/* For every stmt of curr proc, get the vars it uses/ modifies,
	as this means that the curr proc also uses/ modifies these vars */
	ProcStmtMap& procStmtMap = ast.getProcStmtMap();
	RelationshipType& rsTypeS = rsPtoSMap[rsTypeP];

	RelationshipMap& rsSMap = rsToASTMap[rsTypeS];
	RelationshipMap& rsPMap = rsToASTMap[rsTypeP];

	for (StmtIndex stmtIndex : procStmtMap[procIndex]) {
		populateDiffSynonymsRSStmtInfo(rsTypeS, stmtIndex);

		/* Iterate all vars used/ modified by curr stmtIndex to populate the transitive table */
		std::unordered_set<VarIndex> varIndices = rsSMap[stmtIndex];
		for (VarIndex varIndex : varIndices) {
			pkbInserter->insertRSInfo(rsTypeP, procIndex, varIndex);
			rsPMap[procIndex].insert(varIndex);
		}
	}
}

void DesignExtractor::populateDiffSynonymsRSStmtInfo(RelationshipType rsTypeS, StmtIndex stmtIndex) {
	/* Terminates early if the info for curr stmtIndex has been added to PKB */
	std::unordered_set<StmtIndex>& populatedIndices = rsToPopulatedIndicesMap[rsTypeS];
	if (populatedIndices.find(stmtIndex) != populatedIndices.end()) {
		return;
	}

	/* Mark curr stmtIndex as populated */
	populatedIndices.insert(stmtIndex);

	/* Populates PKB direct relationship table for curr stmtIndex */
	RelationshipMap& rsSMap = rsToASTMap[rsTypeS];
	std::unordered_set<VarIndex> directVarIndices = rsSMap[stmtIndex];
	for (VarIndex directVarIndex : directVarIndices) {
		pkbInserter->insertRSInfo(rsTypeS, stmtIndex, directVarIndex);
	}

	/* Recursively populates container info */
	RelationshipMap& parentChildMap = ast.getParentChildMap();
	for (StmtIndex childStmtIndex : parentChildMap[stmtIndex]) {
		populateDiffSynonymsRSStmtInfo(rsTypeS, childStmtIndex);

		std::unordered_set<VarIndex> childVarIndices = rsSMap[childStmtIndex];
		for (VarIndex childVarIndex : childVarIndices) {
			pkbInserter->insertRSInfo(rsTypeS, stmtIndex, childVarIndex);
			rsSMap[stmtIndex].insert(childVarIndex);
		}
	}

	/* A call stmt also modifies/ uses vars in that are modified/ used in the proc called */
	RelationshipMap& callStmtToProcIndexCalledMap = ast.getCallStmtToProcIndexCalledMap();
	for (ProcIndex calledProcIndex : callStmtToProcIndexCalledMap[stmtIndex]) {
		RelationshipType rsTypeP = rsStoPMap[rsTypeS];
		populateDiffSynonymsRSProcInfo(rsTypeP, calledProcIndex);

		RelationshipMap& rsPMap = rsToASTMap[rsTypeP];
		/* Iterate all vars used/ modified by called procedure */
		std::unordered_set<VarIndex> varIndicesFromCalledProc = rsPMap[calledProcIndex];
		for (VarIndex varIndex : varIndicesFromCalledProc) {
			pkbInserter->insertRSInfo(rsTypeS, stmtIndex, varIndex);
			rsSMap[stmtIndex].insert(varIndex);
		}
	}
}

void DesignExtractor::populateSameSynonymsRSInfo() {
	/* For every SameSynonymsRS, populates its direct and transitive relationships */
	for (const auto& [rs, rsT] : rsToRsTMap) {
		RelationshipMap& rsMap = rsToASTMap[rs];
		RelationshipMap& rsTMap = rsToASTMap[rsT];

		for (auto& [predecessor, successors] : rsMap) {
			/* Populates direct relationship info */
			for (SynonymIndex successor : successors) {
				pkbInserter->insertRSInfo(rs, predecessor, successor);
			}

			/* Populates transitive relationship info */
			std::unordered_set<SynonymIndex> allSuccessors = getAllSuccessors(predecessor, rsMap, rsTMap);
			for (SynonymIndex successor : allSuccessors) {
				pkbInserter->insertRSInfo(rsT, predecessor, successor);
			}
		}
	}
}

std::unordered_set<SynonymIndex> DesignExtractor::getAllSuccessors(SynonymIndex predecessor, RelationshipMap& rsMap, RelationshipMap& rsTMap) {
	/* Early termination if successors (for the curr predecessor) are already populated in the transitive table */
	if (rsTMap.find(predecessor) != rsTMap.end()) {
		return rsTMap[predecessor];
	}

	std::unordered_set<SynonymIndex> successors = rsMap[predecessor];
	for (SynonymIndex successor : successors) {
		std::unordered_set<SynonymIndex> grandSuccessors = getAllSuccessors(successor, rsMap, rsTMap);
		successors.insert(grandSuccessors.begin(), grandSuccessors.end());
	}

	rsTMap[predecessor] = successors;

	return successors;
}

void DesignExtractor::extract() {
	/* Maps a StmtNode to the StmtIndex of the statement */
	std::unordered_map<StmtNode*, StmtIndex> stmtNodeIndexMap;

	/* Populates entities */
	insertStmt(ast, stmtNodeIndexMap); /* TODO: remove stmtNodeIndexMap, CFG can use entityMap.stmtNodes */
	insertVar(ast);
	insertPattern(ast);
	insertConst(ast);
	insertProc(ast);
	insertStmtFromProc(ast);

	/* Populate relationships */
	populateDiffSynonymsRSInfo();
	populateSameSynonymsRSInfo();

	/* Construct CFGs and populates Next relationship */
	processCFGs(ast.getRoot(), pkbInserter, stmtNodeIndexMap);
}
