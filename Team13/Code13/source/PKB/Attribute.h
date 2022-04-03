#pragma once

#include <map>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Common/Types.h"
#include "./BidirectionalTable/BidirectionalIndexTable.h"
#include "./Entity.h"

class Attribute {
protected:
	static std::vector<EntityAttributeRef> processIntegerAttributeArgVector(EntityType entityType);

	static BidirectionalIndexTable<NameIndex> nameIdxBidirectionalTable;
	static std::unordered_map<NameIndex, std::unordered_set<VarIndex>> nameVarIdxTable;
	static std::unordered_map<NameIndex, std::unordered_set<ProcIndex>> nameProcIdxTable;
	static std::unordered_map<NameIndex, std::unordered_set<ProcIndex>> nameCallProcIdxTable;
	static std::unordered_map<NameIndex, std::unordered_set<VarIndex>> nameReadVarIdxTable;
	static std::unordered_map<NameIndex, std::unordered_set<VarIndex>> namePrintVarIdxTable;
	static std::unordered_map<StmtIndex, NameIndex> stmtIdxAttributeNameTable;

public:
	static NameIndex insertNameValue(std::string nameValue);
	static NameIndex getNameIdx(std::string& nameValue);
	static bool containsName(std::string& nameValue);

	static void insertVarIdxByName(VarIndex& varIdx, NameIndex nameIdx);
	static void insertProcIdxByName(ProcIndex& procIdx, NameIndex nameIdx);
	static void insertStmtByName(StmtIndex& stmtIdx, StatementType stmtType, std::string& nameValue);

	static std::string getAttributeNameByStmtIdx(StmtIndex& stmtIdx);
	static std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> getEqualNameAttributes(EntityType leftEntityType, EntityType rightEntityType);
	static std::vector<EntityAttributeRef> getEqualIntegerAttributes(EntityType leftEntityType, EntityType rightEntityType);
	static std::vector<EntityAttributeRef> getEqualNameAttributesFromName(EntityType entityType, std::string& nameValue);
	static bool hasEqualIntegerAttribute(EntityType entityType, ConstValue integerValue);

	static void performCleanUp();
};
