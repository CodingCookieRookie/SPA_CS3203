#include "RelationshipInstruction.h"

EvaluatedTable RelationshipInstruction::handleModifiesS() {
	/* Modifies (1, v)	or Modifies(1, "x")  => true or Modifies (1, _ ) (under statement) */
	std::unordered_map<std::string, PqlEntityType> PQLentities;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
	PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Variable));
	std::tuple<std::vector<int>, std::vector<int>> allStmtVarInfos = ModifiesS::getAllSynonymVarInfo();
	std::vector<int> allStmts;
	std::vector<int> varIndices;
	if (lhsRef.first == PqlReferenceType::synonym) {
		if (rhsRef.first == PqlReferenceType::ident) {
			if (Entity::containsVar(rhsRef.second)) {
				VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
				allStmts = ModifiesS::getFromVariable(varIndex);
			}
		} else {
			allStmts = std::get<0>(allStmtVarInfos);
		}
		PQLmap[lhsRef.second] = allStmts;
		if (rhsRef.first != PqlReferenceType::wildcard && rhsRef.first != PqlReferenceType::ident) {
			varIndices = std::get<1>(allStmtVarInfos);
			PQLmap[rhsRef.second] = varIndices;
		}
	} else if (lhsRef.first == PqlReferenceType::integer) {
		int lhsRefValue = stoi(lhsRef.second);
		if (Entity::containsStmt(lhsRefValue)) {
			StmtIndex stmtIndex = { lhsRefValue };
			if (rhsRef.first == PqlReferenceType::synonym) {
				varIndices = ModifiesS::getVariables(stmtIndex);
				PQLmap[rhsRef.second] = varIndices;
			} else if (rhsRef.first == PqlReferenceType::ident) {
				if (!Entity::containsVar(rhsRef.second)) {
					return EvaluatedTable(false);
				}
				VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
				return EvaluatedTable(ModifiesS::contains(stmtIndex, varIndex));
			} else {
				return EvaluatedTable(ModifiesS::getVariables(stmtIndex).size() > 0);
			}
		} else {
			return EvaluatedTable(false);
		}
	} else {
		throw EvaluatorException(EvaluatorException::MODIFIES_S_ERROR);
	}
	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable RelationshipInstruction::handleModifiesP() {
	/* Modifies(p / p1, v) or Modifies(p / p1, _)	Modifies(p / p1, "x") */
	/* Modifies("p", v) or Modifies("p" , _)	Modifies("p", "x") */
	std::unordered_map<std::string, PqlEntityType> PQLentities;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Procedure));
	PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Variable));
	std::tuple<std::vector<int>, std::vector<int>> allProcVarInfos = ModifiesP::getAllSynonymVarInfo();
	std::vector<int> allStmts;
	std::vector<int> varIndices;
	if (lhsRef.first == PqlReferenceType::synonym) {
		if (rhsRef.first == PqlReferenceType::ident) {
			if (Entity::containsVar(rhsRef.second)) {
				VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
				allStmts = ModifiesP::getFromVariable(varIndex);
			}
		} else {
			allStmts = std::get<0>(allProcVarInfos);
		}
		PQLmap[lhsRef.second] = allStmts;
		if (rhsRef.first != PqlReferenceType::wildcard && rhsRef.first != PqlReferenceType::ident) {
			varIndices = std::get<1>(allProcVarInfos);
			PQLmap[rhsRef.second] = varIndices;
		}
	} else if (lhsRef.first == PqlReferenceType::ident) {
		if (rhsRef.first == PqlReferenceType::ident) {
			if (Entity::containsProc(lhsRef.second)) {
				ProcIndex procIndex = Entity::getProcIdx(lhsRef.second);
				if (Entity::containsVar(rhsRef.second)) {
					VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
					if (ModifiesP::contains(procIndex, varIndex)) {
						return EvaluatedTable(true);
					} else {
						return EvaluatedTable(false);
					}
				} else {
					return EvaluatedTable(false);
				}
			}
		} else {
			if (Entity::containsProc(lhsRef.second)) {
				ProcIndex procIndex = Entity::getProcIdx(lhsRef.second);
				if (rhsRef.first == PqlReferenceType::synonym) {
					varIndices = ModifiesP::getVariables(procIndex);
					PQLmap[rhsRef.second] = varIndices;
				} else if (rhsRef.first == PqlReferenceType::wildcard) {
					if (ModifiesP::getVariables(procIndex).size() > 0) {
						return EvaluatedTable(true);
					} else {
						return EvaluatedTable(false);
					}
				} else {
					throw EvaluatorException(EvaluatorException::MODIFIES_P_ERROR);
				}
			}
		}
	} else {
		throw EvaluatorException(EvaluatorException::MODIFIES_P_ERROR);
	}
	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable RelationshipInstruction::handleUsesS() {
	/* Uses (a/r/s/a1, v) or Uses(a/r/s/a1, "x") or Uses (a/r/s/a1, _ )
	 Uses (1, v)	=> true or Uses (1, _ ) (under statement) */
	std::unordered_map<std::string, PqlEntityType> PQLentities;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
	PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Variable));
	std::tuple<std::vector<int>, std::vector<int>>  allStmtVarInfos = UsesS::getAllSynonymVarInfo();
	std::vector<int> allStmts;
	std::vector<int> varIndices;
	if (lhsRef.first == PqlReferenceType::synonym) {
		if (rhsRef.first == PqlReferenceType::ident) {
			if (Entity::containsVar(rhsRef.second)) {
				VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
				allStmts = UsesS::getFromVariable(varIndex);
			}
		} else {
			allStmts = std::get<0>(allStmtVarInfos);
		}
		PQLmap[lhsRef.second] = allStmts;
		if (rhsRef.first != PqlReferenceType::wildcard && rhsRef.first != PqlReferenceType::ident) {
			varIndices = std::get<1>(allStmtVarInfos);
			PQLmap[rhsRef.second] = varIndices;
		}
	} else if (lhsRef.first == PqlReferenceType::integer) {
		int lhsRefValue = stoi(lhsRef.second);
		if (Entity::containsStmt(lhsRefValue)) {
			StmtIndex stmtIndex = { lhsRefValue };
			if (rhsRef.first == PqlReferenceType::synonym) {
				varIndices = UsesS::getVariables(stmtIndex);
				PQLmap[rhsRef.second] = varIndices;
			} else if (rhsRef.first == PqlReferenceType::ident) {
				if (!Entity::containsVar(rhsRef.second)) {
					return EvaluatedTable(false);
				}
				VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
				return EvaluatedTable(UsesS::contains(stmtIndex, varIndex));
			} else {
				return EvaluatedTable(UsesS::getVariables(stmtIndex).size() > 0);
			}
		} else {
			return EvaluatedTable(false);
		}
	} else {
		throw EvaluatorException(EvaluatorException::USES_S_ERROR);
	}
	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable RelationshipInstruction::handleUsesP() {
	/* Uses (p/p1, v) or Uses (p/p1, "x") or Uses (p/p1, _ ) */
	std::unordered_map<std::string, PqlEntityType> PQLentities;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Procedure));
	PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Variable));
	std::tuple<std::vector<int>, std::vector<int>>  allProcVarInfos = UsesP::getAllSynonymVarInfo();
	std::vector<int> allStmts;
	std::vector<int> varIndices;

	if (lhsRef.first == PqlReferenceType::synonym) {
		if (rhsRef.first == PqlReferenceType::ident) {
			if (Entity::containsVar(rhsRef.second)) {
				VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
				allStmts = UsesP::getFromVariable(varIndex);
			}
		} else {
			allStmts = std::get<0>(allProcVarInfos);
		}

		PQLmap[lhsRef.second] = allStmts;

		if (rhsRef.first != PqlReferenceType::wildcard && rhsRef.first != PqlReferenceType::ident) {
			varIndices = std::get<1>(allProcVarInfos);
			PQLmap[rhsRef.second] = varIndices;
		}
	} else if (lhsRef.first == PqlReferenceType::ident) {
		if (rhsRef.first == PqlReferenceType::ident) {
			if (Entity::containsProc(lhsRef.second)) {
				ProcIndex procIndex = Entity::getProcIdx(lhsRef.second);
				if (Entity::containsVar(rhsRef.second)) {
					VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
					if (UsesP::contains(procIndex, varIndex)) {
						return EvaluatedTable(true);
					} else {
						return EvaluatedTable(false);
					}
				} else {
					return EvaluatedTable(false);
				}
			}
		} else {
			if (Entity::containsProc(lhsRef.second)) {
				ProcIndex procIndex = Entity::getProcIdx(lhsRef.second);
				if (rhsRef.first == PqlReferenceType::synonym) {
					varIndices = UsesP::getVariables(procIndex);
					PQLmap[rhsRef.second] = varIndices;
				} else if (rhsRef.first == PqlReferenceType::wildcard) {
					if (UsesP::getVariables(procIndex).size() > 0) {
						return EvaluatedTable(true);
					} else {
						return EvaluatedTable(false);
					}
				} else {
					throw EvaluatorException(EvaluatorException::USES_P_ERROR);
				}
			}
		}
	} else {
		throw EvaluatorException(EvaluatorException::USES_P_ERROR);
	}
	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable RelationshipInstruction::handleFollows() {
	EvaluatedTable evTable;
	std::vector<StmtIndex> stmts = Entity::getAllStmts();

	// e.g Follows(6, 7)
	if (lhsRef.first == PqlReferenceType::integer && rhsRef.first == PqlReferenceType::integer) {
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
	else if (lhsRef.first == PqlReferenceType::integer) {
		std::vector<int> results;
		int lhsRefValue = stoi(lhsRef.second); // might throw error if string value can't be converted to int
		if (Entity::containsStmt(lhsRefValue)) { // checks if stmt 6 exists, if not, return empty results
			StmtIndex lhsStmt = StmtIndex(lhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (Follows::containsSuccessor(lhsStmt, stmt)) {
					results.emplace_back(stmt.getIndex()); // e.g {7} because 6 is followed by 7
				}
			}
		}
		if (rhsRef.first == PqlReferenceType::wildcard) {
			bool evTable = !results.empty();
			return EvaluatedTable(evTable);
		}
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[rhsRef.second] = results;

		return EvaluatedTable(PQLentities, PQLmap);
	}
	// e.g. Follows(s1, 7), Follows(_, 7)
	else if (rhsRef.first == PqlReferenceType::integer) {
		std::vector<int> results;
		int rhsRefValue = stoi(rhsRef.second); //might throw error if string value can't be converted to int
		if (Entity::containsStmt(rhsRefValue)) { // checks if stmt 7 exists, if not, return empty results
			StmtIndex rhsStmt = StmtIndex(rhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (Follows::containsSuccessor(stmt, rhsStmt)) {
					results.emplace_back(stmt.getIndex()); //e.g {3} because 3 is followed by 6
				}
			}
		}
		if (lhsRef.first == PqlReferenceType::wildcard) {
			bool evTable = !results.empty();
			return EvaluatedTable(evTable);
		}
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[lhsRef.second] = results;

		return EvaluatedTable(PQLentities, PQLmap);
	}
	// Follows(s1, s2), Follows(s1, _), Follows(_, s2)
	else if (!(lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard)) {
		std::tuple<std::vector<int>, std::vector<int>> results = Follows::getAllPredecessorSuccessorInfo();
		//e.g. {1, 2}, {2, 3}, {3, 6}
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		std::unordered_map<std::string, std::vector<int>> PQLmap;

		if (lhsRef.second == rhsRef.second) { /* Special case: Follows(s1, s1)) */
			PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
			PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));
			/* No values populated to PQLmap for this case */
			return EvaluatedTable(PQLentities, PQLmap);
		}

		if (lhsRef.first == PqlReferenceType::synonym) {
			PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
			PQLmap[lhsRef.second] = std::get<0>(results); // if RHS is wildcard, LHS may have duplicate values
		}
		if (rhsRef.first == PqlReferenceType::synonym) {
			PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));
			PQLmap[rhsRef.second] = std::get<1>(results); // if LHS is wildcard, RHS may have duplicate values
		}
		return EvaluatedTable(PQLentities, PQLmap);
	}
	// Follows(_, _)
	else {
		bool isEmptyTable = true;
		if (lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard) { // Follows(_, _)
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
	if (lhsRef.first == PqlReferenceType::integer && rhsRef.first == PqlReferenceType::integer) {
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
	else if (lhsRef.first == PqlReferenceType::integer) {
		std::vector<int> results;
		int lhsRefValue = stoi(lhsRef.second); // might throw error if string value can't be converted to int
		if (Entity::containsStmt(lhsRefValue)) { // checks if stmt 6 exists, if not, return empty results
			StmtIndex lhsStmt = StmtIndex(lhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (FollowsT::containsSuccessor(lhsStmt, stmt)) {
					results.emplace_back(stmt.getIndex()); // e.g {7} because 6 is followed by 7
				}
			}
		}
		if (rhsRef.first == PqlReferenceType::wildcard) {
			bool evTable = !results.empty();
			return EvaluatedTable(evTable);
		}
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[rhsRef.second] = results;

		return EvaluatedTable(PQLentities, PQLmap);
	}
	// e.g. Follows*(s1, 7), Follows*(_, 7)
	else if (rhsRef.first == PqlReferenceType::integer) {
		std::vector<int> results;
		int rhsRefValue = stoi(rhsRef.second); //might throw error if string value can't be converted to int
		if (Entity::containsStmt(rhsRefValue)) { // checks if stmt 7 exists, if not, return empty results
			StmtIndex rhsStmt = StmtIndex(rhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (FollowsT::containsSuccessor(stmt, rhsStmt)) {
					results.emplace_back(stmt.getIndex()); //e.g {3} because 3 is followed by 6
				}
			}
		}
		if (lhsRef.first == PqlReferenceType::wildcard) {
			bool evTable = !results.empty();
			return EvaluatedTable(evTable);
		}
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[lhsRef.second] = results;

		return EvaluatedTable(PQLentities, PQLmap);
	}
	// Follows*(s1, s2), Follows*(s1, _), Follows*(_, s2)
	else if (!(lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard)) {
		//Assumption: Different synonym names (i.e. Follows(s1, s2), not Follows(s1, s1))
		std::tuple<std::vector<int>, std::vector<int>> results = FollowsT::getAllPredecessorSuccessorInfo();
		//e.g. {1, 2}, {2, 3}, {3, 6}
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		std::unordered_map<std::string, std::vector<int>> PQLmap;

		if (lhsRef.second == rhsRef.second) { /* Special case: Follows*(s1, s1)) */
			PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
			PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));
			/* No values populated to PQLmap for this case */
			return EvaluatedTable(PQLentities, PQLmap);
		}

		if (lhsRef.first == PqlReferenceType::synonym) {
			PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
			PQLmap[lhsRef.second] = std::get<0>(results); // if RHS is wildcard, LHS may have duplicate values
		}
		if (rhsRef.first == PqlReferenceType::synonym) {
			PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));
			PQLmap[rhsRef.second] = std::get<1>(results); // if LHS is wildcard, RHS may have duplicate values
		}
		return EvaluatedTable(PQLentities, PQLmap);
	}
	// Follows*(_, _)
	else {
		bool isEmptyTable = true;
		if (lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard) { // Follows(_, _)
			isEmptyTable = std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).empty();
		}
		// No Follows rs exists => isEmptyTable == true => EvTable.evResult == false (innerJoinMerge() can drop table)
		// Follows rs exists => isEmptyTable == false => EvTable.evResult == true (innerJoinMerge() can merge dummy table, preserving all rows)
		return EvaluatedTable(!isEmptyTable);
	}
}

