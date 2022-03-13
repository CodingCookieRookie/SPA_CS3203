#include <map>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "./Entity.h"

std::unordered_map<std::string, int> Attribute::nameIdxTable;
std::unordered_map<int, std::unordered_set<int>> Attribute::nameVarIdxTable;
std::unordered_map<int, std::unordered_set<int>> Attribute::nameProcIdxTable;
std::unordered_map<int, std::unordered_set<int>> Attribute::nameCallProcIdxTable;
std::unordered_map<int, std::unordered_set<int>> Attribute::nameReadVarIdxTable;
std::unordered_map<int, std::unordered_set<int>> Attribute::namePrintVarIdxTable;

std::unordered_set<int> Attribute::getVarIdxSet(std::string& varName) {
	return nameVarIdxTable[getNameIdx(varName)];
}

std::unordered_set<int> Attribute::getProcIdxSet(std::string& procName) {
	return nameProcIdxTable[getNameIdx(procName)];
}

bool Attribute::containsName(std::string& nameValue) {
	return nameIdxTable.find(nameValue) != nameIdxTable.end();
}

bool Attribute::containsVarName(std::string& varName) {
	int nameIdx = getNameIdx(varName);
	return nameVarIdxTable.find(nameIdx) != nameVarIdxTable.end();
}

bool Attribute::containsProcName(std::string& procName) {
	int nameIdx = getNameIdx(procName);
	return nameProcIdxTable.find(nameIdx) != nameProcIdxTable.end();
}

int Attribute::getNameIdxTableSize() {
	return nameIdxTable.size();
}

int Attribute::insertNameValue(std::string nameValue) {
	if (!containsName(nameValue)) {
		int nameIdx = getNameIdxTableSize() + 1;
		nameIdxTable[nameValue] = nameIdx;
	}
	return nameIdxTable[nameValue];
}

int Attribute::getNameIdx(std::string& nameValue) {
	return nameIdxTable[nameValue];
}

void Attribute::insertVarIdxByName(VarIndex& varIdx, int nameIdx) {
	nameVarIdxTable[nameIdx].insert(varIdx.index);
}

void Attribute::insertProcIdxByName(ProcIndex& procIdx, int nameIdx) {
	nameProcIdxTable[nameIdx].insert(procIdx.index);
}

void Attribute::insertStmtByName(StmtIndex& stmtIdx, StatementType stmtType, std::string& nameValue) {
	int nameIdx = insertNameValue(nameValue);

	switch (stmtType) {
	case StatementType::callType:
		nameCallProcIdxTable[nameIdx].insert(stmtIdx.index);
		break;
	case StatementType::readType:
		nameReadVarIdxTable[nameIdx].insert(stmtIdx.index);
		break;
	case StatementType::printType:
		namePrintVarIdxTable[nameIdx].insert(stmtIdx.index);
		break;
	}
}

std::tuple<std::vector<int>, std::vector<int>> Attribute::getEqualNameAttributes(PqlEntityType leftEntityType, PqlEntityType rightEntityType) {
	std::vector<int> leftEntityTypeIndices;
	std::vector<int> rightEntityTypeIndices;

	std::map<PqlEntityType, std::unordered_map<int, std::unordered_set<int>>> PqlEntityTypeTableMap = {
		{ PqlEntityType::Procedure, Attribute::nameProcIdxTable },
		{ PqlEntityType::Variable, Attribute::nameVarIdxTable },
		{ PqlEntityType::Call, Attribute::nameCallProcIdxTable },
		{ PqlEntityType::Read, Attribute::nameReadVarIdxTable },
		{ PqlEntityType::Print, Attribute::namePrintVarIdxTable }
	};

	std::unordered_map<int, std::unordered_set<int>> leftArgMap = PqlEntityTypeTableMap[leftEntityType];
	std::unordered_map<int, std::unordered_set<int>> rightArgMap = PqlEntityTypeTableMap[rightEntityType];

	for (auto& leftArgMapInfo : leftArgMap) {
		int nameIdx = leftArgMapInfo.first;
		for (auto& leftEntityIdx : leftArgMap[nameIdx]) {
			for (auto& rightEntityIdx : rightArgMap[nameIdx]) {
				leftEntityTypeIndices.push_back(leftEntityIdx);
				rightEntityTypeIndices.push_back(rightEntityIdx);
			}
		}
	}

	return std::make_tuple(leftEntityTypeIndices, rightEntityTypeIndices);
}

std::vector<int> Attribute::processIntegerAttributeArgVector(PqlEntityType entityType) {
	std::map<PqlEntityType, StatementType> PqlEntityTypeStatementTypeMap = {
		{ PqlEntityType::Read, StatementType::readType },
		{ PqlEntityType::Print, StatementType::printType },
		{ PqlEntityType::Call, StatementType::callType },
		{ PqlEntityType::While, StatementType::whileType },
		{ PqlEntityType::If, StatementType::ifType },
		{ PqlEntityType::Assign, StatementType::assignType }
	};

	if (entityType == PqlEntityType::Constant) {
		return Entity::getAllConsts();
	}

	std::vector<StmtIndex> stmtIndices;
	std::vector<int> res;

	if (entityType == PqlEntityType::Stmt) {
		stmtIndices = Entity::getAllStmts();
	} else {
		stmtIndices = Entity::getStmtIdxFromType(PqlEntityTypeStatementTypeMap[entityType]);
	}

	for (auto& stmtIdx : stmtIndices) {
		res.push_back(stmtIdx.index);
	}

	return res;
}

std::vector<int> Attribute::getEqualIntegerAttributes(PqlEntityType leftEntityType, PqlEntityType rightEntityType) {
	std::vector<int> res;

	std::vector<int> leftArgVector = processIntegerAttributeArgVector(leftEntityType);
	std::vector<int> rightArgVector = processIntegerAttributeArgVector(rightEntityType);

	for (auto& intValue : leftArgVector) {
		if (std::find(rightArgVector.begin(), rightArgVector.end(), intValue) != rightArgVector.end()) {
			res.push_back(intValue);
		}
	}

	return res;
}

std::vector<int> Attribute::getEqualNameAttributesFromName(PqlEntityType entityType, std::string& nameValue) {
	std::vector<int> res;

	std::map<PqlEntityType, std::unordered_map<int, std::unordered_set<int>>> PqlEntityTypeTableMap = {
		{ PqlEntityType::Procedure, Attribute::nameProcIdxTable },
		{ PqlEntityType::Variable, Attribute::nameVarIdxTable },
		{ PqlEntityType::Call, Attribute::nameCallProcIdxTable },
		{ PqlEntityType::Read, Attribute::nameReadVarIdxTable },
		{ PqlEntityType::Print, Attribute::namePrintVarIdxTable }
	};

	std::unordered_map<int, std::unordered_set<int>> argMap = PqlEntityTypeTableMap[entityType];

	int nameIdx = getNameIdx(nameValue);

	for (auto& entityIdx : argMap[nameIdx]) {
		res.push_back(entityIdx);
	}

	return res;
}

bool Attribute::hasEqualIntegerAttribute(PqlEntityType entityType, int integerValue) {
	std::vector<int> argVector = processIntegerAttributeArgVector(entityType);
	return std::find(argVector.begin(), argVector.end(), integerValue) != argVector.end();
}

void Attribute::performCleanUp() {
	nameIdxTable = {};
	nameVarIdxTable = {};
	nameProcIdxTable = {};
	nameCallProcIdxTable = {};
	nameReadVarIdxTable = {};
	namePrintVarIdxTable = {};
}
