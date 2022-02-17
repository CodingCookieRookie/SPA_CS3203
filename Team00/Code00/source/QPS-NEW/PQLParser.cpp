#include "PQLParser.h"

// TODO: Make this a static member of PQLParser
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
        // These should throw exceptions
        throw QPSException(QPSException::PARSER_ERROR);
    }
    if (!lexer.match("that")) {
        throw QPSException(QPSException::PARSER_ERROR);
    }
    PqlRelationshipType relationshipType;
    if (lexer.match("Follows*")) {
        relationshipType = PqlRelationshipType::FollowsT;
    }else if (lexer.match("Follows")) {
        relationshipType = PqlRelationshipType::Follows;
    } else if (lexer.match("Parent*")) {
        relationshipType = PqlRelationshipType::ParentT;
    } else if (lexer.match("Parent")) {
        relationshipType = PqlRelationshipType::Parent;
    } else if (lexer.match("Modifies")) {
        relationshipType = PqlRelationshipType::Modifies;
    } else if (lexer.match("Uses")) {
        relationshipType = PqlRelationshipType::Uses;
    } else {
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
    /* Note: Basic PQL only allows subexpression match, in advanced PQL, we can start with either a _ or " */
    if (!lexer.match("_")) {
        throw QPSException(QPSException::PARSER_ERROR);
    }
    if (!lexer.match("\"")) {
        return PqlExpression(PqlExpressionType::wildcard, std::string());
    }
    std::string factor = lexer.nextName();
    if (factor.empty()) {
        throw QPSException(QPSException::PARSER_ERROR);
    }
    if (!lexer.match("\"")) {
        throw QPSException(QPSException::PARSER_ERROR);
    }
    if (!lexer.match("_")) {
        throw QPSException(QPSException::PARSER_ERROR);
    }
    return PqlExpression(PqlExpressionType::partial, factor);
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
