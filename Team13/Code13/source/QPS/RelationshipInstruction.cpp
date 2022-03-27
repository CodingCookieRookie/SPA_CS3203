#include "RelationshipInstruction.h"
/*Should refactor all these to a Modifies Instruction then just extend Instruction class directly? Instead of RsInstruction*/
//EvaluatedTable RelationshipInstruction::handleSynonymLeft(std::unordered_map<std::string, EntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices, PqlRelationshipType pqlRelationshipType) {
//	std::tuple<std::vector<int>, std::vector<int>> allStmtVarInfos = pqlRelationshipType == PqlRelationshipType::ModifiesS ? ModifiesS::getAllSynonymVarInfo() : ModifiesP::getAllSynonymVarInfo();
//	switch (rhsRef.first) {
//	case PqlReferenceType::synonym:
//		varIndices = std::get<1>(allStmtVarInfos);
//		PQLmap[rhsRef.second] = varIndices;
//		/* fall through */
//	case PqlReferenceType::wildcard:
//		allStmts = std::get<0>(allStmtVarInfos);
//		break;
//	case PqlReferenceType::ident:
//		if (Entity::containsVar(rhsRef.second)) {
//			VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
//			allStmts = pqlRelationshipType == PqlRelationshipType::ModifiesS ? ModifiesS::getFromVariable(varIndex) : ModifiesP::getFromVariable(varIndex);
//		}
//		break;
//	default:
//		throw EvaluatorException(EvaluatorException::MODIFIES_S_ERROR);
//	}
//	PQLmap[lhsRef.second] = allStmts;
//	return EvaluatedTable(PQLentities, PQLmap);
//}
//
//EvaluatedTable RelationshipInstruction::handleIntegerLeft(std::unordered_map<std::string, EntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices) {
//	int lhsRefValue = stoi(lhsRef.second);
//	if (!Entity::containsStmt(lhsRefValue)) {
//		return EvaluatedTable(false);
//	}
//	VarIndex varIndex;
//	StmtIndex stmtIndex = { lhsRefValue };
//	switch (rhsRef.first) {
//	case PqlReferenceType::synonym:
//		varIndices = ModifiesS::getVariables(stmtIndex);
//		PQLmap[rhsRef.second] = varIndices;
//		return EvaluatedTable(PQLentities, PQLmap);
//	case PqlReferenceType::wildcard:
//		return EvaluatedTable(ModifiesS::getVariables(stmtIndex).size() > 0);
//	case PqlReferenceType::ident:
//		if (!Entity::containsVar(rhsRef.second)) {
//			return EvaluatedTable(false);
//		}
//		varIndex = Entity::getVarIdx(rhsRef.second);
//		return EvaluatedTable(ModifiesS::contains(stmtIndex, varIndex));
//	default:
//		throw EvaluatorException(EvaluatorException::MODIFIES_S_ERROR);
//	}
//}
//
//EvaluatedTable RelationshipInstruction::handleIdentLeft(std::unordered_map<std::string, EntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices) {
//	if (!Entity::containsProc(lhsRef.second)) {
//		return EvaluatedTable(false);
//	}
//	ProcIndex procIndex = Entity::getProcIdx(lhsRef.second);
//	VarIndex varIndex;
//	switch (rhsRef.first) {
//	case PqlReferenceType::synonym:
//		varIndices = ModifiesP::getVariables(procIndex);
//		PQLmap[rhsRef.second] = varIndices;
//		return EvaluatedTable(PQLentities, PQLmap);
//	case PqlReferenceType::wildcard:
//		return EvaluatedTable(ModifiesP::getVariables(procIndex).size() > 0);
//	case PqlReferenceType::ident:
//		if (!Entity::containsVar(rhsRef.second)) {
//			return EvaluatedTable(false);
//		}
//		varIndex = Entity::getVarIdx(rhsRef.second);
//		return EvaluatedTable(ModifiesP::contains(procIndex, varIndex));
//	default:
//		throw EvaluatorException(EvaluatorException::MODIFIES_S_ERROR);
//	}
//}
//
//EvaluatedTable RelationshipInstruction::handleModifiesS() {
//	/* Modifies (1, v)	or Modifies(1, "x")  => true or Modifies (1, _ ) (under statement) */
//	std::unordered_map<std::string, EntityType> PQLentities;
//	std::unordered_map<std::string, std::vector<int>> PQLmap;
//	std::vector<int> allStmts;
//	std::vector<int> varIndices;
//	PQLentities.insert(std::pair(lhsRef.second, EntityType::STMT));
//	PQLentities.insert(std::pair(rhsRef.second, EntityType::Variable));
//	switch (lhsRef.first) {
//	case PqlReferenceType::synonym:
//		return handleSynonymLeft(PQLentities, PQLmap, lhsRef, rhsRef, allStmts, varIndices, PqlRelationshipType::ModifiesS);
//	case PqlReferenceType::integer:
//		return handleIntegerLeft(PQLentities, PQLmap, lhsRef, rhsRef, allStmts, varIndices);
//	default:
//		throw EvaluatorException(EvaluatorException::MODIFIES_S_ERROR);
//	}
//}
//
//EvaluatedTable RelationshipInstruction::handleModifiesP() {
//	/* Modifies(p / p1, v) or Modifies(p / p1, _)	Modifies(p / p1, "x") */
//	/* Modifies("p", v) or Modifies("p" , _)	Modifies("p", "x") */
//	std::unordered_map<std::string, EntityType> PQLentities;
//	std::unordered_map<std::string, std::vector<int>> PQLmap;
//	std::vector<int> allStmts;
//	std::vector<int> varIndices;
//	PQLentities.insert(std::pair(lhsRef.second, EntityType::PROCEDURE));
//	PQLentities.insert(std::pair(rhsRef.second, EntityType::Variable));
//	switch (lhsRef.first) {
//	case PqlReferenceType::synonym:
//		// same as handleModfiesS
//		return handleSynonymLeft(PQLentities, PQLmap, lhsRef, rhsRef, allStmts, varIndices, PqlRelationshipType::ModifiesP);
//	case PqlReferenceType::ident:
//		return handleIdentLeft(PQLentities, PQLmap, lhsRef, rhsRef, allStmts, varIndices);
//	default:
//		throw EvaluatorException(EvaluatorException::MODIFIES_P_ERROR);
//	}
//}

