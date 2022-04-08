#include "AffectsStarInstruction.h"

AffectsStarInstruction::AffectsStarInstruction(PqlReference lhsRef, PqlReference rhsRef, AffectsTProcessor* affectsTProcessor, PKBGetter* pkbGetter) :
	RelationshipInstruction(lhsRef, rhsRef, affectsTProcessor, pkbGetter) {}

EvaluatedTable AffectsStarInstruction::handleWildCardLeft(std::unordered_map<std::string, std::vector<int>> PQLmap,
	PqlReference lhsRef, PqlReference rhsRef,
	std::vector<int> allStmts, std::vector<int> varIndices) {
	std::tuple<std::vector<int>, std::vector<int>> allStmtVarInfos = affectsTProcessor->getAll(pkbGetter);
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
		allStmts = affectsTProcessor->getUsingRightStmtIndex(rhsRefValue, pkbGetter);
		return EvaluatedTable(allStmts.size() > 0);
	default:
		break;
	}
	return EvaluatedTable(PQLmap);
}

EvaluatedTable AffectsStarInstruction::handleSynonymLeft(std::unordered_map<std::string, std::vector<int>> PQLmap,
	PqlReference lhsRef, PqlReference rhsRef,
	std::vector<int> allStmts, std::vector<int> varIndices) {
	std::tuple<std::vector<int>, std::vector<int>> allStmtVarInfos = affectsTProcessor->getAll(pkbGetter);
	int rhsRefValue;
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		if (lhsRef.second == rhsRef.second) { /* Special case: Affects*(s1, s1) has a legitimate result */
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
		allStmts = affectsTProcessor->getUsingRightStmtIndex(rhsRefValue, pkbGetter);
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
	if (!pkbGetter->containsStmt(lhsRefValue)) {
		return EvaluatedTable(false);
	}
	StmtIndex stmtIndex = { lhsRefValue };
	allStmts = affectsTProcessor->getUsingLeftStmtIndex(lhsRefValue, pkbGetter);
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		PQLmap[rhsRef.second] = allStmts;
		return EvaluatedTable(PQLmap);
	case PqlReferenceType::WILDCARD:
		return EvaluatedTable(allStmts.size() > 0);
	case PqlReferenceType::INTEGER:
		rhsRefValue = stoi(rhsRef.second);
		return EvaluatedTable(affectsTProcessor->doesRsHold(lhsRefValue, rhsRefValue, pkbGetter));
	default:
		break;
	}
	return EvaluatedTable(PQLmap);
}

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
	return evTable;
}
