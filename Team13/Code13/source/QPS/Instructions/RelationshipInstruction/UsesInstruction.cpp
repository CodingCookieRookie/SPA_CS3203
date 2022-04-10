#include "UsesInstruction.h"

UsesInstruction::UsesInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter) :
	RelationshipInstruction(lhsRef, rhsRef, pkbGetter) {}

EvaluatedTable UsesSInstruction::execute() {
	/* Uses (a/r/s/a1, v) or Uses(a/r/s/a1, "x") or Uses (a/r/s/a1, _ ) or Uses (1, v)	=> true or Uses (1, _ ) (under statement) */
	EvaluatedTable evTable;
	Table PQLmap;
	std::vector<Index> allStmts;
	std::vector<Index> varIndices;
	switch (lhsRef.first) {
	case PqlReferenceType::SYNONYM:
		return UsesSInstruction::handleSynonymLeft(PQLmap, lhsRef, rhsRef, allStmts, varIndices, PqlRelationshipType::USES_S);
	case PqlReferenceType::INTEGER:
		return UsesSInstruction::handleIntegerLeft(PQLmap, lhsRef, rhsRef, allStmts, varIndices);
	default:
		break;
	}
	return evTable;
}

EvaluatedTable UsesPInstruction::execute() {
	/* Uses (p/p1, v) or Uses (p/p1, "x") or Uses (p/p1, _ ) */
	Table PQLmap;
	std::vector<Index> allStmts;
	std::vector<Index> varIndices;
	switch (lhsRef.first) {
	case PqlReferenceType::SYNONYM:
		// same as handleModfiesS
		return handleSynonymLeft(PQLmap, lhsRef, rhsRef, allStmts, varIndices, PqlRelationshipType::USES_P);
	case PqlReferenceType::IDENT:
		return handleIdentLeft(PQLmap, lhsRef, rhsRef, allStmts, varIndices);
	default:
		break;
	}
	return EvaluatedTable(PQLmap);
}

EvaluatedTable UsesInstruction::handleSynonymLeft(Table PQLmap, PqlReference lhsRef, PqlReference rhsRef,
	std::vector<Index> allStmts, std::vector<Index> varIndices, PqlRelationshipType pqlRelationshipType) {
	std::tuple<std::vector<Index>, std::vector<Index>> allStmtVarInfos = pqlRelationshipType == PqlRelationshipType::USES_S
		? pkbGetter->getRSAllInfo(RelationshipType::USES_S) : pkbGetter->getRSAllInfo(RelationshipType::USES_P);
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		varIndices = std::get<1>(allStmtVarInfos);
		PQLmap[rhsRef.second] = varIndices;
		allStmts = std::get<0>(allStmtVarInfos);
		break;
	case PqlReferenceType::WILDCARD:
		allStmts = std::get<0>(allStmtVarInfos);
		break;
	case PqlReferenceType::IDENT:
		if (pkbGetter->containsNameIdxEntity(EntityType::VARIABLE, rhsRef.second)) {
			VarIndex varIndex = pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, rhsRef.second);
			allStmts = pqlRelationshipType == PqlRelationshipType::USES_S
				? pkbGetter->getRSInfoFromRightArg(RelationshipType::USES_S, varIndex)
				: pkbGetter->getRSInfoFromRightArg(RelationshipType::USES_P, varIndex);
		}
		break;
	default:
		break;
	}
	PQLmap[lhsRef.second] = allStmts;
	return EvaluatedTable(PQLmap);
}

EvaluatedTable UsesInstruction::handleIntegerLeft(Table PQLmap, PqlReference lhsRef, PqlReference rhsRef,
	std::vector<Index> allStmts, std::vector<Index> varIndices) {
	int lhsRefValue = stoi(lhsRef.second);
	if (!pkbGetter->containsStmt(lhsRefValue)) {
		return EvaluatedTable(false);
	}
	VarIndex varIndex;
	StmtIndex stmtIndex = { lhsRefValue };
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		varIndices = pkbGetter->getRSInfoFromLeftArg(RelationshipType::USES_S, stmtIndex);
		PQLmap[rhsRef.second] = varIndices;
		return EvaluatedTable(PQLmap);
	case PqlReferenceType::WILDCARD:
		return EvaluatedTable(pkbGetter->getRSInfoFromLeftArg(RelationshipType::USES_S, stmtIndex).size() > 0);
	case PqlReferenceType::IDENT:
		if (!pkbGetter->containsNameIdxEntity(EntityType::VARIABLE, rhsRef.second)) {
			return EvaluatedTable(false);
		}
		varIndex = pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, rhsRef.second);
		return EvaluatedTable(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIndex, varIndex));
	default:
		break;
	}
	return EvaluatedTable(PQLmap);
}

EvaluatedTable UsesInstruction::handleIdentLeft(Table PQLmap, PqlReference lhsRef, PqlReference rhsRef,
	std::vector<Index> allStmts, std::vector<Index> varIndices) {
	if (!pkbGetter->containsNameIdxEntity(EntityType::PROCEDURE, lhsRef.second)) {
		return EvaluatedTable(false);
	}
	ProcIndex procIndex = pkbGetter->getNameIdxEntityIndex(EntityType::PROCEDURE, lhsRef.second);
	VarIndex varIndex;
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		varIndices = pkbGetter->getRSInfoFromLeftArg(RelationshipType::USES_P, procIndex);
		PQLmap[rhsRef.second] = varIndices;
		return EvaluatedTable(PQLmap);
	case PqlReferenceType::WILDCARD:
		return EvaluatedTable(pkbGetter->getRSInfoFromLeftArg(RelationshipType::USES_P, procIndex).size() > 0);
	case PqlReferenceType::IDENT:
		if (!pkbGetter->containsNameIdxEntity(EntityType::VARIABLE, rhsRef.second)) {
			return EvaluatedTable(false);
		}
		varIndex = pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, rhsRef.second);
		return EvaluatedTable(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIndex, varIndex));
	default:
		break;
	}
	return EvaluatedTable(PQLmap);
}