EvaluatedTable RelationshipInstruction::handleParent() {
	EvaluatedTable evTable;
	std::vector<StmtIndex> stmts = Entity::getAllStmts();

	// e.g Parent(6, 7)
	if (lhsRef.first == PqlReferenceType::integer && rhsRef.first == PqlReferenceType::integer) {
		StmtIndex lhsStmtIndex, rhsStmtIndex;
		bool evResult = false;
		int lhsRefValue = stoi(lhsRef.second);
		int rhsRefValue = stoi(rhsRef.second);
		if (Entity::containsStmt(lhsRefValue) && Entity::containsStmt(rhsRefValue)) {
			lhsStmtIndex = StmtIndex(lhsRefValue);
			rhsStmtIndex = StmtIndex(rhsRefValue);
			evResult = Parent::containsSuccessor(lhsStmtIndex, rhsStmtIndex);
		}
		return EvaluatedTable(evResult); //e.g evResult == true, if 6 parents 7
	}
	// e.g Parent(6, s2), Parent(6, _)
	else if (lhsRef.first == PqlReferenceType::integer) {
		std::vector<int> results;
		int lhsRefValue = stoi(lhsRef.second); // might throw error if string value can't be converted to int
		if (Entity::containsStmt(lhsRefValue)) { // checks if stmt 6 exists, if not, return empty results
			StmtIndex lhsStmt = StmtIndex(lhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (Parent::containsSuccessor(lhsStmt, stmt)) {
					results.emplace_back(stmt.getIndex()); // e.g {7} because 6 parents 7
				}
			}
		}
		if (rhsRef.first == PqlReferenceType::wildcard) {
			bool evTable = !results.empty();
			return EvaluatedTable(evTable);
		}
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[rhsRef.second] = results;

		return EvaluatedTable(PQLentities, PQLmap);
	}
	// e.g. Parent(s1, 7), Parent(_ 7)
	else if (rhsRef.first == PqlReferenceType::integer) {
		std::vector<int> results;
		int rhsRefValue = stoi(rhsRef.second); //might throw error if string value can't be converted to int
		if (Entity::containsStmt(rhsRefValue)) { // checks if stmt 7 exists, if not, return empty results
			StmtIndex rhsStmt = StmtIndex(rhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (Parent::containsSuccessor(stmt, rhsStmt)) {
					results.emplace_back(stmt.getIndex()); //e.g {3} because 3 is a parent of 7
				}
			}
		}
		if (lhsRef.first == PqlReferenceType::wildcard) {
			bool evTable = !results.empty();
			return EvaluatedTable(evTable);
		}
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[lhsRef.second] = results;

		return EvaluatedTable(PQLentities, PQLmap);
	}
	// Parent(s1, s2), Parent(s1, _), Parent(_, s2)
	else if (!(lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard)) {
		//Assumption: Different synonym names (i.e. Parent(s1, s2), not Parent(s1, s1))
		std::tuple<std::vector<int>, std::vector<int>> results = Parent::getAllPredecessorSuccessorInfo();
		//e.g. {1, 2}, {2, 3}, {3, 6}
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		std::unordered_map<std::string, std::vector<int>> PQLmap;

		if (lhsRef.second == rhsRef.second) { /* Special case: Parent(s1, s1)) */
			PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
			PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));
			/* No values populated to PQLmap for this case */
			return EvaluatedTable(PQLentities, PQLmap);
		}

		if (lhsRef.first == PqlReferenceType::synonym) {
			PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
			PQLmap[lhsRef.second] = std::get<0>(results); // if RHS is wildcard, LHS may have duplicate values
		}
		if (rhsRef.first == PqlReferenceType::synonym) {
			PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));
			PQLmap[rhsRef.second] = std::get<1>(results); // if LHS is wildcard, RHS may have duplicate values
		}
		return EvaluatedTable(PQLentities, PQLmap);
	}
	// Parent(_, _)
	else {
		bool isEmptyTable = true;
		if (lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard) {
			isEmptyTable = std::get<0>(Parent::getAllPredecessorSuccessorInfo()).empty();
		}
		// No Parent rs exists => isEmptyTable == true => EvTable.evResult == false (innerJoinMerge() can drop table)
		// Parent rs exists => isEmptyTable == false => EvTable.evResult == true (innerJoinMerge() can merge dummy table, preserving all rows)
		return EvaluatedTable(!isEmptyTable);
	}
}

