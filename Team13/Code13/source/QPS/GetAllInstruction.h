#pragma once

#include "Instruction.h"

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
