#include "ParsedQuery.h"

bool ParsedQuery::isDeclared(const std::string& synonym) {
    return declarations.find(synonym) != declarations.end();
}

PqlEntityType ParsedQuery::getType(std::string& synonym) {
    return declarations.at(synonym);
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
            if (!isStmtRef(lhs)) {
                throw QPSException(QPSException::VALIDATOR_ERROR);
            }
            if (isWildcardRef(lhs)) {
                throw QPSException(QPSException::VALIDATOR_ERROR);
            }
            if (isSynonymRef(lhs)) {
                std::string lhsVarName = lhs.second;
                if (!isDeclared(lhsVarName)) {
                    throw QPSException(QPSException::VALIDATOR_ERROR);
                }
                /* Note: Add the case for ModifiesP in iteration 2 */
                if (!isStmtSynonym(lhs)) {
                    throw QPSException(QPSException::VALIDATOR_ERROR);
                }
            }
            if (!isEntRef(rhs)) {
                throw QPSException(QPSException::VALIDATOR_ERROR);
            }
            if (isSynonymRef(rhs)) {
                std::string rhsVarName = rhs.second;
                if (!isDeclared(rhsVarName)) {
                    throw QPSException(QPSException::VALIDATOR_ERROR);
                }
                if (!isVarSynonym(rhs)) {
                    throw QPSException(QPSException::VALIDATOR_ERROR);
                }
            }
            PqlRelationshipType relationshipType =
                (isStmtRef(lhs) ? PqlRelationshipType::ModifiesS : PqlRelationshipType::ModifiesP);
            relationships.push_back(
                ParsedRelationship(relationshipType, lhs, rhs));
        } else if (isUsesRelationship(relationship)) {
            if (!isStmtRef(lhs)) {
                throw QPSException(QPSException::VALIDATOR_ERROR);
            }
            if (isWildcardRef(lhs)) {
                throw QPSException(QPSException::VALIDATOR_ERROR);
            }
            if (isSynonymRef(lhs)) {
                std::string lhsVarName = lhs.second;
                if (!isDeclared(lhsVarName)) {
                    throw QPSException(QPSException::VALIDATOR_ERROR);
                }
                /* Note: Add the case for ModifiesP in iteration 2 */
                if (!isStmtSynonym(lhs)) {
                    throw QPSException(QPSException::VALIDATOR_ERROR);
                }
            }
            if (!isEntRef(rhs)) {
                throw QPSException(QPSException::VALIDATOR_ERROR);
            }
            if (isSynonymRef(rhs)) {
                std::string rhsVarName = rhs.second;
                if (!isDeclared(rhsVarName)) {
                    throw QPSException(QPSException::VALIDATOR_ERROR);
                }
                if (!isVarSynonym(rhs)) {
                    throw QPSException(QPSException::VALIDATOR_ERROR);
                }
            }
            PqlRelationshipType relationshipType =
                (isStmtRef(lhs) ? PqlRelationshipType::UsesS : PqlRelationshipType::UsesP);
            relationships.push_back(
                ParsedRelationship(relationshipType, lhs, rhs));
        } else {
            /* By default, else handles Parent / T and FollowsT */
            if (!isStmtRef(lhs)) {
                throw QPSException(QPSException::VALIDATOR_ERROR);
            }
            if (isSynonymRef(lhs)) {
                std::string lhsVarName = lhs.second;
                if (!isDeclared(lhsVarName)) {
                    throw QPSException(QPSException::VALIDATOR_ERROR);
                }
                if (!isStmtSynonym(lhs)) {
                    throw QPSException(QPSException::VALIDATOR_ERROR);
                }
            }
            if (!isStmtRef(rhs)) {
                throw QPSException(QPSException::VALIDATOR_ERROR);
            }
            if (isSynonymRef(rhs)) {
                std::string rhsVarName = rhs.second;
                if (!isDeclared(rhsVarName)) {
                    throw QPSException(QPSException::VALIDATOR_ERROR);
                }
                if (!isStmtSynonym(rhs)) {
                    throw QPSException(QPSException::VALIDATOR_ERROR);
                }
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
        PqlReferenceType refType = ref.first;
        if (!isEntRef(ref)) {
            throw QPSException(QPSException::VALIDATOR_ERROR);
        }
        if (isSynonymRef(ref)) {
            if (!isVarSynonym(ref)) {
                throw QPSException(QPSException::VALIDATOR_ERROR);
            }
        }
        patterns.push_back(pattern);
    }
}

bool ParsedQuery::isStmtSynonym(PqlReference ref) {
    std::string& refName = ref.second;
    PqlEntityType entType = declarations.at(refName);
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
    PqlEntityType entType = declarations.at(refName);
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
