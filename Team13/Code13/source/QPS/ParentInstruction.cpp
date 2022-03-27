#include "ParentInstruction.h"

ParentInstruction::ParentInstruction(PqlReference lhsRef, PqlReference rhsRef) : lhsRef(lhsRef), rhsRef(rhsRef) {}

EvaluatedTable ParentInstruction::execute() {
	// e.g Parent(6, 7)
	if (lhsRef.first == PqlReferenceType::Integer && rhsRef.first == PqlReferenceType::Integer) {
		return helperHandleTwoIntegers();
	}
	// e.g Parent(6, s2), Parent(6, _)
	else if (lhsRef.first == PqlReferenceType::Integer) {
		if (rhsRef.first == PqlReferenceType::Synonym) {
			return helperHandleOneInt(PqlReferenceType::Integer, PqlReferenceType::Synonym);
		} else {
			return helperHandleOneInt(PqlReferenceType::Integer, PqlReferenceType::Wildcard);
		}
	}
	// e.g. Parent(s1, 7), Parent(_ 7)
	else if (rhsRef.first == PqlReferenceType::Integer) {
		if (lhsRef.first == PqlReferenceType::Synonym) {
			return helperHandleOneInt(PqlReferenceType::Synonym, PqlReferenceType::Integer);
		} else {
			return helperHandleOneInt(PqlReferenceType::Wildcard, PqlReferenceType::Integer);
		}
	}
	// Parent(s1, s2), Parent(s1, _), Parent(_, s2)
	else if (!(lhsRef.first == PqlReferenceType::Wildcard && rhsRef.first == PqlReferenceType::Wildcard)) {
		return helperHandleTwoStmtsMaybeWildcard();
	}
	// Parent(_, _)
	else {
		if (lhsRef.first == PqlReferenceType::Wildcard && rhsRef.first == PqlReferenceType::Wildcard) {
			return helperHandleTwoWildcards();
		}
	}
}

EvaluatedTable ParentInstruction::helperHandleTwoIntegers() {
	StmtIndex lhsStmtIndex, rhsStmtIndex;
	bool evResult = false;
	int lhsRefValue = stoi(lhsRef.second);
	int rhsRefValue = stoi(rhsRef.second);
	if (Entity::containsStmt(lhsRefValue) && Entity::containsStmt(rhsRefValue)) {
		lhsStmtIndex = StmtIndex(lhsRefValue);
		rhsStmtIndex = StmtIndex(rhsRefValue);
		evResult = Parent::containsSuccessor(lhsStmtIndex, rhsStmtIndex);
	}
	return EvaluatedTable(evResult);
}

EvaluatedTable ParentInstruction::helperHandleOneInt(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType) {
	std::vector<StmtIndex> stmts = Entity::getAllStmts();
	std::vector<int> results;
	int oneInt;
	std::string otherSynonym;
	if (lhsRefType == PqlReferenceType::Integer) {
		oneInt = stoi(lhsRef.second);
	} else { /* rhsRefType == PqlReferenceType::ident */
		oneInt = stoi(rhsRef.second);
	}
	/* Handle one ident to proc results */
	if (Entity::containsStmt(oneInt)) { /* e.g. checks if stmt 6 exists, if not, return empty results */
		StmtIndex oneIntIndex = StmtIndex(oneInt);
		for (StmtIndex stmt : stmts) {
			if (lhsRefType == PqlReferenceType::Integer) {
				if (Parent::containsSuccessor(oneIntIndex, stmt)) {
					results.emplace_back(stmt); /* e.g {7} if 6 is a Parent of some s2 (e.g. 7) */
				}
			} else if (rhsRefType == PqlReferenceType::Integer) {
				if (Parent::containsSuccessor(stmt, oneIntIndex)) {
					results.emplace_back(stmt); /* e.g {6} if some s1 (e.g. 6) is a Parent of 7 */
				}
			} else {}
			break;
		}
		/* Handle final output, wildcards => boolean, synonyms => table */
		if (lhsRefType == PqlReferenceType::Wildcard || rhsRefType == PqlReferenceType::Wildcard) {
			bool evTable = !results.empty();
			return EvaluatedTable(evTable);
		} else {
			if (lhsRefType == PqlReferenceType::Integer) {
				otherSynonym = rhsRef.second;
			} else {
				otherSynonym = lhsRef.second;
			}
		}
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(otherSynonym, PqlEntityType::Stmt));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[otherSynonym] = results;

		return EvaluatedTable(PQLentities, PQLmap);
	}
}

EvaluatedTable ParentInstruction::helperHandleTwoStmtsMaybeWildcard() {
	std::tuple<std::vector<int>, std::vector<int>> results;
	/* e.g. {1, 2}, {2, 3}, {3, 6} */
	std::unordered_map<std::string, PqlEntityType> PQLentities;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	results = Parent::getAllPredecessorSuccessorInfo();
	//case PqlRelationshipType::Next:
	//	results = Next::getAllPredecessorSuccessorInfo();
	//	if (lhsRef.second == rhsRef.second) { /* Special case: Next(s1, s1) has a legitimate result */
	//		PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt)); /*just do LHS*/
	//		std::vector<int> lhsResults = std::get<0>(results);
	//		std::vector<int> rhsResults = std::get<1>(results);
	//		std::vector<int> finalResults;
	//		for (size_t i = 0; i < lhsResults.size(); i++) {
	//			if (lhsResults[i] == rhsResults[i]) {
	//				finalResults.emplace_back(lhsResults[i]);
	//			}
	//		}
	//		PQLmap[lhsRef.second] = finalResults;
	//		return EvaluatedTable(PQLentities, PQLmap);
	//	}
	//	break;
	if (lhsRef.second == rhsRef.second) { /* Special case: Parent(s1, s1), recursive call, technically shouldn't be allowed */
		PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
		/* No values populated to PQLmap for this case */
		return EvaluatedTable(PQLentities, PQLmap);
	}
	if (lhsRef.first == PqlReferenceType::Synonym) {
		PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
		PQLmap[lhsRef.second] = std::get<0>(results); /* if RHS is wildcard, LHS may have duplicate values */
	}
	if (rhsRef.first == PqlReferenceType::Synonym) {
		PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));
		PQLmap[rhsRef.second] = std::get<1>(results); /* if LHS is wildcard, RHS may have duplicate values */
	}
	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable ParentInstruction::helperHandleTwoWildcards() {
	bool isEmptyTable = true;
	isEmptyTable = std::get<0>(Next::getAllPredecessorSuccessorInfo()).empty();
	// No Parent rs exists => isEmptyTable == true => EvTable.evResult == false (innerJoinMerge() can drop table)
	// Parent rs exists => isEmptyTable == false => EvTable.evResult == true (innerJoinMerge() can merge dummy table, preserving all rows)
	return EvaluatedTable(!isEmptyTable);
}
