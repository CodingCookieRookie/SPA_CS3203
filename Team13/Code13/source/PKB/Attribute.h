#pragma once

#include <map>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Common/Types.h"
#include "./BidirectionalTable/BidirectionalIndexTable.h"
#include "./Constant.h"
#include "./Statement.h"

/* Forward declaration */
class Statement;

class Attribute {
private:
	std::vector<EntityAttributeRef> processIntegerAttributeArgVector(EntityType entityType, Constant* constant, Statement* statement);

	BidirectionalIndexTable<NameIndex> nameIdxBidirectionalTable;
	std::unordered_map<NameIndex, std::unordered_set<VarIndex>>* nameVarIdxTable;
	std::unordered_map<NameIndex, std::unordered_set<ProcIndex>>* nameProcIdxTable;
	std::unordered_map<NameIndex, std::unordered_set<ProcIndex>>* nameCallProcIdxTable;
	std::unordered_map<NameIndex, std::unordered_set<VarIndex>>* nameReadVarIdxTable;
	std::unordered_map<NameIndex, std::unordered_set<VarIndex>>* namePrintVarIdxTable;
	std::unordered_map<EntityType, std::unordered_map<NameIndex, std::unordered_set<NameIdxEntityIndex>>*> entityTypeToNameEntityIdxTable;
	std::unordered_map<StatementType, std::unordered_map<NameIndex, std::unordered_set<NameIdxEntityIndex>>*> stmtTypeToNameEntityIdxTable;
	std::unordered_map<StmtIndex, NameIndex> stmtIdxAttributeNameTable;

public:
	Attribute();
	~Attribute();
	NameIndex insertNameValue(std::string nameValue);
	NameIndex getNameIdx(std::string& nameValue);
	bool containsName(std::string& nameValue);

	void insertNameIdxEntityByName(EntityType entityType, NameIdxEntityIndex entityIdx, NameIndex nameIdx);

	void insertStmtByName(StmtIndex& stmtIdx, StatementType stmtType, std::string& nameValue);

	std::string getAttributeNameByStmtIdx(StmtIndex& stmtIdx);
	std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> getEqualNameAttributes(EntityType leftEntityType, EntityType rightEntityType);
	std::vector<EntityAttributeRef> getEqualIntegerAttributes(
		EntityType leftEntityType, EntityType rightEntityType, Constant* constant, Statement* statement);
	std::vector<EntityAttributeRef> getEqualNameAttributesFromName(
		EntityType entityType, std::string& nameValue);
	bool hasEqualIntegerAttribute(EntityType entityType,
		ConstValue integerValue, Constant* constant, Statement* statement);
};
