#include "ParsedQuery.h"

const std::unordered_set<PqlReferenceType>
ParsedQuery::STMT_REF = { PqlReferenceType::SYNONYM, PqlReferenceType::WILDCARD, PqlReferenceType::INTEGER };

const std::unordered_set<PqlReferenceType>
ParsedQuery::ENT_REF = { PqlReferenceType::SYNONYM, PqlReferenceType::WILDCARD, PqlReferenceType::IDENT };

const std::unordered_set<PqlReferenceType>
ParsedQuery::STMT_REF_NO_WILDCARD = { PqlReferenceType::SYNONYM, PqlReferenceType::INTEGER };

const std::unordered_set<PqlReferenceType>
ParsedQuery::ENT_REF_NO_WILDCARD = { PqlReferenceType::SYNONYM, PqlReferenceType::IDENT };

const std::unordered_set<EntityType> ParsedQuery::STMT_ENTITIES = {
	EntityType::ASSIGN, EntityType::CALL, EntityType::IF, EntityType::PRINT,
	EntityType::READ, EntityType::STMT, EntityType::WHILE
};

const std::unordered_set<EntityType> ParsedQuery::ASSIGN_ENTITIES = { EntityType::ASSIGN, EntityType::STMT };

const std::unordered_set<EntityType> ParsedQuery::VAR_ENTITIES = { EntityType::VARIABLE };

const std::unordered_set<EntityType> ParsedQuery::PROC_ENTITIES = { EntityType::PROCEDURE };

const std::unordered_map<PqlRelationshipType,
	std::vector<std::pair<PqlRelationshipType, std::unordered_set<PqlReferenceType>>>>
	ParsedQuery::VALID_LEFT_ARGS = {
	{ PqlRelationshipType::MODIFIES,	{ { PqlRelationshipType::MODIFIES_S, ParsedQuery::STMT_REF_NO_WILDCARD },
										{ PqlRelationshipType::MODIFIES_P, ParsedQuery::ENT_REF_NO_WILDCARD } } },
	{ PqlRelationshipType::USES,		{ { PqlRelationshipType::USES_S, ParsedQuery::STMT_REF_NO_WILDCARD },
										{ PqlRelationshipType::USES_P, ParsedQuery::ENT_REF_NO_WILDCARD } } },
	{ PqlRelationshipType::FOLLOWS,		{ { PqlRelationshipType::FOLLOWS, ParsedQuery::STMT_REF } } },
	{ PqlRelationshipType::FOLLOWS_T,	{ { PqlRelationshipType::FOLLOWS_T, ParsedQuery::STMT_REF } } },
	{ PqlRelationshipType::PARENT,		{ { PqlRelationshipType::PARENT, ParsedQuery::STMT_REF } } },
	{ PqlRelationshipType::PARENT_T,		{ { PqlRelationshipType::PARENT_T, ParsedQuery::STMT_REF } } },
	{ PqlRelationshipType::CALLS,		{ { PqlRelationshipType::CALLS, ParsedQuery::ENT_REF } } },
	{ PqlRelationshipType::CALLS_T,		{ { PqlRelationshipType::CALLS_T, ParsedQuery::ENT_REF } } },
	{ PqlRelationshipType::NEXT,		{ { PqlRelationshipType::NEXT, ParsedQuery::STMT_REF } } },
	{ PqlRelationshipType::NEXT_T,		{ { PqlRelationshipType::NEXT_T, ParsedQuery::STMT_REF } } },
	{ PqlRelationshipType::AFFECTS,		{ { PqlRelationshipType::AFFECTS, ParsedQuery::STMT_REF } } },
	{ PqlRelationshipType::AFFECTS_T,	{ { PqlRelationshipType::AFFECTS_T, ParsedQuery::STMT_REF } } },
};

