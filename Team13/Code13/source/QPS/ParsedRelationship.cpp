#include "ParsedRelationship.h"

const std::unordered_set<PqlReferenceType>
ParsedRelationship::STMT_REF = { PqlReferenceType::SYNONYM, PqlReferenceType::WILDCARD, PqlReferenceType::INTEGER };

const std::unordered_set<PqlReferenceType>
ParsedRelationship::ENT_REF = { PqlReferenceType::SYNONYM, PqlReferenceType::WILDCARD, PqlReferenceType::IDENT };

const std::unordered_set<PqlReferenceType>
ParsedRelationship::STMT_REF_NO_WILDCARD = { PqlReferenceType::SYNONYM, PqlReferenceType::INTEGER };

const std::unordered_set<PqlReferenceType>
ParsedRelationship::ENT_REF_NO_WILDCARD = { PqlReferenceType::SYNONYM, PqlReferenceType::IDENT };

const std::unordered_set<EntityType> ParsedRelationship::STMT_ENTITIES = {
	EntityType::ASSIGN, EntityType::CALL, EntityType::IF, EntityType::PRINT,
	EntityType::READ, EntityType::STMT, EntityType::WHILE
};

const std::unordered_set<EntityType> ParsedRelationship::ASSIGN_ENTITIES = { EntityType::ASSIGN, EntityType::STMT };

const std::unordered_set<EntityType> ParsedRelationship::VAR_ENTITIES = { EntityType::VARIABLE };

const std::unordered_set<EntityType> ParsedRelationship::PROC_ENTITIES = { EntityType::PROCEDURE };

const std::unordered_map<PqlRelationshipType,
	std::vector<std::pair<PqlRelationshipType, std::unordered_set<PqlReferenceType>>>>
	ParsedRelationship::VALID_LEFT_ARGS = {
		{ PqlRelationshipType::MODIFIES,	{ { PqlRelationshipType::MODIFIES_S, ParsedRelationship::STMT_REF_NO_WILDCARD },
											{ PqlRelationshipType::MODIFIES_P, ParsedRelationship::ENT_REF_NO_WILDCARD } } },
		{ PqlRelationshipType::MODIFIES_S,	{ { PqlRelationshipType::MODIFIES_S, ParsedRelationship::STMT_REF_NO_WILDCARD } } },
		{ PqlRelationshipType::MODIFIES_P,	{ { PqlRelationshipType::MODIFIES_P, ParsedRelationship::ENT_REF_NO_WILDCARD } } },
		{ PqlRelationshipType::USES,		{ { PqlRelationshipType::USES_S, ParsedRelationship::STMT_REF_NO_WILDCARD },
											{ PqlRelationshipType::USES_P, ParsedRelationship::ENT_REF_NO_WILDCARD } } },
		{ PqlRelationshipType::USES_S,		{ { PqlRelationshipType::USES_S, ParsedRelationship::STMT_REF_NO_WILDCARD } } },
		{ PqlRelationshipType::USES_P,		{ { PqlRelationshipType::USES_P, ParsedRelationship::ENT_REF_NO_WILDCARD } } },
		{ PqlRelationshipType::FOLLOWS,		{ { PqlRelationshipType::FOLLOWS, ParsedRelationship::STMT_REF } } },
		{ PqlRelationshipType::FOLLOWS_T,	{ { PqlRelationshipType::FOLLOWS_T, ParsedRelationship::STMT_REF } } },
		{ PqlRelationshipType::PARENT,		{ { PqlRelationshipType::PARENT, ParsedRelationship::STMT_REF } } },
		{ PqlRelationshipType::PARENT_T,	{ { PqlRelationshipType::PARENT_T, ParsedRelationship::STMT_REF } } },
		{ PqlRelationshipType::CALLS,		{ { PqlRelationshipType::CALLS, ParsedRelationship::ENT_REF } } },
		{ PqlRelationshipType::CALLS_T,		{ { PqlRelationshipType::CALLS_T, ParsedRelationship::ENT_REF } } },
		{ PqlRelationshipType::NEXT,		{ { PqlRelationshipType::NEXT, ParsedRelationship::STMT_REF } } },
		{ PqlRelationshipType::NEXT_T,		{ { PqlRelationshipType::NEXT_T, ParsedRelationship::STMT_REF } } },
		{ PqlRelationshipType::AFFECTS,		{ { PqlRelationshipType::AFFECTS, ParsedRelationship::STMT_REF } } },
		{ PqlRelationshipType::AFFECTS_T,	{ { PqlRelationshipType::AFFECTS_T, ParsedRelationship::STMT_REF } } },
};

