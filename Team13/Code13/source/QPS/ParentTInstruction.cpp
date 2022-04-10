#include "ParentTInstruction.h"

ParentTInstruction::ParentTInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter) :
	RelationshipInstruction(lhsRef, rhsRef, pkbGetter) {}

EvaluatedTable ParentTInstruction::execute() {
	EvaluatedTable resultTable;
	// e.g Parent*(6, 7)
	if (lhsRef.first == PqlReferenceType::INTEGER && rhsRef.first == PqlReferenceType::INTEGER) {
		resultTable = helperHandleTwoIntegers();
	}
	// e.g Parent*(6, s2), Parent*(6, _)
	else if (lhsRef.first == PqlReferenceType::INTEGER) {
		if (rhsRef.first == PqlReferenceType::SYNONYM) {
			resultTable = helperHandleOneInt(PqlReferenceType::INTEGER, PqlReferenceType::SYNONYM);
		} else {
			resultTable = helperHandleOneInt(PqlReferenceType::INTEGER, PqlReferenceType::WILDCARD);
		}
	}
	// e.g. Parent*(s1, 7), Parent*(_ 7)
	else if (rhsRef.first == PqlReferenceType::INTEGER) {
		if (lhsRef.first == PqlReferenceType::SYNONYM) {
			resultTable = helperHandleOneInt(PqlReferenceType::SYNONYM, PqlReferenceType::INTEGER);
		} else {
			resultTable = helperHandleOneInt(PqlReferenceType::WILDCARD, PqlReferenceType::INTEGER);
		}
	}
	// Parent*(s1, s2), Parent*(s1, _), Parent*(_, s2)
	else if (!(lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD)) {
		resultTable = helperHandleTwoStmtsMaybeWildcard();
	}
	// Parent*(_, _)
	else {
		if (lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD) {
			resultTable = helperHandleTwoWildcards();
		}
	}
	return resultTable;
}

EvaluatedTable ParentTInstruction::helperHandleTwoIntegers() {
	StmtIndex lhsStmtIndex, rhsStmtIndex;
	bool evResult = false;
	int lhsRefValue = stoi(lhsRef.second);
	int rhsRefValue = stoi(rhsRef.second);
	if (pkbGetter->containsStmt(lhsRefValue) && pkbGetter->containsStmt(rhsRefValue)) {
		lhsStmtIndex = StmtIndex(lhsRefValue);
		rhsStmtIndex = StmtIndex(rhsRefValue);
		evResult = pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, lhsStmtIndex, rhsStmtIndex);
	}
	return EvaluatedTable(evResult);
}

EvaluatedTable ParentTInstruction::helperHandleOneInt(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType) {
	std::vector<StmtIndex> stmts = pkbGetter->getAllStmts();
	std::vector<int> results;
	int oneInt;
	std::string otherSynonym;
	if (lhsRefType == PqlReferenceType::INTEGER) {
		oneInt = stoi(lhsRef.second);
	} else { /* rhsRefType == PqlReferenceType::ident */
		oneInt = stoi(rhsRef.second);
	}
	/* Handle one integer results */
	if (pkbGetter->containsStmt(oneInt)) { /* e.g. checks if STMT 6 exists, if not, return empty results */
		StmtIndex oneIntIndex = StmtIndex(oneInt);
		if (lhsRefType == PqlReferenceType::INTEGER) { /* e.g {7} if 6 is a Parent of some s2 (e.g. 7) */
			results = pkbGetter->getRSInfoFromLeftArg(RelationshipType::PARENT_T, oneIntIndex);
		} else { /* e.g {6} if some s1 (e.g. 6) is a Parent of 7 */
			results = pkbGetter->getRSInfoFromRightArg(RelationshipType::PARENT_T, oneIntIndex);
		}
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
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[otherSynonym] = results;

	return EvaluatedTable(PQLmap);
}

EvaluatedTable ParentTInstruction::helperHandleTwoStmtsMaybeWildcard() {
	std::tuple<std::vector<int>, std::vector<int>> results;
	/* e.g. {1, 2}, {2, 3}, {3, 6} */
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	results = pkbGetter->getRSAllInfo(RelationshipType::PARENT_T);
	if (lhsRef.second == rhsRef.second) { /* Special case: Parent*(s1, s1), recursive call, technically shouldn't be allowed */
		/* No values populated to PQLmap for this case */
		PQLmap[lhsRef.second] = std::vector<int>();
		return EvaluatedTable(PQLmap);
	}
	if (lhsRef.first == PqlReferenceType::SYNONYM) {
		PQLmap[lhsRef.second] = std::get<0>(results); /* if RHS is WILDCARD, LHS may have duplicate values */
	}
	if (rhsRef.first == PqlReferenceType::SYNONYM) {
		PQLmap[rhsRef.second] = std::get<1>(results); /* if LHS is WILDCARD, RHS may have duplicate values */
	}
	return EvaluatedTable(PQLmap);
}

EvaluatedTable ParentTInstruction::helperHandleTwoWildcards() {
	bool isEmptyTable = true;
	isEmptyTable = std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).empty();
	// No Parent* rs exists => isEmptyTable == true => EvTable.evResult == false (innerJoinMerge() can drop table)
	// Parent* rs exists => isEmptyTable == false => EvTable.evResult == true (innerJoinMerge() can merge dummy table, preserving all rows)
	return EvaluatedTable(!isEmptyTable);
}
