#include "SPAException.h"

SPAException::SPAException(const std::string& reason) : reason(reason) {}

const char* SPAException::what() const noexcept {
	return reason.c_str();
}

/* LexerException */
const std::string LexerException::INVALID_INT = "Integer cannot start with 0";

LexerException::LexerException(const std::string& reason) : SPAException(reason) {}

/* ParserException */
const std::string ParserException::INVALID_COND_EXPR = "Conditional expression is invalid";
const std::string ParserException::INVALID_REL_EXPR = "Relational expression is invalid";
const std::string ParserException::INVALID_EXPR = "Expression is invalid";
const std::string ParserException::INVALID_PROG = "A program must have at least 1 procedure";
const std::string ParserException::INVALID_STMT = "Invalid statement (stmt : read | print | call | while | if | assign)";
const std::string ParserException::MISSING_THEN = "Then keyword is missing";
const std::string ParserException::MISSING_ELSE = "Else keyword is missing";
const std::string ParserException::MISSING_LEFT_BRACKET = "Left bracket is missing";
const std::string ParserException::MISSING_RIGHT_BRACKET = "Right bracket is missing";
const std::string ParserException::MISSING_LEFT_CURLY = "Left curly bracket is missing";
const std::string ParserException::MISSING_RIGHT_CURLY = "Right curly bracket is missing";
const std::string ParserException::MISSING_PROC_KEYWORD = "Procedure keyword is missing";
const std::string ParserException::MISSING_PROC_NAME = "Procedure name is missing";
const std::string ParserException::MISSING_PROC_NAME_IN_CALL_STMT = "Procedure name in call stmt is missing";
const std::string ParserException::MISSING_SEMICOLON = "Semicolon is missing";
const std::string ParserException::MISSING_VAR_NAME = "Variable name is missing";
const std::string ParserException::MISSING_WHITESPACE = "Whitespace is missing";

ParserException::ParserException(const std::string& reason) : SPAException(reason) {}

/* ASTException */
const std::string ASTException::DUPLICATE_PROC_NAMES_ERROR = "A program cannot have two procedures with the same name";
const std::string ASTException::CALL_NON_EXISTENT_PROC_NAME_ERROR = "A procedure cannot call a non-existing procedure";
const std::string ASTException::CYCLIC_CALL_ERROR = "Recursive and cyclic calls are not allowed";
ASTException::ASTException(const std::string& reason) : SPAException(reason) {}

/* QPSException */
const std::string QPSException::PARSER_ERROR = "Syntax error detected while parsing PQL query";
const std::string QPSException::VALIDATOR_ERROR = "PQL query failed validator check";

QPSException::QPSException(const std::string& reason) : SPAException(reason) {}

/* EvaluatorException */

const std::string EvaluatorException::PATTERN_ERROR = "Invalid expression type for Pattern";
const std::string EvaluatorException::MODIFIES_S_ERROR = "Invalid handling of ModifiesS query";
const std::string EvaluatorException::MODIFIES_P_ERROR = "Invalid handling of ModifiesP query";
const std::string EvaluatorException::USES_S_ERROR = "Invalid handling of UsesS query";
const std::string EvaluatorException::USES_P_ERROR = "Invalid handling of UsesP query";

EvaluatorException::EvaluatorException(const std::string& reason) : SPAException(reason) {}

/* ExpressionException */
const std::string ExpressionException::INVALID_EXPR = "Expression is invalid";
ExpressionException::ExpressionException(const std::string& reason) : SPAException(reason) {}
