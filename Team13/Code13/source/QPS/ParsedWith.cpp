#include "ParsedWith.h"

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
