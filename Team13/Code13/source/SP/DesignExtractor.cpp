#include "DesignExtractor.h"

static const StmtIndex EXIT_NODE_IDX;

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

void DesignExtractor::insertStmt() {
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
	}
}

void DesignExtractor::insertVar() {
	SortedVarIndexToNameMap sortedVarIndexToNameMap = ast.getSortedVarIndexToNameMap();
	for (const auto& [varIndex, varName] : sortedVarIndexToNameMap) {
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, varName);
	}
}

void DesignExtractor::insertPattern() {
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

void DesignExtractor::insertConst() {
	ConstSet usesConsts = ast.getConstSet();
	for (const std::string& usesConst : usesConsts) {
		pkbInserter->insertConst(stoi(usesConst));
	}
}

void DesignExtractor::insertProc() {
	SortedProcIndexToNameMap sortedProcIndexToName = ast.getSortedProcIndexToNameMap();
	for (auto& [procIndex, procName] : sortedProcIndexToName) {
		pkbInserter->insertNameIdxEntity(EntityType::PROCEDURE, procName);
	}
}

void DesignExtractor::insertStmtFromProc() {
	ProcStmtMap procStmtMap = ast.getProcStmtMap();
	for (auto& procStmt : procStmtMap) {
		StmtIndex procIndex = procStmt.first;
		std::vector<StmtIndex> stmtIndices = procStmt.second;
		for (StmtIndex stmtIndex : stmtIndices) {
			pkbInserter->insertStmtFromProc(procIndex, stmtIndex);
		}
	}
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

		for (const auto& [predecessor, successors] : rsMap) {

			/* Populates direct relationship info */
			for (SynonymIndex successor : successors) {
				pkbInserter->insertRSInfo(rs, predecessor, successor);
			}

			/* Populates transitive relationship info */
			populateSameSynonymRST(predecessor, rs, rsT);
		}
	}
}

void DesignExtractor::populateSameSynonymRST(SynonymIndex predecessor, RelationshipType rs, RelationshipType rsT) {
	/* Terminates early if the info for curr procIndex has been added to PKB */
	std::unordered_set<StmtIndex>& populatedIndices = rsToPopulatedIndicesMap[rsT];
	if (populatedIndices.find(predecessor) != populatedIndices.end()) {
		return;
	}
	populatedIndices.insert(predecessor);

	RelationshipMap& rsMap = rsToASTMap[rs];
	RelationshipMap& rsTMap = rsToASTMap[rsT];

	std::unordered_set successors = rsMap[predecessor];
	for (SynonymIndex successor : successors) {
		pkbInserter->insertRSInfo(rsT, predecessor, successor);
		rsTMap[predecessor].insert(successor);
		populateSameSynonymRST(successor, rs, rsT);
		std::unordered_set<SynonymIndex> subsuccessors = rsTMap[successor];
		for (SynonymIndex subsuccessor : subsuccessors) {
			pkbInserter->insertRSInfo(rsT, predecessor, subsuccessor);
			rsTMap[predecessor].insert(subsuccessor);
		}
	}
}

void DesignExtractor::generateCFG(StmtLstNode* stmtLstNode) {
	std::vector<StmtNode*> stmtNodes = stmtLstNode->getStmtNodes();
	for (size_t i = 0; i < stmtNodes.size(); i++) {
		StmtNode* currNode = stmtNodes.at(i);
		StmtIndex currStmtIdx = currNode->getStmtIdx();
		StmtIndex nextStmtIdx = i == stmtNodes.size() - 1 ? EXIT_NODE_IDX : stmtNodes.at(i + 1)->getStmtIdx();
		generateCFGFromStmt(currNode, currStmtIdx, nextStmtIdx);
	}
}

void DesignExtractor::generateCFGFromStmt(StmtNode* currNode, StmtIndex currStmtIdx, StmtIndex nextStmtIdx) {
	StatementType stmtType = currNode->getStmtType();
	switch (stmtType) {
	case StatementType::IF_TYPE:
		generateCFGfromIfStmt(currNode, currStmtIdx, nextStmtIdx);
		break;
	case StatementType::WHILE_TYPE:
		generateCFGfromWhileStmt(currNode, currStmtIdx, nextStmtIdx);
		break;
	default:
		if (nextStmtIdx != EXIT_NODE_IDX) {
			cfg->insert(currStmtIdx, nextStmtIdx);
		}
		break;
	}
}

void DesignExtractor::generateCFGfromIfStmt(StmtNode* currNode, StmtIndex currStmtIdx, StmtIndex nextStmtIdx) {
	/* Process for then statement list and else statement list respectively */
	for (size_t i = 0; i <= 1; i++) {
		StmtLstNode* childStmtLst = currNode->getChildStmtLst()[i];
		StmtNode* firstChildStmtNode = childStmtLst->getStmtNodes().at(0);
		StmtIndex firstChildStmtIdx = firstChildStmtNode->getStmtIdx();
		cfg->insert(currStmtIdx, firstChildStmtIdx);
		generateCFG(childStmtLst);
		StmtNode* lastChildStmtNode = childStmtLst->getStmtNodes().at(childStmtLst->getStmtNodes().size() - 1);
		StmtIndex lastChildStmtIdx = lastChildStmtNode->getStmtIdx();
		generateCFGFromStmt(lastChildStmtNode, lastChildStmtIdx, nextStmtIdx);
	}
}

void DesignExtractor::generateCFGfromWhileStmt(StmtNode* currNode, StmtIndex currStmtIdx, StmtIndex nextStmtIdx) {
	if (nextStmtIdx != EXIT_NODE_IDX) {
		cfg->insert(currStmtIdx, nextStmtIdx);
	}
	StmtLstNode* childStmtLst = currNode->getChildStmtLst()[0];
	StmtNode* firstChildStmtNode = childStmtLst->getStmtNodes().at(0);
	StmtIndex firstChildStmtIdx = firstChildStmtNode->getStmtIdx();
	cfg->insert(currStmtIdx, firstChildStmtIdx);
	generateCFG(childStmtLst);
	StmtNode* lastChildStmtNode = childStmtLst->getStmtNodes().at(childStmtLst->getStmtNodes().size() - 1);
	StmtIndex lastChildStmtIdx = lastChildStmtNode->getStmtIdx();
	generateCFGFromStmt(lastChildStmtNode, lastChildStmtIdx, currStmtIdx);
}

void DesignExtractor::insertNext() {
	std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> cfgTable = cfg->getCFGTable();
	for (auto& [keyStmtIdx, nextStmtIndices] : cfgTable) {
		for (StmtIndex nextStmtIdx : nextStmtIndices) {
			pkbInserter->insertRSInfo(RelationshipType::NEXT, keyStmtIdx, nextStmtIdx);
		}
	}
}

void DesignExtractor::processCFGs(ProgramNode* programNode) {
	for (ProcedureNode* procedureNode : programNode->getProcedureNodes()) {
		StmtLstNode* stmtLstNode = procedureNode->getStmtLstNode();
		generateCFG(stmtLstNode);
	}
	insertNext();
}

std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> DesignExtractor::getCFG() {
	return cfg->getCFGTable();
}

void DesignExtractor::extract() {
	/* Populates entities */
	insertStmt();
	insertVar();
	insertPattern();
	insertConst();
	insertProc();
	insertStmtFromProc();

	/* Populate relationships */
	populateDiffSynonymsRSInfo();
	populateSameSynonymsRSInfo();

	/* Construct CFGs and populates Next relationship */
	processCFGs(ast.getRoot());
}
