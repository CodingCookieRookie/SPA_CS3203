#pragma once

#include <map>
#include <unordered_map>
#include <unordered_set>

#include "../Common/Types.h"

typedef std::unordered_map<SynonymIndex, std::unordered_set<SynonymIndex>> RelationshipMap;

/* Maps stmtIndex to its pattern.
The map value is a set, in order to accommodate container stmt's pattern, i.e. the vars it uses. */
typedef std::unordered_map<StmtIndex, std::unordered_set<std::string>> PatternMap;

/* Maps stmtIndex to its stmtType */
typedef std::unordered_map<StmtIndex, StatementType> StmtTypeMap;

/* A set of const that exist in the program */
typedef std::unordered_set<std::string> ConstSet;

/* Maps procName to its procIndex */
typedef std::unordered_map<std::string, ProcIndex> ProcNameToIndexMap;

/* Maps procIndex to its procName, sorted by procIndex in ascending order */
typedef std::map<ProcIndex, std::string> SortedProcIndexToNameMap;

/* Maps procIndex to all of its stmtIndices */
typedef std::unordered_map<ProcIndex, std::vector<StmtIndex>> ProcStmtMap;

/* Maps stmtIndex to the procIndex in which it exists */
typedef std::unordered_map<StmtIndex, ProcIndex> StmtProcMap;

/* Maps varName to its varIndex */
typedef std::unordered_map<std::string, VarIndex> VarNameToIndexMap;

/* Maps varIndex to its varName, sorted by varIndex in ascending order */
typedef std::map<VarIndex, std::string> SortedVarIndexToNameMap;
