#pragma once

#include <string>

#include "../Common/Types.h"

enum class InstructionType {
	getAll, relationship, pattern
};

enum class RelationshipInstructionType {
	getFollows, getFollowsT, getParent, getParentT,
	getUsesS, getUsesP, getModifiesS, getModifiesP,
	getUses, getModifies
};

enum class PatternInstructionType {
	getPattern
};

enum class PqlRelationshipType {
	Follows, FollowsT, Parent, ParentT,
	UsesS, UsesP, ModifiesS, ModifiesP,
	Calls, CallsT, Next, NextT,
	Affects, AffectsT,
	Uses, Modifies
};

enum class PqlPatternType {
	Pattern, PatternA, PatternI, PatternW
};

enum class PqlReferenceType {
	Synonym, Wildcard, Integer, Ident,
	ProcName, VarName, Value, StmtNum
};

enum class PqlExpressionType {
	Full, Partial, Wildcard
};

enum class PqlAttributeType {
	String, Integer, Unvalidated
};

enum class ProjectionType {
	single, tuple, boolean
};

typedef std::pair<PqlEntityType, std::string> PqlEntity;

typedef std::pair<PqlReferenceType, std::string> PqlReference;

typedef std::pair<PqlExpressionType, std::string> PqlExpression;

bool isSynonymRef(PqlReference reference);
bool isStatementEntity(PqlEntityType entityType);
