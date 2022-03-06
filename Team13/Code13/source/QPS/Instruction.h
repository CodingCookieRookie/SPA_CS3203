#pragma once

#include <unordered_map>
#include <vector>

#include "../Exception/SPAException.h"
#include "../PKB/Entity.h"
#include "../PKB/ExpressionProcessor.h"
#include "../PKB/Follows.h"
#include "../PKB/FollowsT.h"
#include "../PKB/Modifies.h"
#include "../PKB/Parent.h"
#include "../PKB/ParentT.h"
#include "../PKB/Pattern.h"
#include "../PKB/Uses.h"
#include "EvaluatedTable.h"
#include "QPSCommons.h"

class Instruction {
public:
	virtual EvaluatedTable execute() = 0;
};

class GetAllInstruction : public Instruction {
private:
	PqlEntityType pqlEntityType;
	std::string synonym;

	/* All handlers to set EvaluatedTable to results from PKB. Uses Pass by Reference on EvTable. */
	EvaluatedTable handleGetAllStmt(std::string& synonym);
	EvaluatedTable handleGetAllStmtByType(std::string& synonym, StatementType stmtType);
	EvaluatedTable handleGetAllVar(std::string& synonym);
	EvaluatedTable handleGetAllProc(std::string& synonym);
	EvaluatedTable handleGetAllConst(std::string& synonym);

public:
	/* Constructor for a GetAllInstruction object */
	GetAllInstruction(PqlEntityType type, std::string synonym);
	PqlEntityType getType();
	std::string getSynonym();

	/* Main entry method for executing instruction based on PqlEntityType in Select-cl */
	EvaluatedTable execute() override;
};

class RelationshipInstruction : public Instruction {
private:
	PqlRelationshipType pqlRelationshipType;
	PqlReference lhsRef;
	PqlReference rhsRef;

	EvaluatedTable handleModifiesS();
	EvaluatedTable handleModifiesP();
	EvaluatedTable handleUsesS();
	EvaluatedTable handleUsesP();
	EvaluatedTable handleFollows();
	EvaluatedTable handleFollowsT();
	EvaluatedTable handleParent();
	EvaluatedTable handleParentT();

public:
	RelationshipInstruction(PqlRelationshipType pqlRSType, PqlReference lhs, PqlReference rhs);
	EvaluatedTable execute() override;
};

class PatternInstruction : public Instruction {
private:
	std::string synonym;
	PqlReference entRef;
	PqlExpression expressionSpec;

public:
	PatternInstruction(std::string synonym, PqlReference entRef, PqlExpression expressionSpec);
	EvaluatedTable handlePatterns();
	EvaluatedTable execute() override;
};