EvaluatedTable RelationshipInstruction::handleParentT() {
	EvaluatedTable evTable;
	std::vector<StmtIndex> stmts = Entity::getAllStmts();

	// e.g Parent*(6, 7)
	if (lhsRef.first == PqlReferenceType::integer && rhsRef.first == PqlReferenceType::integer) {
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
	else if (lhsRef.first == PqlReferenceType::integer) {
		std::vector<int> results;
		int lhsRefValue = stoi(lhsRef.second); // might throw error if string value can't be converted to int
		if (Entity::containsStmt(lhsRefValue)) { // checks if stmt 6 exists, if not, return empty results
			StmtIndex lhsStmt = StmtIndex(lhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (ParentT::containsSuccessor(lhsStmt, stmt)) {
					results.emplace_back(stmt.getIndex()); // e.g {7} because 6 is a parent* of 7
				}
			}
		}
		if (rhsRef.first == PqlReferenceType::wildcard) {
			bool evTable = !results.empty();
			return EvaluatedTable(evTable);
		}
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[rhsRef.second] = results;

		return EvaluatedTable(PQLentities, PQLmap);
	}
	// e.g. Parent*(s1, 7), Parent*(_ 7)
	else if (rhsRef.first == PqlReferenceType::integer) {
		std::vector<int> results;
		int rhsRefValue = stoi(rhsRef.second); //might throw error if string value can't be converted to int
		if (Entity::containsStmt(rhsRefValue)) { // checks if stmt 7 exists, if not, return empty results
			StmtIndex rhsStmt = StmtIndex(rhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (ParentT::containsSuccessor(stmt, rhsStmt)) {
					results.emplace_back(stmt.getIndex()); //e.g {3} because 3 is a parent* of 7
				}
			}
		}
		if (lhsRef.first == PqlReferenceType::wildcard) {
			bool evTable = !results.empty();
			return EvaluatedTable(evTable);
		}
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[lhsRef.second] = results;

		return EvaluatedTable(PQLentities, PQLmap);
	}
	// Parent*(s1, s2), Parent*(s1, _), Parent*(_, s2)
	else if (!(lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard)) {
		//Assumption: Different synonym names (i.e. Parent*(s1, s2), not Parent*(s1, s1))
		std::tuple<std::vector<int>, std::vector<int>> results = ParentT::getAllPredecessorSuccessorInfo();
		//e.g. {1, 2}, {2, 3}, {3, 6}
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		std::unordered_map<std::string, std::vector<int>> PQLmap;

		if (lhsRef.second == rhsRef.second) { /* Special case: Parent*(s1, s1)) */
			PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
			PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));
			/* No values populated to PQLmap for this case */
			return EvaluatedTable(PQLentities, PQLmap);
		}

		if (lhsRef.first == PqlReferenceType::synonym) {
			PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
			PQLmap[lhsRef.second] = std::get<0>(results); // if RHS is wildcard, LHS may have duplicate values
		}
		if (rhsRef.first == PqlReferenceType::synonym) {
			PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));
			PQLmap[rhsRef.second] = std::get<1>(results); // if LHS is wildcard, RHS may have duplicate values
		}
		return EvaluatedTable(PQLentities, PQLmap);
	}
	// Parent*(_, _)
	else {
		bool isEmptyTable = true;
		if (lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard) {
			isEmptyTable = std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).empty();
		}
		// No Parent rs exists => isEmptyTable == true => EvTable.evResult == false (innerJoinMerge() can drop table)
		// Parent rs exists => isEmptyTable == false => EvTable.evResult == true (innerJoinMerge() can merge dummy table, preserving all rows)
		return EvaluatedTable(!isEmptyTable);
	}
}

