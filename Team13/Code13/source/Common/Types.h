#pragma once

enum class StatementType {
	ASSIGN_TYPE, WHILE_TYPE, IF_TYPE, READ_TYPE, PRINT_TYPE, CALL_TYPE
};

enum class ExprNodeValueType {
	VAR_NAME, CONST_VALUE, ARITHMETIC_OPERATOR, LOGICAL_OPERATOR, REL_OPERATOR
};

enum class EntityType {
	STMT, READ, PRINT, CALL, WHILE,
	IF, ASSIGN, VARIABLE, CONSTANT, PROCEDURE
};

enum class RelationshipType {
	USES_S, MODIFIES_S, USES_P, MODIFIES_P, FOLLOWS, FOLLOWS_T, PARENT,
	PARENT_T, CALLS, CALLS_T, NEXT, NEXT_T, AFFECTS, AFFECTS_T
};

typedef int EntityAttributeRef;
typedef int NameIdxEntityIndex;
typedef int VarIndex;
typedef int ProcIndex;
typedef int StmtIndex;
typedef int NameIndex;
typedef int SynonymIndex;
typedef int ConstValue;
