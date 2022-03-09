#include "ParsedQuery.h"

const std::unordered_set<PqlReferenceType>
	ParsedQuery::stmtRef = { PqlReferenceType::synonym, PqlReferenceType::wildcard, PqlReferenceType::integer };
const std::unordered_set<PqlReferenceType>
	ParsedQuery::entRef = { PqlReferenceType::synonym, PqlReferenceType::wildcard, PqlReferenceType::ident };
const std::unordered_set<PqlReferenceType>
	ParsedQuery::stmtRefNoWildcard = { PqlReferenceType::synonym, PqlReferenceType::integer };
const std::unordered_set<PqlReferenceType>
	ParsedQuery::entRefNoWildcard = { PqlReferenceType::synonym, PqlReferenceType::ident };

const std::unordered_set<PqlEntityType> ParsedQuery::stmtEntities = {
	PqlEntityType::Assign, PqlEntityType::Call, PqlEntityType::If, PqlEntityType::Print,
	PqlEntityType::Read, PqlEntityType::Stmt, PqlEntityType::While
};
const std::unordered_set<PqlEntityType> ParsedQuery::varEntities = { PqlEntityType::Variable };
const std::unordered_set<PqlEntityType> ParsedQuery::procEntities = { PqlEntityType::Procedure };

const std::unordered_map<PqlRelationshipType,
	std::vector<std::pair<PqlRelationshipType, std::unordered_set<PqlReferenceType>>>>
	ParsedQuery::validLeftArgs = {
	{ PqlRelationshipType::Modifies, { { PqlRelationshipType::ModifiesS, ParsedQuery::stmtRefNoWildcard },
									   { PqlRelationshipType::ModifiesP, ParsedQuery::entRefNoWildcard } } },
	{ PqlRelationshipType::Uses, { { PqlRelationshipType::UsesS, ParsedQuery::stmtRefNoWildcard },
									   { PqlRelationshipType::UsesP, ParsedQuery::entRefNoWildcard } } },
	{ PqlRelationshipType::Follows, { { PqlRelationshipType::Follows, ParsedQuery::stmtRef } } },
	{ PqlRelationshipType::FollowsT, { { PqlRelationshipType::FollowsT, ParsedQuery::stmtRef } } },
	{ PqlRelationshipType::Parent, { { PqlRelationshipType::Parent, ParsedQuery::stmtRef } } },
	{ PqlRelationshipType::ParentT, { { PqlRelationshipType::ParentT, ParsedQuery::stmtRef } } },
};
const std::unordered_map<PqlRelationshipType, std::unordered_set<PqlReferenceType>>
	ParsedQuery::validRightArgs = {
	{ PqlRelationshipType::Modifies, ParsedQuery::entRef },
	{ PqlRelationshipType::Uses, ParsedQuery::entRef },
	{ PqlRelationshipType::Follows, ParsedQuery::stmtRef },
	{ PqlRelationshipType::FollowsT, ParsedQuery::stmtRef },
	{ PqlRelationshipType::Parent, ParsedQuery::stmtRef },
	{ PqlRelationshipType::ParentT, ParsedQuery::stmtRef },
};

const std::unordered_map<PqlRelationshipType, std::unordered_set<PqlEntityType>>
	ParsedQuery::validLeftDesignEntities = {
	{ PqlRelationshipType::ModifiesS, ParsedQuery::stmtEntities },
	{ PqlRelationshipType::ModifiesP, ParsedQuery::procEntities },
	{ PqlRelationshipType::UsesS, ParsedQuery::stmtEntities },
	{ PqlRelationshipType::UsesP, ParsedQuery::procEntities },
	{ PqlRelationshipType::Follows, ParsedQuery::stmtEntities },
	{ PqlRelationshipType::FollowsT, ParsedQuery::stmtEntities },
	{ PqlRelationshipType::Parent, ParsedQuery::stmtEntities },
	{ PqlRelationshipType::ParentT, ParsedQuery::stmtEntities },
};
const std::unordered_map<PqlRelationshipType, std::unordered_set<PqlEntityType>>
	ParsedQuery::validRightDesignEntities = {
	{ PqlRelationshipType::Modifies, ParsedQuery::varEntities },
	{ PqlRelationshipType::Uses, ParsedQuery::varEntities },
	{ PqlRelationshipType::Follows, ParsedQuery::stmtEntities },
	{ PqlRelationshipType::FollowsT, ParsedQuery::stmtEntities },
	{ PqlRelationshipType::Parent, ParsedQuery::stmtEntities },
	{ PqlRelationshipType::ParentT, ParsedQuery::stmtEntities },
};

