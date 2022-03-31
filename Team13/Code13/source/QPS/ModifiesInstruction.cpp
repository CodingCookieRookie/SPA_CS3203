#include "ModifiesInstruction.h"

ModifiesInstruction::ModifiesInstruction(PqlReference lhsRef, PqlReference rhsRef) : RelationshipInstruction(lhsRef, rhsRef) {}

//ModifiesSInstruction::ModifiesSInstruction(PqlReference lhsRef, PqlReference rhsRef) : lhsRef(lhsRef), rhsRef(rhsRef) {}
//
//ModifiesPInstruction::ModifiesPInstruction(PqlReference lhsRef, PqlReference rhsRef) : lhsRef(lhsRef), rhsRef(rhsRef) {}

//ModifiesSInstruction::ModifiesSInstruction(PqlReference lhsRef, PqlReference rhsRef) {}
//
//ModifiesPInstruction::ModifiesPInstruction(PqlReference lhsRef, PqlReference rhsRef) {}

EvaluatedTable ModifiesSInstruction::execute() {
	/* Modifies (1, v)	or Modifies(1, "x")  => true or Modifies (1, _ ) (under statement) */
	EvaluatedTable evTable;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	std::vector<int> allStmts;
	std::vector<int> varIndices;
	switch (lhsRef.first) {
	case PqlReferenceType::SYNONYM:
		return ModifiesInstruction::handleSynonymLeft(PQLmap, lhsRef, rhsRef, allStmts, varIndices, PqlRelationshipType::MODIFIES_S);
	case PqlReferenceType::INTEGER:
		return ModifiesInstruction::handleIntegerLeft(PQLmap, lhsRef, rhsRef, allStmts, varIndices);
	default:
		break;
	}
	return evTable;
}

EvaluatedTable ModifiesPInstruction::execute() {
	/* Modifies(p / p1, v) or Modifies(p / p1, _)	Modifies(p / p1, "x") */
	/* Modifies("p", v) or Modifies("p" , _)	Modifies("p", "x") */
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	std::vector<int> allStmts;
	std::vector<int> varIndices;
	switch (lhsRef.first) {
	case PqlReferenceType::SYNONYM:
		// same as handleModfiesS
		return handleSynonymLeft(PQLmap, lhsRef, rhsRef, allStmts, varIndices, PqlRelationshipType::MODIFIES_P);
	case PqlReferenceType::IDENT:
		return handleIdentLeft(PQLmap, lhsRef, rhsRef, allStmts, varIndices);
	default:
		break;
	}
}

EvaluatedTable ModifiesInstruction::handleSynonymLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices, PqlRelationshipType pqlRelationshipType) {
	std::tuple<std::vector<int>, std::vector<int>> allStmtVarInfos = pqlRelationshipType == PqlRelationshipType::MODIFIES_S ? ModifiesS::getAllSynonymVarInfo() : ModifiesP::getAllSynonymVarInfo();
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
			allStmts = pqlRelationshipType == PqlRelationshipType::MODIFIES_S ? ModifiesS::getFromVariable(varIndex) : ModifiesP::getFromVariable(varIndex);
		}
		break;
	default:
		break;
	}
	PQLmap[lhsRef.second] = allStmts;
	return EvaluatedTable(PQLmap);
}

EvaluatedTable ModifiesInstruction::handleIntegerLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices) {
	int lhsRefValue = stoi(lhsRef.second);
	if (!Entity::containsStmt(lhsRefValue)) {
		return EvaluatedTable(false);
	}
	VarIndex varIndex;
	StmtIndex stmtIndex = { lhsRefValue };
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		varIndices = ModifiesS::getVariables(stmtIndex);
		PQLmap[rhsRef.second] = varIndices;
		return EvaluatedTable(PQLmap);
	case PqlReferenceType::WILDCARD:
		return EvaluatedTable(ModifiesS::getVariables(stmtIndex).size() > 0);
	case PqlReferenceType::IDENT:
		if (!Entity::containsVar(rhsRef.second)) {
			return EvaluatedTable(false);
		}
		varIndex = Entity::getVarIdx(rhsRef.second);
		return EvaluatedTable(ModifiesS::contains(stmtIndex, varIndex));
	default:
		break;
	}
	return EvaluatedTable(PQLmap);
}

EvaluatedTable ModifiesInstruction::handleIdentLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices) {
	if (!Entity::containsProc(lhsRef.second)) {
		return EvaluatedTable(false);
	}
	ProcIndex procIndex = Entity::getProcIdx(lhsRef.second);
	VarIndex varIndex;
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		varIndices = ModifiesP::getVariables(procIndex);
		PQLmap[rhsRef.second] = varIndices;
		return EvaluatedTable(PQLmap);
	case PqlReferenceType::WILDCARD:
		return EvaluatedTable(ModifiesP::getVariables(procIndex).size() > 0);
	case PqlReferenceType::IDENT:
		if (!Entity::containsVar(rhsRef.second)) {
			return EvaluatedTable(false);
		}
		varIndex = Entity::getVarIdx(rhsRef.second);
		return EvaluatedTable(ModifiesP::contains(procIndex, varIndex));
	default:
		break;
	}
	return EvaluatedTable(PQLmap);
}
