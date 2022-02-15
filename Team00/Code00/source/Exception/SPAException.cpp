#include "SPAException.h"

SPAException::SPAException(const std::string& reason) : reason(reason) {}

const char* SPAException::what() const noexcept {
    return reason.c_str();
}

/* LexerException */
const std::string LexerException::INVALID_INT = "Integer cannot start with 0";

LexerException::LexerException(const std::string& reason) : SPAException(reason) {}

/* ParserException */
const std::string ParserException::INVALID_EXPR = "Expression is invalid";
const std::string ParserException::INVALID_PROG = "A program must have at least 1 procedure";
const std::string ParserException::INVALID_STMT = "Invalid statement (stmt : read | print | call | while | if | assign)";
const std::string ParserException::MISSING_LEFT_CURLY = "Left curly bracket is missing";
const std::string ParserException::MISSING_RIGHT_CURLY = "Right curly bracket is missing";
const std::string ParserException::MISSING_PROC_KEYWORD = "Procedure keyword is missing";
const std::string ParserException::MISSING_PROC_NAME = "Procedure name is missing";
const std::string ParserException::MISSING_SEMICOLON = "Semicolon is missing";
const std::string ParserException::MISSING_VAR_NAME = "Variable name is missing";
const std::string ParserException::MISSING_WHITESPACE = "Whitespace is missing";

ParserException::ParserException(const std::string& reason) : SPAException(reason) {}

/* QPSException */
const std::string QPSException::PARSER_ERROR = "Error occurred while parsing PQL query";
const std::string QPSException::VALIDATOR_ERROR = "Error occurred while validating semantics of PQL query";

QPSException::QPSException(const std::string& reason) : SPAException(reason) {}