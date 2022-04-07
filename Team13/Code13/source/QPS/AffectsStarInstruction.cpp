#include "AffectsStarInstruction.h"

EvaluatedTable AffectsStarInstruction::handleWildCardLeft(std::unordered_map<std::string, std::vector<int>> PQLmap,
	PqlReference lhsRef, PqlReference rhsRef,
	std::vector<int> allStmts, std::vector<int> varIndices) {
	std::tuple<std::vector<int>, std::vector<int>> allStmtVarInfos = affectsTProcessor->getAll();
	int rhsRefValue;
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		varIndices = std::get<1>(allStmtVarInfos);
		PQLmap[rhsRef.second] = varIndices;
		allStmts = std::get<0>(allStmtVarInfos);
		break;
	case PqlReferenceType::WILDCARD:
		allStmts = std::get<0>(allStmtVarInfos);
		return EvaluatedTable(allStmts.size() > 0);
	case PqlReferenceType::INTEGER:
		rhsRefValue = stoi(rhsRef.second);
		allStmts = affectsTProcessor->getUsingRightStmtIndex(rhsRefValue);
		return EvaluatedTable(allStmts.size() > 0);
	default:
		break;
	}
	return EvaluatedTable(PQLmap);
}

EvaluatedTable AffectsStarInstruction::handleSynonymLeft(std::unordered_map<std::string, std::vector<int>> PQLmap,
	PqlReference lhsRef, PqlReference rhsRef,
	std::vector<int> allStmts, std::vector<int> varIndices) {
	std::tuple<std::vector<int>, std::vector<int>> allStmtVarInfos = affectsTProcessor->getAll();
	int rhsRefValue;
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		if (lhsRef.second == rhsRef.second) return EvaluatedTable(false);
		varIndices = std::get<1>(allStmtVarInfos);
		PQLmap[rhsRef.second] = varIndices;
		allStmts = std::get<0>(allStmtVarInfos);
		break;
	case PqlReferenceType::WILDCARD:
		allStmts = std::get<0>(allStmtVarInfos);
		break;
	case PqlReferenceType::INTEGER:
		rhsRefValue = stoi(rhsRef.second);
		allStmts = affectsTProcessor->getUsingRightStmtIndex(rhsRefValue);
		break;
	default:
		break;
	}
	PQLmap[lhsRef.second] = allStmts;
	return EvaluatedTable(PQLmap);
}

EvaluatedTable AffectsStarInstruction::handleIntegerLeft(std::unordered_map<std::string, std::vector<int>> PQLmap,
	PqlReference lhsRef, PqlReference rhsRef,
	std::vector<int> allStmts, std::vector<int> varIndices) {
	int lhsRefValue = stoi(lhsRef.second);
	int rhsRefValue;
	if (!Entity::containsStmt(lhsRefValue)) {
		return EvaluatedTable(false);
	}
	VarIndex varIndex;
	StmtIndex stmtIndex = { lhsRefValue };
	allStmts = affectsTProcessor->getUsingLeftStmtIndex(lhsRefValue);
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		PQLmap[rhsRef.second] = allStmts;
		return EvaluatedTable(PQLmap);
	case PqlReferenceType::WILDCARD:
		return EvaluatedTable(allStmts.size() > 0);
	case PqlReferenceType::INTEGER:
		rhsRefValue = stoi(rhsRef.second);
		return EvaluatedTable(affectsTProcessor->doesRsHold(lhsRefValue, rhsRefValue));
	default:
		break;
	}
	return EvaluatedTable(PQLmap);
}

AffectsStarInstruction::AffectsStarInstruction(PqlReference lhsRef, PqlReference rhsRef, AffectsTProcessor* affectsTProcessor) :
	RelationshipInstruction(lhsRef, rhsRef, affectsTProcessor) {}

EvaluatedTable AffectsStarInstruction::execute() {
	EvaluatedTable evTable;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	std::vector<int> allStmts;
	std::vector<int> varIndices;
	switch (lhsRef.first) {
	case PqlReferenceType::WILDCARD:
		return AffectsStarInstruction::handleWildCardLeft(PQLmap, lhsRef, rhsRef, allStmts, varIndices);
	case PqlReferenceType::SYNONYM:
		return AffectsStarInstruction::handleSynonymLeft(PQLmap, lhsRef, rhsRef, allStmts, varIndices);
	case PqlReferenceType::INTEGER:
		return AffectsStarInstruction::handleIntegerLeft(PQLmap, lhsRef, rhsRef, allStmts, varIndices);
	default:
		break;
	}
}
