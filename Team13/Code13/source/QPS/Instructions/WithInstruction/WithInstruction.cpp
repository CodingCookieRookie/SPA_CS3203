#include "WithInstruction.h"

EvaluatedTable WithInstruction::handleInt() {
	if (lhs.first == PqlReferenceType::INTEGER && rhs.first == PqlReferenceType::INTEGER) {
		return EvaluatedTable(lhs.second == rhs.second);
	}
	if (lhs.first == PqlReferenceType::INTEGER) {
		int intVal = stoi(lhs.second);
		if (pkbGetter->hasEqualIntegerAttribute(rhsEntity, intVal)) { /* Read, 1*/
			Table PQLmap{ { rhs.second, { intVal } } };
			return EvaluatedTable(PQLmap);
		}
		return EvaluatedTable(false);
	}
	if (rhs.first == PqlReferenceType::INTEGER) {
		int intVal = stoi(rhs.second);
		if (pkbGetter->hasEqualIntegerAttribute(lhsEntity, intVal)) {
			Table PQLmap{ { lhs.second, { intVal } } };
			return EvaluatedTable(PQLmap);
		}
	}
	std::vector<Index> column = pkbGetter->getEqualIntegerAttributes(lhsEntity, rhsEntity);
	Table PQLmap{ { lhs.second, column }, { rhs.second, column } };
	return EvaluatedTable(PQLmap);
}

EvaluatedTable WithInstruction::handleString() {
	if (lhs.first == PqlReferenceType::IDENT && rhs.first == PqlReferenceType::IDENT) {
		return EvaluatedTable(lhs.second == rhs.second);
	}
	if (lhs.first == PqlReferenceType::IDENT) { /* "proc1" = p.procName,  get(Procedure, "proc1") */
		std::vector<Index> column = pkbGetter->getEqualNameAttributesFromName(rhsEntity, lhs.second);
		Table PQLmap{ { rhs.second, column } };
		return EvaluatedTable(PQLmap);
	}
	if (rhs.first == PqlReferenceType::IDENT) {
		std::vector<Index> column = pkbGetter->getEqualNameAttributesFromName(lhsEntity, rhs.second);
		Table PQLmap{ { lhs.second, column } };
		return EvaluatedTable(PQLmap);
	}
	auto [leftCol, rightCol] = pkbGetter->getEqualNameAttributes(lhsEntity, rhsEntity);
	Table PQLmap{ { lhs.second, leftCol }, { rhs.second, rightCol } };
	return EvaluatedTable(PQLmap);
}

WithInstruction::WithInstruction(PqlReference lhs, PqlReference rhs, EntityType lhsEntity, EntityType rhsEntity, PqlAttributeType attribType, PKBGetter* pkbGetter)
	: lhs(lhs), rhs(rhs), lhsEntity(lhsEntity), rhsEntity(rhsEntity), attribType(attribType), pkbGetter(pkbGetter) {}

EvaluatedTable WithInstruction::execute() {
	EvaluatedTable evTable;
	switch (attribType) {
	case PqlAttributeType::INTEGER:
		evTable = handleInt();
		break;
	case PqlAttributeType::STRING:
		evTable = handleString();
		break;
	default:
		break;
	}
	return evTable;
}

std::unordered_set<std::string> WithInstruction::getSynonyms() {
	std::unordered_set<std::string> synonyms;
	if (QPSCommons::isSynonymRef(lhs)) {
		synonyms.insert(lhs.second);
	}
	if (QPSCommons::isSynonymRef(rhs)) {
		synonyms.insert(rhs.second);
	}
	return synonyms;
}

WithStringInstruction::WithStringInstruction(PqlReference lhs, PqlReference rhs, EntityType lhsEntity, EntityType rhsEntity, PKBGetter* pkbGetter) :
	WithInstruction(lhs, rhs, lhsEntity, rhsEntity, PqlAttributeType::UNVALIDATED, pkbGetter) {}

EvaluatedTable WithStringInstruction::execute() {
	if (lhs.first == PqlReferenceType::IDENT && rhs.first == PqlReferenceType::IDENT) {
		return EvaluatedTable(lhs.second == rhs.second);
	}
	if (lhs.first == PqlReferenceType::IDENT) { /* "proc1" = p.procName,  get(Procedure, "proc1") */
		std::vector<Index> column = pkbGetter->getEqualNameAttributesFromName(rhsEntity, lhs.second);
		Table PQLmap{ { rhs.second, column } };
		return EvaluatedTable(PQLmap);
	}
	if (rhs.first == PqlReferenceType::IDENT) {
		std::vector<Index> column = pkbGetter->getEqualNameAttributesFromName(lhsEntity, rhs.second);
		Table PQLmap{ { lhs.second, column } };
		return EvaluatedTable(PQLmap);
	}
	auto [leftCol, rightCol] = pkbGetter->getEqualNameAttributes(lhsEntity, rhsEntity);
	Table PQLmap{ { lhs.second, leftCol }, { rhs.second, rightCol } };
	return EvaluatedTable(PQLmap);
}

WithIntegerInstruction::WithIntegerInstruction(PqlReference lhs, PqlReference rhs, EntityType lhsEntity, EntityType rhsEntity, PKBGetter* pkbGetter) :
	WithInstruction(lhs, rhs, lhsEntity, rhsEntity, PqlAttributeType::UNVALIDATED, pkbGetter) {}

EvaluatedTable WithIntegerInstruction::execute() {
	if (lhs.first == PqlReferenceType::INTEGER && rhs.first == PqlReferenceType::INTEGER) {
		return EvaluatedTable(lhs.second == rhs.second);
	}
	if (lhs.first == PqlReferenceType::INTEGER) {
		int intVal = stoi(lhs.second);
		if (pkbGetter->hasEqualIntegerAttribute(rhsEntity, intVal)) { /* Read, 1*/
			Table PQLmap{ { rhs.second, { intVal } } };
			return EvaluatedTable(PQLmap);
		}
		return EvaluatedTable(false);
	}
	if (rhs.first == PqlReferenceType::INTEGER) {
		int intVal = stoi(rhs.second);
		if (pkbGetter->hasEqualIntegerAttribute(lhsEntity, intVal)) {
			Table PQLmap{ { lhs.second, { intVal } } };
			return EvaluatedTable(PQLmap);
		}
		return EvaluatedTable(false);
	}
	std::vector<Index> column = pkbGetter->getEqualIntegerAttributes(lhsEntity, rhsEntity);
	Table PQLmap{ { lhs.second, column }, { rhs.second, column } };
	return EvaluatedTable(PQLmap);
}
