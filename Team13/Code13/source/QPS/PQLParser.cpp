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
	{"Uses",		PqlRelationshipType::Uses},
	{"Next*",		PqlRelationshipType::NextT},
	{"Next",		PqlRelationshipType::Next},
	{"Calls*",		PqlRelationshipType::CallsT},
	{"Calls",		PqlRelationshipType::Calls},
	{"Affects*",	PqlRelationshipType::AffectsT},
	{"Affects",		PqlRelationshipType::Affects},
};

const std::vector<std::pair<std::string, PqlReferenceType>> PQLParser::attributeMap = {
	{"procName",	PqlReferenceType::procName},
	{"varName",		PqlReferenceType::varName},
	{"value",		PqlReferenceType::value},
	{"stmt#",		PqlReferenceType::stmtNum},
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

PqlReference PQLParser::parseElem() {
	std::string synonym = lexer.nextName();
	if (synonym.empty()) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	std::string whitespace = lexer.nextWhitespace();
	if (!lexer.match(".")) {
		return PqlReference(PqlReferenceType::synonym, synonym);
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

std::vector<PqlReference> PQLParser::parseSelect() {
	std::vector<PqlReference> elems;
	if (!lexer.match("Select")) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	std::string whitespace = lexer.nextWhitespace();
	if (lexer.match("<")) {
		do {
			PqlReference elem = parseElem();
			elems.push_back(elem);
		} while (lexer.match(","));
		if (!lexer.match(">")) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		return elems;
	}
	if (whitespace.empty()) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	if (lexer.match("BOOLEAN")) {
		whitespace = lexer.nextWhitespace();
		bool validDelimiter = !whitespace.empty() || lexer.reachedEnd();
		if (!validDelimiter) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		return elems;
	}
	PqlReference elem = parseElem();
	elems.push_back(elem);
	return elems;
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
	return ParsedRelationship(relationshipType, lValue, rValue);
}

std::vector<ParsedRelationship> PQLParser::parseSuchThat() {
	std::vector<ParsedRelationship> suchThats;
	do {
		ParsedRelationship suchThat = parseSingleSuchThat();
		suchThats.push_back(suchThat);
	} while (lexer.match("and"));
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
	if (!lexer.match("(")) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	int numOfArgs = 0;
	PqlReference ref = parseRef();
	numOfArgs++;
	if (!lexer.match(",")) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	PqlExpression expression = parseExpression();
	numOfArgs++;
	if (lexer.match(")")) {
		return ParsedPattern(synonym, ref, expression, numOfArgs);
	}
	if (!lexer.match(",")) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	if (!lexer.match("_")) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	numOfArgs++;
	if (!lexer.match(")")) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	return ParsedPattern(synonym, ref, expression, numOfArgs);
}

std::vector<ParsedPattern> PQLParser::parsePattern() {
	std::vector<ParsedPattern> patterns;
	do {
		ParsedPattern pattern = parseSinglePattern();
		patterns.push_back(pattern);
	} while (lexer.match("and"));
	return patterns;
}

ParsedWith PQLParser::parseSingleWith() {
	std::string whitespace = lexer.nextWhitespace();
	if (whitespace.empty()) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	PqlReference lhs = parseRef();
	if (lhs.first == PqlReferenceType::synonym) {
		if (!lexer.match(".")) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		bool foundAttr = false;
		/* Re-assign the reference based on the attrName */
		for (const std::pair<std::string, PqlReferenceType>& attribute : attributeMap) {
			std::string attrName = attribute.first;
			if (lexer.match(attrName)) {
				lhs = PqlReference(attribute.second, lhs.second);
				foundAttr = true;
				break;
			}
		}
		if (!foundAttr) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
	}
	if (!lexer.match("=")) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	PqlReference rhs = parseRef();
	if (rhs.first == PqlReferenceType::wildcard) {
		throw QPSException(QPSException::PARSER_ERROR);
	}
	if (rhs.first == PqlReferenceType::synonym) {
		if (!lexer.match(".")) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
		bool foundAttr = false;
		/* Re-assign the reference based on the attrName */
		for (const std::pair<std::string, PqlReferenceType>& attribute : attributeMap) {
			std::string attrName = attribute.first;
			if (lexer.match(attrName)) {
				/* Additional check -- if the last character of the attrName is an
				alphanumeric character, and there is no space following it and we have
				not reached the end of the lexed string, throw a parser exception.
				This is because if we have not reached the end of the string, the
				next token will be either "and", "such", "pattern" or "with". */
				std::string whitespace = lexer.nextWhitespace();
				if (isalpha(attrName.back()) && whitespace.empty() && !lexer.reachedEnd()) {
					throw QPSException(QPSException::PARSER_ERROR);
				}
				rhs = PqlReference(attribute.second, rhs.second);
				foundAttr = true;
				break;
			}
		}
		if (!foundAttr) {
			throw QPSException(QPSException::PARSER_ERROR);
		}
	}
	return ParsedWith(lhs, rhs);
}

std::vector<ParsedWith> PQLParser::parseWith() {
	std::vector<ParsedWith> withs;
	do {
		ParsedWith with = parseSingleWith();
		withs.push_back(with);
	} while (lexer.match("and"));
	return withs;
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
	std::vector<PqlReference> columns = parseSelect();
	std::vector<ParsedRelationship> relationships;
	std::vector<ParsedPattern> patterns;
	std::vector<ParsedWith> withs;
	while (!lexer.reachedEnd()) {
		if (lexer.match("such")) {
			std::string whitespace = lexer.nextWhitespace();
			if (whitespace.empty()) {
				throw QPSException(QPSException::PARSER_ERROR);
			}
			if (!lexer.match("that")) {
				throw QPSException(QPSException::PARSER_ERROR);
			}
			std::vector<ParsedRelationship> parsedRelationships = parseSuchThat();
			for (ParsedRelationship parsedRelationship : parsedRelationships) {
				relationships.push_back(parsedRelationship);
			}
		} else if (lexer.match("pattern")) {
			std::vector<ParsedPattern> parsedPatterns = parsePattern();
			for (ParsedPattern parsedPattern : parsedPatterns) {
				patterns.push_back(parsedPattern);
			}
		} else if (lexer.match("with")) {
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
	return ParsedQuery(allDeclarations, columns, relationships, patterns, withs);
}
