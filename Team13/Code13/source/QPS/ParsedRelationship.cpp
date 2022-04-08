#include "ParsedRelationship.h"

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
Instruction* ParsedRelationship::toInstruction(PKBGetter* pkbGetter, Processors processors) const {
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
		instruction = new FollowsStarInstruction(lhsRef, rhsRef, pkbGetter);
		break;
	case PqlRelationshipType::PARENT:
		instruction = new ParentInstruction(lhsRef, rhsRef, pkbGetter);
		break;
	case PqlRelationshipType::PARENT_T:
		instruction = new ParentStarInstruction(lhsRef, rhsRef, pkbGetter);
		break;
	case PqlRelationshipType::CALLS:
		instruction = new CallsInstruction(lhsRef, rhsRef, pkbGetter);
		break;
	case PqlRelationshipType::CALLS_T:
		instruction = new CallsStarInstruction(lhsRef, rhsRef, pkbGetter);
		break;
	case PqlRelationshipType::NEXT:
		instruction = new NextInstruction(lhsRef, rhsRef, pkbGetter);
		break;
	case PqlRelationshipType::NEXT_T:
		instruction = new NextStarInstruction(lhsRef, rhsRef, processors.getNextTProcessor(), pkbGetter);
		break;
	case PqlRelationshipType::AFFECTS:
		instruction = new AffectsInstruction(lhsRef, rhsRef, processors.getAffectsProcessor(), pkbGetter);
		break;
	case PqlRelationshipType::AFFECTS_T:
		instruction = new AffectsStarInstruction(lhsRef, rhsRef, processors.getAffectsTProcessor(), pkbGetter);
		break;
	default:
		break;
	}
	return instruction;
}