const std::unordered_map<PqlRelationshipType, std::unordered_set<PqlReferenceType>>
ParsedQuery::VALID_RIGHT_ARGS = {
{ PqlRelationshipType::MODIFIES,	ParsedQuery::ENT_REF },
{ PqlRelationshipType::USES,		ParsedQuery::ENT_REF },
{ PqlRelationshipType::FOLLOWS,		ParsedQuery::STMT_REF },
{ PqlRelationshipType::FOLLOWS_T,	ParsedQuery::STMT_REF },
{ PqlRelationshipType::PARENT,		ParsedQuery::STMT_REF },
{ PqlRelationshipType::PARENT_T,		ParsedQuery::STMT_REF },
{ PqlRelationshipType::CALLS,		ParsedQuery::ENT_REF },
{ PqlRelationshipType::CALLS_T,		ParsedQuery::ENT_REF },
{ PqlRelationshipType::NEXT,		ParsedQuery::STMT_REF },
{ PqlRelationshipType::NEXT_T,		ParsedQuery::STMT_REF },
{ PqlRelationshipType::AFFECTS,		ParsedQuery::STMT_REF },
{ PqlRelationshipType::AFFECTS_T,	ParsedQuery::STMT_REF },
};

const std::unordered_map<PqlRelationshipType, std::unordered_set<EntityType>>
ParsedQuery::VALID_LEFT_DESIGN_ENTITIES = {
{ PqlRelationshipType::MODIFIES_S,	ParsedQuery::STMT_ENTITIES },
{ PqlRelationshipType::MODIFIES_P,	ParsedQuery::PROC_ENTITIES },
{ PqlRelationshipType::USES_S,		ParsedQuery::STMT_ENTITIES },
{ PqlRelationshipType::USES_P,		ParsedQuery::PROC_ENTITIES },
{ PqlRelationshipType::FOLLOWS,		ParsedQuery::STMT_ENTITIES },
{ PqlRelationshipType::FOLLOWS_T,	ParsedQuery::STMT_ENTITIES },
{ PqlRelationshipType::PARENT,		ParsedQuery::STMT_ENTITIES },
{ PqlRelationshipType::PARENT_T,		ParsedQuery::STMT_ENTITIES },
{ PqlRelationshipType::CALLS,		ParsedQuery::PROC_ENTITIES },
{ PqlRelationshipType::CALLS_T,		ParsedQuery::PROC_ENTITIES },
{ PqlRelationshipType::NEXT,		ParsedQuery::STMT_ENTITIES },
{ PqlRelationshipType::NEXT_T,		ParsedQuery::STMT_ENTITIES },
{ PqlRelationshipType::AFFECTS,		ParsedQuery::ASSIGN_ENTITIES },
{ PqlRelationshipType::AFFECTS_T,	ParsedQuery::ASSIGN_ENTITIES },
};

const std::unordered_map<PqlRelationshipType, std::unordered_set<EntityType>>
ParsedQuery::VALID_RIGHT_DESIGN_ENTITIES = {
{ PqlRelationshipType::MODIFIES,	ParsedQuery::VAR_ENTITIES },
{ PqlRelationshipType::USES,		ParsedQuery::VAR_ENTITIES },
{ PqlRelationshipType::FOLLOWS,		ParsedQuery::STMT_ENTITIES },
{ PqlRelationshipType::FOLLOWS_T,	ParsedQuery::STMT_ENTITIES },
{ PqlRelationshipType::PARENT,		ParsedQuery::STMT_ENTITIES },
{ PqlRelationshipType::PARENT_T,		ParsedQuery::STMT_ENTITIES },
{ PqlRelationshipType::CALLS,		ParsedQuery::PROC_ENTITIES },
{ PqlRelationshipType::CALLS_T,		ParsedQuery::PROC_ENTITIES },
{ PqlRelationshipType::NEXT,		ParsedQuery::STMT_ENTITIES },
{ PqlRelationshipType::NEXT_T,		ParsedQuery::STMT_ENTITIES },
{ PqlRelationshipType::AFFECTS,		ParsedQuery::ASSIGN_ENTITIES },
{ PqlRelationshipType::AFFECTS_T,	ParsedQuery::ASSIGN_ENTITIES },
};

const std::unordered_map<EntityType, PqlPatternType>
ParsedQuery::VALID_PATTERN_SYN_DESIGN_ENTITIES = {
{ EntityType::ASSIGN, PqlPatternType::PATTERN_A },
{ EntityType::WHILE, PqlPatternType::PATTERN_W },
{ EntityType::IF, PqlPatternType::PATTERN_I },
};

const std::unordered_map<PqlPatternType, std::unordered_set<PqlReferenceType>>
ParsedQuery::VALID_PATTERN_ENT_REF = {
{ PqlPatternType::PATTERN_A, ParsedQuery::ENT_REF },
{ PqlPatternType::PATTERN_W, ParsedQuery::ENT_REF },
{ PqlPatternType::PATTERN_I, ParsedQuery::ENT_REF },
};

