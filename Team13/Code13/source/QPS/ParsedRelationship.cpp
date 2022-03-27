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
		/*case PqlRelationshipType::Follows:
				evTable = handleFollows();
				break;
			case PqlRelationshipType::FollowsT:
				evTable = handleFollowsT();
				break;*/
	case PqlRelationshipType::PARENT:
		instruction = new ParentInstruction(lhsRef, rhsRef);
		//evTable = handleParent(pqlRelationshipType);
		//instruction = new ParentInstruction(lhsRef, rhsRef);
		break;
	case PqlRelationshipType::PARENT_T:
		instruction = new ParentStarInstruction(lhsRef, rhsRef);
		break;
		/*case PqlRelationshipType::Calls:
			evTable = handleCalls(pqlRelationshipType);
			break;
		case PqlRelationshipType::CallsT:
			evTable = handleCalls(pqlRelationshipType);
			break;
		case PqlRelationshipType::Next:
			evTable = handleNext(pqlRelationshipType);
			break;*/
	default:
		break;
	}
	return instruction;
}
