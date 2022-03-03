#include "PQLParser.h"

const std::vector<std::pair<std::string, PqlEntityType>> PQLParser::designEntityMap = {
	{"stmt",		PqlEntityType::Stmt},
	{"read",		PqlEntityType::Read},
	{"print",		PqlEntityType::Print},
	{"call",		PqlEntityType::Call},
	{"while",		PqlEntityType::While},
	{"if",			PqlEntityType::If},
	{"assign",		PqlEntityType::Assign},
	{"variable",	PqlEntityType::Variable},
	{"constant",	PqlEntityType::Constant},
	{"procedure",	PqlEntityType::Procedure}
};

const std::vector<std::pair<std::string, PqlRelationshipType>> PQLParser::suchThatRelationshipMap = {
	{"Follows*",	PqlRelationshipType::FollowsT},
	{"Follows",		PqlRelationshipType::Follows},
	{"Parent*",		PqlRelationshipType::ParentT},
	{"Parent",		PqlRelationshipType::Parent},
	{"Modifies",	PqlRelationshipType::Modifies},
	{"Uses",		PqlRelationshipType::Uses}
};

Lexer PQLParser::lexer = Lexer();

std::vector<PQL_VARIABLE> PQLParser::parseSingleDeclaration() {
	PqlEntityType variableType;
	std::vector<PQL_VARIABLE> declarations;
	bool matchedEntity = false;
	for (const std::pair<std::string, PqlEntityType>& designEntity : designEntityMap) {
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
		throw QPSException(QPSException::PARSER_ERROR);
	}
	std::string variableName = lexer.nextName();
	if (variableName.empty()) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	declarations.emplace_back(variableType, variableName);
	while (lexer.match(",")) {
		// Lookahead 1, expect more tokens
		variableName = lexer.nextName();
		if (variableName.empty()) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		declarations.emplace_back(variableType, variableName);
	}
	if (!lexer.match(";")) {
		throw QPSException(QPSException::PARSER_ERROR);
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
		throw QPSException(QPSException::PARSER_ERROR);
	}
	std::string whitespace = lexer.nextWhitespace();
	if (whitespace.empty()) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	std::vector<std::string> columns;
	std::string columnVariable = lexer.nextName();
	if (columnVariable.empty()) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	columns.push_back(columnVariable);
	return columns;
}

std::vector<ParsedRelationship> PQLParser::parseSuchThat() {
	if (!lexer.match("such")) {
		// Not a such-that clause, return empty vector
		return std::vector<ParsedRelationship>();
	}
	std::string whitespace = lexer.nextWhitespace();
	if (whitespace.empty()) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	if (!lexer.match("that")) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	PqlRelationshipType relationshipType;
	bool matchedRelationship = false;
	for (const std::pair<std::string, PqlRelationshipType>& relationship : suchThatRelationshipMap) {
		std::string relationshipString = relationship.first;
		if (lexer.match(relationshipString)) {
			relationshipType = relationship.second;
			matchedRelationship = true;
			break;
		}
	}
	if (!matchedRelationship) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	if (!lexer.match("(")) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	PqlReference lValue = parseRef();
	if (!lexer.match(",")) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	PqlReference rValue = parseRef();
	if (!lexer.match(")")) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	ParsedRelationship relationship(relationshipType, lValue, rValue);
	return { relationship };
}

std::vector<ParsedPattern> PQLParser::parsePattern() {
	if (!lexer.match("pattern")) {
		return std::vector<ParsedPattern>();
	}
	std::string whitespace = lexer.nextWhitespace();
	if (whitespace.empty()) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	std::string synAssign = lexer.nextName();
	if (synAssign.empty()) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	if (!lexer.match("(")) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	PqlReference ref = parseRef();
	if (!lexer.match(",")) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	PqlExpression expression = parseExpression();
	if (!lexer.match(")")) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	return { ParsedPattern(synAssign, ref, expression) };
}

PqlReference PQLParser::parseRef() {
	if (lexer.match("_")) {
		return PqlReference(PqlReferenceType::wildcard, std::string());
	}
	if (lexer.match("\"")) {
		std::string ident = lexer.nextName();
		if (ident.empty()) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		if (!lexer.match("\"")) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		return PqlReference(PqlReferenceType::ident, ident);
	}
	std::string nextToken = lexer.nextInteger();
	if (!nextToken.empty()) {
		return PqlReference(PqlReferenceType::integer, nextToken);
	}
	nextToken = lexer.nextName();
	if (!nextToken.empty()) {
		return PqlReference(PqlReferenceType::synonym, nextToken);
	}
	throw QPSException(QPSException::PARSER_ERROR);
}

PqlExpression PQLParser::parseExpression() {
	if (lexer.match("_")) {
		if (!lexer.match("\"")) {
			return PqlExpression(PqlExpressionType::wildcard, std::string());
		}
		ExprNode* expr = ExpressionParser::matchExpr(lexer);
		std::string pattern = expr->getPattern();
		delete expr;
		if (!lexer.match("\"")) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		if (!lexer.match("_")) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		return PqlExpression(PqlExpressionType::partial, pattern);
	}
	/* If the next token is not an _ or ", there is a syntax error */
	if (!lexer.match("\"")) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	ExprNode* expr = ExpressionParser::matchExpr(lexer);
	std::string pattern = expr->getPattern();
	delete expr;
	if (!lexer.match("\"")) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	return PqlExpression(PqlExpressionType::full, pattern);
}

ParsedQuery PQLParser::parseQuery(const std::string& query) {
	lexer = Lexer(query);
	std::vector<PQL_VARIABLE> allDeclarations = parseDeclarations();
	std::vector<std::string> columns = parseSelect();
	std::vector<ParsedRelationship> relationships = parseSuchThat();
	std::vector<ParsedPattern> patterns = parsePattern();
	if (!lexer.reachedEnd()) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	return ParsedQuery(allDeclarations, columns, relationships, patterns);
}