EvaluatedTable RelationshipInstruction::handleCalls(std::string PqlRsType) {
	EvaluatedTable evTable;
	std::vector<ProcIndex> procs = Entity::getAllProcs();

	/* e.g Calls/Calls*("first", "second") */
	if (lhsRef.first == PqlReferenceType::ident && rhsRef.first == PqlReferenceType::ident) {
		return helperHandleTwoIdents(PqlRsType);
	}
	/* e.g Calls/Calls*("first", q), Calls/Calls*("first", _) */
	else if (lhsRef.first == PqlReferenceType::ident) {
		if (rhsRef.first == PqlReferenceType::synonym) {
			return helperHandleOneIdent(PqlRsType, PqlReferenceType::ident, PqlReferenceType::synonym);
		} else {
			return helperHandleOneIdent(PqlRsType, PqlReferenceType::ident, PqlReferenceType::wildcard);
		}
	}
	/*  e.g.Calls/Calls*(p, "second"), Calls/Calls*(_, "second") */
	else if (rhsRef.first == PqlReferenceType::ident) {
		if (lhsRef.first == PqlReferenceType::synonym) {
			return helperHandleOneIdent(PqlRsType, PqlReferenceType::synonym, PqlReferenceType::ident);
		} else {
			return helperHandleOneIdent(PqlRsType, PqlReferenceType::wildcard, PqlReferenceType::ident);
		}
	}
	/* Calls/Calls*(p, q), Calls/Calls*(p, _), Calls/Calls*(_, q) */
	else if (!(lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard)) {
		return helperHandleTwoProcMaybeWildcard(PqlRsType);
	}
	// Calls/Calls*(_, _)
	else {
		return helperHandleTwoWildcards(PqlRsType);
	}
}

