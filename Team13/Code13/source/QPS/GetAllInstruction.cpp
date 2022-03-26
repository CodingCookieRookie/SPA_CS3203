#include "./GetAllInstruction.h"

EvaluatedTable GetAllInstruction::handleGetAllStmt(std::string& synonym) {
	std::vector<StmtIndex> results = Entity::getAllStmts();

	std::vector<int> resultsToInt;
	for (StmtIndex result : results) {
		resultsToInt.emplace_back(result);
	}

	std::unordered_map<std::string, EntityType> PQLentities;
	PQLentities.insert(std::pair(synonym, EntityType::STMT));

	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = resultsToInt;

	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable GetAllInstruction::handleGetAllStmtByType(std::string& synonym, StatementType stmtType) {
	std::vector<StmtIndex> results = Entity::getStmtIdxFromType(stmtType);

	std::vector<int> resultsToInt;
	for (StmtIndex result : results) {
		resultsToInt.emplace_back(result);
	}

	std::unordered_map<std::string, EntityType> PQLentities;
	PQLentities.insert(std::pair(synonym, EntityType::STMT));

	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = resultsToInt;

	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable GetAllInstruction::handleGetAllVar(std::string& synonym) {
	std::vector<VarIndex> results = Entity::getAllVars();
	std::vector<int> resultsToInt;
	for (VarIndex result : results) {
		resultsToInt.emplace_back(result);
	}

	std::unordered_map<std::string, EntityType> PQLentities;
	PQLentities.insert(std::pair(synonym, EntityType::VARIABLE));

	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = resultsToInt;

	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable GetAllInstruction::handleGetAllProc(std::string& synonym) {
	std::vector<ProcIndex> results = Entity::getAllProcs();
	std::vector<int> resultsToInt;
	for (ProcIndex result : results) {
		resultsToInt.emplace_back(result);
	}

	std::unordered_map<std::string, EntityType> PQLentities;
	PQLentities.insert(std::pair(synonym, EntityType::PROCEDURE));

	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = resultsToInt;

	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable GetAllInstruction::handleGetAllConst(std::string& synonym) {
	std::vector<int> results = Entity::getAllConsts();

	std::unordered_map<std::string, EntityType> PQLentities;
	PQLentities.insert(std::pair(synonym, EntityType::CONSTANT));

	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = results;

	return EvaluatedTable(PQLentities, PQLmap);
}

GetAllInstruction::GetAllInstruction(EntityType type, std::string synonym) : entityType(type), synonym(synonym) {}

EntityType GetAllInstruction::getType() {
	return entityType;
};

std::string GetAllInstruction::getSynonym() {
	return synonym;
};

EvaluatedTable GetAllInstruction::execute() {
	EvaluatedTable evTable;
	switch (entityType) {
	case EntityType::STMT:
		evTable = handleGetAllStmt(synonym);
		break;
	case EntityType::READ:
		evTable = handleGetAllStmtByType(synonym, StatementType::READ_TYPE);
		break;
	case EntityType::PRINT:
		evTable = handleGetAllStmtByType(synonym, StatementType::PRINT_TYPE);
		break;
	case EntityType::CALL:
		evTable = handleGetAllStmtByType(synonym, StatementType::CALL_TYPE);
		break;
	case EntityType::WHILE:
		evTable = handleGetAllStmtByType(synonym, StatementType::WHILE_TYPE);
		break;
	case EntityType::IF:
		evTable = handleGetAllStmtByType(synonym, StatementType::IF_TYPE);
		break;
	case EntityType::ASSIGN:
		evTable = handleGetAllStmtByType(synonym, StatementType::ASSIGN_TYPE);
		break;
	case EntityType::VARIABLE:
		evTable = handleGetAllVar(synonym);
		break;
	case EntityType::CONSTANT:
		evTable = handleGetAllConst(synonym);
		break;
	case EntityType::PROCEDURE:
		evTable = handleGetAllProc(synonym);
		break;
	}
	return evTable;
}
