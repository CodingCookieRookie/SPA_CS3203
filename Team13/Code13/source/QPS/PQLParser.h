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
	const static std::vector<std::pair<std::string, PqlReferenceType>> attributeMap;
	static Lexer lexer;

	static std::vector<PQL_VARIABLE> parseSingleDeclaration();
	static std::vector<PQL_VARIABLE> parseDeclarations();
	static PqlReference parseElem();
	static std::vector<PqlReference> parseSelect();
	static ParsedRelationship parseSingleSuchThat();
	static std::vector<ParsedRelationship> parseSuchThat();
	static ParsedPattern parseSinglePattern();
	static std::vector<ParsedPattern> parsePattern();
	static ParsedWith parseSingleWith();
	static std::vector<ParsedWith> parseWith();
	static PqlReference parseRef();
	static PqlExpression parseExpression();

public:
	/* Main entry method for parsing the query */
	static ParsedQuery parseQuery(const std::string& query);
};
