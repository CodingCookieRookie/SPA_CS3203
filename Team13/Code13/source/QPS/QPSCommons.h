#pragma once

#include <string>

#include "../Common/Types.h"

enum class InstructionType {
	GET_ALL, RELATIONSHIP, PATTERN
};

enum class RelationshipInstructionType {
	GET_FOLLOWS, GET_FOLLOWS_T, GET_PARENT, GET_PARENT_T,
	GET_USES_S, GET_USES_P, GET_MODIFIES_S, GET_MODIFIES_P,
	GET_USES, GET_MODIFIES
};

enum class PatternInstructionType {
	GET_PATTERN
};

enum class PqlRelationshipType {
	FOLLOWS, FOLLOWS_T, PARENT, PARENT_T,
	USES_S, USES_P, MODIFIES_S, MODIFIES_P,
	CALLS, CALLS_T, NEXT, NEXT_T,
	AFFECTS, AFFECTS_T,
	USES, MODIFIES
};

enum class PqlPatternType {
	PATTERN, PATTERN_A, PATTERN_I, PATTERN_W
};

enum class PqlReferenceType {
	SYNONYM, WILDCARD, INTEGER, IDENT,
	PROC_NAME, VAR_NAME, VALUE, STMT_NUM
};

enum class PqlExpressionType {
	FULL, PARTIAL, WILDCARD
};

enum class PqlAttributeType {
	STRING, INTEGER, UNVALIDATED
};

enum class ProjectionType {
	SINGLE, TUPLE, BOOLEAN
};

typedef std::pair<EntityType, std::string> PqlEntity;

typedef std::pair<PqlReferenceType, std::string> PqlReference;

typedef std::pair<PqlExpressionType, std::string> PqlExpression;

bool isSynonymRef(PqlReference reference);
bool isStatementEntity(EntityType entityType);
