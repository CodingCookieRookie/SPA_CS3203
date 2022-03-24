#pragma once

#include <string>
#include <vector>

class Common {
public:
	/* Common PQL and SIMPLE tokens*/
	const static std::string PROCEDURE;
	const static std::string READ;
	const static std::string PRINT;
	const static std::string WHILE;
	const static std::string IF;
	const static std::string THEN;
	const static std::string ELSE;
	const static std::string CALL;
	const static std::string STMT;
	const static std::string ASSIGN;
	const static std::string VARIABLE;
	const static std::string CONSTANT;

	/* PQL such-that clauses */
	static const std::string FOLLOWST;
	static const std::string FOLLOWS;
	static const std::string PARENTT;
	static const std::string PARENT;
	static const std::string MODIFIES;
	static const std::string USES;
	static const std::string NEXTT;
	static const std::string NEXT;
	static const std::string CALLST;
	static const std::string CALLS;
	static const std::string AFFECTST;
	static const std::string AFFECTS;

	/* PQL attributes */
	static const std::string PROCNAME;
	static const std::string VARNAME;
	static const std::string VALUE;
	static const std::string STMTNUM;

	/* PQL tokens */
	static const std::string SELECT;
	static const std::string BOOLEAN;
	static const std::string AND;
	static const std::string SUCH;
	static const std::string THAT;
	static const std::string PATTERN;
	static const std::string WITH;

	/* Mathematical operators and punctuation */
	const static std::string EQUAL;
	const static std::string NOT;
	const static std::string LEFT_BRACKET;
	const static std::string RIGHT_BRACKET;
	const static std::string LEFT_CURLY;
	const static std::string RIGHT_CURLY;
	const static std::string LEFT_ANGLE;
	const static std::string RIGHT_ANGLE;
	const static std::string SEMICOLON;
	const static std::string COMMMA;
	const static std::string FULLSTOP;
	const static std::string UNDERSCORE;
	const static std::string QUOTATION;

	const static std::vector<std::string> relOperators;
	const static std::vector<std::string> logicalOperators;
};