//EvaluatedTable RelationshipInstruction::handleUsesS() {
//	/* Uses (a/r/s/a1, v) or Uses(a/r/s/a1, "x") or Uses (a/r/s/a1, _ )
//	 Uses (1, v)	=> true or Uses (1, _ ) (under statement) */
//	std::unordered_map<std::string, EntityType> PQLentities;
//	std::unordered_map<std::string, std::vector<int>> PQLmap;
//	PQLentities.insert(std::pair(lhsRef.second, EntityType::STMT));
//	PQLentities.insert(std::pair(rhsRef.second, EntityType::Variable));
//	std::tuple<std::vector<int>, std::vector<int>>  allStmtVarInfos = UsesS::getAllSynonymVarInfo();
//	std::vector<int> allStmts;
//	std::vector<int> varIndices;
//	if (lhsRef.first == PqlReferenceType::Synonym) {
//		if (rhsRef.first == PqlReferenceType::Ident) {
//			if (Entity::containsVar(rhsRef.second)) {
//				VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
//				allStmts = UsesS::getFromVariable(varIndex);
//			}
//		} else {
//			allStmts = std::get<0>(allStmtVarInfos);
//		}
//		PQLmap[lhsRef.second] = allStmts;
//		if (rhsRef.first != PqlReferenceType::Wildcard && rhsRef.first != PqlReferenceType::Ident) {
//			varIndices = std::get<1>(allStmtVarInfos);
//			PQLmap[rhsRef.second] = varIndices;
//		}
//	} else if (lhsRef.first == PqlReferenceType::Integer) {
//		int lhsRefValue = stoi(lhsRef.second);
//		if (Entity::containsStmt(lhsRefValue)) {
//			StmtIndex stmtIndex = { lhsRefValue };
//			if (rhsRef.first == PqlReferenceType::Synonym) {
//				varIndices = UsesS::getVariables(stmtIndex);
//				PQLmap[rhsRef.second] = varIndices;
//			} else if (rhsRef.first == PqlReferenceType::Ident) {
//				if (!Entity::containsVar(rhsRef.second)) {
//					return EvaluatedTable(false);
//				}
//				VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
//				return EvaluatedTable(UsesS::contains(stmtIndex, varIndex));
//			} else {
//				return EvaluatedTable(UsesS::getVariables(stmtIndex).size() > 0);
//			}
//		} else {
//			return EvaluatedTable(false);
//		}
//	} else {
//		throw EvaluatorException(EvaluatorException::USES_S_ERROR);
//	}
//	return EvaluatedTable(PQLentities, PQLmap);
//}
//
//EvaluatedTable RelationshipInstruction::handleUsesP() {
//	/* Uses (p/p1, v) or Uses (p/p1, "x") or Uses (p/p1, _ ) */
//	std::unordered_map<std::string, EntityType> PQLentities;
//	std::unordered_map<std::string, std::vector<int>> PQLmap;
//	PQLentities.insert(std::pair(lhsRef.second, EntityType::PROCEDURE));
//	PQLentities.insert(std::pair(rhsRef.second, EntityType::Variable));
//	std::tuple<std::vector<int>, std::vector<int>>  allProcVarInfos = UsesP::getAllSynonymVarInfo();
//	std::vector<int> allStmts;
//	std::vector<int> varIndices;
//
//	if (lhsRef.first == PqlReferenceType::Synonym) {
//		if (rhsRef.first == PqlReferenceType::Ident) {
//			if (Entity::containsVar(rhsRef.second)) {
//				VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
//				allStmts = UsesP::getFromVariable(varIndex);
//			}
//		} else {
//			allStmts = std::get<0>(allProcVarInfos);
//		}
//
//		PQLmap[lhsRef.second] = allStmts;
//
//		if (rhsRef.first != PqlReferenceType::Wildcard && rhsRef.first != PqlReferenceType::Ident) {
//			varIndices = std::get<1>(allProcVarInfos);
//			PQLmap[rhsRef.second] = varIndices;
//		}
//	} else if (lhsRef.first == PqlReferenceType::Ident) {
//		if (rhsRef.first == PqlReferenceType::Ident) {
//			if (Entity::containsProc(lhsRef.second)) {
//				ProcIndex procIndex = Entity::getProcIdx(lhsRef.second);
//				if (Entity::containsVar(rhsRef.second)) {
//					VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
//					if (UsesP::contains(procIndex, varIndex)) {
//						return EvaluatedTable(true);
//					} else {
//						return EvaluatedTable(false);
//					}
//				} else {
//					return EvaluatedTable(false);
//				}
//			}
//		} else {
//			if (Entity::containsProc(lhsRef.second)) {
//				ProcIndex procIndex = Entity::getProcIdx(lhsRef.second);
//				if (rhsRef.first == PqlReferenceType::Synonym) {
//					varIndices = UsesP::getVariables(procIndex);
//					PQLmap[rhsRef.second] = varIndices;
//				} else if (rhsRef.first == PqlReferenceType::Wildcard) {
//					if (UsesP::getVariables(procIndex).size() > 0) {
//						return EvaluatedTable(true);
//					} else {
//						return EvaluatedTable(false);
//					}
//				} else {
//					throw EvaluatorException(EvaluatorException::USES_P_ERROR);
//				}
//			}
//		}
//	} else {
//		throw EvaluatorException(EvaluatorException::USES_P_ERROR);
//	}
//	return EvaluatedTable(PQLentities, PQLmap);
//}

