#include "FollowsInstruction.h"

FollowsInstruction::FollowsInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter) :
	RelationshipInstruction(lhsRef, rhsRef, pkbGetter) {}

EvaluatedTable FollowsInstruction::execute() {
	EvaluatedTable resultTable;
	// e.g Follows(6, 7)
	if (lhsRef.first == PqlReferenceType::INTEGER && rhsRef.first == PqlReferenceType::INTEGER) {
		resultTable = helperHandleTwoIntegers();
	}
	// e.g Follows(6, s2), Follows(6, _)
	else if (lhsRef.first == PqlReferenceType::INTEGER) {
		if (rhsRef.first == PqlReferenceType::SYNONYM) {
			resultTable = helperHandleOneInt(PqlReferenceType::INTEGER, PqlReferenceType::SYNONYM);
		} else {
			resultTable = helperHandleOneInt(PqlReferenceType::INTEGER, PqlReferenceType::WILDCARD);
		}
	}
	// e.g. Follows(s1, 7), Follows(_ 7)
	else if (rhsRef.first == PqlReferenceType::INTEGER) {
		if (lhsRef.first == PqlReferenceType::SYNONYM) {
			resultTable = helperHandleOneInt(PqlReferenceType::SYNONYM, PqlReferenceType::INTEGER);
		} else {
			resultTable = helperHandleOneInt(PqlReferenceType::WILDCARD, PqlReferenceType::INTEGER);
		}
	}
	// Follows(s1, s2), Follows(s1, _), Follows(_, s2)
	else if (!(lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD)) {
		resultTable = helperHandleTwoStmtsMaybeWildcard();
	}
	// Follows(_, _)
	else {
		if (lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD) {
			resultTable = helperHandleTwoWildcards();
		}
	}
	return resultTable;
}

EvaluatedTable FollowsInstruction::helperHandleTwoIntegers() {
	StmtIndex lhsStmtIndex, rhsStmtIndex;
	bool evResult = false;
	int lhsRefValue = stoi(lhsRef.second);
	int rhsRefValue = stoi(rhsRef.second);
	if (pkbGetter->containsStmt(lhsRefValue) && pkbGetter->containsStmt(rhsRefValue)) {
		lhsStmtIndex = StmtIndex(lhsRefValue);
		rhsStmtIndex = StmtIndex(rhsRefValue);
		evResult = pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, lhsStmtIndex, rhsStmtIndex);
	}
	return EvaluatedTable(evResult);
}

EvaluatedTable FollowsInstruction::helperHandleOneInt(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType) {
	std::vector<StmtIndex> stmts = pkbGetter->getAllStmts();
	std::vector<Index> results;
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
		if (lhsRefType == PqlReferenceType::INTEGER) { /* e.g {7} if 6 is a Follows of some s2 (e.g. 7) */
			results = pkbGetter->getRSInfoFromLeftArg(RelationshipType::FOLLOWS, oneIntIndex);
		} else { /* e.g {6} if some s1 (e.g. 6) is a Follows of 7 */
			results = pkbGetter->getRSInfoFromRightArg(RelationshipType::FOLLOWS, oneIntIndex);
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
	Table PQLmap;
	PQLmap[otherSynonym] = results;

	return EvaluatedTable(PQLmap);
}

EvaluatedTable FollowsInstruction::helperHandleTwoStmtsMaybeWildcard() {
	std::tuple<std::vector<Index>, std::vector<Index>> results;
	/* e.g. {1, 2}, {2, 3}, {3, 6} */
	Table PQLmap;
	results = pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS);
	if (lhsRef.second == rhsRef.second) { /* Special case: Follows(s1, s1), recursive call, technically shouldn't be allowed */
		/* No values populated to PQLmap for this case */
		PQLmap[lhsRef.second] = std::vector<Index>();
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

EvaluatedTable FollowsInstruction::helperHandleTwoWildcards() {
	bool isEmptyTable = true;
	isEmptyTable = std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).empty();
	return EvaluatedTable(!isEmptyTable);
}
