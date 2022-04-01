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

Instruction* ParsedRelationship::toInstruction() const {
	Instruction* instruction = nullptr;
	switch (relationshipType) {
	case PqlRelationshipType::MODIFIES_S:
		instruction = new ModifiesSInstruction(lhsRef, rhsRef);
		break;
	case PqlRelationshipType::MODIFIES_P:
		instruction = new ModifiesPInstruction(lhsRef, rhsRef);
		break;
	case PqlRelationshipType::USES_S:
		instruction = new UsesSInstruction(lhsRef, rhsRef);
		break;
	case PqlRelationshipType::USES_P:
		instruction = new UsesPInstruction(lhsRef, rhsRef);
		break;
	case PqlRelationshipType::FOLLOWS:
		instruction = new FollowsInstruction(lhsRef, rhsRef);
		break;
	case PqlRelationshipType::FOLLOWS_T:
		instruction = new FollowsStarInstruction(lhsRef, rhsRef);
		break;
	case PqlRelationshipType::PARENT:
		instruction = new ParentInstruction(lhsRef, rhsRef);
		break;
	case PqlRelationshipType::PARENT_T:
		instruction = new ParentStarInstruction(lhsRef, rhsRef);
		break;
	case PqlRelationshipType::CALLS:
		instruction = new CallsInstruction(lhsRef, rhsRef);
		break;
	case PqlRelationshipType::CALLS_T:
		instruction = new CallsStarInstruction(lhsRef, rhsRef);
		break;
	case PqlRelationshipType::NEXT:
		instruction = new NextInstruction(lhsRef, rhsRef);
		break;
	default:
		break;
	}
	return instruction;
}