EvaluatedTable RelationshipInstruction::handleFollows() {
	EvaluatedTable evTable;
	std::vector<StmtIndex> stmts = Entity::getAllStmts();

	// e.g Follows(6, 7)
	if (lhsRef.first == PqlReferenceType::INTEGER && rhsRef.first == PqlReferenceType::INTEGER) {
		StmtIndex lhsStmtIndex, rhsStmtIndex;
		bool evResult = false;
		int lhsRefValue = stoi(lhsRef.second);
		int rhsRefValue = stoi(rhsRef.second);
		if (Entity::containsStmt(lhsRefValue) && Entity::containsStmt(rhsRefValue)) {
			lhsStmtIndex = StmtIndex(lhsRefValue);
			rhsStmtIndex = StmtIndex(rhsRefValue);
			evResult = Follows::containsSuccessor(lhsStmtIndex, rhsStmtIndex);
		}
		return EvaluatedTable(evResult); //e.g evResult == true, if 6 is followed by 7
	}
	// e.g Follows(6, s2), Follows(6, _)
	else if (lhsRef.first == PqlReferenceType::INTEGER) {
		std::vector<int> results;
		int lhsRefValue = stoi(lhsRef.second); // might throw error if string value can't be converted to int
		if (Entity::containsStmt(lhsRefValue)) { // checks if stmt 6 exists, if not, return empty results
			StmtIndex lhsStmt = StmtIndex(lhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (Follows::containsSuccessor(lhsStmt, stmt)) {
					results.emplace_back(stmt); // e.g {7} because 6 is followed by 7
				}
			}
		}
		if (rhsRef.first == PqlReferenceType::WILDCARD) {
			bool evTable = !results.empty();
			return EvaluatedTable(evTable);
		}
		std::unordered_map<std::string, EntityType> PQLentities;
		PQLentities.insert(std::pair(rhsRef.second, EntityType::STMT));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[rhsRef.second] = results;

		return EvaluatedTable(PQLentities, PQLmap);
	}
	// e.g. Follows(s1, 7), Follows(_, 7)
	else if (rhsRef.first == PqlReferenceType::INTEGER) {
		std::vector<int> results;
		int rhsRefValue = stoi(rhsRef.second); //might throw error if string value can't be converted to int
		if (Entity::containsStmt(rhsRefValue)) { // checks if stmt 7 exists, if not, return empty results
			StmtIndex rhsStmt = StmtIndex(rhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (Follows::containsSuccessor(stmt, rhsStmt)) {
					results.emplace_back(stmt); //e.g {3} because 3 is followed by 6
				}
			}
		}
		if (lhsRef.first == PqlReferenceType::WILDCARD) {
			bool evTable = !results.empty();
			return EvaluatedTable(evTable);
		}
		std::unordered_map<std::string, EntityType> PQLentities;
		PQLentities.insert(std::pair(lhsRef.second, EntityType::STMT));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[lhsRef.second] = results;

		return EvaluatedTable(PQLentities, PQLmap);
	}
	// Follows(s1, s2), Follows(s1, _), Follows(_, s2)
	else if (!(lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD)) {
		std::tuple<std::vector<int>, std::vector<int>> results = Follows::getAllPredecessorSuccessorInfo();
		//e.g. {1, 2}, {2, 3}, {3, 6}
		std::unordered_map<std::string, EntityType> PQLentities;
		std::unordered_map<std::string, std::vector<int>> PQLmap;

		if (lhsRef.second == rhsRef.second) { /* Special case: Follows(s1, s1)) */
			PQLentities.insert(std::pair(lhsRef.second, EntityType::STMT));
			PQLentities.insert(std::pair(rhsRef.second, EntityType::STMT));
			/* No values populated to PQLmap for this case */
			return EvaluatedTable(PQLentities, PQLmap);
		}

		if (lhsRef.first == PqlReferenceType::SYNONYM) {
			PQLentities.insert(std::pair(lhsRef.second, EntityType::STMT));
			PQLmap[lhsRef.second] = std::get<0>(results); // if RHS is wildcard, LHS may have duplicate values
		}
		if (rhsRef.first == PqlReferenceType::SYNONYM) {
			PQLentities.insert(std::pair(rhsRef.second, EntityType::STMT));
			PQLmap[rhsRef.second] = std::get<1>(results); // if LHS is wildcard, RHS may have duplicate values
		}
		return EvaluatedTable(PQLentities, PQLmap);
	}
	// Follows(_, _)
	else {
		bool isEmptyTable = true;
		if (lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD) { // Follows(_, _)
			isEmptyTable = std::get<0>(Follows::getAllPredecessorSuccessorInfo()).empty();
		}
		// No Follows rs exists => isEmptyTable == true => EvTable.evResult == false (innerJoinMerge() can drop table)
		// Follows rs exists => isEmptyTable == false => EvTable.evResult == true (innerJoinMerge() can merge dummy table, preserving all rows)
		return EvaluatedTable(!isEmptyTable);
	}
}

