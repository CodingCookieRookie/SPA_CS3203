#pragma once

#include <stack>
#include <string>
#include <unordered_set>
#include <vector>

#include "../Common/ExprNode.h"
#include "../Common/Types.h"
#include "RsEntitiesMapsTypes.h"

/* Forward definition for compatibility */
class StmtLstNode;
class ExprNode;
class StmtNode;

/* A vector of the ast's stmtNodes.
Needed to populate PKB stmt table in order.
Declared here to avoid circular dependency with RsEntitiesMapsTypes.h. */
typedef std::vector<StmtNode*> StmtNodes;

struct RelationshipMaps {
	FollowsMap followsMap;
	ModifiesMap modifiesMap;
	UsesMap usesMap;
	ParentChildMap parentChildMap;
	CallStmtProcCalledMap callStmtProcCalledMap;
};

struct EntityMaps {
	StmtNodes stmtNodes;
	StmtTypeMap stmtTypeMap;
	PatternMap patternMap;
	ConstSet constSet;
	ProcNameIndexMap procNameIndexMap;
	ProcNames procNames;
	ProcStmtMap procStmtMap;
	StmtProcMap stmtProcMap;
};

class SourceASTNode {
public:
	SourceASTNode();
	~SourceASTNode();

	virtual void process();
	virtual void process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps);
};

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

	StmtIndex getStmtIdx();
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

class StmtLstNode : public SourceASTNode {
private:
	std::vector<StmtNode*> stmtNodes;
public:
	StmtLstNode();
	void addStmtNode(StmtNode* stmtNode);
	void process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) override;

	std::vector<StmtNode*> getStmtNodes();
	std::vector<StmtIndex> getDirectStmtNodeIndices();
	std::vector<StmtIndex> getAllStmtNodeIndices();
};

class ProcedureNode : public SourceASTNode {
private:
	std::string procName;
	StmtLstNode* stmtLstNode;
public:
	ProcedureNode(std::string procName);
	void addStmtLst(StmtLstNode* stmtLstNode);
	void process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) override;

	StmtLstNode* getStmtLstNode();
	std::string getProcName();
};

class ProgramNode : public SourceASTNode {
private:
	std::vector<ProcedureNode*> procedureNodes;
	RelationshipMaps relationshipMaps;
	EntityMaps entityMaps;
public:
	ProgramNode();
	void addProcedure(ProcedureNode* procedureNode);
	void process() override;

	std::vector<ProcedureNode*> getProcedureNodes();
	RelationshipMaps getRelationshipMaps();
	EntityMaps getEntityMaps();
};
