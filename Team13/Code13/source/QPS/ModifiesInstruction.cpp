#include "ModifiesInstruction.h"

ModifiesInstruction::ModifiesInstruction(PqlReference lhsRef, PqlReference rhsRef) : lhsRef(lhsRef), rhsRef(rhsRef) {}

//ModifiesSInstruction::ModifiesSInstruction(PqlReference lhsRef, PqlReference rhsRef) : lhsRef(lhsRef), rhsRef(rhsRef) {}
//
//ModifiesPInstruction::ModifiesPInstruction(PqlReference lhsRef, PqlReference rhsRef) : lhsRef(lhsRef), rhsRef(rhsRef) {}

//ModifiesSInstruction::ModifiesSInstruction(PqlReference lhsRef, PqlReference rhsRef) {}
//
//ModifiesPInstruction::ModifiesPInstruction(PqlReference lhsRef, PqlReference rhsRef) {}

EvaluatedTable ModifiesSInstruction::execute() {
	/* Modifies (1, v)	or Modifies(1, "x")  => true or Modifies (1, _ ) (under statement) */
	EvaluatedTable evTable;
	std::unordered_map<std::string, PqlEntityType> PQLentities;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	std::vector<int> allStmts;
	std::vector<int> varIndices;
	PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
	PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Variable));
	switch (lhsRef.first) {
	case PqlReferenceType::Synonym:
		return ModifiesInstruction::handleSynonymLeft(PQLentities, PQLmap, lhsRef, rhsRef, allStmts, varIndices, PqlRelationshipType::ModifiesS);
	case PqlReferenceType::Integer:
		return ModifiesInstruction::handleIntegerLeft(PQLentities, PQLmap, lhsRef, rhsRef, allStmts, varIndices);
	default:
		break;
	}
	return evTable;
}

EvaluatedTable ModifiesPInstruction::execute() {
	/* Modifies(p / p1, v) or Modifies(p / p1, _)	Modifies(p / p1, "x") */
	/* Modifies("p", v) or Modifies("p" , _)	Modifies("p", "x") */
	std::unordered_map<std::string, PqlEntityType> PQLentities;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	std::vector<int> allStmts;
	std::vector<int> varIndices;
	PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Procedure));
	PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Variable));
	switch (lhsRef.first) {
	case PqlReferenceType::Synonym:
		// same as handleModfiesS
		return handleSynonymLeft(PQLentities, PQLmap, lhsRef, rhsRef, allStmts, varIndices, PqlRelationshipType::ModifiesP);
	case PqlReferenceType::Ident:
		return handleIdentLeft(PQLentities, PQLmap, lhsRef, rhsRef, allStmts, varIndices);
	default:
		break;
	}
}

EvaluatedTable ModifiesInstruction::handleSynonymLeft(std::unordered_map<std::string, PqlEntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices, PqlRelationshipType pqlRelationshipType) {
	std::tuple<std::vector<int>, std::vector<int>> allStmtVarInfos = pqlRelationshipType == PqlRelationshipType::ModifiesS ? ModifiesS::getAllSynonymVarInfo() : ModifiesP::getAllSynonymVarInfo();
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
			allStmts = pqlRelationshipType == PqlRelationshipType::ModifiesS ? ModifiesS::getFromVariable(varIndex) : ModifiesP::getFromVariable(varIndex);
		}
		break;
	default:
		break;
	}
	PQLmap[lhsRef.second] = allStmts;
	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable ModifiesInstruction::handleIntegerLeft(std::unordered_map<std::string, PqlEntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices) {
	int lhsRefValue = stoi(lhsRef.second);
	if (!Entity::containsStmt(lhsRefValue)) {
		return EvaluatedTable(false);
	}
	VarIndex varIndex;
	StmtIndex stmtIndex = { lhsRefValue };
	switch (rhsRef.first) {
	case PqlReferenceType::Synonym:
		varIndices = ModifiesS::getVariables(stmtIndex);
		PQLmap[rhsRef.second] = varIndices;
		return EvaluatedTable(PQLentities, PQLmap);
	case PqlReferenceType::Wildcard:
		return EvaluatedTable(ModifiesS::getVariables(stmtIndex).size() > 0);
	case PqlReferenceType::Ident:
		if (!Entity::containsVar(rhsRef.second)) {
			return EvaluatedTable(false);
		}
		varIndex = Entity::getVarIdx(rhsRef.second);
		return EvaluatedTable(ModifiesS::contains(stmtIndex, varIndex));
	default:
		break;
	}
}

EvaluatedTable ModifiesInstruction::handleIdentLeft(std::unordered_map<std::string, PqlEntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices) {
	if (!Entity::containsProc(lhsRef.second)) {
		return EvaluatedTable(false);
	}
	ProcIndex procIndex = Entity::getProcIdx(lhsRef.second);
	VarIndex varIndex;
	switch (rhsRef.first) {
	case PqlReferenceType::Synonym:
		varIndices = ModifiesP::getVariables(procIndex);
		PQLmap[rhsRef.second] = varIndices;
		return EvaluatedTable(PQLentities, PQLmap);
	case PqlReferenceType::Wildcard:
		return EvaluatedTable(ModifiesP::getVariables(procIndex).size() > 0);
	case PqlReferenceType::Ident:
		if (!Entity::containsVar(rhsRef.second)) {
			return EvaluatedTable(false);
		}
		varIndex = Entity::getVarIdx(rhsRef.second);
		return EvaluatedTable(ModifiesP::contains(procIndex, varIndex));
	default:
		break;
	}
}
