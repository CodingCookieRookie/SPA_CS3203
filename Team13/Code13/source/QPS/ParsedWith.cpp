#include "ParsedWith.h"

/* Default constructor used while parsing
Temporarily assign attribType as unvalidated because
we delay type validation to the validator */
ParsedWith::ParsedWith(PqlReference lhs, PqlReference rhs)
	: ParsedWith(lhs, rhs, EntityType::CONSTANT, EntityType::CONSTANT, PqlAttributeType::UNVALIDATED) {}

ParsedWith::ParsedWith(PqlReference lhs, PqlReference rhs, EntityType lhsEntity, EntityType rhsEntity, PqlAttributeType attribType)
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

Instruction* ParsedWith::toInstruction(PKBGetter* pkbGetter) const {
	//PqlAttributeType lhsAttrib;
	//PqlAttributeType rhsAttrib;
	//if (lhsAttrib != rhsAttrib) {
	//	/* Incompatible types */
	//	throw QPSException(QPSException::VALIDATOR_ERROR);
	//}
	Instruction* instruction = nullptr;
	switch (attribType) {
	case PqlAttributeType::STRING:
		instruction = new WithStringInstruction(lhs, rhs, lhsEntity, rhsEntity, pkbGetter);
		break;
	case PqlAttributeType::INTEGER:
		instruction = new WithIntegerInstruction(lhs, rhs, lhsEntity, rhsEntity, pkbGetter);
		break;
	default:
		break;
	}
	return instruction;
}
