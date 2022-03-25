#include "WithInstruction.h"

EvaluatedTable WithInstruction::handleInt() {
	if (lhs.first == PqlReferenceType::Integer && rhs.first == PqlReferenceType::Integer) {
		return EvaluatedTable(lhs.second == rhs.second);
	}
	if (lhs.first == PqlReferenceType::Integer) {
		int intVal = stoi(lhs.second);
		if (Attribute::hasEqualIntegerAttribute(rhsEntity, intVal)) { /* Read, 1*/
			std::unordered_map<std::string, std::vector<int>> PQLmap{ { rhs.second, { intVal } } };
			std::unordered_map<std::string, PqlEntityType> PQLentities{ { rhs.second, rhsEntity } };
			return EvaluatedTable(PQLentities, PQLmap);
		}
		return EvaluatedTable(false);
	}
	if (rhs.first == PqlReferenceType::Integer) {
		int intVal = stoi(rhs.second);
		if (Attribute::hasEqualIntegerAttribute(lhsEntity, intVal)) {
			std::unordered_map<std::string, std::vector<int>> PQLmap{ { lhs.second, { intVal } } };
			std::unordered_map<std::string, PqlEntityType> PQLentities{ { lhs.second, lhsEntity } };
			return EvaluatedTable(PQLentities, PQLmap);
		}
	}
	std::vector<int> column = Attribute::getEqualIntegerAttributes(lhsEntity, rhsEntity);
	std::unordered_map<std::string, std::vector<int>> PQLmap{ { lhs.second, column }, { rhs.second, column } };
	std::unordered_map<std::string, PqlEntityType> PQLentities{ { lhs.second, lhsEntity }, { rhs.second, rhsEntity } };
	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable WithInstruction::handleString() {
	if (lhs.first == PqlReferenceType::Ident && rhs.first == PqlReferenceType::Ident) {
		return EvaluatedTable(lhs.second == rhs.second);
	}
	if (lhs.first == PqlReferenceType::Ident) { /* "proc1" = p.procName,  get(Procedure, "proc1") */
		std::vector<int> column = Attribute::getEqualNameAttributesFromName(rhsEntity, lhs.second);
		std::unordered_map<std::string, std::vector<int>> PQLmap{ { rhs.second, column } };
		std::unordered_map<std::string, PqlEntityType> PQLentities{ { rhs.second, rhsEntity } };
		return EvaluatedTable(PQLentities, PQLmap);
	}
	if (rhs.first == PqlReferenceType::Ident) {
		std::vector<int> column = Attribute::getEqualNameAttributesFromName(lhsEntity, rhs.second);
		std::unordered_map<std::string, std::vector<int>> PQLmap{ { lhs.second, column } };
		std::unordered_map<std::string, PqlEntityType> PQLentities{ { lhs.second, lhsEntity } };
		return EvaluatedTable(PQLentities, PQLmap);
	}
	auto [leftCol, rightCol] = Attribute::getEqualNameAttributes(lhsEntity, rhsEntity);
	std::unordered_map<std::string, std::vector<int>> PQLmap{ { lhs.second, leftCol }, { rhs.second, rightCol } };
	std::unordered_map<std::string, PqlEntityType> PQLentities{ { lhs.second, lhsEntity }, { rhs.second, rhsEntity } };
	return EvaluatedTable(PQLentities, PQLmap);
}

WithInstruction::WithInstruction(PqlReference lhs, PqlReference rhs, PqlEntityType lhsEntity, PqlEntityType rhsEntity, PqlAttributeType attribType)
	: lhs(lhs), rhs(rhs), lhsEntity(lhsEntity), rhsEntity(rhsEntity), attribType(attribType) {}

EvaluatedTable WithInstruction::execute() {
	EvaluatedTable evTable;
	switch (attribType) {
	case PqlAttributeType::Integer:
		evTable = handleInt();
		break;
	case PqlAttributeType::String:
		evTable = handleString();
		break;
	default:
		break;
	}
	return evTable;
}

WithStringInstruction::WithStringInstruction(PqlReference lhs, PqlReference rhs, PqlEntityType lhsEntity, PqlEntityType rhsEntity) :
	WithInstruction(lhs, rhs, lhsEntity, rhsEntity, PqlAttributeType::Unvalidated) {}

EvaluatedTable WithStringInstruction::execute() {
	if (lhs.first == PqlReferenceType::Ident && rhs.first == PqlReferenceType::Ident) {
		return EvaluatedTable(lhs.second == rhs.second);
	}
	if (lhs.first == PqlReferenceType::Ident) { /* "proc1" = p.procName,  get(Procedure, "proc1") */
		std::vector<int> column = Attribute::getEqualNameAttributesFromName(rhsEntity, lhs.second);
		std::unordered_map<std::string, std::vector<int>> PQLmap{ { rhs.second, column } };
		std::unordered_map<std::string, PqlEntityType> PQLentities{ { rhs.second, rhsEntity } };
		return EvaluatedTable(PQLentities, PQLmap);
	}
	if (rhs.first == PqlReferenceType::Ident) {
		std::vector<int> column = Attribute::getEqualNameAttributesFromName(lhsEntity, rhs.second);
		std::unordered_map<std::string, std::vector<int>> PQLmap{ { lhs.second, column } };
		std::unordered_map<std::string, PqlEntityType> PQLentities{ { lhs.second, lhsEntity } };
		return EvaluatedTable(PQLentities, PQLmap);
	}
	auto [leftCol, rightCol] = Attribute::getEqualNameAttributes(lhsEntity, rhsEntity);
	std::unordered_map<std::string, std::vector<int>> PQLmap{ { lhs.second, leftCol }, { rhs.second, rightCol } };
	std::unordered_map<std::string, PqlEntityType> PQLentities{ { lhs.second, lhsEntity }, { rhs.second, rhsEntity } };
	return EvaluatedTable(PQLentities, PQLmap);
}

WithIntegerInstruction::WithIntegerInstruction(PqlReference lhs, PqlReference rhs, PqlEntityType lhsEntity, PqlEntityType rhsEntity) :
	WithInstruction(lhs, rhs, lhsEntity, rhsEntity, PqlAttributeType::Unvalidated) {}

EvaluatedTable WithIntegerInstruction::execute() {
	if (lhs.first == PqlReferenceType::Integer && rhs.first == PqlReferenceType::Integer) {
		return EvaluatedTable(lhs.second == rhs.second);
	}
	if (lhs.first == PqlReferenceType::Integer) {
		int intVal = stoi(lhs.second);
		if (Attribute::hasEqualIntegerAttribute(rhsEntity, intVal)) { /* Read, 1*/
			std::unordered_map<std::string, std::vector<int>> PQLmap{ { rhs.second, { intVal } } };
			std::unordered_map<std::string, PqlEntityType> PQLentities{ { rhs.second, rhsEntity } };
			return EvaluatedTable(PQLentities, PQLmap);
		}
		return EvaluatedTable(false);
	}
	if (rhs.first == PqlReferenceType::Integer) {
		int intVal = stoi(rhs.second);
		if (Attribute::hasEqualIntegerAttribute(lhsEntity, intVal)) {
			std::unordered_map<std::string, std::vector<int>> PQLmap{ { lhs.second, { intVal } } };
			std::unordered_map<std::string, PqlEntityType> PQLentities{ { lhs.second, lhsEntity } };
			return EvaluatedTable(PQLentities, PQLmap);
		}
	}
	std::vector<int> column = Attribute::getEqualIntegerAttributes(lhsEntity, rhsEntity);
	std::unordered_map<std::string, std::vector<int>> PQLmap{ { lhs.second, column }, { rhs.second, column } };
	std::unordered_map<std::string, PqlEntityType> PQLentities{ { lhs.second, lhsEntity }, { rhs.second, rhsEntity } };
	return EvaluatedTable(PQLentities, PQLmap);
}