EvaluatedTable RelationshipInstruction::handleFollowsT() {
	EvaluatedTable evTable;
	std::vector<StmtIndex> stmts = Entity::getAllStmts();

	// e.g Follows*(6, 7)
	if (lhsRef.first == PqlReferenceType::INTEGER && rhsRef.first == PqlReferenceType::INTEGER) {
		StmtIndex lhsStmtIndex, rhsStmtIndex;
		bool evResult = false;
		int lhsRefValue = stoi(lhsRef.second);
		int rhsRefValue = stoi(rhsRef.second);
		if (Entity::containsStmt(lhsRefValue) && Entity::containsStmt(rhsRefValue)) {
			lhsStmtIndex = StmtIndex(lhsRefValue);
			rhsStmtIndex = StmtIndex(rhsRefValue);
			evResult = FollowsT::containsSuccessor(lhsStmtIndex, rhsStmtIndex);
		}
		return EvaluatedTable(evResult); //e.g evResult == true, if 6 is followed* by 7
	}
	// e.g Follows*(6, s2), Follows*(6, _)
	else if (lhsRef.first == PqlReferenceType::INTEGER) {
		std::vector<int> results;
		int lhsRefValue = stoi(lhsRef.second); // might throw error if string value can't be converted to int
		if (Entity::containsStmt(lhsRefValue)) { // checks if stmt 6 exists, if not, return empty results
			StmtIndex lhsStmt = StmtIndex(lhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (FollowsT::containsSuccessor(lhsStmt, stmt)) {
					results.emplace_back(stmt); // e.g {7} because 6 is followed by 7
				}
			}
		}
		if (rhsRef.first == PqlReferenceType::WILDCARD) {
			bool evTable = !results.empty();
			return EvaluatedTable(evTable);
		}
		std::unordered_map<std::string, EntityType> PQLentities;
		PQLentities.insert(std::pair(rhsRef.second, EntityType::STMT));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[rhsRef.second] = results;

		return EvaluatedTable(PQLentities, PQLmap);
	}
	// e.g. Follows*(s1, 7), Follows*(_, 7)
	else if (rhsRef.first == PqlReferenceType::INTEGER) {
		std::vector<int> results;
		int rhsRefValue = stoi(rhsRef.second); //might throw error if string value can't be converted to int
		if (Entity::containsStmt(rhsRefValue)) { // checks if stmt 7 exists, if not, return empty results
			StmtIndex rhsStmt = StmtIndex(rhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (FollowsT::containsSuccessor(stmt, rhsStmt)) {
					results.emplace_back(stmt); //e.g {3} because 3 is followed by 6
				}
			}
		}
		if (lhsRef.first == PqlReferenceType::WILDCARD) {
			bool evTable = !results.empty();
			return EvaluatedTable(evTable);
		}
		std::unordered_map<std::string, EntityType> PQLentities;
		PQLentities.insert(std::pair(lhsRef.second, EntityType::STMT));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[lhsRef.second] = results;

		return EvaluatedTable(PQLentities, PQLmap);
	}
	// Follows*(s1, s2), Follows*(s1, _), Follows*(_, s2)
	else if (!(lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD)) {
		//Assumption: Different synonym names (i.e. Follows(s1, s2), not Follows(s1, s1))
		std::tuple<std::vector<int>, std::vector<int>> results = FollowsT::getAllPredecessorSuccessorInfo();
		//e.g. {1, 2}, {2, 3}, {3, 6}
		std::unordered_map<std::string, EntityType> PQLentities;
		std::unordered_map<std::string, std::vector<int>> PQLmap;

		if (lhsRef.second == rhsRef.second) { /* Special case: Follows*(s1, s1)) */
			PQLentities.insert(std::pair(lhsRef.second, EntityType::STMT));
			PQLentities.insert(std::pair(rhsRef.second, EntityType::STMT));
			/* No values populated to PQLmap for this case */
			return EvaluatedTable(PQLentities, PQLmap);
		}

		if (lhsRef.first == PqlReferenceType::SYNONYM) {
			PQLentities.insert(std::pair(lhsRef.second, EntityType::STMT));
			PQLmap[lhsRef.second] = std::get<0>(results); // if RHS is wildcard, LHS may have duplicate values
		}
		if (rhsRef.first == PqlReferenceType::SYNONYM) {
			PQLentities.insert(std::pair(rhsRef.second, EntityType::STMT));
			PQLmap[rhsRef.second] = std::get<1>(results); // if LHS is wildcard, RHS may have duplicate values
		}
		return EvaluatedTable(PQLentities, PQLmap);
	}
	// Follows*(_, _)
	else {
		bool isEmptyTable = true;
		if (lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD) { // Follows(_, _)
			isEmptyTable = std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).empty();
		}
		// No Follows rs exists => isEmptyTable == true => EvTable.evResult == false (innerJoinMerge() can drop table)
		// Follows rs exists => isEmptyTable == false => EvTable.evResult == true (innerJoinMerge() can merge dummy table, preserving all rows)
		return EvaluatedTable(!isEmptyTable);
	}
}

//EvaluatedTable RelationshipInstruction::handleParent(PqlRelationshipType pqlRsType) {
//	EvaluatedTable evTable;
//	std::vector<StmtIndex> stmts = Entity::getAllStmts();
//
//	// e.g Parent(6, 7)
//	if (lhsRef.first == PqlReferenceType::Integer && rhsRef.first == PqlReferenceType::Integer) {
//		return helperHandleTwoIntegers(pqlRsType);
//	}
//	// e.g Parent(6, s2), Parent(6, _)
//	else if (lhsRef.first == PqlReferenceType::Integer) {
//		if (rhsRef.first == PqlReferenceType::Synonym) {
//			return helperHandleOneInt(pqlRsType, PqlReferenceType::Integer, PqlReferenceType::Synonym);
//		} else {
//			return helperHandleOneInt(pqlRsType, PqlReferenceType::Integer, PqlReferenceType::Wildcard);
//		}
//	}
//	// e.g. Parent(s1, 7), Parent(_ 7)
//	else if (rhsRef.first == PqlReferenceType::Integer) {
//		if (lhsRef.first == PqlReferenceType::Synonym) {
//			return helperHandleOneInt(pqlRsType, PqlReferenceType::Synonym, PqlReferenceType::Integer);
//		} else {
//			return helperHandleOneInt(pqlRsType, PqlReferenceType::Wildcard, PqlReferenceType::Integer);
//		}
//	}
//	// Parent(s1, s2), Parent(s1, _), Parent(_, s2)
//	else if (!(lhsRef.first == PqlReferenceType::Wildcard && rhsRef.first == PqlReferenceType::Wildcard)) {
//		return helperHandleTwoStmtsMaybeWildcard(pqlRsType);
//	}
//	// Parent(_, _)
//	else {
//		bool isEmptyTable = true;
//		if (lhsRef.first == PqlReferenceType::Wildcard && rhsRef.first == PqlReferenceType::Wildcard) {
//			isEmptyTable = std::get<0>(Parent::getAllPredecessorSuccessorInfo()).empty();
//		}
//		// No Parent rs exists => isEmptyTable == true => EvTable.evResult == false (innerJoinMerge() can drop table)
//		// Parent rs exists => isEmptyTable == false => EvTable.evResult == true (innerJoinMerge() can merge dummy table, preserving all rows)
//		return EvaluatedTable(!isEmptyTable);
//	}
//}