const std::unordered_map<PqlPatternType, std::unordered_set<EntityType>> ParsedQuery::VALID_PATTERN_ENT_REF_ENTITIES = {
	{ PqlPatternType::PATTERN_A, ParsedQuery::VAR_ENTITIES },
	{ PqlPatternType::PATTERN_W, ParsedQuery::VAR_ENTITIES },
	{ PqlPatternType::PATTERN_I, ParsedQuery::VAR_ENTITIES },
};

const std::unordered_map<PqlPatternType, std::unordered_set<PqlExpressionType>>
ParsedQuery::VALID_PATTERN_EXPR_SPEC = {
{ PqlPatternType::PATTERN_A, { PqlExpressionType::FULL, PqlExpressionType::PARTIAL, PqlExpressionType::WILDCARD } },
{ PqlPatternType::PATTERN_W, { PqlExpressionType::WILDCARD } },
{ PqlPatternType::PATTERN_I, { PqlExpressionType::WILDCARD } },
};

const std::unordered_map<PqlPatternType, int>
ParsedQuery::VALID_PATTERN_NUM_OF_ARGS = {
{ PqlPatternType::PATTERN_A, 2 },
{ PqlPatternType::PATTERN_W, 2 },
{ PqlPatternType::PATTERN_I, 3 },
};

const std::unordered_set<PqlReferenceType> ParsedQuery::ATTRIB_REFERENCES = {
	PqlReferenceType::PROC_NAME, PqlReferenceType::VAR_NAME,
	PqlReferenceType::VALUE, PqlReferenceType::STMT_NUM
};

const std::unordered_map<EntityType, std::unordered_set<PqlReferenceType>>
ParsedQuery::VALID_ATTRIBS = {
	{ EntityType::PROCEDURE, { PqlReferenceType::PROC_NAME } },
	{ EntityType::CALL,		{ PqlReferenceType::PROC_NAME, PqlReferenceType::STMT_NUM } },
	{ EntityType::VARIABLE,	{ PqlReferenceType::VAR_NAME } },
	{ EntityType::READ,		{ PqlReferenceType::VAR_NAME, PqlReferenceType::STMT_NUM } },
	{ EntityType::PRINT,		{ PqlReferenceType::VAR_NAME, PqlReferenceType::STMT_NUM } },
	{ EntityType::CONSTANT,	{ PqlReferenceType::VALUE } },
	{ EntityType::STMT,		{ PqlReferenceType::STMT_NUM } },
	{ EntityType::WHILE,		{ PqlReferenceType::STMT_NUM } },
	{ EntityType::IF,		{ PqlReferenceType::STMT_NUM } },
	{ EntityType::ASSIGN,	{ PqlReferenceType::STMT_NUM } },
};

const std::unordered_map<PqlReferenceType, PqlAttributeType>
ParsedQuery::REF_TO_ATTRIB_MAP = {
	{ PqlReferenceType::IDENT,		PqlAttributeType::STRING },
	{ PqlReferenceType::INTEGER,	PqlAttributeType::INTEGER },
	{ PqlReferenceType::PROC_NAME,	PqlAttributeType::STRING },
	{ PqlReferenceType::VAR_NAME,	PqlAttributeType::STRING },
	{ PqlReferenceType::VALUE,		PqlAttributeType::INTEGER },
	{ PqlReferenceType::STMT_NUM,	PqlAttributeType::INTEGER },
};

bool ParsedQuery::isDeclared(const std::string& synonym) {
	return declarations.find(synonym) != declarations.end();
}

EntityType ParsedQuery::getType(std::string& synonym) {
	return declarations.at(synonym);
}

void ParsedQuery::populateDeclarations(
	const std::vector<PqlEntity>& allDeclarations) {
	for (const PqlEntity& variable : allDeclarations) {
		EntityType variableType = variable.first;
		std::string variableName = variable.second;
		if (isDeclared(variableName)) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		declarations[variableName] = variableType;
	}
}