bool ParsedQuery::isDeclared(const std::string& synonym) {
	return declarations.find(synonym) != declarations.end();
}

PqlEntityType ParsedQuery::getType(std::string& synonym) {
	return declarations.at(synonym);
}


bool ParsedQuery::validateEntRefVar(PqlReference ref) {
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
		PqlRelationshipType relationshipType = relationship.getRelationshipType();
		PqlReference lhs = relationship.getLhs();
		PqlReference rhs = relationship.getRhs();
		PqlReferenceType lhsType = lhs.first;
		PqlReferenceType rhsType = rhs.first;

		/* If we haven't implemented a certain relationship, throw an exception
		TODO: remove once all relationships have been implemented */
		if (validRightArgs.find(relationshipType) == validRightArgs.end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		/* Type validate the right argument */
		if (validRightArgs.at(relationshipType).find(rhsType) == validRightArgs.at(relationshipType).end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		/* If the right argument is a synonym, perform further
		type validation on the design entity */
		if (rhsType == PqlReferenceType::synonym) {
			if (validRightDesignEntities.find(relationshipType) == validRightDesignEntities.end()) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
			std::string& rhsName = rhs.second;
			if (!isDeclared(rhsName)) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
			PqlEntityType rhsEntity = getType(rhsName);
			if (validRightDesignEntities.at(relationshipType).find(rhsEntity) == validRightDesignEntities.at(relationshipType).end()) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
		}

		/* If we haven't implemented a certain relationship, throw an exception
		TODO: remove once all relationships have been implemented */
		if (validLeftArgs.find(relationshipType) == validLeftArgs.end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		bool foundRelationship = false;
		for (const std::pair<PqlRelationshipType, std::unordered_set<PqlReferenceType>>& validRefs : validLeftArgs.at(relationshipType)) {
			PqlRelationshipType reassignType = validRefs.first;
			const std::unordered_set<PqlReferenceType>& validRefTypes = validRefs.second;
			/* Type validate the left argument */
			if (validRefTypes.find(lhsType) == validRefTypes.end()) {
				continue;
			}
			/* If the right argument is a synonym, perform further
			type validation on the design entity */
			if (lhsType == PqlReferenceType::synonym) {
				/* If we haven't implemented a certain relationship, throw an exception
				TODO: remove once all relationships have been implemented */
				if (validLeftDesignEntities.find(reassignType) == validLeftDesignEntities.end()) {
					throw QPSException(QPSException::VALIDATOR_ERROR);
				}
				std::string& lhsName = lhs.second;
				if (!isDeclared(lhsName)) {
					throw QPSException(QPSException::VALIDATOR_ERROR);
				}
				PqlEntityType lhsEntity = getType(lhsName);
				if (validLeftDesignEntities.at(reassignType).find(lhsEntity) == validLeftDesignEntities.at(reassignType).end()) {
					continue;
				}
			}
			/* If the synonym has been previously declared, and it is of
			the correct design entity, add it to the list of relationships */
			relationships.push_back(ParsedRelationship(reassignType, lhs, rhs));
			foundRelationship = true;
			break;
		}
		if (!foundRelationship) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
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
		if (!validateEntRefVar(ref)) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		patterns.push_back(pattern);
	}
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

bool ParsedQuery::isStmtSubtype(PqlReference ref) {
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
