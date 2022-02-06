#pragma once
#include <string>

enum class PQL_VARIABLE_TYPE {
	STMT, READ, PRINT, CALL, WHILE,
	IF, ASSIGN, VARIABLE, CONSTANT, PROCEDURE
};

enum class INSTRUCTION_TYPE {
	getAllStmt, getAllVar, getAllAsgn
};

typedef std::pair<PQL_VARIABLE_TYPE, std::string> PQL_VARIABLE;