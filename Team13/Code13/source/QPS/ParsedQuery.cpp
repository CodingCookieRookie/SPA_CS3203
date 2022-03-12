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

const std::unordered_set<PqlEntityType> ParsedQuery::assignEntities = { PqlEntityType::Assign, PqlEntityType::Stmt };

const std::unordered_set<PqlEntityType> ParsedQuery::varEntities = { PqlEntityType::Variable };

const std::unordered_set<PqlEntityType> ParsedQuery::procEntities = { PqlEntityType::Procedure };

const std::unordered_map<PqlRelationshipType,
	std::vector<std::pair<PqlRelationshipType, std::unordered_set<PqlReferenceType>>>>
	ParsedQuery::validLeftArgs = {
	{ PqlRelationshipType::Modifies,	{ { PqlRelationshipType::ModifiesS, ParsedQuery::stmtRefNoWildcard },
										{ PqlRelationshipType::ModifiesP, ParsedQuery::entRefNoWildcard } } },
	{ PqlRelationshipType::Uses,		{ { PqlRelationshipType::UsesS, ParsedQuery::stmtRefNoWildcard },
										{ PqlRelationshipType::UsesP, ParsedQuery::entRefNoWildcard } } },
	{ PqlRelationshipType::Follows,		{ { PqlRelationshipType::Follows, ParsedQuery::stmtRef } } },
	{ PqlRelationshipType::FollowsT,	{ { PqlRelationshipType::FollowsT, ParsedQuery::stmtRef } } },
	{ PqlRelationshipType::Parent,		{ { PqlRelationshipType::Parent, ParsedQuery::stmtRef } } },
	{ PqlRelationshipType::ParentT,		{ { PqlRelationshipType::ParentT, ParsedQuery::stmtRef } } },
	{ PqlRelationshipType::Calls,		{ { PqlRelationshipType::Calls, ParsedQuery::entRef } } },
	{ PqlRelationshipType::CallsT,		{ { PqlRelationshipType::CallsT, ParsedQuery::entRef } } },
	{ PqlRelationshipType::Next,		{ { PqlRelationshipType::Next, ParsedQuery::stmtRef } } },
	{ PqlRelationshipType::NextT,		{ { PqlRelationshipType::NextT, ParsedQuery::stmtRef } } },
	{ PqlRelationshipType::Affects,		{ { PqlRelationshipType::Affects, ParsedQuery::stmtRef } } },
	{ PqlRelationshipType::AffectsT,	{ { PqlRelationshipType::AffectsT, ParsedQuery::stmtRef } } },
};

const std::unordered_map<PqlRelationshipType, std::unordered_set<PqlReferenceType>>
	ParsedQuery::validRightArgs = {
	{ PqlRelationshipType::Modifies,	ParsedQuery::entRef },
	{ PqlRelationshipType::Uses,		ParsedQuery::entRef },
	{ PqlRelationshipType::Follows,		ParsedQuery::stmtRef },
	{ PqlRelationshipType::FollowsT,	ParsedQuery::stmtRef },
	{ PqlRelationshipType::Parent,		ParsedQuery::stmtRef },
	{ PqlRelationshipType::ParentT,		ParsedQuery::stmtRef },
	{ PqlRelationshipType::Calls,		ParsedQuery::entRef },
	{ PqlRelationshipType::CallsT,		ParsedQuery::entRef },
	{ PqlRelationshipType::Next,		ParsedQuery::stmtRef },
	{ PqlRelationshipType::NextT,		ParsedQuery::stmtRef },
	{ PqlRelationshipType::Affects,		ParsedQuery::stmtRef },
	{ PqlRelationshipType::AffectsT,	ParsedQuery::stmtRef },
};

const std::unordered_map<PqlRelationshipType, std::unordered_set<PqlEntityType>>
	ParsedQuery::validLeftDesignEntities = {
	{ PqlRelationshipType::ModifiesS,	ParsedQuery::stmtEntities },
	{ PqlRelationshipType::ModifiesP,	ParsedQuery::procEntities },
	{ PqlRelationshipType::UsesS,		ParsedQuery::stmtEntities },
	{ PqlRelationshipType::UsesP,		ParsedQuery::procEntities },
	{ PqlRelationshipType::Follows,		ParsedQuery::stmtEntities },
	{ PqlRelationshipType::FollowsT,	ParsedQuery::stmtEntities },
	{ PqlRelationshipType::Parent,		ParsedQuery::stmtEntities },
	{ PqlRelationshipType::ParentT,		ParsedQuery::stmtEntities },
	{ PqlRelationshipType::Calls,		ParsedQuery::procEntities },
	{ PqlRelationshipType::CallsT,		ParsedQuery::procEntities },
	{ PqlRelationshipType::Next,		ParsedQuery::stmtEntities },
	{ PqlRelationshipType::NextT,		ParsedQuery::stmtEntities },
	{ PqlRelationshipType::Affects,		ParsedQuery::assignEntities },
	{ PqlRelationshipType::AffectsT,	ParsedQuery::assignEntities },
};

