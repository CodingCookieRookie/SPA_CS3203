#include "ParentStarInstruction.h"

ParentStarInstruction::ParentStarInstruction(PqlReference lhsRef, PqlReference rhsRef) : lhsRef(lhsRef), rhsRef(rhsRef) {}

EvaluatedTable ParentStarInstruction::execute() {
	// e.g Parent*(6, 7)
	if (lhsRef.first == PqlReferenceType::INTEGER && rhsRef.first == PqlReferenceType::INTEGER) {
		return helperHandleTwoIntegers();
	}
	// e.g Parent*(6, s2), Parent*(6, _)
	else if (lhsRef.first == PqlReferenceType::INTEGER) {
		if (rhsRef.first == PqlReferenceType::SYNONYM) {
			return helperHandleOneInt(PqlReferenceType::INTEGER, PqlReferenceType::SYNONYM);
		} else {
			return helperHandleOneInt(PqlReferenceType::INTEGER, PqlReferenceType::WILDCARD);
		}
	}
	// e.g. Parent*(s1, 7), Parent*(_ 7)
	else if (rhsRef.first == PqlReferenceType::INTEGER) {
		if (lhsRef.first == PqlReferenceType::SYNONYM) {
			return helperHandleOneInt(PqlReferenceType::SYNONYM, PqlReferenceType::INTEGER);
		} else {
			return helperHandleOneInt(PqlReferenceType::WILDCARD, PqlReferenceType::INTEGER);
		}
	}
	// Parent*(s1, s2), Parent*(s1, _), Parent*(_, s2)
	else if (!(lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD)) {
		return helperHandleTwoStmtsMaybeWildcard();
	}
	// Parent*(_, _)
	else {
		if (lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD) {
			return helperHandleTwoWildcards();
		}
	}
}

EvaluatedTable ParentStarInstruction::helperHandleTwoIntegers() {
	StmtIndex lhsStmtIndex, rhsStmtIndex;
	bool evResult = false;
	int lhsRefValue = stoi(lhsRef.second);
	int rhsRefValue = stoi(rhsRef.second);
	if (Entity::containsStmt(lhsRefValue) && Entity::containsStmt(rhsRefValue)) {
		lhsStmtIndex = StmtIndex(lhsRefValue);
		rhsStmtIndex = StmtIndex(rhsRefValue);
		evResult = ParentT::containsSuccessor(lhsStmtIndex, rhsStmtIndex);
	}
	return EvaluatedTable(evResult);
}

EvaluatedTable ParentStarInstruction::helperHandleOneInt(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType) {
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
	if (Entity::containsStmt(oneInt)) { /* e.g. checks if STMT 6 exists, if not, return empty results */
		StmtIndex oneIntIndex = StmtIndex(oneInt);
		for (StmtIndex STMT : stmts) {
			if (lhsRefType == PqlReferenceType::INTEGER) {
				if (ParentT::containsSuccessor(oneIntIndex, STMT)) {
					results.emplace_back(STMT); /* e.g {7} if 6 is a Parent* of some s2 (e.g. 7) */
				}
			} else if (rhsRefType == PqlReferenceType::INTEGER) {
				if (ParentT::containsSuccessor(STMT, oneIntIndex)) {
					results.emplace_back(STMT); /* e.g {6} if some s1 (e.g. 6) is a Parent* of 7 */
				}
			} else {}
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

EvaluatedTable ParentStarInstruction::helperHandleTwoStmtsMaybeWildcard() {
	std::tuple<std::vector<int>, std::vector<int>> results;
	/* e.g. {1, 2}, {2, 3}, {3, 6} */
	std::unordered_map<std::string, EntityType> PQLentities;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	results = ParentT::getAllPredecessorSuccessorInfo();
	if (lhsRef.second == rhsRef.second) { /* Special case: Parent*(s1, s1), recursive call, technically shouldn't be allowed */
		PQLentities.insert(std::pair(lhsRef.second, EntityType::STMT));
		/* No values populated to PQLmap for this case */
		return EvaluatedTable(PQLentities, PQLmap);
	}
	if (lhsRef.first == PqlReferenceType::SYNONYM) {
		PQLentities.insert(std::pair(lhsRef.second, EntityType::STMT));
		PQLmap[lhsRef.second] = std::get<0>(results); /* if RHS is WILDCARD, LHS may have duplicate values */
	}
	if (rhsRef.first == PqlReferenceType::SYNONYM) {
		PQLentities.insert(std::pair(rhsRef.second, EntityType::STMT));
		PQLmap[rhsRef.second] = std::get<1>(results); /* if LHS is WILDCARD, RHS may have duplicate values */
	}
	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable ParentStarInstruction::helperHandleTwoWildcards() {
	bool isEmptyTable = true;
	isEmptyTable = std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).empty();
	// No Parent* rs exists => isEmptyTable == true => EvTable.evResult == false (innerJoinMerge() can drop table)
	// Parent* rs exists => isEmptyTable == false => EvTable.evResult == true (innerJoinMerge() can merge dummy table, preserving all rows)
	return EvaluatedTable(!isEmptyTable);
}