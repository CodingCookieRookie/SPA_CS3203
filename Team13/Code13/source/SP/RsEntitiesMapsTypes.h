#pragma once

#include <unordered_map>
#include <unordered_set>

#include "../Common/Types.h"

/* Maps stmtIndex to the variable that it modifies */
typedef std::unordered_map<StmtIndex, std::string> ModifiesMap;

/* Maps stmtIndex to the variables that it uses */
typedef std::unordered_map<StmtIndex, std::unordered_set<std::string>> UsesMap;

/* Maps call stmtIndex to the procedure name that it calls */
typedef std::unordered_map<StmtIndex, std::string> CallStmtProcCalledMap;

/* Maps parent's stmtIndex to its children's stmtIndices */
typedef std::unordered_map<StmtIndex, std::vector<StmtIndex>> ParentChildMap;

/* Maps prevIndex to the stmtIndex that follows the former */
typedef std::unordered_map<StmtIndex, StmtIndex> FollowsMap;

/* Maps stmtIndex to its pattern.
The map value is a set, in order to accommodate container stmt's pattern, i.e. the vars it uses. */
typedef std::unordered_map<StmtIndex, std::unordered_set<std::string>> PatternMap;

/* Maps stmtIndex to its stmtType */
typedef std::unordered_map<StmtIndex, StatementType> StmtTypeMap;

/* A set of const that exist in the program */
typedef std::unordered_set<std::string> ConstSet;

/* Maps procName to its procIndex */
typedef std::unordered_map<std::string, ProcIndex> ProcNameIndexMap;

/* Stores the procedure names */
typedef std::vector<std::string> ProcNames;

/* Maps procIndex to all of its stmtIndices */
typedef std::unordered_map<ProcIndex, std::vector<StmtIndex>> ProcStmtMap;

/* Maps stmtIndex to the procIndex in which it exists */
typedef std::unordered_map<StmtIndex, ProcIndex> StmtProcMap;
