#include "ModifiesInstruction.h"

ModifiesInstruction::ModifiesInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter) : RelationshipInstruction(lhsRef, rhsRef, pkbGetter) {}

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
	return EvaluatedTable(PQLmap);
}

EvaluatedTable ModifiesInstruction::handleSynonymLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices, PqlRelationshipType pqlRelationshipType) {
	std::tuple<std::vector<int>, std::vector<int>> allStmtVarInfos = pqlRelationshipType == PqlRelationshipType::MODIFIES_S ? pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S) : pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P);

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
			allStmts = pqlRelationshipType == PqlRelationshipType::MODIFIES_S ? pkbGetter->getRSInfoFromRightArg(RelationshipType::MODIFIES_S, varIndex) : pkbGetter->getRSInfoFromRightArg(RelationshipType::MODIFIES_P, varIndex);
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
	if (!pkbGetter->containsStmt(lhsRefValue)) {
		return EvaluatedTable(false);
	}
	VarIndex varIndex;
	StmtIndex stmtIndex = { lhsRefValue };
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		//varIndices = ModifiesS::getFromLeftArg(stmtIndex);
		varIndices = pkbGetter->getRSInfoFromLeftArg(RelationshipType::MODIFIES_S, stmtIndex);
		PQLmap[rhsRef.second] = varIndices;
		return EvaluatedTable(PQLmap);
	case PqlReferenceType::WILDCARD:
		//return EvaluatedTable(ModifiesS::getFromLeftArg(stmtIndex).size() > 0);
		return EvaluatedTable(pkbGetter->getRSInfoFromLeftArg(RelationshipType::MODIFIES_S, stmtIndex).size() > 0);
	case PqlReferenceType::IDENT:
		if (!pkbGetter->containsNameIdxEntity(EntityType::VARIABLE, rhsRef.second)) {
			return EvaluatedTable(false);
		}
		varIndex = pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, rhsRef.second);
		//return EvaluatedTable(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIndex, varIndex));
		return EvaluatedTable(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIndex, varIndex));
	default:
		break;
	}
	return EvaluatedTable(PQLmap);
}

EvaluatedTable ModifiesInstruction::handleIdentLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices) {
	if (!pkbGetter->containsNameIdxEntity(EntityType::PROCEDURE, lhsRef.second)) {
		return EvaluatedTable(false);
	}
	ProcIndex procIndex = pkbGetter->getNameIdxEntityIndex(EntityType::PROCEDURE, lhsRef.second);
	VarIndex varIndex;
	switch (rhsRef.first) {
	case PqlReferenceType::SYNONYM:
		varIndices = pkbGetter->getRSInfoFromLeftArg(RelationshipType::MODIFIES_P, procIndex);
		PQLmap[rhsRef.second] = varIndices;
		return EvaluatedTable(PQLmap);
	case PqlReferenceType::WILDCARD:
		return EvaluatedTable(pkbGetter->getRSInfoFromLeftArg(RelationshipType::MODIFIES_P, procIndex).size() > 0);
	case PqlReferenceType::IDENT:
		if (!pkbGetter->containsNameIdxEntity(EntityType::VARIABLE, rhsRef.second)) {
			return EvaluatedTable(false);
		}
		varIndex = pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, rhsRef.second);
		return EvaluatedTable(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIndex, varIndex));
	default:
		break;
	}
	return EvaluatedTable(PQLmap);
}