EvaluatedTable RelationshipInstruction::handleParentT() {
	EvaluatedTable evTable;
	std::vector<StmtIndex> stmts = Entity::getAllStmts();

	// e.g Parent*(6, 7)
	if (lhsRef.first == PqlReferenceType::INTEGER && rhsRef.first == PqlReferenceType::INTEGER) {
		StmtIndex lhsStmtIndex, rhsStmtIndex;
		bool evResult = false;
		int lhsRefValue = stoi(lhsRef.second);
		int rhsRefValue = stoi(rhsRef.second);
		if (Entity::containsStmt(lhsRefValue) && Entity::containsStmt(rhsRefValue)) {
			lhsStmtIndex = StmtIndex(lhsRefValue);
			rhsStmtIndex = StmtIndex(rhsRefValue);
			evResult = ParentT::containsSuccessor(lhsStmtIndex, rhsStmtIndex);
		}
		return EvaluatedTable(evResult); //e.g evResult == true, if 6 is a parent* of 7
	}
	// e.g Parent*(6, s2), Parent*(6, _)
	else if (lhsRef.first == PqlReferenceType::INTEGER) {
		std::vector<int> results;
		int lhsRefValue = stoi(lhsRef.second); // might throw error if string value can't be converted to int
		if (Entity::containsStmt(lhsRefValue)) { // checks if stmt 6 exists, if not, return empty results
			StmtIndex lhsStmt = StmtIndex(lhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (ParentT::containsSuccessor(lhsStmt, stmt)) {
					results.emplace_back(stmt); // e.g {7} because 6 is a parent* of 7
				}
			}
		}
		if (rhsRef.first == PqlReferenceType::WILDCARD) {
			bool evTable = !results.empty();
			return EvaluatedTable(evTable);
		}
		std::unordered_map<std::string, EntityType> PQLentities;
		PQLentities.insert(std::pair(rhsRef.second, EntityType::STMT));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[rhsRef.second] = results;

		return EvaluatedTable(PQLentities, PQLmap);
	}
	// e.g. Parent*(s1, 7), Parent*(_ 7)
	else if (rhsRef.first == PqlReferenceType::INTEGER) {
		std::vector<int> results;
		int rhsRefValue = stoi(rhsRef.second); //might throw error if string value can't be converted to int
		if (Entity::containsStmt(rhsRefValue)) { // checks if stmt 7 exists, if not, return empty results
			StmtIndex rhsStmt = StmtIndex(rhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (ParentT::containsSuccessor(stmt, rhsStmt)) {
					results.emplace_back(stmt); //e.g {3} because 3 is a parent* of 7
				}
			}
		}
		if (lhsRef.first == PqlReferenceType::WILDCARD) {
			bool evTable = !results.empty();
			return EvaluatedTable(evTable);
		}
		std::unordered_map<std::string, EntityType> PQLentities;
		PQLentities.insert(std::pair(lhsRef.second, EntityType::STMT));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[lhsRef.second] = results;

		return EvaluatedTable(PQLentities, PQLmap);
	}
	// Parent*(s1, s2), Parent*(s1, _), Parent*(_, s2)
	else if (!(lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD)) {
		//Assumption: Different synonym names (i.e. Parent*(s1, s2), not Parent*(s1, s1))
		std::tuple<std::vector<int>, std::vector<int>> results = ParentT::getAllPredecessorSuccessorInfo();
		//e.g. {1, 2}, {2, 3}, {3, 6}
		std::unordered_map<std::string, EntityType> PQLentities;
		std::unordered_map<std::string, std::vector<int>> PQLmap;

		if (lhsRef.second == rhsRef.second) { /* Special case: Parent*(s1, s1)) */
			PQLentities.insert(std::pair(lhsRef.second, EntityType::STMT));
			PQLentities.insert(std::pair(rhsRef.second, EntityType::STMT));
			/* No values populated to PQLmap for this case */
			return EvaluatedTable(PQLentities, PQLmap);
		}

		if (lhsRef.first == PqlReferenceType::SYNONYM) {
			PQLentities.insert(std::pair(lhsRef.second, EntityType::STMT));
			PQLmap[lhsRef.second] = std::get<0>(results); // if RHS is wildcard, LHS may have duplicate values
		}
		if (rhsRef.first == PqlReferenceType::SYNONYM) {
			PQLentities.insert(std::pair(rhsRef.second, EntityType::STMT));
			PQLmap[rhsRef.second] = std::get<1>(results); // if LHS is wildcard, RHS may have duplicate values
		}
		return EvaluatedTable(PQLentities, PQLmap);
	}
	// Parent*(_, _)
	else {
		bool isEmptyTable = true;
		if (lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD) {
			isEmptyTable = std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).empty();
		}
		// No Parent rs exists => isEmptyTable == true => EvTable.evResult == false (innerJoinMerge() can drop table)
		// Parent rs exists => isEmptyTable == false => EvTable.evResult == true (innerJoinMerge() can merge dummy table, preserving all rows)
		return EvaluatedTable(!isEmptyTable);
	}
}

