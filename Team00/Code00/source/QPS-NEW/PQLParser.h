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
	PQLParser(const std::string& query);
	ParsedQuery parseQuery();
};
