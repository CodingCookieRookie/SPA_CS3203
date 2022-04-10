#include "CallsTInstruction.h"

CallsTInstruction::CallsTInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter) :
	RelationshipInstruction(lhsRef, rhsRef, pkbGetter) {}

EvaluatedTable CallsTInstruction::execute() {
	EvaluatedTable resultTable;
	/* e.g Calls/Calls*("first", "second") */
	if (lhsRef.first == PqlReferenceType::IDENT && rhsRef.first == PqlReferenceType::IDENT) {
		resultTable = helperHandleTwoIdents();
	}
	/* e.g Calls/Calls*("first", q), Calls/Calls*("first", _) */
	else if (lhsRef.first == PqlReferenceType::IDENT) {
		if (rhsRef.first == PqlReferenceType::SYNONYM) {
			resultTable = helperHandleOneIdent(PqlReferenceType::IDENT, PqlReferenceType::SYNONYM);
		} else {
			resultTable = helperHandleOneIdent(PqlReferenceType::IDENT, PqlReferenceType::WILDCARD);
		}
	}
	/*  e.g.Calls/Calls*(p, "second"), Calls/Calls*(_, "second") */
	else if (rhsRef.first == PqlReferenceType::IDENT) {
		if (lhsRef.first == PqlReferenceType::SYNONYM) {
			resultTable = helperHandleOneIdent(PqlReferenceType::SYNONYM, PqlReferenceType::IDENT);
		} else {
			resultTable = helperHandleOneIdent(PqlReferenceType::WILDCARD, PqlReferenceType::IDENT);
		}
	}
	/* Calls/Calls*(p, q), Calls/Calls*(p, _), Calls/Calls*(_, q) */
	else if (!(lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD)) {
		resultTable = helperHandleTwoProcMaybeWildcard();
	}
	// Calls/Calls*(_, _)
	else {
		resultTable = helperHandleTwoWildcards();
	}
	return resultTable;
}

EvaluatedTable CallsTInstruction::helperHandleTwoIdents() {
	ProcIndex lhsProcIndex, rhsProcIndex;
	bool evResult = false;
	if (pkbGetter->containsNameIdxEntity(EntityType::PROCEDURE, lhsRef.second) &&
		pkbGetter->containsNameIdxEntity(EntityType::PROCEDURE, rhsRef.second)) {
		lhsProcIndex = pkbGetter->getNameIdxEntityIndex(EntityType::PROCEDURE, lhsRef.second);
		rhsProcIndex = pkbGetter->getNameIdxEntityIndex(EntityType::PROCEDURE, rhsRef.second);
		evResult = pkbGetter->getRSContainsInfo(RelationshipType::CALLS_T, lhsProcIndex, rhsProcIndex);
	}
	return EvaluatedTable(evResult); /* e.g evResult == true, if "first" calls "second" */
}

EvaluatedTable CallsTInstruction::helperHandleOneIdent(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType) {
	std::vector<ProcIndex> procs = pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE);
	std::vector<Index> results;
	std::string oneIdent;
	std::string otherSynonym;
	if (lhsRefType == PqlReferenceType::IDENT) {
		oneIdent = lhsRef.second;
	} else { /* rhsRefType == PqlReferenceType::ident */
		oneIdent = rhsRef.second;
	}
	/* Handle one ident to proc results */
	if (pkbGetter->containsNameIdxEntity(EntityType::PROCEDURE, oneIdent)) {
		/* e.g. checks if proc named "first" exists, if not, return empty results */
		ProcIndex oneIdentRef = pkbGetter->getNameIdxEntityIndex(EntityType::PROCEDURE, oneIdent);
		if (lhsRefType == PqlReferenceType::IDENT) { /* e.g {7} if 6 is a Parent of some s2 (e.g. 7) */
			results = pkbGetter->getRSInfoFromLeftArg(RelationshipType::CALLS_T, oneIdentRef);
		} else { /* e.g {6} if some s1 (e.g. 6) is a Parent of 7 */
			results = pkbGetter->getRSInfoFromRightArg(RelationshipType::CALLS_T, oneIdentRef);
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
	Table PQLmap;
	PQLmap[otherSynonym] = results;
	return EvaluatedTable(PQLmap);
}

EvaluatedTable CallsTInstruction::helperHandleTwoProcMaybeWildcard() {
	/* Assumption: Different synonym names(i.e. Calls(p, q), not Calls(p, p)) */
	std::tuple<std::vector<Index>, std::vector<Index>> results;
	results = pkbGetter->getRSAllInfo(RelationshipType::CALLS_T);

	/* e.g. {1, 2}, {2, 3}, {3, 6} */
	Table PQLmap;

	if (lhsRef.second == rhsRef.second) { /* Special case: Calls*(p, p), recursive call, technically shouldn't be allowed */
		/* No values populated to PQLmap for this case => short-circuit */
		PQLmap[lhsRef.second] = std::vector<Index>();
		return EvaluatedTable(PQLmap);
	}
	if (lhsRef.first == PqlReferenceType::SYNONYM) {
		PQLmap[lhsRef.second] = std::get<0>(results); /* if RHS is wildcard, LHS may have duplicate values */
	}
	if (rhsRef.first == PqlReferenceType::SYNONYM) {
		PQLmap[rhsRef.second] = std::get<1>(results); /* if LHS is wildcard, RHS may have duplicate values */
	}
	return EvaluatedTable(PQLmap);
}

EvaluatedTable CallsTInstruction::helperHandleTwoWildcards() {
	bool isEmptyTable = true;
	if (lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD) {
		isEmptyTable = std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).empty();
	}
	return EvaluatedTable(!isEmptyTable);
}
