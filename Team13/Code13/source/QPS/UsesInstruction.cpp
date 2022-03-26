#include "UsesInstruction.h"

UsesInstruction::UsesInstruction(PqlReference lhsRef, PqlReference rhsRef) : lhsRef(lhsRef), rhsRef(rhsRef) {}

EvaluatedTable UsesSInstruction::execute() {
	/* Uses (a/r/s/a1, v) or Uses(a/r/s/a1, "x") or Uses (a/r/s/a1, _ ) or Uses (1, v)	=> true or Uses (1, _ ) (under statement) */
	EvaluatedTable evTable;
	std::unordered_map<std::string, PqlEntityType> PQLentities;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	std::vector<int> allStmts;
	std::vector<int> varIndices;
	PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
	PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Variable));
	switch (lhsRef.first) {
	case PqlReferenceType::Synonym:
		return UsesSInstruction::handleSynonymLeft(PQLentities, PQLmap, lhsRef, rhsRef, allStmts, varIndices, PqlRelationshipType::UsesS);
	case PqlReferenceType::Integer:
		return UsesSInstruction::handleIntegerLeft(PQLentities, PQLmap, lhsRef, rhsRef, allStmts, varIndices);
	default:
		break;
	}
	return evTable;
}

EvaluatedTable UsesPInstruction::execute() {
	/* Uses (p/p1, v) or Uses (p/p1, "x") or Uses (p/p1, _ ) */
	std::unordered_map<std::string, PqlEntityType> PQLentities;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	std::vector<int> allStmts;
	std::vector<int> varIndices;
	PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Procedure));
	PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Variable));
	switch (lhsRef.first) {
	case PqlReferenceType::Synonym:
		// same as handleModfiesS
		return handleSynonymLeft(PQLentities, PQLmap, lhsRef, rhsRef, allStmts, varIndices, PqlRelationshipType::UsesP);
	case PqlReferenceType::Ident:
		return handleIdentLeft(PQLentities, PQLmap, lhsRef, rhsRef, allStmts, varIndices);
	default:
		break;
	}
}

EvaluatedTable UsesInstruction::handleSynonymLeft(std::unordered_map<std::string, PqlEntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices, PqlRelationshipType pqlRelationshipType) {
	std::tuple<std::vector<int>, std::vector<int>> allStmtVarInfos = pqlRelationshipType == PqlRelationshipType::UsesS ? UsesS::getAllSynonymVarInfo() : UsesP::getAllSynonymVarInfo();
	switch (rhsRef.first) {
	case PqlReferenceType::Synonym:
		varIndices = std::get<1>(allStmtVarInfos);
		PQLmap[rhsRef.second] = varIndices;
		/* fall through */
	case PqlReferenceType::Wildcard:
		allStmts = std::get<0>(allStmtVarInfos);
		break;
	case PqlReferenceType::Ident:
		if (Entity::containsVar(rhsRef.second)) {
			VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
			allStmts = pqlRelationshipType == PqlRelationshipType::UsesS ? UsesS::getFromVariable(varIndex) : UsesP::getFromVariable(varIndex);
		}
		break;
	default:
		break;
	}
	PQLmap[lhsRef.second] = allStmts;
	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable UsesInstruction::handleIntegerLeft(std::unordered_map<std::string, PqlEntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices) {
	int lhsRefValue = stoi(lhsRef.second);
	if (!Entity::containsStmt(lhsRefValue)) {
		return EvaluatedTable(false);
	}
	VarIndex varIndex;
	StmtIndex stmtIndex = { lhsRefValue };
	switch (rhsRef.first) {
	case PqlReferenceType::Synonym:
		varIndices = UsesS::getVariables(stmtIndex);
		PQLmap[rhsRef.second] = varIndices;
		return EvaluatedTable(PQLentities, PQLmap);
	case PqlReferenceType::Wildcard:
		return EvaluatedTable(UsesS::getVariables(stmtIndex).size() > 0);
	case PqlReferenceType::Ident:
		if (!Entity::containsVar(rhsRef.second)) {
			return EvaluatedTable(false);
		}
		varIndex = Entity::getVarIdx(rhsRef.second);
		return EvaluatedTable(UsesS::contains(stmtIndex, varIndex));
	default:
		break;
	}
}

EvaluatedTable UsesInstruction::handleIdentLeft(std::unordered_map<std::string, PqlEntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices) {
	if (!Entity::containsProc(lhsRef.second)) {
		return EvaluatedTable(false);
	}
	ProcIndex procIndex = Entity::getProcIdx(lhsRef.second);
	VarIndex varIndex;
	switch (rhsRef.first) {
	case PqlReferenceType::Synonym:
		varIndices = UsesP::getVariables(procIndex);
		PQLmap[rhsRef.second] = varIndices;
		return EvaluatedTable(PQLentities, PQLmap);
	case PqlReferenceType::Wildcard:
		return EvaluatedTable(UsesP::getVariables(procIndex).size() > 0);
	case PqlReferenceType::Ident:
		if (!Entity::containsVar(rhsRef.second)) {
			return EvaluatedTable(false);
		}
		varIndex = Entity::getVarIdx(rhsRef.second);
		return EvaluatedTable(UsesP::contains(procIndex, varIndex));
	default:
		break;
	}
}
