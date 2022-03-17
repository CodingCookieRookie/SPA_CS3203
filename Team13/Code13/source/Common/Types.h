#pragma once

enum class StatementType {
	assignType, whileType, ifType, readType, printType, callType
};

enum class ExprNodeValueType {
	varName, constValue, arithmeticOperator, logicalOperator, relOperator
};

enum class PqlEntityType {
	Stmt, Read, Print, Call, While,
	If, Assign, Variable, Constant, Procedure
};

typedef int VarIndex;
typedef int ProcIndex;
typedef int StmtIndex;
typedef int NameIndex;
typedef int ConstValue;
