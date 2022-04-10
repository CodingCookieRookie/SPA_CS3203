#include "AffectsInstruction.h"

const int AffectsInstruction::AFFECTS_DIFFICULTY = 10;

AffectsInstruction::AffectsInstruction(PqlReference lhsRef, PqlReference rhsRef,
	AffectsProcessor* affectsProcessor, PKBGetter* pkbGetter) :
	RelationshipInstruction(lhsRef, rhsRef, affectsProcessor, pkbGetter) {}

EvaluatedTable AffectsInstruction::handleWildCardLeft(Table PQLmap,
	PqlReference lhsRef, PqlReference rhsRef,
	std::vector<Index> allStmts, std::vector<Index> varIndices) {
	std::tuple<std::vector<Index>, std::vector<Index>> allStmtVarInfos = affectsProcessor->getAll(pkbGetter);
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
		allStmts = affectsProcessor->getUsingRightStmtIndex(rhsRefValue, pkbGetter);
		return EvaluatedTable(allStmts.size() > 0);
	default:
		break;
	}
	return EvaluatedTable(PQLmap);
}

EvaluatedTable AffectsInstruction::handleSynonymLeft(Table PQLmap,
	PqlReference lhsRef, PqlReference rhsRef,
	std::vector<Index> allStmts, std::vector<Index> varIndices) {
	std::tuple<std::vector<Index>, std::vector<Index>> allStmtVarInfos = affectsProcessor->getAll(pkbGetter);
	int rhsRefValue;
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		if (lhsRef.second == rhsRef.second) { /* Special case: Affects(s1, s1) has a legitimate result */
			std::vector<Index> lhsAssigns = std::get<0>(allStmtVarInfos);
			std::vector<Index> rhsAssigns = std::get<1>(allStmtVarInfos);
			std::vector<Index> finalAssigns;
			for (size_t i = 0; i < lhsAssigns.size(); i++) {
				if (lhsAssigns[i] == rhsAssigns[i]) {
					finalAssigns.emplace_back(lhsAssigns[i]);
				}
			}
			allStmts = finalAssigns;
		} else {
			allStmts = std::get<0>(allStmtVarInfos);
		}
		varIndices = std::get<1>(allStmtVarInfos);
		PQLmap[rhsRef.second] = varIndices;
		break;
	case PqlReferenceType::WILDCARD:
		allStmts = std::get<0>(allStmtVarInfos);
		break;
	case PqlReferenceType::INTEGER:
		rhsRefValue = stoi(rhsRef.second);
		allStmts = affectsProcessor->getUsingRightStmtIndex(rhsRefValue, pkbGetter);
		break;
	default:
		break;
	}
	PQLmap[lhsRef.second] = allStmts;
	return EvaluatedTable(PQLmap);
}

EvaluatedTable AffectsInstruction::handleIntegerLeft(Table PQLmap,
	PqlReference lhsRef, PqlReference rhsRef,
	std::vector<Index> allStmts, std::vector<Index> varIndices) {
	int lhsRefValue = stoi(lhsRef.second);
	int rhsRefValue;
	if (!pkbGetter->containsStmt(StmtIndex(lhsRefValue))) {
		return EvaluatedTable(false);
	}

	StmtIndex stmtIndex = { lhsRefValue };
	allStmts = affectsProcessor->getUsingLeftStmtIndex(lhsRefValue, pkbGetter);
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		PQLmap[rhsRef.second] = allStmts;
		return EvaluatedTable(PQLmap);
	case PqlReferenceType::WILDCARD:
		return EvaluatedTable(allStmts.size() > 0);
	case PqlReferenceType::INTEGER:
		rhsRefValue = stoi(rhsRef.second);
		return EvaluatedTable(affectsProcessor->doesRsHold(lhsRefValue, rhsRefValue, pkbGetter));
	default:
		break;
	}
	return EvaluatedTable(PQLmap);
}

EvaluatedTable AffectsInstruction::execute() {
	EvaluatedTable evTable;
	Table PQLmap;
	std::vector<Index> allStmts;
	std::vector<Index> varIndices;
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
	return evTable;
}

QueryScore AffectsInstruction::getDifficultyScore() {
	return Instruction::getDifficultyScore() + AFFECTS_DIFFICULTY;
}
