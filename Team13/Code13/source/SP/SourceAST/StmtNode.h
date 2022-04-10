#pragma once

#include <stack>

#include "SourceASTNode.h"
#include "StmtLstNode.h"

class StmtNode : public SourceASTNode {
private:
	StmtIndex stmtIdx;
protected:
	/* Returns the unordered set of vars or consts used by the StmtNode.
	Set to protected so it can be accessed by the relevant StmtNode's subclasses
	which uses vars and/ or consts. */
	std::unordered_set<std::string> getUses(ExprNode* expr, ExprNodeValueType valueType);
public:
	StmtNode(StmtIndex stmtIdx);
	virtual ~StmtNode();

	virtual StatementType getStmtType() = 0;
	virtual std::unordered_set<std::string> getPattern();
	virtual std::vector<StmtLstNode*> getChildStmtLst();

	/* Returns the unordered set of vars used by the StmtNode */
	virtual std::unordered_set<std::string> getUsesVars();

	/* Returns the unordered set of vars modified by the StmtNode */
	virtual std::unordered_set<std::string> getModifiesVars();

	/* Returns the unordered set of consts used by the StmtNode */
	virtual std::unordered_set<std::string> getUsesConsts();

	/* Returns the procedure name called by the StmtNode */
	virtual std::string getProcCalled();

	/* Returns the name value attribute of a StmtNode */
	virtual std::string getNameValue();

	virtual void process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) = 0;
	void process(StmtIndex prevIndex, RelationshipMaps& relationshipMaps, EntityMaps& entityMaps);

	StmtIndex getStmtIdx();

	/* Populates entities */
	void bidirectionalPopulateVarNameAndIndex(
		VarNameToIndexMap& varNameToIndexMap,
		SortedVarIndexToNameMap& sortedVarIndexToNameMap,
		std::unordered_set<std::string> varNames);
	void populateConsts(ConstSet& constSet);
	void populatePattern(PatternMap& patternMap);
	void populateStmtType(StmtTypeMap& stmtTypeMap);
	void populateStmtNodes(StmtNodes& stmtNode);

	/* Populates Uses and Modifies */
	void populateDiffSynonymsRS(RelationshipMap& rsMap, EntityMaps& entityMaps, std::unordered_set<std::string>& varNames);

	/* Populates Follows, Parent, and Calls */
	void populateSameSynonymsRS(RelationshipMap& rsMap, SynonymIndex predecessor, std::unordered_set<SynonymIndex> successors);
};

class ReadNode : public StmtNode {
private:
	std::string varName;
public:
	ReadNode(std::string varName, StmtIndex stmtIdx);
	void process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) override;

	StatementType getStmtType() override;
	std::unordered_set<std::string> getModifiesVars() override;
	std::string getNameValue() override;
};

class PrintNode : public StmtNode {
private:
	std::string varName;
public:
	PrintNode(std::string varName, StmtIndex stmtIdx);
	void process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) override;

	StatementType getStmtType() override;
	std::unordered_set<std::string> getUsesVars() override;
	std::string getNameValue() override;
};

class AssignNode : public StmtNode {
private:
	std::string varName;
	ExprNode* expr;
	std::unordered_set<std::string> usesVars;
	std::unordered_set<std::string> usesConsts;
	std::string pattern;
public:
	AssignNode(std::string varName, ExprNode* expr, StmtIndex stmtIdx);
	virtual ~AssignNode();
	void process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) override;

	ExprNode* getExpr();
	StatementType getStmtType() override;
	std::unordered_set<std::string> getUsesVars() override;
	std::unordered_set<std::string> getModifiesVars() override;
	std::unordered_set<std::string> getUsesConsts() override;
	std::unordered_set<std::string> getPattern() override;
};

class ContainerNode : public StmtNode {
protected:
	ExprNode* condExpr;
	std::vector<StmtLstNode*> childStmtLst;
public:
	ContainerNode(ExprNode* condExpr, std::vector<StmtLstNode*> childStmtLst, StmtIndex stmtIdx);
	virtual ~ContainerNode();
	void process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) override;

	ExprNode* getCondExpr();
	std::unordered_set<std::string> getUsesVars() override;
	std::unordered_set<std::string> getUsesConsts() override;
	std::vector<StmtLstNode*> getChildStmtLst() override;
	std::unordered_set<std::string> getPattern() override;
};

class WhileNode : public ContainerNode {
public:
	WhileNode(ExprNode* condExpr, StmtLstNode* stmtLst, StmtIndex stmtIdx);
	StatementType getStmtType() override;
};

class IfNode : public ContainerNode {
public:
	IfNode(ExprNode* condExpr, StmtLstNode* thenStmtLst, StmtLstNode* elseStmtLst, StmtIndex stmtIdx);
	StatementType getStmtType() override;
};

class CallNode : public StmtNode {
private:
	std::string procName;
public:
	CallNode(std::string procName, StmtIndex stmtIdx);
	void process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) override;

	StatementType getStmtType() override;
	std::string getProcCalled() override;
	std::string getNameValue() override;
};
