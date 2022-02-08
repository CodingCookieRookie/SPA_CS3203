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
        throw SPAException(std::string("Whitespace expected between two alphanumeric tokens"));
    }
    std::string variableName = lexer.nextName();
    if (variableName.empty()) {
        throw SPAException(std::string("Expected alphanumeric token, got nothing"));
    }
    declarations.emplace_back(variableType, variableName);
    while (lexer.match(",")) {
        // Lookahead 1, expect more tokens
        variableName = lexer.nextName();
        if (variableName.empty()) {
            throw SPAException(std::string("Expected alphanumeric token, got nothing"));
        }
        declarations.emplace_back(variableType, variableName);
    }
    if (!lexer.match(";")) {
        throw SPAException(std::string("Expected \";\", got nothing"));
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
        throw SPAException(std::string("Expected \"Select\", got nothing"));
    }
    std::string whitespace = lexer.nextWhitespace();
    if (whitespace.empty()) {
        throw SPAException(std::string("Whitespace expected between two alphanumeric tokens"));
    }
    std::vector<std::string> columns;
    std::string columnVariable = lexer.nextName();
    if (columnVariable.empty()) {
        throw SPAException(std::string("Expected alphanumeric token, got nothing"));
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
        return std::vector<ParsedRelationship>();
    }
    if (!lexer.match("that")) {
        return std::vector<ParsedRelationship>();
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
        // throw exception
    }
    if (!lexer.match("(")) {
        // throw exception
    }
    PqlReference lValue = parseRef();
    if (!lexer.match(",")) {
        // throw exception
    }
    PqlReference rValue = parseRef();
    if (!lexer.match(")")) {
        // throw exception
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
        // Throw exception
    }
    std::string synAssign = lexer.nextName();
    if (synAssign.empty()) {
    
    }
    if (!lexer.match("(")) {
    
    }
    PqlReference ref = parseRef();
    if (!lexer.match(",")) {
    }
    PqlExpression expression = parseExpression();
    if (!lexer.match(")")) {
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
            // throw exception
        }
        if (!lexer.match("\"")) {
            // throw exception
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
    // throw exception
}

PqlExpression PQLParser::parseExpression() {
    if (!lexer.match("_")) {
        // Throw exception
    }
    if (!lexer.match("\"")) {
        return PqlExpression(PqlExpressionType::wildcard, std::string());
    }
    std::string factor = lexer.nextName();
    if (factor.empty()) {
        // Throw exception
    }
    if (!lexer.match("\"")) {

    }
    if (!lexer.match("_")) {

    }
    return PqlExpression(PqlExpressionType::partial, factor);
}

ParsedQuery PQLParser::parseQuery() {
    std::vector<PQL_VARIABLE> allDeclarations = parseDeclarations();
    std::vector<std::string> columns = parseSelect();
    std::vector<ParsedRelationship> relationships = parseSuchThat();
    std::vector<ParsedPattern> patterns = parsePattern();
    if (!lexer.reachedEnd()) {
        throw SPAException(std::string("Extra characters at end of query"));
    }
    return ParsedQuery(allDeclarations, columns, relationships, patterns);
}
