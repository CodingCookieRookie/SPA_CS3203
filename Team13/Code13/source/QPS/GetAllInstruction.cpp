#include "./GetAllInstruction.h"

EvaluatedTable GetAllInstruction::handleGetAllStmt(std::string& synonym) {
	std::vector<StmtIndex> results = Entity::getAllStmts();

	std::vector<int> resultsToInt;
	for (StmtIndex result : results) {
		resultsToInt.emplace_back(result.getIndex());
	}

	std::unordered_map<std::string, PqlEntityType> PQLentities;
	PQLentities.insert(std::pair(synonym, PqlEntityType::Stmt));

	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = resultsToInt;

	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable GetAllInstruction::handleGetAllStmtByType(std::string& synonym, StatementType stmtType) {
	std::vector<StmtIndex> results = Entity::getStmtIdxFromType(stmtType);

	std::vector<int> resultsToInt;
	for (StmtIndex result : results) {
		resultsToInt.emplace_back(result.getIndex());
	}

	std::unordered_map<std::string, PqlEntityType> PQLentities;
	PQLentities.insert(std::pair(synonym, PqlEntityType::Stmt));

	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = resultsToInt;

	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable GetAllInstruction::handleGetAllVar(std::string& synonym) {
	std::vector<VarIndex> results = Entity::getAllVars();
	std::vector<int> resultsToInt;
	for (VarIndex result : results) {
		resultsToInt.emplace_back(result.getIndex());
	}

	std::unordered_map<std::string, PqlEntityType> PQLentities;
	PQLentities.insert(std::pair(synonym, PqlEntityType::Variable));

	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = resultsToInt;

	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable GetAllInstruction::handleGetAllProc(std::string& synonym) {
	std::vector<ProcIndex> results = Entity::getAllProcs();
	std::vector<int> resultsToInt;
	for (ProcIndex result : results) {
		resultsToInt.emplace_back(result.getIndex());
	}

	std::unordered_map<std::string, PqlEntityType> PQLentities;
	PQLentities.insert(std::pair(synonym, PqlEntityType::Procedure));

	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = resultsToInt;

	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable GetAllInstruction::handleGetAllConst(std::string& synonym) {
	std::vector<int> results = Entity::getAllConsts();

	std::unordered_map<std::string, PqlEntityType> PQLentities;
	PQLentities.insert(std::pair(synonym, PqlEntityType::Constant));

	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = results;

	return EvaluatedTable(PQLentities, PQLmap);
}

GetAllInstruction::GetAllInstruction(PqlEntityType type, std::string synonym) : pqlEntityType(type), synonym(synonym) {}

PqlEntityType GetAllInstruction::getType() {
	return pqlEntityType;
};

std::string GetAllInstruction::getSynonym() {
	return synonym;
};

EvaluatedTable GetAllInstruction::execute() {
	EvaluatedTable evTable;
	switch (pqlEntityType) {
	case PqlEntityType::Stmt:
		evTable = handleGetAllStmt(synonym);
		break;
	case PqlEntityType::Read:
		evTable = handleGetAllStmtByType(synonym, StatementType::readType);
		break;
	case PqlEntityType::Print:
		evTable = handleGetAllStmtByType(synonym, StatementType::printType);
		break;
	case PqlEntityType::Call:
		evTable = handleGetAllStmtByType(synonym, StatementType::callType);
		break;
	case PqlEntityType::While:
		evTable = handleGetAllStmtByType(synonym, StatementType::whileType);
		break;
	case PqlEntityType::If:
		evTable = handleGetAllStmtByType(synonym, StatementType::ifType);
		break;
	case PqlEntityType::Assign:
		evTable = handleGetAllStmtByType(synonym, StatementType::assignType);
		break;
	case PqlEntityType::Variable:
		evTable = handleGetAllVar(synonym);
		break;
	case PqlEntityType::Constant:
		evTable = handleGetAllConst(synonym);
		break;
	case PqlEntityType::Procedure:
		evTable = handleGetAllProc(synonym);
		break;
	}
	return evTable;
}