const std::unordered_map<PqlRelationshipType, std::unordered_set<PqlReferenceType>>
ParsedRelationship::VALID_RIGHT_ARGS = {
	{ PqlRelationshipType::MODIFIES,	ParsedRelationship::ENT_REF },
	{ PqlRelationshipType::MODIFIES_S,	ParsedRelationship::ENT_REF },
	{ PqlRelationshipType::MODIFIES_P,	ParsedRelationship::ENT_REF },
	{ PqlRelationshipType::USES,		ParsedRelationship::ENT_REF },
	{ PqlRelationshipType::USES_S,		ParsedRelationship::ENT_REF },
	{ PqlRelationshipType::USES_P,		ParsedRelationship::ENT_REF },
	{ PqlRelationshipType::FOLLOWS,		ParsedRelationship::STMT_REF },
	{ PqlRelationshipType::FOLLOWS_T,	ParsedRelationship::STMT_REF },
	{ PqlRelationshipType::PARENT,		ParsedRelationship::STMT_REF },
	{ PqlRelationshipType::PARENT_T,	ParsedRelationship::STMT_REF },
	{ PqlRelationshipType::CALLS,		ParsedRelationship::ENT_REF },
	{ PqlRelationshipType::CALLS_T,		ParsedRelationship::ENT_REF },
	{ PqlRelationshipType::NEXT,		ParsedRelationship::STMT_REF },
	{ PqlRelationshipType::NEXT_T,		ParsedRelationship::STMT_REF },
	{ PqlRelationshipType::AFFECTS,		ParsedRelationship::STMT_REF },
	{ PqlRelationshipType::AFFECTS_T,	ParsedRelationship::STMT_REF },
};

const std::unordered_map<PqlRelationshipType, std::unordered_set<EntityType>>
ParsedRelationship::VALID_LEFT_DESIGN_ENTITIES = {
	{ PqlRelationshipType::MODIFIES_S,	ParsedRelationship::STMT_ENTITIES },
	{ PqlRelationshipType::MODIFIES_P,	ParsedRelationship::PROC_ENTITIES },
	{ PqlRelationshipType::USES_S,		ParsedRelationship::STMT_ENTITIES },
	{ PqlRelationshipType::USES_P,		ParsedRelationship::PROC_ENTITIES },
	{ PqlRelationshipType::FOLLOWS,		ParsedRelationship::STMT_ENTITIES },
	{ PqlRelationshipType::FOLLOWS_T,	ParsedRelationship::STMT_ENTITIES },
	{ PqlRelationshipType::PARENT,		ParsedRelationship::STMT_ENTITIES },
	{ PqlRelationshipType::PARENT_T,	ParsedRelationship::STMT_ENTITIES },
	{ PqlRelationshipType::CALLS,		ParsedRelationship::PROC_ENTITIES },
	{ PqlRelationshipType::CALLS_T,		ParsedRelationship::PROC_ENTITIES },
	{ PqlRelationshipType::NEXT,		ParsedRelationship::STMT_ENTITIES },
	{ PqlRelationshipType::NEXT_T,		ParsedRelationship::STMT_ENTITIES },
	{ PqlRelationshipType::AFFECTS,		ParsedRelationship::ASSIGN_ENTITIES },
	{ PqlRelationshipType::AFFECTS_T,	ParsedRelationship::ASSIGN_ENTITIES },
};

