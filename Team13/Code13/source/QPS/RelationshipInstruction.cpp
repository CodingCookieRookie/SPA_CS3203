#include "RelationshipInstruction.h"

RelationshipInstruction::RelationshipInstruction() {};

RelationshipInstruction::RelationshipInstruction(PqlRelationshipType pqlRSType, PqlReference lhs, PqlReference rhs) :
	pqlRelationshipType(pqlRSType), lhsRef(lhs), rhsRef(rhs) {}

EvaluatedTable RelationshipInstruction::execute() {
	return EvaluatedTable();
}