/* Helper Handle Methods */

EvaluatedTable RelationshipInstruction::helperHandleTwoIdents(std::string pqlRsType) {
	ProcIndex lhsProcIndex, rhsProcIndex;
	bool evResult = false;
	if (Entity::containsProc(lhsRef.second) && Entity::containsProc(rhsRef.second)) {
		lhsProcIndex = Entity::getProcIdx(lhsRef.second);
		rhsProcIndex = Entity::getProcIdx(rhsRef.second);
		if (pqlRsType == "Calls") {
			evResult = Calls::containsSuccessor(lhsProcIndex, rhsProcIndex);
		} else { /* pqlRsType == "CallsT" */
			evResult = CallsT::containsSuccessor(lhsProcIndex, rhsProcIndex);
		}
	}
	return EvaluatedTable(evResult); /* e.g evResult == true, if "first" calls "second" */
}

EvaluatedTable RelationshipInstruction::helperHandleOneIdent(
	std::string pqlRsType, PqlReferenceType lhsRefType, PqlReferenceType rhsRefType) {
	std::vector<ProcIndex> procs = Entity::getAllProcs();
	std::vector<int> results;
	std::string oneIdent;
	std::string otherSynonym;
	if (lhsRefType == PqlReferenceType::ident) {
		oneIdent = lhsRef.second;
	} else { /* rhsRefType == PqlReferenceType::ident */
		oneIdent = rhsRef.second;
	}
	/* Handle one ident to proc results */
	if (Entity::containsProc(oneIdent)) { /* e.g. checks if proc named "first" exists, if not, return empty results */
		ProcIndex oneIdentRef = Entity::getProcIdx(oneIdent);
		for (ProcIndex proc : procs) {
			if (lhsRefType == PqlReferenceType::ident && pqlRsType == "Calls") {
				if (Calls::containsSuccessor(oneIdentRef, proc)) {
					results.emplace_back(proc.getIndex()); /* e.g {"first"} if "first" calls some q */
				}
			} else if (rhsRefType == PqlReferenceType::ident && pqlRsType == "Calls") {
				if (Calls::containsSuccessor(proc, oneIdentRef)) {
					results.emplace_back(proc.getIndex()); /* e.g {"second"} if some p calls "second" */
				}
			} else if (lhsRefType == PqlReferenceType::ident && pqlRsType == "CallsT") {
				if (CallsT::containsSuccessor(oneIdentRef, proc)) {
					results.emplace_back(proc.getIndex()); /* e.g {"first"} if "first" calls some q */
				}
			} else if (rhsRefType == PqlReferenceType::ident && pqlRsType == "CallsT") {
				if (CallsT::containsSuccessor(proc, oneIdentRef)) {
					results.emplace_back(proc.getIndex()); /* e.g {"second"} if some p calls "second" */
				}
			} else {
			}
		}
	}
	/* Handle final output, wildcards => boolean, synonyms => table */
	if (lhsRefType == PqlReferenceType::wildcard || rhsRefType == PqlReferenceType::wildcard) {
		bool evTable = !results.empty();
		return EvaluatedTable(evTable);
	} else {
		if (lhsRefType == PqlReferenceType::ident) {
			otherSynonym = rhsRef.second;
		} else {
			otherSynonym = lhsRef.second;
		}

	}
	std::unordered_map<std::string, PqlEntityType> PQLentities;
	PQLentities.insert(std::pair(otherSynonym, PqlEntityType::Procedure));

	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[otherSynonym] = results;

	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable RelationshipInstruction::helperHandleTwoProcMaybeWildcard(std::string pqlRsType) {
	/* Assumption: Different synonym names(i.e. Calls(p, q), not Calls(p, p)) */
	std::tuple<std::vector<int>, std::vector<int>> results;
	if (pqlRsType == "Calls") {
		results = Calls::getAllPredecessorSuccessorInfo();
	} else {
		results = CallsT::getAllPredecessorSuccessorInfo();
	}
	/* e.g. {1, 2}, {2, 3}, {3, 6} */
	std::unordered_map<std::string, PqlEntityType> PQLentities;
	std::unordered_map<std::string, std::vector<int>> PQLmap;

	if (lhsRef.second == rhsRef.second) { /* Special case: Calls(p, p), recursive call, technically shouldn't be allowed */
		PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Procedure));
		PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Procedure));
		/* No values populated to PQLmap for this case */
		return EvaluatedTable(PQLentities, PQLmap);
	}

	if (lhsRef.first == PqlReferenceType::synonym) {
		PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Procedure));
		PQLmap[lhsRef.second] = std::get<0>(results); /* if RHS is wildcard, LHS may have duplicate values */
	}
	if (rhsRef.first == PqlReferenceType::synonym) {
		PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Procedure));
		PQLmap[rhsRef.second] = std::get<1>(results); /* if LHS is wildcard, RHS may have duplicate values */
	}
	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable RelationshipInstruction::helperHandleTwoWildcards(std::string pqlRsType) {
	bool isEmptyTable = true;
	if (lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard) {
		if (pqlRsType == "Calls") {
			isEmptyTable = std::get<0>(Calls::getAllPredecessorSuccessorInfo()).empty();
		} else {
			isEmptyTable = std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).empty();
		}
	}
	// No Parent rs exists => isEmptyTable == true => EvTable.evResult == false (innerJoinMerge() can drop table)
	// Parent rs exists => isEmptyTable == false => EvTable.evResult == true (innerJoinMerge() can merge dummy table, preserving all rows)
	return EvaluatedTable(!isEmptyTable);
}

