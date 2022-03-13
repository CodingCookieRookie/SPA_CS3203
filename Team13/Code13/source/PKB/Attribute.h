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
	static int getNameIdxTableSize();
	static std::vector<int> processIntegerAttributeArgVector(PqlEntityType entityType);

	static std::unordered_map<std::string, int> nameIdxTable;
	static std::unordered_map<int, std::unordered_set<int>> nameVarIdxTable;
	static std::unordered_map<int, std::unordered_set<int>> nameProcIdxTable;
	static std::unordered_map<int, std::unordered_set<int>> nameCallProcIdxTable;
	static std::unordered_map<int, std::unordered_set<int>> nameReadVarIdxTable;
	static std::unordered_map<int, std::unordered_set<int>> namePrintVarIdxTable;

public:
	static int insertNameValue(std::string nameValue);
	static int getNameIdx(std::string& nameValue);
	static bool containsName(std::string& nameValue);

	static bool containsVarName(std::string& varName);
	static bool containsProcName(std::string& procName);
	static std::unordered_set<int> getVarIdxSet(std::string& varName);
	static std::unordered_set<int> getProcIdxSet(std::string& procName);

	static void insertVarIdxByName(VarIndex& varIdx, int nameIdx);
	static void insertProcIdxByName(ProcIndex& procIdx, int nameIdx);
	static void insertStmtByName(StmtIndex& stmtIdx, StatementType stmtType, std::string& nameValue);

	static std::tuple<std::vector<int>, std::vector<int>> getEqualNameAttributes(PqlEntityType leftEntityType, PqlEntityType rightEntityType);
	static std::vector<int> getEqualIntegerAttributes(PqlEntityType leftEntityType, PqlEntityType rightEntityType);
	static std::vector<int> getEqualNameAttributesFromName(PqlEntityType entityType, std::string& nameValue);
	static bool hasEqualIntegerAttribute(PqlEntityType entityType, int integerValue);

	static void performCleanUp();
};