const std::unordered_map<PqlRelationshipType, std::unordered_set<EntityType>>
ParsedRelationship::VALID_RIGHT_DESIGN_ENTITIES = {
	{ PqlRelationshipType::MODIFIES,	ParsedRelationship::VAR_ENTITIES },
	{ PqlRelationshipType::MODIFIES_S,	ParsedRelationship::VAR_ENTITIES },
	{ PqlRelationshipType::MODIFIES_P,	ParsedRelationship::VAR_ENTITIES },
	{ PqlRelationshipType::USES,		ParsedRelationship::VAR_ENTITIES },
	{ PqlRelationshipType::USES_S,		ParsedRelationship::VAR_ENTITIES },
	{ PqlRelationshipType::USES_P,		ParsedRelationship::VAR_ENTITIES },
	{ PqlRelationshipType::FOLLOWS,		ParsedRelationship::STMT_ENTITIES },
	{ PqlRelationshipType::FOLLOWS_T,	ParsedRelationship::STMT_ENTITIES },
	{ PqlRelationshipType::PARENT,		ParsedRelationship::STMT_ENTITIES },
	{ PqlRelationshipType::PARENT_T,	ParsedRelationship::STMT_ENTITIES },
	{ PqlRelationshipType::CALLS,		ParsedRelationship::PROC_ENTITIES },
	{ PqlRelationshipType::CALLS_T,		ParsedRelationship::PROC_ENTITIES },
	{ PqlRelationshipType::NEXT,		ParsedRelationship::STMT_ENTITIES },
	{ PqlRelationshipType::NEXT_T,		ParsedRelationship::STMT_ENTITIES },
	{ PqlRelationshipType::AFFECTS,		ParsedRelationship::ASSIGN_ENTITIES },
	{ PqlRelationshipType::AFFECTS_T,	ParsedRelationship::ASSIGN_ENTITIES },
};

const std::unordered_set<PqlReferenceType> ParsedRelationship::SYNONYM_REFERENCE_TYPES = {
	PqlReferenceType::PROC_NAME, PqlReferenceType::STMT_NUM,
	PqlReferenceType::VAR_NAME, PqlReferenceType::VALUE
};

ParsedRelationship::ParsedRelationship(PqlRelationshipType relationshipType,
	PqlReference lhsRef, PqlReference rhsRef)
	: relationshipType(relationshipType), lhsRef(lhsRef), rhsRef(rhsRef) {}

PqlRelationshipType ParsedRelationship::getRelationshipType() const {
	return relationshipType;
}

PqlReference ParsedRelationship::getLhs() const {
	return lhsRef;
}

PqlReference ParsedRelationship::getRhs() const {
	return rhsRef;
}

