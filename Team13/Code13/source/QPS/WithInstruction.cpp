#include "WithInstruction.h"

EvaluatedTable WithInstruction::handleInt() {
	if (lhs.first == PqlReferenceType::INTEGER && rhs.first == PqlReferenceType::INTEGER) {
		return EvaluatedTable(lhs.second == rhs.second);
	}
	if (lhs.first == PqlReferenceType::INTEGER) {
		int intVal = stoi(lhs.second);
		if (Attribute::hasEqualIntegerAttribute(rhsEntity, intVal)) { /* Read, 1*/
			std::unordered_map<std::string, std::vector<int>> PQLmap{ { rhs.second, { intVal } } };
			std::unordered_map<std::string, EntityType> PQLentities{ { rhs.second, rhsEntity } };
			return EvaluatedTable(PQLentities, PQLmap);
		}
		return EvaluatedTable(false);
	}
	if (rhs.first == PqlReferenceType::INTEGER) {
		int intVal = stoi(rhs.second);
		if (Attribute::hasEqualIntegerAttribute(lhsEntity, intVal)) {
			std::unordered_map<std::string, std::vector<int>> PQLmap{ { lhs.second, { intVal } } };
			std::unordered_map<std::string, EntityType> PQLentities{ { lhs.second, lhsEntity } };
			return EvaluatedTable(PQLentities, PQLmap);
		}
	}
	std::vector<int> column = Attribute::getEqualIntegerAttributes(lhsEntity, rhsEntity);
	std::unordered_map<std::string, std::vector<int>> PQLmap{ { lhs.second, column }, { rhs.second, column } };
	std::unordered_map<std::string, EntityType> PQLentities{ { lhs.second, lhsEntity }, { rhs.second, rhsEntity } };
	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable WithInstruction::handleString() {
	if (lhs.first == PqlReferenceType::IDENT && rhs.first == PqlReferenceType::IDENT) {
		return EvaluatedTable(lhs.second == rhs.second);
	}
	if (lhs.first == PqlReferenceType::IDENT) { /* "proc1" = p.procName,  get(Procedure, "proc1") */
		std::vector<int> column = Attribute::getEqualNameAttributesFromName(rhsEntity, lhs.second);
		std::unordered_map<std::string, std::vector<int>> PQLmap{ { rhs.second, column } };
		std::unordered_map<std::string, EntityType> PQLentities{ { rhs.second, rhsEntity } };
		return EvaluatedTable(PQLentities, PQLmap);
	}
	if (rhs.first == PqlReferenceType::IDENT) {
		std::vector<int> column = Attribute::getEqualNameAttributesFromName(lhsEntity, rhs.second);
		std::unordered_map<std::string, std::vector<int>> PQLmap{ { lhs.second, column } };
		std::unordered_map<std::string, EntityType> PQLentities{ { lhs.second, lhsEntity } };
		return EvaluatedTable(PQLentities, PQLmap);
	}
	auto [leftCol, rightCol] = Attribute::getEqualNameAttributes(lhsEntity, rhsEntity);
	std::unordered_map<std::string, std::vector<int>> PQLmap{ { lhs.second, leftCol }, { rhs.second, rightCol } };
	std::unordered_map<std::string, EntityType> PQLentities{ { lhs.second, lhsEntity }, { rhs.second, rhsEntity } };
	return EvaluatedTable(PQLentities, PQLmap);
}

WithInstruction::WithInstruction(PqlReference lhs, PqlReference rhs, EntityType lhsEntity, EntityType rhsEntity, PqlAttributeType attribType)
	: lhs(lhs), rhs(rhs), lhsEntity(lhsEntity), rhsEntity(rhsEntity), attribType(attribType) {}

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

WithStringInstruction::WithStringInstruction(PqlReference lhs, PqlReference rhs, EntityType lhsEntity, EntityType rhsEntity) :
	WithInstruction(lhs, rhs, lhsEntity, rhsEntity, PqlAttributeType::UNVALIDATED) {}

EvaluatedTable WithStringInstruction::execute() {
	if (lhs.first == PqlReferenceType::IDENT && rhs.first == PqlReferenceType::IDENT) {
		return EvaluatedTable(lhs.second == rhs.second);
	}
	if (lhs.first == PqlReferenceType::IDENT) { /* "proc1" = p.procName,  get(Procedure, "proc1") */
		std::vector<int> column = Attribute::getEqualNameAttributesFromName(rhsEntity, lhs.second);
		std::unordered_map<std::string, std::vector<int>> PQLmap{ { rhs.second, column } };
		std::unordered_map<std::string, EntityType> PQLentities{ { rhs.second, rhsEntity } };
		return EvaluatedTable(PQLentities, PQLmap);
	}
	if (rhs.first == PqlReferenceType::IDENT) {
		std::vector<int> column = Attribute::getEqualNameAttributesFromName(lhsEntity, rhs.second);
		std::unordered_map<std::string, std::vector<int>> PQLmap{ { lhs.second, column } };
		std::unordered_map<std::string, EntityType> PQLentities{ { lhs.second, lhsEntity } };
		return EvaluatedTable(PQLentities, PQLmap);
	}
	auto [leftCol, rightCol] = Attribute::getEqualNameAttributes(lhsEntity, rhsEntity);
	std::unordered_map<std::string, std::vector<int>> PQLmap{ { lhs.second, leftCol }, { rhs.second, rightCol } };
	std::unordered_map<std::string, EntityType> PQLentities{ { lhs.second, lhsEntity }, { rhs.second, rhsEntity } };
	return EvaluatedTable(PQLentities, PQLmap);
}

WithIntegerInstruction::WithIntegerInstruction(PqlReference lhs, PqlReference rhs, EntityType lhsEntity, EntityType rhsEntity) :
	WithInstruction(lhs, rhs, lhsEntity, rhsEntity, PqlAttributeType::UNVALIDATED) {}

EvaluatedTable WithIntegerInstruction::execute() {
	if (lhs.first == PqlReferenceType::INTEGER && rhs.first == PqlReferenceType::INTEGER) {
		return EvaluatedTable(lhs.second == rhs.second);
	}
	if (lhs.first == PqlReferenceType::INTEGER) {
		int intVal = stoi(lhs.second);
		if (Attribute::hasEqualIntegerAttribute(rhsEntity, intVal)) { /* Read, 1*/
			std::unordered_map<std::string, std::vector<int>> PQLmap{ { rhs.second, { intVal } } };
			std::unordered_map<std::string, EntityType> PQLentities{ { rhs.second, rhsEntity } };
			return EvaluatedTable(PQLentities, PQLmap);
		}
		return EvaluatedTable(false);
	}
	if (rhs.first == PqlReferenceType::INTEGER) {
		int intVal = stoi(rhs.second);
		if (Attribute::hasEqualIntegerAttribute(lhsEntity, intVal)) {
			std::unordered_map<std::string, std::vector<int>> PQLmap{ { lhs.second, { intVal } } };
			std::unordered_map<std::string, EntityType> PQLentities{ { lhs.second, lhsEntity } };
			return EvaluatedTable(PQLentities, PQLmap);
		}
	}
	std::vector<int> column = Attribute::getEqualIntegerAttributes(lhsEntity, rhsEntity);
	std::unordered_map<std::string, std::vector<int>> PQLmap{ { lhs.second, column }, { rhs.second, column } };
	std::unordered_map<std::string, EntityType> PQLentities{ { lhs.second, lhsEntity }, { rhs.second, rhsEntity } };
	return EvaluatedTable(PQLentities, PQLmap);
}
