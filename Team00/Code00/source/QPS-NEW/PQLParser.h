#pragma once
#include <string>
#include <vector>

#include "QPSCommons.h"
#include "../QPS-NEW/PQLLexer.h"
#include "ParsedQuery.h"

class PQLParser {
private:
	//static const std::vector<std::pair<std::string, PQL_VARIABLE_TYPE>> designEntityMap;
	PQLLexer lexer;
	std::vector<PQL_VARIABLE> parseSingleDeclaration();
	std::vector<PQL_VARIABLE> parseDeclarations();
	std::vector<std::string> parseSelect();
	std::vector<ParsedRelationship> parseSuchThat();
	std::vector<ParsedPattern> parsePattern();
	ParsedRelationship parseFollows();
	ParsedRelationship parseFollowsT();
	ParsedRelationship parseParent();
	ParsedRelationship parseParentT();
	ParsedRelationship parseModifies();
	ParsedRelationship parseUses();
	PqlReference parseRef();

public:

	/* Instiation of a PQLParser that calls a Lexer to lex the query */
	PQLParser(const std::string& query);

	/* Main entry method for parsing the query */
	ParsedQuery parseQuery();
};
