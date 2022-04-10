#include "./Attribute.h"

Attribute::Attribute() {
	nameProcIdxTable = new std::unordered_map<NameIndex, std::unordered_set<VarIndex>>();
	nameVarIdxTable = new std::unordered_map<NameIndex, std::unordered_set<ProcIndex>>();
	nameCallProcIdxTable = new std::unordered_map<NameIndex, std::unordered_set<ProcIndex>>();
	nameReadVarIdxTable = new std::unordered_map<NameIndex, std::unordered_set<VarIndex>>();
	namePrintVarIdxTable = new std::unordered_map<NameIndex, std::unordered_set<VarIndex>>();
	entityTypeToNameEntityIdxTable[EntityType::PROCEDURE] = nameProcIdxTable;
	entityTypeToNameEntityIdxTable[EntityType::VARIABLE] = nameVarIdxTable;
	stmtTypeToNameEntityIdxTable[StatementType::CALL_TYPE] = nameCallProcIdxTable;
	stmtTypeToNameEntityIdxTable[StatementType::READ_TYPE] = nameReadVarIdxTable;
	stmtTypeToNameEntityIdxTable[StatementType::PRINT_TYPE] = namePrintVarIdxTable;
}

Attribute::~Attribute() {}

bool Attribute::containsName(std::string& nameValue) {
	return nameIdxBidirectionalTable.contains(nameValue);
}

NameIndex Attribute::insertNameValue(std::string nameValue) {
	return nameIdxBidirectionalTable.insert(nameValue);
}

NameIndex Attribute::getNameIdx(std::string& nameValue) {
	return nameIdxBidirectionalTable.getIndexFromString(nameValue);
}

void Attribute::insertNameIdxEntityByName(EntityType entityType, NameIdxEntityIndex entityIdx, NameIndex nameIdx) {
	(*entityTypeToNameEntityIdxTable[entityType])[nameIdx].insert(entityIdx);
}

void Attribute::insertStmtByName(StmtIndex& stmtIdx, StatementType stmtType, std::string& nameValue) {
	NameIndex nameIdx = insertNameValue(nameValue);
	stmtIdxAttributeNameTable[stmtIdx] = nameIdx;

	(*stmtTypeToNameEntityIdxTable[stmtType])[nameIdx].insert(stmtIdx);
}

std::string Attribute::getAttributeNameByStmtIdx(StmtIndex& stmtIdx) {
	return nameIdxBidirectionalTable.getStringFromIndex(stmtIdxAttributeNameTable[stmtIdx]);
}

std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> Attribute::getEqualNameAttributes(EntityType leftEntityType, EntityType rightEntityType) {
	std::vector<EntityAttributeRef> leftEntityTypeIndices;
	std::vector<EntityAttributeRef> rightEntityTypeIndices;

	std::map<EntityType, std::unordered_map<EntityAttributeRef, std::unordered_set<EntityAttributeRef>>*> EntityTypeTableMap = {
		{ EntityType::PROCEDURE, Attribute::nameProcIdxTable },
		{ EntityType::VARIABLE, Attribute::nameVarIdxTable },
		{ EntityType::CALL, Attribute::nameCallProcIdxTable },
		{ EntityType::READ, Attribute::nameReadVarIdxTable },
		{ EntityType::PRINT, Attribute::namePrintVarIdxTable }
	};

	std::unordered_map<EntityAttributeRef, std::unordered_set<EntityAttributeRef>>* leftArgMap = EntityTypeTableMap[leftEntityType];
	std::unordered_map<EntityAttributeRef, std::unordered_set<EntityAttributeRef>>* rightArgMap = EntityTypeTableMap[rightEntityType];

	for (auto& leftArgMapInfo : *leftArgMap) {
		NameIndex nameIdx = leftArgMapInfo.first;
		for (auto& leftEntityIdx : (*leftArgMap)[nameIdx]) {
			for (auto& rightEntityIdx : (*rightArgMap)[nameIdx]) {
				leftEntityTypeIndices.push_back(leftEntityIdx);
				rightEntityTypeIndices.push_back(rightEntityIdx);
			}
		}
	}

	return std::make_tuple(leftEntityTypeIndices, rightEntityTypeIndices);
}

std::vector<EntityAttributeRef> Attribute::processIntegerAttributeArgVector(EntityType entityType, Constant* constant, Statement* statement) {
	std::map<EntityType, StatementType> EntityTypeStatementTypeMap = {
		{ EntityType::READ, StatementType::READ_TYPE },
		{ EntityType::PRINT, StatementType::PRINT_TYPE },
		{ EntityType::CALL, StatementType::CALL_TYPE },
		{ EntityType::WHILE, StatementType::WHILE_TYPE },
		{ EntityType::IF, StatementType::IF_TYPE },
		{ EntityType::ASSIGN, StatementType::ASSIGN_TYPE }
	};

	if (entityType == EntityType::CONSTANT) {
		return constant->getAllConsts();
	}

	std::vector<StmtIndex> stmtIndices;
	std::vector<StmtIndex> res;

	if (entityType == EntityType::STMT) {
		stmtIndices = statement->getAllStmts();
	} else {
		stmtIndices = statement->getStmtIdxFromType(EntityTypeStatementTypeMap[entityType]);
	}

	for (auto& stmtIdx : stmtIndices) {
		res.push_back(stmtIdx);
	}

	return res;
}

std::vector<EntityAttributeRef> Attribute::getEqualIntegerAttributes(EntityType leftEntityType, EntityType rightEntityType, Constant* constant, Statement* statement) {
	std::vector<EntityAttributeRef> res;

	std::vector<EntityAttributeRef> leftArgVector = processIntegerAttributeArgVector(leftEntityType, constant, statement);
	std::vector<EntityAttributeRef> rightArgVector = processIntegerAttributeArgVector(rightEntityType, constant, statement);

	for (auto& intValue : leftArgVector) {
		if (std::find(rightArgVector.begin(), rightArgVector.end(), intValue) != rightArgVector.end()) {
			res.push_back(intValue);
		}
	}

	return res;
}

std::vector<EntityAttributeRef> Attribute::getEqualNameAttributesFromName(EntityType entityType, std::string& nameValue) {
	std::vector<EntityAttributeRef> res;

	std::map<EntityType, std::unordered_map<EntityAttributeRef, std::unordered_set<EntityAttributeRef>>*> EntityTypeTableMap = {
		{ EntityType::PROCEDURE, Attribute::nameProcIdxTable },
		{ EntityType::VARIABLE, Attribute::nameVarIdxTable },
		{ EntityType::CALL, Attribute::nameCallProcIdxTable },
		{ EntityType::READ, Attribute::nameReadVarIdxTable },
		{ EntityType::PRINT, Attribute::namePrintVarIdxTable }
	};

	std::unordered_map<EntityAttributeRef, std::unordered_set<EntityAttributeRef>>* argMap = EntityTypeTableMap[entityType];

	NameIndex nameIdx = getNameIdx(nameValue);

	for (auto& entityIdx : (*argMap)[nameIdx]) {
		res.push_back(entityIdx);
	}

	return res;
}

bool Attribute::hasEqualIntegerAttribute(EntityType entityType, ConstValue integerValue, Constant* constant, Statement* statement) {
	std::vector<EntityAttributeRef> argVector = processIntegerAttributeArgVector(entityType, constant, statement);
	return std::find(argVector.begin(), argVector.end(), integerValue) != argVector.end();
}
