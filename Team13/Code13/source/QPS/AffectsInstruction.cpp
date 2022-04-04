#include "AffectsInstruction.h"

EvaluatedTable AffectsInstruction::handleWildCardLeft(std::unordered_map<std::string, std::vector<int>> PQLmap,
	PqlReference lhsRef, PqlReference rhsRef,
	std::vector<int> allStmts, std::vector<int> varIndices) {
	std::tuple<std::vector<int>, std::vector<int>> allStmtVarInfos = affectsProcessor->getAll();
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
		allStmts = affectsProcessor->getUsingRightStmtIndex(StmtIndex(rhsRefValue));
		return EvaluatedTable(allStmts.size() > 0);
	default:
		break;
	}
	return EvaluatedTable(PQLmap);
}

EvaluatedTable AffectsInstruction::handleSynonymLeft(std::unordered_map<std::string, std::vector<int>> PQLmap,
	PqlReference lhsRef, PqlReference rhsRef,
	std::vector<int> allStmts, std::vector<int> varIndices) {
	std::tuple<std::vector<int>, std::vector<int>> allStmtVarInfos = affectsProcessor->getAll();
	int rhsRefValue;
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		if (lhsRef.second == rhsRef.second) { /* Special case: Affects(s1, s1) has a legitimate result */
			std::vector<int> lhsAssigns = std::get<0>(allStmtVarInfos);
			std::vector<int> rhsAssigns = std::get<1>(allStmtVarInfos);
			std::vector<int> finalAssigns;
			for (size_t i = 0; i < lhsAssigns.size(); i++) {
				if (lhsAssigns[i] == rhsAssigns[i]) {
					finalAssigns.emplace_back(lhsAssigns[i]);
				}
			}
			PQLmap[lhsRef.second] = finalAssigns;
			return EvaluatedTable(PQLmap);
		}
		varIndices = std::get<1>(allStmtVarInfos);
		PQLmap[rhsRef.second] = varIndices;
		allStmts = std::get<0>(allStmtVarInfos);
		break;
	case PqlReferenceType::WILDCARD:
		allStmts = std::get<0>(allStmtVarInfos);
		break;
	case PqlReferenceType::INTEGER:
		rhsRefValue = stoi(rhsRef.second);
		allStmts = affectsProcessor->getUsingRightStmtIndex(StmtIndex(rhsRefValue));
		break;
	default:
		break;
	}
	PQLmap[lhsRef.second] = allStmts;
	return EvaluatedTable(PQLmap);
}

EvaluatedTable AffectsInstruction::handleIntegerLeft(std::unordered_map<std::string, std::vector<int>> PQLmap,
	PqlReference lhsRef, PqlReference rhsRef,
	std::vector<int> allStmts, std::vector<int> varIndices) {
	int lhsRefValue = stoi(lhsRef.second);
	int rhsRefValue;
	if (!Entity::containsStmt(StmtIndex(lhsRefValue))) {
		return EvaluatedTable(false);
	}
	VarIndex varIndex;
	StmtIndex leftStmtIndex = { lhsRefValue };
	allStmts = affectsProcessor->getUsingLeftStmtIndex(leftStmtIndex);
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		PQLmap[rhsRef.second] = allStmts;
		return EvaluatedTable(PQLmap);
	case PqlReferenceType::WILDCARD:
		return EvaluatedTable(allStmts.size() > 0);
	case PqlReferenceType::INTEGER:
		rhsRefValue = stoi(rhsRef.second);
		return EvaluatedTable(affectsProcessor->doesRsHold(leftStmtIndex, StmtIndex(rhsRefValue)));
	default:
		break;
	}
	return EvaluatedTable(PQLmap);
}

EvaluatedTable AffectsInstruction::execute() {
	EvaluatedTable evTable;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	std::vector<int> allStmts;
	std::vector<int> varIndices;
	switch (lhsRef.first) {
	case PqlReferenceType::WILDCARD:
		return AffectsInstruction::handleWildCardLeft(PQLmap, lhsRef, rhsRef, allStmts, varIndices);
	case PqlReferenceType::SYNONYM:
		return AffectsInstruction::handleSynonymLeft(PQLmap, lhsRef, rhsRef, allStmts, varIndices);
	case PqlReferenceType::INTEGER:
		return AffectsInstruction::handleIntegerLeft(PQLmap, lhsRef, rhsRef, allStmts, varIndices);
	default:
		break;
	}
}