EvaluatedTable RelationshipInstruction::handleCalls(PqlRelationshipType pqlRsType) {
	EvaluatedTable evTable;
	std::vector<ProcIndex> procs = Entity::getAllProcs();

	/* e.g Calls/Calls*("first", "second") */
	if (lhsRef.first == PqlReferenceType::IDENT && rhsRef.first == PqlReferenceType::IDENT) {
		return helperHandleTwoIdents(pqlRsType);
	}
	/* e.g Calls/Calls*("first", q), Calls/Calls*("first", _) */
	else if (lhsRef.first == PqlReferenceType::IDENT) {
		if (rhsRef.first == PqlReferenceType::SYNONYM) {
			return helperHandleOneIdent(pqlRsType, PqlReferenceType::IDENT, PqlReferenceType::SYNONYM);
		} else {
			return helperHandleOneIdent(pqlRsType, PqlReferenceType::IDENT, PqlReferenceType::WILDCARD);
		}
	}
	/*  e.g.Calls/Calls*(p, "second"), Calls/Calls*(_, "second") */
	else if (rhsRef.first == PqlReferenceType::IDENT) {
		if (lhsRef.first == PqlReferenceType::SYNONYM) {
			return helperHandleOneIdent(pqlRsType, PqlReferenceType::SYNONYM, PqlReferenceType::IDENT);
		} else {
			return helperHandleOneIdent(pqlRsType, PqlReferenceType::WILDCARD, PqlReferenceType::IDENT);
		}
	}
	/* Calls/Calls*(p, q), Calls/Calls*(p, _), Calls/Calls*(_, q) */
	else if (!(lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD)) {
		return helperHandleTwoProcMaybeWildcard(pqlRsType);
	}
	// Calls/Calls*(_, _)
	else {
		return helperHandleTwoWildcards(pqlRsType);
	}
}

EvaluatedTable RelationshipInstruction::handleNext(PqlRelationshipType pqlRsType) {
	EvaluatedTable evTable;
	std::vector<ProcIndex> procs = Entity::getAllProcs();

	/* e.g Next/Next*(6, 7) */
	if (lhsRef.first == PqlReferenceType::INTEGER && rhsRef.first == PqlReferenceType::INTEGER) {
		return helperHandleTwoIntegers(pqlRsType);
	}
	/* e.g Next/Next*(6, s2), Next/Next*(6, _) */
	else if (lhsRef.first == PqlReferenceType::INTEGER) {
		if (rhsRef.first == PqlReferenceType::SYNONYM) {
			return helperHandleOneInt(pqlRsType, PqlReferenceType::INTEGER, PqlReferenceType::SYNONYM);
		} else {
			return helperHandleOneInt(pqlRsType, PqlReferenceType::INTEGER, PqlReferenceType::WILDCARD);
		}
	}
	/* e.g Next/Next*(s1, 7), Next/Next*(_, 7) */
	else if (rhsRef.first == PqlReferenceType::INTEGER) {
		if (lhsRef.first == PqlReferenceType::SYNONYM) {
			return helperHandleOneInt(pqlRsType, PqlReferenceType::SYNONYM, PqlReferenceType::INTEGER);
		} else {
			return helperHandleOneInt(pqlRsType, PqlReferenceType::WILDCARD, PqlReferenceType::INTEGER);
		}
	}
	/* Next/Next*(s1, s2), Next/Next*(s1, _), Next/Next*(_, s2) */
	else if (!(lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD)) {
		return helperHandleTwoStmtsMaybeWildcard(pqlRsType);
	}
	// Next/Next*(_, _)
	else {
		return helperHandleTwoWildcards(pqlRsType);
	}
}

/* Helper Handle Methods */

EvaluatedTable RelationshipInstruction::helperHandleTwoIntegers(PqlRelationshipType pqlRsType) {
	StmtIndex lhsStmtIndex, rhsStmtIndex;
	bool evResult = false;
	int lhsRefValue = stoi(lhsRef.second);
	int rhsRefValue = stoi(rhsRef.second);
	if (Entity::containsStmt(lhsRefValue) && Entity::containsStmt(rhsRefValue)) {
		lhsStmtIndex = StmtIndex(lhsRefValue);
		rhsStmtIndex = StmtIndex(rhsRefValue);
		switch (pqlRsType) {
		case PqlRelationshipType::PARENT:
			evResult = Parent::containsSuccessor(lhsStmtIndex, rhsStmtIndex);
			break;
		case PqlRelationshipType::NEXT:
			evResult = Next::containsSuccessor(lhsStmtIndex, rhsStmtIndex);
			break;
		}
	}
	return EvaluatedTable(evResult);
}

EvaluatedTable RelationshipInstruction::helperHandleTwoIdents(PqlRelationshipType pqlRsType) {
	ProcIndex lhsProcIndex, rhsProcIndex;
	bool evResult = false;
	if (Entity::containsProc(lhsRef.second) && Entity::containsProc(rhsRef.second)) {
		lhsProcIndex = Entity::getProcIdx(lhsRef.second);
		rhsProcIndex = Entity::getProcIdx(rhsRef.second);
		if (pqlRsType == PqlRelationshipType::CALLS) {
			evResult = Calls::containsSuccessor(lhsProcIndex, rhsProcIndex);
		} else { /* == PqlRelationshipType::CallsT */
			evResult = CallsT::containsSuccessor(lhsProcIndex, rhsProcIndex);
		}
	}
	return EvaluatedTable(evResult); /* e.g evResult == true, if "first" calls "second" */
}

