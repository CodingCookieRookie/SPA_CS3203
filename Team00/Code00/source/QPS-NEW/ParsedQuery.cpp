#include "ParsedQuery.h"

bool ParsedQuery::isDeclared(const std::string& synonym) {
    return declarations.find(synonym) != declarations.end();
}

PqlEntityType ParsedQuery::getType(std::string& synonym) {
    return declarations.at(synonym);
}

bool ParsedQuery::validateStmtRef(PqlReference ref) {
    /* Check grammar rule - stmtRef : synonym | '_' | INTEGER */
    if (!isStmtRef(ref)) {
        return false;
    }
    /* If ref is a synonym, validate that it has type stmt or any of its subtypes.
       This indirectly checks that it has also been previously declared. */
    if (isSynonymRef(ref)) {
        std::string synonymName = ref.second;
        if (!isDeclared(synonymName)) {
            return false;
        }
        if (!isStmtSynonym(ref)) {
            return false;
        }
    }
    return true;
}

bool ParsedQuery::validateEntRef(PqlReference ref) {
    /* Check grammar rule - entRef : synonym | '_' | '"' IDENT '"' */
    if (!isEntRef(ref)) {
        return false;
    }
    /* If ref is a synonym, validate that it has type variable or procedure.
       This indirectly checks that it has also been previously declared. */
    if (isSynonymRef(ref)) {
        std::string synonymName = ref.second;
        if (!isDeclared(synonymName)) {
            return false;
        }
        /* TODO: to add another check for checking procedures */
        if (!isVarSynonym(ref)) {
            return false;
        }
    }
    return true;
}

void ParsedQuery::populateDeclarations(
    const std::vector<PQL_VARIABLE>& allDeclarations) {
    for (const PQL_VARIABLE& variable : allDeclarations) {
        PqlEntityType variableType = variable.first;
        std::string variableName = variable.second;
        if (isDeclared(variableName)) {
            throw QPSException(QPSException::VALIDATOR_ERROR);
        }
        declarations[variableName] = variableType;
    }
}

void ParsedQuery::populateColumns(const std::vector<std::string>& allColumns) {
    for (const std::string& column : allColumns) {
        if (!isDeclared(column)) {
            throw QPSException(QPSException::VALIDATOR_ERROR);
        }
        columns.push_back(column);
    }
}

void ParsedQuery::populateRelationships(const std::vector<ParsedRelationship>& allRelationships) {
    for (const ParsedRelationship& relationship : allRelationships) {
        PqlReference lhs = relationship.getLhs();
        PqlReference rhs = relationship.getRhs();

        if (isModifiesRelationship(relationship)) {
            /* Additional constraint for Modifies/Uses, to avoid ambiguity */
            if (isWildcardRef(lhs)) {
                throw QPSException(QPSException::VALIDATOR_ERROR);
            }
            /* Note: Add the case for ModifiesP in iteration 2 */
            if (!validateStmtRef(lhs)) {
                throw QPSException(QPSException::VALIDATOR_ERROR);
            }
            if (!validateEntRef(rhs)) {
                throw QPSException(QPSException::VALIDATOR_ERROR);
            }
            PqlRelationshipType relationshipType =
                (isStmtRef(lhs) ? PqlRelationshipType::ModifiesS : PqlRelationshipType::ModifiesP);
            relationships.push_back(
                ParsedRelationship(relationshipType, lhs, rhs));
        } else if (isUsesRelationship(relationship)) {
            /* Additional constraint for Modifies/Uses, to avoid ambiguity */
            if (isWildcardRef(lhs)) {
                throw QPSException(QPSException::VALIDATOR_ERROR);
            }
            /* Note: Add the case for UsesP in iteration 2 */
            if (!validateStmtRef(lhs)) {
                throw QPSException(QPSException::VALIDATOR_ERROR);
            }
            if (!validateEntRef(rhs)) {
                throw QPSException(QPSException::VALIDATOR_ERROR);
            }
            PqlRelationshipType relationshipType =
                (isStmtRef(lhs) ? PqlRelationshipType::UsesS : PqlRelationshipType::UsesP);
            relationships.push_back(
                ParsedRelationship(relationshipType, lhs, rhs));
        } else {
            /* By default, else handles Parent/T and Follows/T */
            if (!validateStmtRef(lhs)) {
                throw QPSException(QPSException::VALIDATOR_ERROR);
            }
            if (!validateStmtRef(rhs)) {
                throw QPSException(QPSException::VALIDATOR_ERROR);
            }
            relationships.push_back(relationship);
        }
    }
}

void ParsedQuery::populatePatterns(const std::vector<ParsedPattern>& allPatterns) {
    for (const ParsedPattern& pattern : allPatterns) {
        std::string synAssign = pattern.getSynonym();
        if (!isDeclared(synAssign)) {
            throw QPSException(QPSException::VALIDATOR_ERROR);
        }
        if (getType(synAssign) != PqlEntityType::Assign) {
            throw QPSException(QPSException::VALIDATOR_ERROR);
        }
        PqlReference ref = pattern.getEntRef();
        if (!validateEntRef(ref)) {
            throw QPSException(QPSException::VALIDATOR_ERROR);
        }
        patterns.push_back(pattern);
    }
}

bool ParsedQuery::isStmtSynonym(PqlReference ref) {
    std::string& refName = ref.second;
    PqlEntityType entType = getType(refName);
    return (entType == PqlEntityType::Assign)
        || (entType == PqlEntityType::Call)
        || (entType == PqlEntityType::If)
        || (entType == PqlEntityType::Print)
        || (entType == PqlEntityType::Read)
        || (entType == PqlEntityType::Stmt)
        || (entType == PqlEntityType::While);
}

bool ParsedQuery::isVarSynonym(PqlReference ref) {
    std::string& refName = ref.second;
    PqlEntityType entType = getType(refName);
    return (entType == PqlEntityType::Variable);
}

ParsedQuery::ParsedQuery(const std::vector<PQL_VARIABLE>& allDeclarations,
    const std::vector<std::string>& allColumns,
    const std::vector<ParsedRelationship>& allRelationships,
    const std::vector<ParsedPattern>& allPatterns) {
    populateDeclarations(allDeclarations);
    populateColumns(allColumns);
    populateRelationships(allRelationships);
    populatePatterns(allPatterns);
}

std::unordered_map<std::string, PqlEntityType> ParsedQuery::getDeclarations() {
    return declarations;
}

std::vector<std::string> ParsedQuery::getColumns() {
    return columns;
}

std::vector<ParsedRelationship> ParsedQuery::getRelationships() {
    return relationships;
}

std::vector<ParsedPattern> ParsedQuery::getPatterns() {
    return patterns;
}
