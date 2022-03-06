#pragma once

#include <string>
#include <vector>

#include "../Common/ExpressionParser.h"
#include "../Common/Lexer.h"
#include "ParsedQuery.h"
#include "QPSCommons.h"

class PQLParser {
private:
	/* Prevent instantiation of a PQLParser object */
	PQLParser();

	const static std::vector<std::pair<std::string, PqlEntityType>> designEntityMap;
	const static std::vector<std::pair<std::string, PqlRelationshipType>> suchThatRelationshipMap;
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
