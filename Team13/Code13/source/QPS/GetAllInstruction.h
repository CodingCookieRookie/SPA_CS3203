#pragma once

#include "Instruction.h"
#include "../Common/Types.h"

class GetAllInstruction : public Instruction {
private:
	EntityType entityType;
	std::string synonym;

	/* All handlers to set EvaluatedTable to results from PKB. Uses Pass by Reference on EvTable. */
	EvaluatedTable handleGetAllStmt(std::string& synonym);
	EvaluatedTable handleGetAllStmtByType(std::string& synonym, StatementType stmtType);
	EvaluatedTable handleGetAllVar(std::string& synonym);
	EvaluatedTable handleGetAllProc(std::string& synonym);
	EvaluatedTable handleGetAllConst(std::string& synonym);

public:
	/* Constructor for a GetAllInstruction object */
	GetAllInstruction(EntityType type, std::string synonym);
	EntityType getType();
	std::string getSynonym();

	/* Main entry method for executing instruction based on EntityType in Select-cl */
	EvaluatedTable execute() override;
};
