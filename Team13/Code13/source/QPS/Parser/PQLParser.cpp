#include "PQLParser.h"

const std::vector<std::pair<std::string, EntityType>> PQLParser::designEntityMap = {
	{"stmt",		EntityType::STMT},
	{"read",		EntityType::READ},
	{"print",		EntityType::PRINT},
	{"call",		EntityType::CALL},
	{"while",		EntityType::WHILE},
	{"if",			EntityType::IF},
	{"assign",		EntityType::ASSIGN},
	{"variable",	EntityType::VARIABLE},
	{"constant",	EntityType::CONSTANT},
	{"procedure",	EntityType::PROCEDURE}
};

const std::vector<std::pair<std::string, PqlRelationshipType>> PQLParser::suchThatRelationshipMap = {
	{"Follows*",	PqlRelationshipType::FOLLOWS_T},
	{"Follows",		PqlRelationshipType::FOLLOWS},
	{"Parent*",		PqlRelationshipType::PARENT_T},
	{"Parent",		PqlRelationshipType::PARENT},
	{"Modifies",	PqlRelationshipType::MODIFIES},
	{"Uses",		PqlRelationshipType::USES},
	{"Next*",		PqlRelationshipType::NEXT_T},
	{"Next",		PqlRelationshipType::NEXT},
	{"Calls*",		PqlRelationshipType::CALLS_T},
	{"Calls",		PqlRelationshipType::CALLS},
	{"Affects*",	PqlRelationshipType::AFFECTS_T},
	{"Affects",		PqlRelationshipType::AFFECTS},
};

const std::vector<std::pair<std::string, PqlReferenceType>> PQLParser::attributeMap = {
	{"procName",	PqlReferenceType::PROC_NAME},
	{"varName",		PqlReferenceType::VAR_NAME},
	{"value",		PqlReferenceType::VALUE},
	{"stmt#",		PqlReferenceType::STMT_NUM},
};

Lexer PQLParser::lexer;

