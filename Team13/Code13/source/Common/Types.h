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

typedef int EntityAttributeRef;
typedef int VarIndex;
typedef int ProcIndex;
typedef int StmtIndex;
typedef int NameIndex;
typedef int ConstValue;
