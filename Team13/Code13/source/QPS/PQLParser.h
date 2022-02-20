#pragma once
#include <string>
#include <vector>

#include "../common/Lexer.h"
#include "QPSCommons.h"
#include "ParsedQuery.h"

class PQLParser {
private:
	/* Prevent instantiation of a PQLParser object */
	PQLParser();

	const static std::vector<std::pair<std::string, PqlEntityType>> designEntityMap;
	static Lexer lexer;

	static std::vector<PQL_VARIABLE> parseSingleDeclaration();
	static std::vector<PQL_VARIABLE> parseDeclarations();
	static std::vector<std::string> parseSelect();
	static std::vector<ParsedRelationship> parseSuchThat();
	static std::vector<ParsedPattern> parsePattern();
	static PqlReference parseRef();
	static PqlExpression parseExpression();

public:
	/* Main entry method for parsing the query */
	static ParsedQuery parseQuery(const std::string& query);
};