RelationshipInstruction::RelationshipInstruction(PqlRelationshipType pqlRSType, PqlReference lhs, PqlReference rhs) :
	pqlRelationshipType(pqlRSType), lhsRef(lhs), rhsRef(rhs) {}

EvaluatedTable RelationshipInstruction::execute() {
	EvaluatedTable evTable;
	switch (pqlRelationshipType) {
	case PqlRelationshipType::ModifiesS:
		evTable = handleModifiesS();
		break;
	case PqlRelationshipType::ModifiesP:
		evTable = handleModifiesP();
		break;
	case PqlRelationshipType::UsesS:
		evTable = handleUsesS();
		break;
	case PqlRelationshipType::UsesP:
		evTable = handleUsesP();
		break;
	case PqlRelationshipType::Follows:
		evTable = handleFollows();
		break;
	case PqlRelationshipType::FollowsT:
		evTable = handleFollowsT();
		break;
	case PqlRelationshipType::Parent:
		evTable = handleParent();
		break;
	case PqlRelationshipType::ParentT:
		evTable = handleParentT();
		break;
	case PqlRelationshipType::Calls:
		evTable = handleCalls("Calls");
		break;
	case PqlRelationshipType::CallsT:
		evTable = handleCalls("CallsT");
		break;
	}
	return evTable;
}