void ParsedQuery::populateColumns(const std::vector<PqlReference>& allElems) {
	if (allElems.empty() && isDeclared("BOOLEAN")) {
		/* Handle corner case --If Select BOOLEAN, allElems will be an empty vector
		But if BOOLEAN is declared as some synonym, treat it as a synonym instead */
		columns.insert("BOOLEAN");
		attributes.emplace_back(PqlReferenceType::SYNONYM, "BOOLEAN");
	}
	for (const PqlReference& elem : allElems) {
		std::string synonym = elem.second;
		if (!isDeclared(synonym)) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		PqlReferenceType refType = elem.first;
		if (refType != PqlReferenceType::SYNONYM) {
			EntityType entityType = getType(synonym);
			if (VALID_ATTRIBS.at(entityType).find(refType) == VALID_ATTRIBS.at(entityType).end()) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
		}
		columns.insert(synonym);
		attributes.push_back(elem);
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
		if (VALID_RIGHT_ARGS.find(relationshipType) == VALID_RIGHT_ARGS.end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		/* Type validate the right argument */
		if (VALID_RIGHT_ARGS.at(relationshipType).find(rhsType) == VALID_RIGHT_ARGS.at(relationshipType).end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		/* If the right argument is a synonym, perform further
		type validation on the design entity */
		if (rhsType == PqlReferenceType::SYNONYM) {
			if (VALID_RIGHT_DESIGN_ENTITIES.find(relationshipType) == VALID_RIGHT_DESIGN_ENTITIES.end()) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
			std::string& rhsName = rhs.second;
			if (!isDeclared(rhsName)) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
			EntityType rhsEntity = getType(rhsName);
			if (VALID_RIGHT_DESIGN_ENTITIES.at(relationshipType).find(rhsEntity) == VALID_RIGHT_DESIGN_ENTITIES.at(relationshipType).end()) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
		}

		/* If we haven't implemented a certain relationship, throw an exception
		TODO: remove once all relationships have been implemented */
		if (VALID_LEFT_ARGS.find(relationshipType) == VALID_LEFT_ARGS.end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		bool foundRelationship = false;
		for (const std::pair<PqlRelationshipType, std::unordered_set<PqlReferenceType>>& validRefs : VALID_LEFT_ARGS.at(relationshipType)) {
			PqlRelationshipType reassignType = validRefs.first;
			const std::unordered_set<PqlReferenceType>& validRefTypes = validRefs.second;
			/* Type validate the left argument */
			if (validRefTypes.find(lhsType) == validRefTypes.end()) {
				continue;
			}
			/* If the right argument is a synonym, perform further
			type validation on the design entity */
			if (lhsType == PqlReferenceType::SYNONYM) {
				/* If we haven't implemented a certain relationship, throw an exception
				TODO: remove once all relationships have been implemented */
				if (VALID_LEFT_DESIGN_ENTITIES.find(reassignType) == VALID_LEFT_DESIGN_ENTITIES.end()) {
					throw QPSException(QPSException::VALIDATOR_ERROR);
				}
				std::string& lhsName = lhs.second;
				if (!isDeclared(lhsName)) {
					throw QPSException(QPSException::VALIDATOR_ERROR);
				}
				EntityType lhsEntity = getType(lhsName);
				if (VALID_LEFT_DESIGN_ENTITIES.at(reassignType).find(lhsEntity) == VALID_LEFT_DESIGN_ENTITIES.at(reassignType).end()) {
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
		EntityType synonymType = getType(synonym);
		if (VALID_PATTERN_SYN_DESIGN_ENTITIES.find(synonymType) == VALID_PATTERN_SYN_DESIGN_ENTITIES.end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		/* Reassign to a specific pattern type based on the type of the synonym */
		PqlPatternType patternType = VALID_PATTERN_SYN_DESIGN_ENTITIES.at(synonymType);
		/* Type-validate the first entRef */
		PqlReference entRef = pattern.getEntRef();
		PqlReferenceType entRefType = entRef.first;
		if (VALID_PATTERN_ENT_REF.at(patternType).find(entRefType) == VALID_PATTERN_ENT_REF.at(patternType).end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		/* If the entRef is a synonym, perform further type validation on the design entity */
		if (entRefType == PqlReferenceType::SYNONYM) {
			std::string& entRefName = entRef.second;
			if (!isDeclared(entRefName)) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
			EntityType entRefEntity = getType(entRefName);
			if (VALID_PATTERN_ENT_REF_ENTITIES.at(patternType).find(entRefEntity) == VALID_PATTERN_ENT_REF_ENTITIES.at(patternType).end()) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
		}
		/* Type-validate the expression-spec */
		PqlExpression expression = pattern.getExpression();
		PqlExpressionType expressionType = expression.first;
		if (VALID_PATTERN_EXPR_SPEC.at(patternType).find(expressionType) == VALID_PATTERN_EXPR_SPEC.at(patternType).end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		/* Type-validate the number of arguments */
		int numOfArgs = pattern.getNumOfArgs();
		if (VALID_PATTERN_NUM_OF_ARGS.at(patternType) != numOfArgs) {
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
		if (ATTRIB_REFERENCES.find(lhsType) != ATTRIB_REFERENCES.end()) {
			std::string lhsName = lhs.second;
			if (!isDeclared(lhsName)) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
			EntityType lhsEntity = getType(lhsName);
			/* Validate that the design entity and attribute are compatible */
			if (VALID_ATTRIBS.find(lhsEntity) == VALID_ATTRIBS.end()) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
			if (VALID_ATTRIBS.at(lhsEntity).find(lhsType) == VALID_ATTRIBS.at(lhsEntity).end()) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
		}
		/* Validate RHS argument */
		PqlReference rhs = with.getRhs();
		PqlReferenceType rhsType = rhs.first;
		if (ATTRIB_REFERENCES.find(rhsType) != ATTRIB_REFERENCES.end()) {
			std::string rhsName = rhs.second;
			if (!isDeclared(rhsName)) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
			EntityType rhsEntity = getType(rhsName);
			/* Validate that the design entity and attribute are compatible */
			if (VALID_ATTRIBS.find(rhsEntity) == VALID_ATTRIBS.end()) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
			if (VALID_ATTRIBS.at(rhsEntity).find(rhsType) == VALID_ATTRIBS.at(rhsEntity).end()) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
		}
		/* Validate that LHS argument and RHS arguments are the same type */
		if (REF_TO_ATTRIB_MAP.find(lhsType) == REF_TO_ATTRIB_MAP.end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		PqlAttributeType lhsAttribType = REF_TO_ATTRIB_MAP.at(lhsType);
		if (REF_TO_ATTRIB_MAP.find(rhsType) == REF_TO_ATTRIB_MAP.end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		PqlAttributeType rhsAttribType = REF_TO_ATTRIB_MAP.at(rhsType);
		if (lhsAttribType != rhsAttribType) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		EntityType lhsEntity = declarations.find(lhs.second) != declarations.end() ? declarations.at(lhs.second) : EntityType::CONSTANT;
		EntityType rhsEntity = declarations.find(rhs.second) != declarations.end() ? declarations.at(rhs.second) : EntityType::CONSTANT;
		withs.emplace_back(with.getLhs(), with.getRhs(), lhsEntity, rhsEntity, lhsAttribType);
	}
}

ParsedQuery::ParsedQuery(const std::vector<PqlEntity>& allDeclarations,
	const std::vector<PqlReference>& allColumns,
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
	EntityType entType = getType(refName);
	return (entType == EntityType::ASSIGN)
		|| (entType == EntityType::CALL)
		|| (entType == EntityType::IF)
		|| (entType == EntityType::PRINT)
		|| (entType == EntityType::READ)
		|| (entType == EntityType::STMT)
		|| (entType == EntityType::WHILE);
}

ProjectionType ParsedQuery::getProjectionType(std::vector<PqlReference> attributesProjected) {
	if (attributesProjected.size() == 0) {
		return ProjectionType::BOOLEAN;
	} else if (attributesProjected.size() == 1) {
		return ProjectionType::SINGLE;
	} else if (attributesProjected.size() >= 1) {
		return ProjectionType::TUPLE;
	} else {
		throw EvaluatorException(EvaluatorException::PROJECTION_TYPE_ERROR);
	}
}

bool ParsedQuery::isClausePresent(ParsedQuery& parsedQuery) {
	std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
	std::vector<ParsedPattern> patterns = parsedQuery.getPatterns();
	std::vector<ParsedWith> withs = parsedQuery.getWiths();
	return !(relationships.empty() && patterns.empty() && withs.empty());
}

std::unordered_map<std::string, EntityType> ParsedQuery::getDeclarations() {
	return declarations;
}

std::unordered_set<std::string> ParsedQuery::getColumns() {
	return columns;
}

std::vector<PqlReference> ParsedQuery::getAttributes() {
	return attributes;
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