const std::unordered_map<PqlRelationshipType, std::unordered_set<PqlEntityType>>
	ParsedQuery::validRightDesignEntities = {
	{ PqlRelationshipType::Modifies,	ParsedQuery::varEntities },
	{ PqlRelationshipType::Uses,		ParsedQuery::varEntities },
	{ PqlRelationshipType::Follows,		ParsedQuery::stmtEntities },
	{ PqlRelationshipType::FollowsT,	ParsedQuery::stmtEntities },
	{ PqlRelationshipType::Parent,		ParsedQuery::stmtEntities },
	{ PqlRelationshipType::ParentT,		ParsedQuery::stmtEntities },
	{ PqlRelationshipType::Calls,		ParsedQuery::procEntities },
	{ PqlRelationshipType::CallsT,		ParsedQuery::procEntities },
	{ PqlRelationshipType::Next,		ParsedQuery::stmtEntities },
	{ PqlRelationshipType::NextT,		ParsedQuery::stmtEntities },
	{ PqlRelationshipType::Affects,		ParsedQuery::assignEntities },
	{ PqlRelationshipType::AffectsT,	ParsedQuery::assignEntities },
};

const std::unordered_map<PqlEntityType, PqlPatternType>
	ParsedQuery::validPatternSynDesignEntities = {
	{ PqlEntityType::Assign, PqlPatternType::PatternA },
	{ PqlEntityType::While, PqlPatternType::PatternW },
	{ PqlEntityType::If, PqlPatternType::PatternI },
};

const std::unordered_map<PqlPatternType, std::unordered_set<PqlReferenceType>>
	ParsedQuery::validPatternEntRef = {
	{ PqlPatternType::PatternA, ParsedQuery::entRef },
	{ PqlPatternType::PatternW, ParsedQuery::entRef },
	{ PqlPatternType::PatternI, ParsedQuery::entRef },
};

const std::unordered_map<PqlPatternType, std::unordered_set<PqlEntityType>> ParsedQuery::validPatternEntRefEntities = {
	{ PqlPatternType::PatternA, ParsedQuery::varEntities },
	{ PqlPatternType::PatternW, ParsedQuery::varEntities },
	{ PqlPatternType::PatternI, ParsedQuery::varEntities },
};

const std::unordered_map<PqlPatternType, std::unordered_set<PqlExpressionType>>
	ParsedQuery::validPatternExprSpec = {
	{ PqlPatternType::PatternA, { PqlExpressionType::full, PqlExpressionType::partial, PqlExpressionType::wildcard } },
	{ PqlPatternType::PatternW, { PqlExpressionType::wildcard } },
	{ PqlPatternType::PatternI, { PqlExpressionType::wildcard } },
};

const std::unordered_map<PqlPatternType, int>
	ParsedQuery::validPatternNumOfArgs = {
	{ PqlPatternType::PatternA, 2 },
	{ PqlPatternType::PatternW, 2 },
	{ PqlPatternType::PatternI, 3 },
};

const std::unordered_set<PqlReferenceType> ParsedQuery::attribReferences = {
	PqlReferenceType::procName, PqlReferenceType::varName,
	PqlReferenceType::value, PqlReferenceType::stmtNum
};

const std::unordered_map<PqlEntityType, std::unordered_set<PqlReferenceType>>
	ParsedQuery::validAttribs = {
	{ PqlEntityType::Procedure, { PqlReferenceType::procName } },
	{ PqlEntityType::Call,		{ PqlReferenceType::procName, PqlReferenceType::stmtNum } },
	{ PqlEntityType::Variable,	{ PqlReferenceType::varName } },
	{ PqlEntityType::Read,		{ PqlReferenceType::varName, PqlReferenceType::stmtNum } },
	{ PqlEntityType::Print,		{ PqlReferenceType::varName, PqlReferenceType::stmtNum } },
	{ PqlEntityType::Constant,	{ PqlReferenceType::value } },
	{ PqlEntityType::Stmt,		{ PqlReferenceType::stmtNum } },
	{ PqlEntityType::While,		{ PqlReferenceType::stmtNum } },
	{ PqlEntityType::If,		{ PqlReferenceType::stmtNum } },
	{ PqlEntityType::Assign,	{ PqlReferenceType::stmtNum } },
};