/* Factory Pattern used to create instructions */
Instruction* ParsedRelationship::toInstruction(
	const std::unordered_map<std::string, EntityType>& declarationMap,
	PKBGetter* pkbGetter,
	Processors processors) {
	validate(declarationMap);

	Instruction* instruction = nullptr;
	switch (relationshipType) {
	case PqlRelationshipType::MODIFIES_S:
		instruction = new ModifiesSInstruction(lhsRef, rhsRef, pkbGetter);
		break;
	case PqlRelationshipType::MODIFIES_P:
		instruction = new ModifiesPInstruction(lhsRef, rhsRef, pkbGetter);
		break;
	case PqlRelationshipType::USES_S:
		instruction = new UsesSInstruction(lhsRef, rhsRef, pkbGetter);
		break;
	case PqlRelationshipType::USES_P:
		instruction = new UsesPInstruction(lhsRef, rhsRef, pkbGetter);
		break;
	case PqlRelationshipType::FOLLOWS:
		instruction = new FollowsInstruction(lhsRef, rhsRef, pkbGetter);
		break;
	case PqlRelationshipType::FOLLOWS_T:
		instruction = new FollowsTInstruction(lhsRef, rhsRef, pkbGetter);
		break;
	case PqlRelationshipType::PARENT:
		instruction = new ParentInstruction(lhsRef, rhsRef, pkbGetter);
		break;
	case PqlRelationshipType::PARENT_T:
		instruction = new ParentTInstruction(lhsRef, rhsRef, pkbGetter);
		break;
	case PqlRelationshipType::CALLS:
		instruction = new CallsInstruction(lhsRef, rhsRef, pkbGetter);
		break;
	case PqlRelationshipType::CALLS_T:
		instruction = new CallsTInstruction(lhsRef, rhsRef, pkbGetter);
		break;
	case PqlRelationshipType::NEXT:
		instruction = new NextInstruction(lhsRef, rhsRef, pkbGetter);
		break;
	case PqlRelationshipType::NEXT_T:
		instruction = new NextTInstruction(lhsRef, rhsRef, processors.getNextTProcessor(), pkbGetter);
		break;
	case PqlRelationshipType::AFFECTS:
		instruction = new AffectsInstruction(lhsRef, rhsRef, processors.getAffectsProcessor(), pkbGetter);
		break;
	case PqlRelationshipType::AFFECTS_T:
		instruction = new AffectsTInstruction(lhsRef, rhsRef, processors.getAffectsTProcessor(), pkbGetter);
		break;
	default:
		break;
	}
	return instruction;
}

Instruction* ParsedRelationship::getLeftFilter(const std::unordered_map<std::string, EntityType>& declarationMap, PKBGetter* pkbGetter) {
	return getArgFilter(lhsRef, declarationMap, pkbGetter);
}

Instruction* ParsedRelationship::getRightFilter(const std::unordered_map<std::string, EntityType>& declarationMap, PKBGetter* pkbGetter) {
	return getArgFilter(rhsRef, declarationMap, pkbGetter);
}

void ParsedRelationship::validate(const std::unordered_map<std::string, EntityType>& declarationMap) {
	PqlReferenceType lhsType = lhsRef.first;
	PqlReferenceType rhsType = rhsRef.first;

	/* If we haven't implemented a certain relationship, throw an exception */
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
		std::string& rhsName = rhsRef.second;
		if (declarationMap.find(rhsName) == declarationMap.end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		EntityType rhsEntity = declarationMap.at(rhsName);
		if (VALID_RIGHT_DESIGN_ENTITIES.at(relationshipType).find(rhsEntity) == VALID_RIGHT_DESIGN_ENTITIES.at(relationshipType).end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
	}

	/* If we haven't implemented a certain relationship, throw an exception */
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
			/* If we haven't implemented a certain relationship, throw an exception */
			if (VALID_LEFT_DESIGN_ENTITIES.find(reassignType) == VALID_LEFT_DESIGN_ENTITIES.end()) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
			std::string& lhsName = lhsRef.second;
			if (declarationMap.find(lhsName) == declarationMap.end()) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
			EntityType lhsEntity = declarationMap.at(lhsName);
			if (VALID_LEFT_DESIGN_ENTITIES.at(reassignType).find(lhsEntity) == VALID_LEFT_DESIGN_ENTITIES.at(reassignType).end()) {
				continue;
			}
		}
		relationshipType = reassignType;
		foundRelationship = true;
		break;
	}
	if (!foundRelationship) {
		throw QPSException(QPSException::VALIDATOR_ERROR);
	}
}

Instruction* ParsedRelationship::getArgFilter(
	const PqlReference& pqlRef,
	const std::unordered_map<std::string, EntityType>& declarationMap,
	PKBGetter* pkbGetter) {
	if (!QPSCommons::isSynonymRef(pqlRef)) {
		return nullptr;
	}
	if (declarationMap.find(pqlRef.second) == declarationMap.end()) {
		throw QPSException(QPSException::VALIDATOR_ERROR);
	}
	EntityType entType = declarationMap.at(pqlRef.second);
	ParsedGetAll getAll(entType, pqlRef.second);
	return getAll.toInstruction(pkbGetter);
}
