#include "CallsStarInstruction.h"

CallsStarInstruction::CallsStarInstruction(PqlReference lhsRef, PqlReference rhsRef) : RelationshipInstruction(lhsRef, rhsRef) {}

EvaluatedTable CallsStarInstruction::execute() {
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

EvaluatedTable CallsStarInstruction::helperHandleTwoIdents() {
	ProcIndex lhsProcIndex, rhsProcIndex;
	bool evResult = false;
	if (Entity::containsProc(lhsRef.second) && Entity::containsProc(rhsRef.second)) {
		lhsProcIndex = Entity::getProcIdx(lhsRef.second);
		rhsProcIndex = Entity::getProcIdx(rhsRef.second);
		evResult = CallsT::containsSuccessor(lhsProcIndex, rhsProcIndex);
	}
	return EvaluatedTable(evResult); /* e.g evResult == true, if "first" calls "second" */
}

EvaluatedTable CallsStarInstruction::helperHandleOneIdent(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType) {
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
			if (lhsRefType == PqlReferenceType::IDENT) {
				if (CallsT::containsSuccessor(oneIdentRef, proc)) {
					results.emplace_back(proc); /* e.g {"first"} if "first" calls some q */
				}
			} else if (rhsRefType == PqlReferenceType::IDENT) {
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
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[otherSynonym] = results;

	return EvaluatedTable(PQLmap);
}

EvaluatedTable CallsStarInstruction::helperHandleTwoProcMaybeWildcard() {
	/* Assumption: Different synonym names(i.e. Calls(p, q), not Calls(p, p)) */
	std::tuple<std::vector<int>, std::vector<int>> results;
	results = CallsT::getAllPredecessorSuccessorInfo();

	/* e.g. {1, 2}, {2, 3}, {3, 6} */
	std::unordered_map<std::string, std::vector<int>> PQLmap;

	if (lhsRef.second == rhsRef.second) { /* Special case: Calls(p, p), recursive call, technically shouldn't be allowed */
		/* No values populated to PQLmap for this case */
		PQLmap[lhsRef.second] = std::vector<int>();
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

EvaluatedTable CallsStarInstruction::helperHandleTwoWildcards() {
	bool isEmptyTable = true;
	if (lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD) {
		isEmptyTable = std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).empty();
	}
	// No Calls rs exists => isEmptyTable == true => EvTable.evResult == false (innerJoinMerge() can drop table)
	// Calls rs exists => isEmptyTable == false => EvTable.evResult == true (innerJoinMerge() can merge dummy table, preserving all rows)
	return EvaluatedTable(!isEmptyTable);
}
