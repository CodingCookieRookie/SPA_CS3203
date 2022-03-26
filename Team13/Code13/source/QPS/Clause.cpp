#include "Clause.h"
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
	case PqlRelationshipType::ModifiesS:
		instruction = new ModifiesSInstruction(lhsRef, rhsRef);
		break;
	case PqlRelationshipType::ModifiesP:
		instruction = new ModifiesPInstruction(lhsRef, rhsRef);
		break;
	case PqlRelationshipType::UsesS:
		instruction = new UsesSInstruction(lhsRef, rhsRef);
		break;
	case PqlRelationshipType::UsesP:
		instruction = new UsesPInstruction(lhsRef, rhsRef);
		break;
		/*case PqlRelationshipType::Follows:
			evTable = handleFollows();
			break;
		case PqlRelationshipType::FollowsT:
			evTable = handleFollowsT();
			break;
		case PqlRelationshipType::Parent:
			evTable = handleParent(pqlRelationshipType);
			break;
		case PqlRelationshipType::ParentT:
			evTable = handleParentT();
			break;
		case PqlRelationshipType::Calls:
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

ParsedPattern::ParsedPattern(std::string& synonym, PqlReference entRef,
	PqlExpression expressionSpec, int numOfArgs)
	: ParsedPattern(synonym, PqlPatternType::Pattern, entRef, expressionSpec, numOfArgs) {}

ParsedPattern::ParsedPattern(std::string& synonym, PqlPatternType patternType,
	PqlReference entRef, PqlExpression expressionSpec, int numOfArgs)
	: synonym(synonym), patternType(patternType), entRef(entRef),
	expressionSpec(expressionSpec), numOfArgs(numOfArgs) {}

std::string ParsedPattern::getSynonym() const {
	return synonym;
}

PqlPatternType ParsedPattern::getPatternType() const {
	return patternType;
}

PqlReference ParsedPattern::getEntRef() const {
	return entRef;
}

PqlExpression ParsedPattern::getExpression() const {
	return expressionSpec;
}

int ParsedPattern::getNumOfArgs() const {
	return numOfArgs;
}

/* Default constructor used while parsing
Temporarily assign attribType as unvalidated because
we delay type validation to the validator */
ParsedWith::ParsedWith(PqlReference lhs, PqlReference rhs)
	: ParsedWith(lhs, rhs, PqlEntityType::Constant, PqlEntityType::Constant, PqlAttributeType::Unvalidated) {}

ParsedWith::ParsedWith(PqlReference lhs, PqlReference rhs, PqlEntityType lhsEntity, PqlEntityType rhsEntity, PqlAttributeType attribType)
	: lhs(lhs), rhs(rhs), lhsEntity(lhsEntity), rhsEntity(rhsEntity), attribType(attribType) {}

PqlReference ParsedWith::getLhs() const {
	return lhs;
}

PqlReference ParsedWith::getRhs() const {
	return rhs;
}

PqlAttributeType ParsedWith::getAttribType() const {
	return attribType;
}

Instruction* ParsedWith::toInstruction() const {
	//PqlAttributeType lhsAttrib;
	//PqlAttributeType rhsAttrib;
	//if (lhsAttrib != rhsAttrib) {
	//	/* Incompatible types */
	//	throw QPSException(QPSException::VALIDATOR_ERROR);
	//}
	Instruction* instruction = nullptr;
	switch (attribType) {
	case PqlAttributeType::String:
		instruction = new WithStringInstruction(lhs, rhs, lhsEntity, rhsEntity);
		break;
	case PqlAttributeType::Integer:
		instruction = new WithIntegerInstruction(lhs, rhs, lhsEntity, rhsEntity);
		break;
	default:
		break;
	}
	return instruction;
}
