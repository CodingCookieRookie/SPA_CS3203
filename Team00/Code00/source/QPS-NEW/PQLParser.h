#pragma once
#include <string>
#include <vector>

#include "QPSCommons.h"
#include "Lexer.h"
#include "ParsedQuery.h"

class PQLParser {
private:
	//static const std::vector<std::pair<std::string, PQL_VARIABLE_TYPE>> designEntityMap;
	Lexer lexer;
	std::vector<PQL_VARIABLE> parseSingleDeclaration();
	std::vector<PQL_VARIABLE> parseDeclarations();
	std::vector<std::string> parseSelect();

public:

	/* @brief Instiation of a PQLParser that calls a Lexer to lex the query */
	PQLParser(const std::string& query);

	/* @brief Main entry method for parsing the query */
	ParsedQuery parseQuery();
};
