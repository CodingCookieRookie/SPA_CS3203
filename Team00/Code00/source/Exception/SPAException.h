#pragma once

#ifndef SPAEXCEPTION_H
#define SPAEXCEPTION_H

#include <exception>
#include <string>

/* Base class that we inherit for all SPA - related exceptions */
class SPAException : std::exception {
private:
	const std::string reason;
public:
	SPAException(const std::string& reason);
	const char* what() const noexcept override;
};

class LexerException : public SPAException {
public:
	static const std::string INVALID_INT;

	LexerException(const std::string& reason);
};

class ParserException : public SPAException {
public:
	static const std::string INVALID_COND_EXPR;
	static const std::string INVALID_REL_EXPR;
	static const std::string INVALID_EXPR;
	static const std::string INVALID_PROG;
	static const std::string INVALID_STMT;
	static const std::string MISSING_ELSE;
	static const std::string MISSING_THEN;
	static const std::string MISSING_LEFT_BRACKET;
	static const std::string MISSING_RIGHT_BRACKET;
	static const std::string MISSING_LEFT_CURLY;
	static const std::string MISSING_RIGHT_CURLY;
	static const std::string MISSING_PROC_KEYWORD;
	static const std::string MISSING_PROC_NAME;
	static const std::string MISSING_SEMICOLON;
	static const std::string MISSING_WHITESPACE;
	static const std::string MISSING_VAR_NAME;

	ParserException(const std::string& reason);
};

class QPSException : public SPAException {
public:
	static const std::string PARSER_ERROR;
	static const std::string VALIDATOR_ERROR;

	QPSException(const std::string& reason);
};

class EvaluatorException : public SPAException {
public:
	static const std::string PATTERN_ERROR;
	static const std::string MODIFIES_S_ERROR;
	static const std::string MODIFIES_P_ERROR;
	static const std::string USES_S_ERROR;
	static const std::string USES_P_ERROR;

	EvaluatorException(const std::string& reason);
};

#endif