EvaluatedTable RelationshipInstruction::helperHandleOneInt(
	PqlRelationshipType pqlRsType, PqlReferenceType lhsRefType, PqlReferenceType rhsRefType) {
	std::vector<StmtIndex> stmts = Entity::getAllStmts();
	std::vector<int> results;
	int oneInt;
	std::string otherSynonym;
	if (lhsRefType == PqlReferenceType::INTEGER) {
		oneInt = stoi(lhsRef.second);
	} else { /* rhsRefType == PqlReferenceType::ident */
		oneInt = stoi(rhsRef.second);
	}
	/* Handle one ident to proc results */
	if (Entity::containsStmt(oneInt)) { /* e.g. checks if stmt 6 exists, if not, return empty results */
		StmtIndex oneIntIndex = StmtIndex(oneInt);
		for (StmtIndex stmt : stmts) {
			switch (pqlRsType) {
				//TODO: Compress even more
			case PqlRelationshipType::PARENT:
				if (lhsRefType == PqlReferenceType::INTEGER) {
					if (Parent::containsSuccessor(oneIntIndex, stmt)) {
						results.emplace_back(stmt); /* e.g {7} if 6 is a Parent of some s2 (e.g. 7) */
					}
				} else if (rhsRefType == PqlReferenceType::INTEGER) {
					if (Parent::containsSuccessor(stmt, oneIntIndex)) {
						results.emplace_back(stmt); /* e.g {6} if some s1 (e.g. 6) is a Parent of 7 */
					}
				} else {}
				break;
			case PqlRelationshipType::NEXT:
				if (lhsRefType == PqlReferenceType::INTEGER) {
					if (Next::containsSuccessor(oneIntIndex, stmt)) {
						results.emplace_back(stmt); /* e.g {7} if after 6, some s2 (e.g. 7) is next in the CFG */
					}
				} else if (rhsRefType == PqlReferenceType::INTEGER) {
					if (Next::containsSuccessor(stmt, oneIntIndex)) {
						results.emplace_back(stmt); /* e.g {6} if after some s1 (e.g. 6), 7 is next in the CFG */
					}
				} else {}
			}
			//TODO: Account for NextT
		}
		/* Handle final output, wildcards => boolean, synonyms => table */
		if (lhsRefType == PqlReferenceType::WILDCARD || rhsRefType == PqlReferenceType::WILDCARD) {
			bool evTable = !results.empty();
			return EvaluatedTable(evTable);
		} else {
			if (lhsRefType == PqlReferenceType::INTEGER) {
				otherSynonym = rhsRef.second;
			} else {
				otherSynonym = lhsRef.second;
			}
		}
		std::unordered_map<std::string, EntityType> PQLentities;
		PQLentities.insert(std::pair(otherSynonym, EntityType::STMT));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[otherSynonym] = results;

		return EvaluatedTable(PQLentities, PQLmap);
	}
}

