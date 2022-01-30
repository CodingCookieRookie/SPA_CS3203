#include "PQLParser.h"

// TODO: Make this a static member of PQLParser
std::vector<std::pair<std::string, PQL_VARIABLE_TYPE>> designEntityMap = {
	{"stmt",		PQL_VARIABLE_TYPE::STMT},
	{"read",		PQL_VARIABLE_TYPE::READ},
	{"print",		PQL_VARIABLE_TYPE::PRINT},
	{"call",		PQL_VARIABLE_TYPE::CALL},
	{"while",		PQL_VARIABLE_TYPE::WHILE},
	{"if",			PQL_VARIABLE_TYPE::IF},
	{"assign",		PQL_VARIABLE_TYPE::ASSIGN},
	{"variable",	PQL_VARIABLE_TYPE::VARIABLE},
	{"constant",	PQL_VARIABLE_TYPE::CONSTANT},
	{"procedure",	PQL_VARIABLE_TYPE::PROCEDURE}
};

PQLParser::PQLParser(const std::string& query) : lexer(query) {}

std::vector<PQL_VARIABLE> PQLParser::parseSingleDeclaration() {
	PQL_VARIABLE_TYPE variableType;
	std::vector<PQL_VARIABLE> declarations;
	bool matchedEntity = false;
	for (const std::pair<std::string, PQL_VARIABLE_TYPE>& designEntity : designEntityMap) {
		std::string designEntityString = designEntity.first;
		if (lexer.match(designEntityString)) {
			variableType = designEntity.second;
			matchedEntity = true;
			break;
		}
	}
	if (!matchedEntity) {
		return std::vector<PQL_VARIABLE>();
	}
	std::string whitespace = lexer.nextWhitespace();
	if (whitespace.empty()) {
		// Expected whitespace between two alphanumeric tokens -> throw exception
		return std::vector<PQL_VARIABLE>();
	}
	std::string variableName = lexer.nextName();
	if (variableName.empty()) {
		// Expected variable name, got nothing -> throw exception
		return std::vector<PQL_VARIABLE>();
	}
	declarations.emplace_back(variableType, variableName);
	while (lexer.match(",")) {
		// Lookahead 1, expect more tokens
		variableName = lexer.nextName();
		if (variableName.empty()) {
			// Expected variable name, got nothing -> throw exception
			return std::vector<PQL_VARIABLE>();
		}
		declarations.emplace_back(variableType, variableName);
	}
	if (!lexer.match(";")) {
		// Expected end of declaration ";" but got nothing -> throw exception
		return std::vector<PQL_VARIABLE>();
	}
	return declarations;
}

std::vector<PQL_VARIABLE> PQLParser::parseDeclarations() {
	std::vector<PQL_VARIABLE> allDeclarations;
	std::vector<PQL_VARIABLE> singleDeclaration;
	while (singleDeclaration = parseSingleDeclaration(), !singleDeclaration.empty()) {
		for (const PQL_VARIABLE& variable : singleDeclaration) {
			allDeclarations.push_back(variable);
		}
	}
	return allDeclarations;
}

std::vector<std::string> PQLParser::parseSelect() {
	if (!lexer.match("Select")) {
		// Expected "Select", but got nothing -> throw exception
		return std::vector<std::string>();
	}
	std::string whitespace = lexer.nextWhitespace();
	if (whitespace.empty()) {
		// Expected whitespace between two alphanumeric tokens -> throw exception
		return std::vector<std::string>();
	}
	std::vector<std::string> columns;
	std::string columnVariable = lexer.nextName();
	if (columnVariable.empty()) {
		// Expected column, got nothing -> throw exception
		return std::vector<std::string>();
	}
	columns.push_back(columnVariable);
	return columns;
}

ParsedQuery PQLParser::parseQuery() {
	std::vector<PQL_VARIABLE> allDeclarations = parseDeclarations();
	std::vector<std::string> columns = parseSelect();
	return ParsedQuery(allDeclarations, columns);
}