const std::unordered_map<PqlReferenceType, PqlAttributeType>
	ParsedQuery::refToAttribMap = {
	{ PqlReferenceType::ident,		PqlAttributeType::string },
	{ PqlReferenceType::integer,	PqlAttributeType::integer },
	{ PqlReferenceType::procName,	PqlAttributeType::string },
	{ PqlReferenceType::varName,	PqlAttributeType::string },
	{ PqlReferenceType::value,		PqlAttributeType::integer },
	{ PqlReferenceType::stmtNum,	PqlAttributeType::integer },
};

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
		std::string synonym = pattern.getSynonym();
		if (!isDeclared(synonym)) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		PqlEntityType synonymType = getType(synonym);
		if (validPatternSynDesignEntities.find(synonymType) == validPatternSynDesignEntities.end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		/* Reassign to a specific pattern type based on the type of the synonym */
		PqlPatternType patternType = validPatternSynDesignEntities.at(synonymType);
		/* Type-validate the first entRef */
		PqlReference entRef = pattern.getEntRef();
		PqlReferenceType entRefType = entRef.first;
		if (validPatternEntRef.at(patternType).find(entRefType) == validPatternEntRef.at(patternType).end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		/* If the entRef is a synonym, perform further type validation on the design entity */
		if (entRefType == PqlReferenceType::synonym) {
			std::string& entRefName = entRef.second;
			if (!isDeclared(entRefName)) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
			PqlEntityType entRefEntity = getType(entRefName);
			if (validPatternEntRefEntities.at(patternType).find(entRefEntity) == validPatternEntRefEntities.at(patternType).end()) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
		}
		/* Type-validate the expression-spec */
		PqlExpression expression = pattern.getExpression();
		PqlExpressionType expressionType = expression.first;
		if (validPatternExprSpec.at(patternType).find(expressionType) == validPatternExprSpec.at(patternType).end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		/* Type-validate the number of arguments */
		int numOfArgs = pattern.getNumOfArgs();
		if (validPatternNumOfArgs.at(patternType) != numOfArgs) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		patterns.emplace_back(pattern.getSynonym(), patternType,
			pattern.getEntRef(), pattern.getExpression(), pattern.getNumOfArgs());
	}
}

void ParsedQuery::populateWiths(const std::vector<ParsedWith>& allWiths) {
	for (const ParsedWith& with : allWiths) {
		/* TODO: can abstract this out */
		/* Validate LHS argument */
		PqlReference lhs = with.getLhs();
		PqlReferenceType lhsType = lhs.first;
		if (attribReferences.find(lhsType) != attribReferences.end()) {
			std::string lhsName = lhs.second;
			if (!isDeclared(lhsName)) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
			PqlEntityType lhsEntity = getType(lhsName);
			/* Validate that the design entity and attribute are compatible */
			if (validAttribs.find(lhsEntity) == validAttribs.end()) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
			if (validAttribs.at(lhsEntity).find(lhsType) == validAttribs.at(lhsEntity).end()) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
		}
		/* Validate RHS argument */
		PqlReference rhs = with.getRhs();
		PqlReferenceType rhsType = rhs.first;
		if (attribReferences.find(rhsType) != attribReferences.end()) {
			std::string rhsName = rhs.second;
			if (!isDeclared(rhsName)) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
			PqlEntityType rhsEntity = getType(rhsName);
			/* Validate that the design entity and attribute are compatible */
			if (validAttribs.find(rhsEntity) == validAttribs.end()) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
			if (validAttribs.at(rhsEntity).find(rhsType) == validAttribs.at(rhsEntity).end()) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
		}
		/* Validate that LHS argument and RHS arguments are the same type */
		if (refToAttribMap.find(lhsType) == refToAttribMap.end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		PqlAttributeType lhsAttribType = refToAttribMap.at(lhsType);
		if (refToAttribMap.find(rhsType) == refToAttribMap.end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		PqlAttributeType rhsAttribType = refToAttribMap.at(rhsType);
		if (lhsAttribType != rhsAttribType) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		withs.push_back(with);
	}
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

ParsedQuery::ParsedQuery(const std::vector<PQL_VARIABLE>& allDeclarations,
	const std::vector<std::string>& allColumns,
	const std::vector<ParsedRelationship>& allRelationships,
	const std::vector<ParsedPattern>& allPatterns,
	const std::vector<ParsedWith>& allWiths) {
	populateDeclarations(allDeclarations);
	populateColumns(allColumns);
	populateRelationships(allRelationships);
	populatePatterns(allPatterns);
	populateWiths(allWiths);
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

std::vector<ParsedWith> ParsedQuery::getWiths() {
	return withs;
}
