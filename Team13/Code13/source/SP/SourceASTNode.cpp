#include "SourceASTNode.h"

/* SourceASTNode */
SourceASTNode::SourceASTNode() {}
SourceASTNode::~SourceASTNode() {}

void SourceASTNode::process() {}
void SourceASTNode::process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) {}

/* StmtNode */
StmtNode::StmtNode(StmtIndex stmtIdx) : SourceASTNode(), stmtIdx(stmtIdx) {}

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

StmtIndex StmtNode::getStmtIdx() {
	return stmtIdx;
}

/* ReadNode */
ReadNode::ReadNode(std::string varName, StmtIndex stmtIdx) : StmtNode(stmtIdx), varName(varName) {}

void ReadNode::process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) {
	/* Modifies */
	relationshipMaps.modifiesMap[getStmtIdx()] = varName;
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
	/* Uses vars */
	relationshipMaps.usesMap[getStmtIdx()] = getUsesVars();
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

void AssignNode::process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) {
	StmtIndex stmtIndex = getStmtIdx();
	/* Modifies */
	relationshipMaps.modifiesMap[stmtIndex] = varName;

	/* Uses vars */
	std::unordered_set<std::string> usesVars = getUsesVars();
	if (!usesVars.empty()) {
		relationshipMaps.usesMap[stmtIndex] = usesVars;
	}

	/* Pattern */
	entityMaps.patternMap[stmtIndex] = getPattern();

	/* Const */
	std::unordered_set<std::string> usesConsts = getUsesConsts();
	entityMaps.constSet.insert(usesConsts.begin(), usesConsts.end());
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
ContainerNode::ContainerNode(ExprNode* condExpr, std::vector<StmtLstNode*> childStmtLst, StmtIndex stmtIdx) : StmtNode(stmtIdx), condExpr(condExpr), childStmtLst(childStmtLst) {}

void ContainerNode::process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) {
	StmtIndex stmtIndex = getStmtIdx();
	for (StmtLstNode* stmtLstNode : getChildStmtLst()) {
		stmtLstNode->process(relationshipMaps, entityMaps);

		/* Parent */
		std::vector<StmtIndex> childIndices = stmtLstNode->getDirectStmtNodeIndices();
		for (StmtIndex childIndex : childIndices) {
			relationshipMaps.parentChildMap[stmtIndex].push_back(childIndex);
		}
	}

	/* Uses */
	relationshipMaps.usesMap[stmtIndex] = getUsesVars();

	/* Pattern */
	entityMaps.patternMap[stmtIndex] = getPattern();

	/* Const */
	std::unordered_set<std::string> usesConsts = getUsesConsts();
	entityMaps.constSet.insert(usesConsts.begin(), usesConsts.end());
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
IfNode::IfNode(ExprNode* condExpr, StmtLstNode* thenStmtLst, StmtLstNode* elseStmtLst, StmtIndex stmtIdx) : ContainerNode(condExpr, { thenStmtLst, elseStmtLst }, stmtIdx) {}

StatementType IfNode::getStmtType() {
	return StatementType::IF_TYPE;
}

/* CallNode */
CallNode::CallNode(std::string procName, StmtIndex stmtIdx) : StmtNode(stmtIdx), procName(procName) {}

void CallNode::process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) {
	/* Calls */
	relationshipMaps.callStmtProcCalledMap[getStmtIdx()] = getProcCalled();
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

/* StmtLstNode */
StmtLstNode::StmtLstNode() : SourceASTNode() {}

void StmtLstNode::addStmtNode(StmtNode* stmtNode) {
	stmtNodes.push_back(stmtNode);
}

void StmtLstNode::process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) {
	StmtIndex prevIndex = 0;
	for (StmtNode* stmtNode : getStmtNodes()) {
		entityMaps.stmtNodes.push_back(stmtNode);

		stmtNode->process(relationshipMaps, entityMaps);

		/* StmtType */
		entityMaps.stmtTypeMap[stmtNode->getStmtIdx()] = stmtNode->getStmtType();

		/* Follows */
		StmtIndex currIndex = stmtNode->getStmtIdx();
		if (!(prevIndex == StmtIndex())) {
			relationshipMaps.followsMap[prevIndex] = currIndex;
		}
		prevIndex = currIndex;
	}
}

std::vector<StmtNode*> StmtLstNode::getStmtNodes() {
	return stmtNodes;
}

std::vector<StmtIndex> StmtLstNode::getDirectStmtNodeIndices() {
	std::vector<StmtIndex> stmtNodeIndices;
	for (StmtNode* stmtNode : getStmtNodes()) {
		stmtNodeIndices.push_back(stmtNode->getStmtIdx());
	}

	return stmtNodeIndices;
}

std::vector<StmtIndex> StmtLstNode::getAllStmtNodeIndices() {
	std::vector<StmtIndex> stmtNodeIndices;
	for (StmtNode* stmtNode : getStmtNodes()) {
		stmtNodeIndices.push_back(stmtNode->getStmtIdx());

		for (StmtLstNode* nestedStmtLstNode : stmtNode->getChildStmtLst()) {
			std::vector<StmtIndex> nestedIndices = nestedStmtLstNode->getAllStmtNodeIndices();
			stmtNodeIndices.insert(stmtNodeIndices.end(), nestedIndices.begin(), nestedIndices.end());
		}
	}

	return stmtNodeIndices;
}

/* ProcedureNode */
ProcedureNode::ProcedureNode(std::string procName) : SourceASTNode(), procName(procName) {
	stmtLstNode = new StmtLstNode();
}

void ProcedureNode::addStmtLst(StmtLstNode* stmtLstNode) {
	this->stmtLstNode = stmtLstNode;
}

void ProcedureNode::process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) {
	getStmtLstNode()->process(relationshipMaps, entityMaps);

	/* Bidirectional population of ProcStmtMap and StmtProcMap.
	Note that procIndex starts with 1. */
	ProcIndex procIndex = entityMaps.procStmtMap.size() + 1;
	std::vector<StmtIndex> stmtIndices = stmtLstNode->getAllStmtNodeIndices();
	for (StmtIndex stmtIndex : stmtIndices) {
		entityMaps.procStmtMap[procIndex].push_back(stmtIndex);
		entityMaps.stmtProcMap[stmtIndex] = procIndex;
	}

	/* ProcNameIndexMap */
	entityMaps.procNameIndexMap[procName] = procIndex++;

	/* ProcNames */
	entityMaps.procNames.push_back(procName);
}

StmtLstNode* ProcedureNode::getStmtLstNode() {
	return stmtLstNode;
}

std::string ProcedureNode::getProcName() {
	return procName;
}

/* ProgramNode */
ProgramNode::ProgramNode() : SourceASTNode() {}

void ProgramNode::addProcedure(ProcedureNode* procedureNode) {
	procedureNodes.push_back(procedureNode);
}

void ProgramNode::process() {
	for (ProcedureNode* procedureNode : getProcedureNodes()) {
		procedureNode->process(relationshipMaps, entityMaps);
	}
}

std::vector<ProcedureNode*> ProgramNode::getProcedureNodes() {
	return procedureNodes;
}

RelationshipMaps ProgramNode::getRelationshipMaps() {
	return relationshipMaps;
}

EntityMaps ProgramNode::getEntityMaps() {
	return entityMaps;
}
