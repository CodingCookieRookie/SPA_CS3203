#include "./Attribute.h"

std::unordered_map<std::string, NameIndex> Attribute::nameIdxTable;
std::unordered_map<NameIndex, std::string> Attribute::idxNameTable;
std::unordered_map<NameIndex, std::unordered_set<VarIndex>> Attribute::nameVarIdxTable;
std::unordered_map<NameIndex, std::unordered_set<ProcIndex>> Attribute::nameProcIdxTable;
std::unordered_map<NameIndex, std::unordered_set<StmtIndex>> Attribute::nameCallProcIdxTable;
std::unordered_map<NameIndex, std::unordered_set<StmtIndex>> Attribute::nameReadVarIdxTable;
std::unordered_map<NameIndex, std::unordered_set<StmtIndex>> Attribute::namePrintVarIdxTable;
std::unordered_map<StmtIndex, NameIndex> Attribute::stmtIdxAttributeNameTable;

std::unordered_set<VarIndex> Attribute::getVarIdxSet(std::string& varName) {
	return nameVarIdxTable[getNameIdx(varName)];
}

std::unordered_set<ProcIndex> Attribute::getProcIdxSet(std::string& procName) {
	return nameProcIdxTable[getNameIdx(procName)];
}

bool Attribute::containsName(std::string& nameValue) {
	return nameIdxTable.find(nameValue) != nameIdxTable.end();
}

bool Attribute::containsVarName(std::string& varName) {
	NameIndex nameIdx = getNameIdx(varName);
	return nameVarIdxTable.find(nameIdx) != nameVarIdxTable.end();
}

bool Attribute::containsProcName(std::string& procName) {
	NameIndex nameIdx = getNameIdx(procName);
	return nameProcIdxTable.find(nameIdx) != nameProcIdxTable.end();
}

size_t Attribute::getNameIdxTableSize() {
	return nameIdxTable.size();
}

NameIndex Attribute::insertNameValue(std::string nameValue) {
	if (!containsName(nameValue)) {
		NameIndex nameIdx = NameIndex(getNameIdxTableSize()) + 1;
		nameIdxTable[nameValue] = nameIdx;
		idxNameTable[nameIdx] = nameValue;
	}
	return nameIdxTable[nameValue];
}

NameIndex Attribute::getNameIdx(std::string& nameValue) {
	return nameIdxTable[nameValue];
}

void Attribute::insertVarIdxByName(VarIndex& varIdx, NameIndex nameIdx) {
	nameVarIdxTable[nameIdx].insert(varIdx);
}

void Attribute::insertProcIdxByName(ProcIndex& procIdx, NameIndex nameIdx) {
	nameProcIdxTable[nameIdx].insert(procIdx);
}

void Attribute::insertStmtByName(StmtIndex& stmtIdx, StatementType stmtType, std::string& nameValue) {
	NameIndex nameIdx = insertNameValue(nameValue);
	stmtIdxAttributeNameTable[stmtIdx] = nameIdx;

	switch (stmtType) {
	case StatementType::CALL_TYPE:
		nameCallProcIdxTable[nameIdx].insert(stmtIdx);
		break;
	case StatementType::READ_TYPE:
		nameReadVarIdxTable[nameIdx].insert(stmtIdx);
		break;
	case StatementType::PRINT_TYPE:
		namePrintVarIdxTable[nameIdx].insert(stmtIdx);
		break;
	}
}

std::string Attribute::getAttributeNameByStmtIdx(StmtIndex& stmtIdx) {
	return idxNameTable[stmtIdxAttributeNameTable[stmtIdx]];
}

