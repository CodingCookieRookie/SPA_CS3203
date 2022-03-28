#include "UsesInstruction.h"

UsesInstruction::UsesInstruction(PqlReference lhsRef, PqlReference rhsRef) : lhsRef(lhsRef), rhsRef(rhsRef) {}

EvaluatedTable UsesSInstruction::execute() {
	/* Uses (a/r/s/a1, v) or Uses(a/r/s/a1, "x") or Uses (a/r/s/a1, _ ) or Uses (1, v)	=> true or Uses (1, _ ) (under statement) */
	EvaluatedTable evTable;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	std::vector<int> allStmts;
	std::vector<int> varIndices;
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
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	std::vector<int> allStmts;
	std::vector<int> varIndices;
	switch (lhsRef.first) {
	case PqlReferenceType::SYNONYM:
		// same as handleModfiesS
		return handleSynonymLeft(PQLmap, lhsRef, rhsRef, allStmts, varIndices, PqlRelationshipType::USES_P);
	case PqlReferenceType::IDENT:
		return handleIdentLeft(PQLmap, lhsRef, rhsRef, allStmts, varIndices);
	default:
		break;
	}
}

EvaluatedTable UsesInstruction::handleSynonymLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices, PqlRelationshipType pqlRelationshipType) {
	std::tuple<std::vector<int>, std::vector<int>> allStmtVarInfos = pqlRelationshipType == PqlRelationshipType::USES_S ? UsesS::getAllSynonymVarInfo() : UsesP::getAllSynonymVarInfo();
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		varIndices = std::get<1>(allStmtVarInfos);
		PQLmap[rhsRef.second] = varIndices;
		/* fall through */
	case PqlReferenceType::WILDCARD:
		allStmts = std::get<0>(allStmtVarInfos);
		break;
	case PqlReferenceType::IDENT:
		if (Entity::containsVar(rhsRef.second)) {
			VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
			allStmts = pqlRelationshipType == PqlRelationshipType::USES_S ? UsesS::getFromVariable(varIndex) : UsesP::getFromVariable(varIndex);
		}
		break;
	default:
		break;
	}
	PQLmap[lhsRef.second] = allStmts;
	return EvaluatedTable(PQLmap);
}

EvaluatedTable UsesInstruction::handleIntegerLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices) {
	int lhsRefValue = stoi(lhsRef.second);
	if (!Entity::containsStmt(lhsRefValue)) {
		return EvaluatedTable(false);
	}
	VarIndex varIndex;
	StmtIndex stmtIndex = { lhsRefValue };
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		varIndices = UsesS::getVariables(stmtIndex);
		PQLmap[rhsRef.second] = varIndices;
		return EvaluatedTable(PQLmap);
	case PqlReferenceType::WILDCARD:
		return EvaluatedTable(UsesS::getVariables(stmtIndex).size() > 0);
	case PqlReferenceType::IDENT:
		if (!Entity::containsVar(rhsRef.second)) {
			return EvaluatedTable(false);
		}
		varIndex = Entity::getVarIdx(rhsRef.second);
		return EvaluatedTable(UsesS::contains(stmtIndex, varIndex));
	default:
		break;
	}
}

EvaluatedTable UsesInstruction::handleIdentLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices) {
	if (!Entity::containsProc(lhsRef.second)) {
		return EvaluatedTable(false);
	}
	ProcIndex procIndex = Entity::getProcIdx(lhsRef.second);
	VarIndex varIndex;
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		varIndices = UsesP::getVariables(procIndex);
		PQLmap[rhsRef.second] = varIndices;
		return EvaluatedTable(PQLmap);
	case PqlReferenceType::WILDCARD:
		return EvaluatedTable(UsesP::getVariables(procIndex).size() > 0);
	case PqlReferenceType::IDENT:
		if (!Entity::containsVar(rhsRef.second)) {
			return EvaluatedTable(false);
		}
		varIndex = Entity::getVarIdx(rhsRef.second);
		return EvaluatedTable(UsesP::contains(procIndex, varIndex));
	default:
		break;
	}
}