std::vector<PqlEntity> PQLParser::parseSingleDeclaration() {
	EntityType variableType;
	std::vector<PqlEntity> declarations;
	bool matchedEntity = false;
	for (const std::pair<std::string, EntityType>& designEntity : designEntityMap) {
		std::string designEntityString = designEntity.first;
		if (lexer.match(designEntityString)) {
			variableType = designEntity.second;
			matchedEntity = true;
			break;
		}
	}
	if (!matchedEntity) {
		return std::vector<PqlEntity>();
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
	while (lexer.match(Common::COMMMA)) {
		// Lookahead 1, expect more tokens
		variableName = lexer.nextName();
		if (variableName.empty()) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		declarations.emplace_back(variableType, variableName);
	}
	if (!lexer.match(Common::SEMICOLON)) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	return declarations;
}

std::vector<PqlEntity> PQLParser::parseDeclarations() {
	std::vector<PqlEntity> allDeclarations;
	std::vector<PqlEntity> singleDeclaration;
	while (singleDeclaration = parseSingleDeclaration(), !singleDeclaration.empty()) {
		for (const PqlEntity& variable : singleDeclaration) {
			allDeclarations.push_back(variable);
		}
	}
	return allDeclarations;
}

std::pair<std::vector<PqlReference>, bool> PQLParser::parseSelect() {
	bool tupleSelect = false;
	std::vector<PqlReference> elems;
	if (!lexer.match(Common::SELECT)) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	std::string whitespace = lexer.nextWhitespace();
	if (lexer.match(Common::LEFT_ANGLE)) {
		tupleSelect = true;
		do {
			PqlReference elem = parseElem();
			elems.push_back(elem);
		} while (lexer.match(Common::COMMMA));
		if (!lexer.match(Common::RIGHT_ANGLE)) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		return { elems, tupleSelect };
	}
	if (whitespace.empty()) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	PqlReference elem = parseElem();
	elems.push_back(elem);
	return { elems, tupleSelect };
}

ParsedRelationship PQLParser::parseSingleSuchThat() {
	std::string whitespace = lexer.nextWhitespace();
	if (whitespace.empty()) {
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
	if (!lexer.match(Common::LEFT_BRACKET)) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	PqlReference lValue = parseRef();
	if (!lexer.match(Common::COMMMA)) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	PqlReference rValue = parseRef();
	if (!lexer.match(Common::RIGHT_BRACKET)) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	return ParsedRelationship(relationshipType, lValue, rValue);
}

std::vector<ParsedRelationship> PQLParser::parseSuchThat() {
	std::vector<ParsedRelationship> suchThats;
	do {
		ParsedRelationship suchThat = parseSingleSuchThat();
		suchThats.push_back(suchThat);
	} while (lexer.match(Common::AND));
	return suchThats;
}

ParsedPattern PQLParser::parseSinglePattern() {
	std::string whitespace = lexer.nextWhitespace();
	if (whitespace.empty()) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	std::string synonym = lexer.nextName();
	if (synonym.empty()) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	if (!lexer.match(Common::LEFT_BRACKET)) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	int numOfArgs = 0;
	PqlReference ref = parseRef();
	numOfArgs++;
	if (!lexer.match(Common::COMMMA)) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	PqlExpression expression = parseExpression();
	numOfArgs++;
	if (lexer.match(Common::RIGHT_BRACKET)) {
		return ParsedPattern(synonym, ref, expression, numOfArgs);
	}
	if (!lexer.match(Common::COMMMA)) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	if (!lexer.match(Common::UNDERSCORE)) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	numOfArgs++;
	if (!lexer.match(Common::RIGHT_BRACKET)) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	return ParsedPattern(synonym, ref, expression, numOfArgs);
}

std::vector<ParsedPattern> PQLParser::parsePattern() {
	std::vector<ParsedPattern> patterns;
	do {
		ParsedPattern pattern = parseSinglePattern();
		patterns.push_back(pattern);
	} while (lexer.match(Common::AND));
	return patterns;
}

ParsedWith PQLParser::parseSingleWith() {
	std::string whitespace = lexer.nextWhitespace();
	if (whitespace.empty()) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	PqlReference lhs = parseAttr();
	if (!lexer.match(Common::EQUAL)) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	PqlReference rhs = parseAttr();
	if (isalnum(rhs.second.back()) && !validDelimiter()) {
		/* Additional check -- if the last character of the attrName is an
		alphanumeric character, and there is no space following it and we have
		not reached the end of the lexed string, throw a parser exception.
		This is because if we have not reached the end of the string, the
		next token will be either "and", "such", "pattern" or "with". */
		throw QPSException(QPSException::PARSER_ERROR);
	}
	return ParsedWith(lhs, rhs);
}

std::vector<ParsedWith> PQLParser::parseWith() {
	std::vector<ParsedWith> withs;
	do {
		ParsedWith with = parseSingleWith();
		withs.push_back(with);
	} while (lexer.match(Common::AND));
	return withs;
}

PqlReference PQLParser::parseRef() {
	if (lexer.match(Common::UNDERSCORE)) {
		return PqlReference(PqlReferenceType::WILDCARD, std::string());
	}
	if (lexer.match(Common::QUOTATION)) {
		std::string ident = lexer.nextName();
		if (ident.empty()) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		if (!lexer.match(Common::QUOTATION)) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		return PqlReference(PqlReferenceType::IDENT, ident);
	}
	std::string nextToken = lexer.nextInteger();
	if (!nextToken.empty()) {
		return PqlReference(PqlReferenceType::INTEGER, nextToken);
	}
	nextToken = lexer.nextName();
	if (!nextToken.empty()) {
		return PqlReference(PqlReferenceType::SYNONYM, nextToken);
	}
	throw QPSException(QPSException::PARSER_ERROR);
}

PqlReference PQLParser::parseElem() {
	std::string synonym = lexer.nextName();
	if (synonym.empty()) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	std::string whitespace = lexer.nextWhitespace();
	if (!lexer.match(Common::FULLSTOP)) {
		return PqlReference(PqlReferenceType::SYNONYM, synonym);
	}
	bool matchedAttribute = false;
	PqlReferenceType attributeType;
	for (const std::pair<std::string, PqlReferenceType>& attribute : attributeMap) {
		std::string attributeName = attribute.first;
		if (!lexer.match(attributeName)) {
			continue;
		}
		attributeType = attribute.second;
		matchedAttribute = true;
		break;
	}
	if (!matchedAttribute) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	return PqlReference(attributeType, synonym);
}

PqlReference PQLParser::parseAttr() {
	if (lexer.match(Common::QUOTATION)) {
		std::string ident = lexer.nextName();
		if (ident.empty()) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		if (!lexer.match(Common::QUOTATION)) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		return PqlReference(PqlReferenceType::IDENT, ident);
	}
	std::string integer = lexer.nextInteger();
	if (!integer.empty()) {
		return PqlReference(PqlReferenceType::INTEGER, integer);
	}
	std::string synonym = lexer.nextName();
	if (synonym.empty()) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	if (!lexer.match(Common::FULLSTOP)) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	bool matchedAttribute = false;
	PqlReferenceType attributeType;
	for (const std::pair<std::string, PqlReferenceType>& attribute : attributeMap) {
		std::string attributeName = attribute.first;
		if (!lexer.match(attributeName)) {
			continue;
		}
		attributeType = attribute.second;
		matchedAttribute = true;
		break;
	}
	if (!matchedAttribute) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	return PqlReference(attributeType, synonym);
}

PqlExpression PQLParser::parseExpression() {
	if (lexer.match(Common::UNDERSCORE)) {
		if (!lexer.match(Common::QUOTATION)) {
			return PqlExpression(PqlExpressionType::WILDCARD, std::string());
		}
		ExprNode* expr = ExpressionParser::matchExpr(lexer);
		std::string pattern = expr->getPattern();
		delete expr;
		if (!lexer.match(Common::QUOTATION)) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		if (!lexer.match(Common::UNDERSCORE)) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		return PqlExpression(PqlExpressionType::PARTIAL, pattern);
	}
	/* If the next token is not an _ or ", there is a syntax error */
	if (!lexer.match(Common::QUOTATION)) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	ExprNode* expr = ExpressionParser::matchExpr(lexer);
	std::string pattern = expr->getPattern();
	delete expr;
	if (!lexer.match(Common::QUOTATION)) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	return PqlExpression(PqlExpressionType::FULL, pattern);
}

ParsedQuery PQLParser::parseQuery(const std::string& query) {
	lexer = Lexer(query);
	std::vector<PqlEntity> allDeclarations = parseDeclarations();
	auto [columns, tupleSelect] = parseSelect();
	std::vector<ParsedRelationship> relationships;
	std::vector<ParsedPattern> patterns;
	std::vector<ParsedWith> withs;
	while (!lexer.reachedEnd()) {
		if (lexer.match(Common::SUCH)) {
			std::string whitespace = lexer.nextWhitespace();
			if (whitespace.empty()) {
				throw QPSException(QPSException::PARSER_ERROR);
			}
			if (!lexer.match(Common::THAT)) {
				throw QPSException(QPSException::PARSER_ERROR);
			}
			std::vector<ParsedRelationship> parsedRelationships = parseSuchThat();
			for (ParsedRelationship parsedRelationship : parsedRelationships) {
				relationships.push_back(parsedRelationship);
			}
		} else if (lexer.match(Common::PATTERN)) {
			std::vector<ParsedPattern> parsedPatterns = parsePattern();
			for (ParsedPattern parsedPattern : parsedPatterns) {
				patterns.push_back(parsedPattern);
			}
		} else if (lexer.match(Common::WITH)) {
			std::vector<ParsedWith> parsedWiths = parseWith();
			for (ParsedWith parsedWith : parsedWiths) {
				withs.push_back(parsedWith);
			}
		} else {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		/* Consume additional whitespace in the case of trailing whitespace */
		lexer.nextWhitespace();
	}
	return ParsedQuery(allDeclarations, columns, relationships, patterns, withs, tupleSelect);
}

bool PQLParser::validDelimiter() {
	std::string whitespace = lexer.nextWhitespace();
	return (!whitespace.empty() || lexer.reachedEnd());
}