EvaluatedTable RelationshipInstruction::helperHandleOneIdent(
	PqlRelationshipType pqlRsType, PqlReferenceType lhsRefType, PqlReferenceType rhsRefType) {
	std::vector<ProcIndex> procs = Entity::getAllProcs();
	std::vector<int> results;
	std::string oneIdent;
	std::string otherSynonym;
	if (lhsRefType == PqlReferenceType::IDENT) {
		oneIdent = lhsRef.second;
	} else { /* rhsRefType == PqlReferenceType::ident */
		oneIdent = rhsRef.second;
	}
	/* Handle one ident to proc results */
	if (Entity::containsProc(oneIdent)) { /* e.g. checks if proc named "first" exists, if not, return empty results */
		ProcIndex oneIdentRef = Entity::getProcIdx(oneIdent);
		for (ProcIndex proc : procs) {
			if (lhsRefType == PqlReferenceType::IDENT && pqlRsType == PqlRelationshipType::CALLS) {
				if (Calls::containsSuccessor(oneIdentRef, proc)) {
					results.emplace_back(proc); /* e.g {"first"} if "first" calls some q */
				}
			} else if (rhsRefType == PqlReferenceType::IDENT && pqlRsType == PqlRelationshipType::CALLS) {
				if (Calls::containsSuccessor(proc, oneIdentRef)) {
					results.emplace_back(proc); /* e.g {"second"} if some p calls "second" */
				}
			} else if (lhsRefType == PqlReferenceType::IDENT && pqlRsType == PqlRelationshipType::CALLS_T) {
				if (CallsT::containsSuccessor(oneIdentRef, proc)) {
					results.emplace_back(proc); /* e.g {"first"} if "first" calls some q */
				}
			} else if (rhsRefType == PqlReferenceType::IDENT && pqlRsType == PqlRelationshipType::CALLS_T) {
				if (CallsT::containsSuccessor(proc, oneIdentRef)) {
					results.emplace_back(proc); /* e.g {"second"} if some p calls "second" */
				}
			} else {
			}
		}
	}
	/* Handle final output, wildcards => boolean, synonyms => table */
	if (lhsRefType == PqlReferenceType::WILDCARD || rhsRefType == PqlReferenceType::WILDCARD) {
		bool evTable = !results.empty();
		return EvaluatedTable(evTable);
	} else {
		if (lhsRefType == PqlReferenceType::IDENT) {
			otherSynonym = rhsRef.second;
		} else {
			otherSynonym = lhsRef.second;
		}
	}
	std::unordered_map<std::string, EntityType> PQLentities;
	PQLentities.insert(std::pair(otherSynonym, EntityType::PROCEDURE));

	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[otherSynonym] = results;

	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable RelationshipInstruction::helperHandleTwoStmtsMaybeWildcard(PqlRelationshipType pqlRsType) {
	std::tuple<std::vector<int>, std::vector<int>> results;
	/* e.g. {1, 2}, {2, 3}, {3, 6} */
	std::unordered_map<std::string, EntityType> PQLentities;
	std::unordered_map<std::string, std::vector<int>> PQLmap;

	switch (pqlRsType) {
	case PqlRelationshipType::PARENT:
		results = Parent::getAllPredecessorSuccessorInfo();
		break;
	case PqlRelationshipType::NEXT:
		results = Next::getAllPredecessorSuccessorInfo();
		if (lhsRef.second == rhsRef.second) { /* Special case: Next(s1, s1) has a legitimate result */
			PQLentities.insert(std::pair(lhsRef.second, EntityType::STMT)); /*just do LHS*/
			std::vector<int> lhsResults = std::get<0>(results);
			std::vector<int> rhsResults = std::get<1>(results);
			std::vector<int> finalResults;
			for (size_t i = 0; i < lhsResults.size(); i++) {
				if (lhsResults[i] == rhsResults[i]) {
					finalResults.emplace_back(lhsResults[i]);
				}
			}
			PQLmap[lhsRef.second] = finalResults;
			return EvaluatedTable(PQLentities, PQLmap);
		}
		break;
	case PqlRelationshipType::NEXT_T:
		// TODO: NextT
		/*results = NextT::getAllPredecessorSuccessorInfo(); */
		break;
	}

	if (lhsRef.second == rhsRef.second) { /* Special case: Parent(s1, s1), recursive call, technically shouldn't be allowed */
		PQLentities.insert(std::pair(lhsRef.second, EntityType::STMT));
		/* No values populated to PQLmap for this case */
		return EvaluatedTable(PQLentities, PQLmap);
	}

	if (lhsRef.first == PqlReferenceType::SYNONYM) {
		PQLentities.insert(std::pair(lhsRef.second, EntityType::STMT));
		PQLmap[lhsRef.second] = std::get<0>(results); /* if RHS is wildcard, LHS may have duplicate values */
	}
	if (rhsRef.first == PqlReferenceType::SYNONYM) {
		PQLentities.insert(std::pair(rhsRef.second, EntityType::STMT));
		PQLmap[rhsRef.second] = std::get<1>(results); /* if LHS is wildcard, RHS may have duplicate values */
	}
	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable RelationshipInstruction::helperHandleTwoProcMaybeWildcard(PqlRelationshipType pqlRsType) {
	/* Assumption: Different synonym names(i.e. Calls(p, q), not Calls(p, p)) */
	std::tuple<std::vector<int>, std::vector<int>> results;
	if (pqlRsType == PqlRelationshipType::CALLS) {
		results = Calls::getAllPredecessorSuccessorInfo();
	} else {
		results = CallsT::getAllPredecessorSuccessorInfo();
	}
	/* e.g. {1, 2}, {2, 3}, {3, 6} */
	std::unordered_map<std::string, EntityType> PQLentities;
	std::unordered_map<std::string, std::vector<int>> PQLmap;

	if (lhsRef.second == rhsRef.second) { /* Special case: Calls(p, p), recursive call, technically shouldn't be allowed */
		PQLentities.insert(std::pair(lhsRef.second, EntityType::PROCEDURE));
		PQLentities.insert(std::pair(rhsRef.second, EntityType::PROCEDURE));
		/* No values populated to PQLmap for this case */
		return EvaluatedTable(PQLentities, PQLmap);
	}

	if (lhsRef.first == PqlReferenceType::SYNONYM) {
		PQLentities.insert(std::pair(lhsRef.second, EntityType::PROCEDURE));
		PQLmap[lhsRef.second] = std::get<0>(results); /* if RHS is wildcard, LHS may have duplicate values */
	}
	if (rhsRef.first == PqlReferenceType::SYNONYM) {
		PQLentities.insert(std::pair(rhsRef.second, EntityType::PROCEDURE));
		PQLmap[rhsRef.second] = std::get<1>(results); /* if LHS is wildcard, RHS may have duplicate values */
	}
	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable RelationshipInstruction::helperHandleTwoWildcards(PqlRelationshipType pqlRsType) {
	bool isEmptyTable = true;
	if (lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD) {
		switch (pqlRsType) {
		case PqlRelationshipType::CALLS:
			isEmptyTable = std::get<0>(Calls::getAllPredecessorSuccessorInfo()).empty();
			break;
		case PqlRelationshipType::CALLS_T:
			isEmptyTable = std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).empty();
			break;
		case PqlRelationshipType::NEXT:
			isEmptyTable = std::get<0>(Next::getAllPredecessorSuccessorInfo()).empty();
			break;
		}
	}
	// No Parent rs exists => isEmptyTable == true => EvTable.evResult == false (innerJoinMerge() can drop table)
	// Parent rs exists => isEmptyTable == false => EvTable.evResult == true (innerJoinMerge() can merge dummy table, preserving all rows)
	return EvaluatedTable(!isEmptyTable);
}

RelationshipInstruction::RelationshipInstruction() {}

RelationshipInstruction::RelationshipInstruction(PqlRelationshipType pqlRSType, PqlReference lhs, PqlReference rhs) :
	pqlRelationshipType(pqlRSType), lhsRef(lhs), rhsRef(rhs) {}

EvaluatedTable RelationshipInstruction::execute() {
	EvaluatedTable evTable;
	switch (pqlRelationshipType) {
	case PqlRelationshipType::MODIFIES_S:
		//evTable = handleModifiesS();
		break;
	case PqlRelationshipType::MODIFIES_P:
		//evTable = handleModifiesP();
		break;
	case PqlRelationshipType::USES_S:
		//evTable = handleUsesS();
		break;
	case PqlRelationshipType::USES_P:
		//evTable = handleUsesP();
		break;
	case PqlRelationshipType::FOLLOWS:
		//evTable = handleFollows();
		break;
	case PqlRelationshipType::FOLLOWS_T:
		//evTable = handleFollowsT();
		break;
	case PqlRelationshipType::PARENT:
		//evTable = handleParent(pqlRelationshipType);
		break;
	case PqlRelationshipType::PARENT_T:
		//evTable = handleParentT();
		break;
	case PqlRelationshipType::CALLS:
		//evTable = handleCalls(pqlRelationshipType);
		break;
	case PqlRelationshipType::CALLS_T:
		//evTable = handleCalls(pqlRelationshipType);
		break;
	case PqlRelationshipType::NEXT:
		evTable = handleNext(pqlRelationshipType);
		break;
	}
	return evTable;
}
