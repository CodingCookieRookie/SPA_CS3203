#include "StmtNode.h"

StmtNode::StmtNode(StmtIndex stmtIdx) : SourceASTNode(), stmtIdx(stmtIdx) {}

StmtNode::~StmtNode() {}

std::unordered_set<std::string> StmtNode::getModifiesVars() {
	return std::unordered_set<std::string>();
}

std::unordered_set<std::string> StmtNode::getUsesVars() {
	return std::unordered_set<std::string>();
}

std::unordered_set<std::string> StmtNode::getUsesConsts() {
	return std::unordered_set<std::string>();
}

std::string StmtNode::getProcCalled() {
	return std::string();
}

std::unordered_set<std::string> StmtNode::getPattern() {
	return std::unordered_set<std::string>();
}

std::vector<StmtLstNode*> StmtNode::getChildStmtLst() {
	return std::vector<StmtLstNode*>();
}

std::unordered_set<std::string> StmtNode::getUses(ExprNode* expr, ExprNodeValueType valueType) {
	std::unordered_set<std::string> uses;
	std::stack<ExprNode*> stack;
	ExprNode* curr = expr;

	while (!stack.empty() || curr != nullptr) {
		/* Keeps traversing the left child as much as possible,
		else pops node from stack and traverses the right child. */
		if (curr != nullptr) {
			stack.push(curr);
			curr = curr->getLeftChild();
		} else {
			curr = stack.top();
			stack.pop();
			if (curr->getExprNodeValueType() == valueType) {
				uses.insert(curr->getValue());
			}
			curr = curr->getRightChild();
		}
	}

	return uses;
}

std::string StmtNode::getNameValue() {
	return std::string();
}

void StmtNode::process(StmtIndex prevIndex, RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) {
	populateStmtNodes(entityMaps.stmtNodes);

	process(relationshipMaps, entityMaps);

	/* StmtType */
	populateStmtType(entityMaps.stmtTypeMap);

	/* Follows */
	StmtIndex currIndex = getStmtIdx();
	if (!(prevIndex == StmtIndex())) {
		populateSameSynonymsRS(relationshipMaps.followsMap, prevIndex, { currIndex });
	}
}

StmtIndex StmtNode::getStmtIdx() {
	return stmtIdx;
}

void StmtNode::bidirectionalPopulateVarNameAndIndex(
	VarNameToIndexMap& varNameToIndexMap,
	SortedVarIndexToNameMap& sortedVarIndexToNameMap,
	std::unordered_set<std::string> varNames) {
	for (const std::string varName : varNames) {
		VarIndex varIndex = varNameToIndexMap.size() + 1;

		/* Only bidirectionally populate the maps for unique varName */
		if (varNameToIndexMap.count(varName) <= 0) {
			varNameToIndexMap[varName] = varIndex;
			sortedVarIndexToNameMap[varIndex] = varName;
		}
	}
}

void StmtNode::populateConsts(ConstSet& constSet) {
	std::unordered_set<std::string> consts = getUsesConsts();
	constSet.insert(consts.begin(), consts.end());
}

void StmtNode::populateStmtNodes(StmtNodes& stmtNodes) {
	stmtNodes.push_back(this);
}

void StmtNode::populatePattern(PatternMap& patternMap) {
	patternMap[getStmtIdx()] = getPattern();
}

void StmtNode::populateStmtType(StmtTypeMap& stmtTypeMap) {
	stmtTypeMap[getStmtIdx()] = getStmtType();
}

void StmtNode::populateDiffSynonymsRS(RelationshipMap& rsMap, EntityMaps& entityMaps, std::unordered_set<std::string>& varNames) {
	bidirectionalPopulateVarNameAndIndex(entityMaps.varNameToIndexMap, entityMaps.sortedVarIndexToNameMap, varNames);

	for (const std::string varName : varNames) {
		VarIndex varIndex = entityMaps.varNameToIndexMap[varName];
		rsMap[getStmtIdx()].insert(varIndex);
	}
}

void StmtNode::populateSameSynonymsRS(RelationshipMap& rsMap, SynonymIndex predecessor, std::unordered_set<SynonymIndex> successors) {
	rsMap[predecessor].insert(successors.begin(), successors.end());
}

/* ReadNode */
ReadNode::ReadNode(std::string varName, StmtIndex stmtIdx) : StmtNode(stmtIdx), varName(varName) {}

void ReadNode::process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) {
	/* Read stmt modifies the var it's reading */
	populateDiffSynonymsRS(relationshipMaps.modifiesMap, entityMaps, getModifiesVars());
}

StatementType ReadNode::getStmtType() {
	return StatementType::READ_TYPE;
}

std::unordered_set<std::string> ReadNode::getModifiesVars() {
	return { varName };
}

std::string ReadNode::getNameValue() {
	return varName;
}

/* PrintNode */
PrintNode::PrintNode(std::string varName, StmtIndex stmtIdx) : StmtNode(stmtIdx), varName(varName) {}

void PrintNode::process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) {
	/* Print stmt uses the var it's printing */
	populateDiffSynonymsRS(relationshipMaps.usesMap, entityMaps, getUsesVars());
}

