#pragma once

#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Common/Types.h"
#include "./Entity.h"

class Attribute {
protected:
	static size_t getNameIdxTableSize();
	static std::vector<EntityAttributeRef> processIntegerAttributeArgVector(PqlEntityType entityType);

	static std::unordered_map<std::string, NameIndex> nameIdxTable;
	static std::unordered_map<NameIndex, std::unordered_set<VarIndex>> nameVarIdxTable;
	static std::unordered_map<NameIndex, std::unordered_set<ProcIndex>> nameProcIdxTable;
	static std::unordered_map<NameIndex, std::unordered_set<ProcIndex>> nameCallProcIdxTable;
	static std::unordered_map<NameIndex, std::unordered_set<VarIndex>> nameReadVarIdxTable;
	static std::unordered_map<NameIndex, std::unordered_set<VarIndex>> namePrintVarIdxTable;

public:
	static NameIndex insertNameValue(std::string nameValue);
	static NameIndex getNameIdx(std::string& nameValue);
	static bool containsName(std::string& nameValue);

	static bool containsVarName(std::string& varName);
	static bool containsProcName(std::string& procName);
	static std::unordered_set<VarIndex> getVarIdxSet(std::string& varName);
	static std::unordered_set<ProcIndex> getProcIdxSet(std::string& procName);

	static void insertVarIdxByName(VarIndex& varIdx, NameIndex nameIdx);
	static void insertProcIdxByName(ProcIndex& procIdx, NameIndex nameIdx);
	static void insertStmtByName(StmtIndex& stmtIdx, StatementType stmtType, std::string& nameValue);

	static std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> getEqualNameAttributes(PqlEntityType leftEntityType, PqlEntityType rightEntityType);
	static std::vector<EntityAttributeRef> getEqualIntegerAttributes(PqlEntityType leftEntityType, PqlEntityType rightEntityType);
	static std::vector<EntityAttributeRef> getEqualNameAttributesFromName(PqlEntityType entityType, std::string& nameValue);
	static bool hasEqualIntegerAttribute(PqlEntityType entityType, ConstValue integerValue);

	static void performCleanUp();
};