std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> Attribute::getEqualNameAttributes(EntityType leftEntityType, EntityType rightEntityType) {
	std::vector<EntityAttributeRef> leftEntityTypeIndices;
	std::vector<EntityAttributeRef> rightEntityTypeIndices;

	std::map<EntityType, std::unordered_map<EntityAttributeRef, std::unordered_set<EntityAttributeRef>>> EntityTypeTableMap = {
		{ EntityType::PROCEDURE, Attribute::nameProcIdxTable },
		{ EntityType::VARIABLE, Attribute::nameVarIdxTable },
		{ EntityType::CALL, Attribute::nameCallProcIdxTable },
		{ EntityType::READ, Attribute::nameReadVarIdxTable },
		{ EntityType::PRINT, Attribute::namePrintVarIdxTable }
	};

	std::unordered_map<EntityAttributeRef, std::unordered_set<EntityAttributeRef>> leftArgMap = EntityTypeTableMap[leftEntityType];
	std::unordered_map<EntityAttributeRef, std::unordered_set<EntityAttributeRef>> rightArgMap = EntityTypeTableMap[rightEntityType];

	for (auto& leftArgMapInfo : leftArgMap) {
		NameIndex nameIdx = leftArgMapInfo.first;
		for (auto& leftEntityIdx : leftArgMap[nameIdx]) {
			for (auto& rightEntityIdx : rightArgMap[nameIdx]) {
				leftEntityTypeIndices.push_back(leftEntityIdx);
				rightEntityTypeIndices.push_back(rightEntityIdx);
			}
		}
	}

	return std::make_tuple(leftEntityTypeIndices, rightEntityTypeIndices);
}

std::vector<EntityAttributeRef> Attribute::processIntegerAttributeArgVector(EntityType entityType) {
	std::map<EntityType, StatementType> EntityTypeStatementTypeMap = {
		{ EntityType::READ, StatementType::READ_TYPE },
		{ EntityType::PRINT, StatementType::PRINT_TYPE },
		{ EntityType::CALL, StatementType::CALL_TYPE },
		{ EntityType::WHILE, StatementType::WHILE_TYPE },
		{ EntityType::IF, StatementType::IF_TYPE },
		{ EntityType::ASSIGN, StatementType::ASSIGN_TYPE }
	};

	if (entityType == EntityType::CONSTANT) {
		return Entity::getAllConsts();
	}

	std::vector<StmtIndex> stmtIndices;
	std::vector<StmtIndex> res;

	if (entityType == EntityType::STMT) {
		stmtIndices = Entity::getAllStmts();
	} else {
		stmtIndices = Entity::getStmtIdxFromType(EntityTypeStatementTypeMap[entityType]);
	}

	for (auto& stmtIdx : stmtIndices) {
		res.push_back(stmtIdx);
	}

	return res;
}

std::vector<EntityAttributeRef> Attribute::getEqualIntegerAttributes(EntityType leftEntityType, EntityType rightEntityType) {
	std::vector<EntityAttributeRef> res;

	std::vector<EntityAttributeRef> leftArgVector = processIntegerAttributeArgVector(leftEntityType);
	std::vector<EntityAttributeRef> rightArgVector = processIntegerAttributeArgVector(rightEntityType);

	for (auto& intValue : leftArgVector) {
		if (std::find(rightArgVector.begin(), rightArgVector.end(), intValue) != rightArgVector.end()) {
			res.push_back(intValue);
		}
	}

	return res;
}

std::vector<EntityAttributeRef> Attribute::getEqualNameAttributesFromName(EntityType entityType, std::string& nameValue) {
	std::vector<EntityAttributeRef> res;

	std::map<EntityType, std::unordered_map<EntityAttributeRef, std::unordered_set<EntityAttributeRef>>> EntityTypeTableMap = {
		{ EntityType::PROCEDURE, Attribute::nameProcIdxTable },
		{ EntityType::VARIABLE, Attribute::nameVarIdxTable },
		{ EntityType::CALL, Attribute::nameCallProcIdxTable },
		{ EntityType::READ, Attribute::nameReadVarIdxTable },
		{ EntityType::PRINT, Attribute::namePrintVarIdxTable }
	};

	std::unordered_map<EntityAttributeRef, std::unordered_set<EntityAttributeRef>> argMap = EntityTypeTableMap[entityType];

	NameIndex nameIdx = getNameIdx(nameValue);

	for (auto& entityIdx : argMap[nameIdx]) {
		res.push_back(entityIdx);
	}

	return res;
}

bool Attribute::hasEqualIntegerAttribute(EntityType entityType, ConstValue integerValue) {
	std::vector<EntityAttributeRef> argVector = processIntegerAttributeArgVector(entityType);
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