StatementType PrintNode::getStmtType() {
	return StatementType::PRINT_TYPE;
}

std::unordered_set<std::string> PrintNode::getUsesVars() {
	return { varName };
}

std::string PrintNode::getNameValue() {
	return varName;
}

/* AssignNode */
AssignNode::AssignNode(std::string varName, ExprNode* expr, StmtIndex stmtIdx) : StmtNode(stmtIdx), varName(varName), expr(expr) {}

AssignNode::~AssignNode() {
	delete expr;
	expr = nullptr;
}

void AssignNode::process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) {
	/* Assign stmt modifies the var on its LHS */
	populateDiffSynonymsRS(relationshipMaps.modifiesMap, entityMaps, getModifiesVars());

	/* Assign stmt uses the var(s) on its RHS */
	std::unordered_set<std::string> usesVars = getUsesVars();
	if (!usesVars.empty()) {
		populateDiffSynonymsRS(relationshipMaps.usesMap, entityMaps, getUsesVars());
	}

	/* Pattern */
	populatePattern(entityMaps.patternMap);

	/* Const */
	populateConsts(entityMaps.constSet);
}

StatementType AssignNode::getStmtType() {
	return StatementType::ASSIGN_TYPE;
}

ExprNode* AssignNode::getExpr() {
	return expr;
}

std::unordered_set<std::string> AssignNode::getModifiesVars() {
	return { varName };
}

std::unordered_set<std::string> AssignNode::getPattern() {
	return { expr->getPattern() };
}

std::unordered_set<std::string> AssignNode::getUsesVars() {
	return getUses(getExpr(), ExprNodeValueType::VAR_NAME);
}

std::unordered_set<std::string> AssignNode::getUsesConsts() {
	return getUses(getExpr(), ExprNodeValueType::CONST_VALUE);
}

/* ContainerNode */
ContainerNode::ContainerNode(ExprNode* condExpr, std::vector<StmtLstNode*> childStmtLst, StmtIndex stmtIdx)
	: StmtNode(stmtIdx), condExpr(condExpr), childStmtLst(childStmtLst) {
}

ContainerNode::~ContainerNode() {
	delete condExpr;
	condExpr = nullptr;
	for (StmtLstNode*& childStmt : childStmtLst) {
		delete childStmt;
		childStmt = nullptr;
	}
}

void ContainerNode::process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) {
	StmtIndex stmtIndex = getStmtIdx();
	std::unordered_set<StmtIndex> childIndices;
	for (StmtLstNode* stmtLstNode : getChildStmtLst()) {
		stmtLstNode->process(relationshipMaps, entityMaps);
		std::unordered_set<StmtIndex> stmtLstNodeIndices = stmtLstNode->getDirectStmtNodeIndices();
		childIndices.insert(stmtLstNodeIndices.begin(), stmtLstNodeIndices.end());
	}

	/* Container stmt uses the var(s) in its condition expr */
	populateDiffSynonymsRS(relationshipMaps.usesMap, entityMaps, getUsesVars());

	/* Parent */
	populateSameSynonymsRS(relationshipMaps.parentChildMap, stmtIndex, childIndices);

	/* Pattern */
	populatePattern(entityMaps.patternMap);

	/* Const */
	populateConsts(entityMaps.constSet);
}

ExprNode* ContainerNode::getCondExpr() {
	return condExpr;
}

std::vector<StmtLstNode*> ContainerNode::getChildStmtLst() {
	return childStmtLst;
}

std::unordered_set<std::string> ContainerNode::getUsesVars() {
	return getUses(condExpr, ExprNodeValueType::VAR_NAME);
}

std::unordered_set<std::string> ContainerNode::getUsesConsts() {
	return getUses(condExpr, ExprNodeValueType::CONST_VALUE);
}

std::unordered_set<std::string> ContainerNode::getPattern() {
	return getUsesVars();
}

/* WhileNode */
WhileNode::WhileNode(ExprNode* condExpr, StmtLstNode* stmtLst, StmtIndex stmtIdx) : ContainerNode(condExpr, { stmtLst }, stmtIdx) {}

StatementType WhileNode::getStmtType() {
	return StatementType::WHILE_TYPE;
}

/* IfNode */
IfNode::IfNode(ExprNode* condExpr, StmtLstNode* thenStmtLst, StmtLstNode* elseStmtLst, StmtIndex stmtIdx)
	: ContainerNode(condExpr, { thenStmtLst, elseStmtLst }, stmtIdx) {
}

StatementType IfNode::getStmtType() {
	return StatementType::IF_TYPE;
}

/* CallNode */
CallNode::CallNode(std::string procName, StmtIndex stmtIdx) : StmtNode(stmtIdx), procName(procName) {}

void CallNode::process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) {
	/* Stores Calls relationship in the intermediate map.
	This is because the mapping of procName called to the procIndex called can only be done
	after all procedures in the source program have been processed. */
	relationshipMaps.callStmtToProcNameCalledMap[getStmtIdx()] = getProcCalled();
}

StatementType CallNode::getStmtType() {
	return StatementType::CALL_TYPE;
}

std::string CallNode::getProcCalled() {
	return procName;
}

std::string CallNode::getNameValue() {
	return procName;
}
