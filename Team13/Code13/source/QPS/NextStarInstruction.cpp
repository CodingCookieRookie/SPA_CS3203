#include "NextTInstruction.h"

NextTInstruction::NextTInstruction(PqlReference lhsRef, PqlReference rhsRef, NextTProcessor* nextTProcessor, PKBGetter* pkbGetter) :
	RelationshipInstruction(lhsRef, rhsRef, nextTProcessor, pkbGetter) {}

EvaluatedTable NextTInstruction::execute() {
	EvaluatedTable resultTable;
	/* e.g Next*(6, 7) */
	if (lhsRef.first == PqlReferenceType::INTEGER && rhsRef.first == PqlReferenceType::INTEGER) {
		resultTable = helperHandleTwoIntegers();
	}
	/* e.g Next/Next*(6, s2), Next/Next*(6, _) */
	else if (lhsRef.first == PqlReferenceType::INTEGER) {
		if (rhsRef.first == PqlReferenceType::SYNONYM) {
			resultTable = helperHandleOneInt(PqlReferenceType::INTEGER, PqlReferenceType::SYNONYM);
		} else {
			resultTable = helperHandleOneInt(PqlReferenceType::INTEGER, PqlReferenceType::WILDCARD);
		}
	}
	/* e.g Next/Next*(s1, 7), Next/Next*(_, 7) */
	else if (rhsRef.first == PqlReferenceType::INTEGER) {
		if (lhsRef.first == PqlReferenceType::SYNONYM) {
			resultTable = helperHandleOneInt(PqlReferenceType::SYNONYM, PqlReferenceType::INTEGER);
		} else {
			resultTable = helperHandleOneInt(PqlReferenceType::WILDCARD, PqlReferenceType::INTEGER);
		}
	}
	/* Next/Next*(s1, s2), Next/Next*(s1, _), Next/Next*(_, s2) */
	else if (!(lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD)) {
		resultTable = helperHandleTwoStmtsMaybeWildcard();
	}
	// Next/Next*(_, _)
	else {
		if (lhsRef.first == PqlReferenceType::WILDCARD && rhsRef.first == PqlReferenceType::WILDCARD) {
			resultTable = helperHandleTwoWildcards();
		}
	}
	return resultTable;
}

EvaluatedTable NextTInstruction::helperHandleTwoIntegers() {
	StmtIndex lhsStmtIndex, rhsStmtIndex;
	bool evResult = false;
	int lhsRefValue = stoi(lhsRef.second);
	int rhsRefValue = stoi(rhsRef.second);
	if (pkbGetter->containsStmt(lhsRefValue) && pkbGetter->containsStmt(rhsRefValue)) {
		lhsStmtIndex = StmtIndex(lhsRefValue);
		rhsStmtIndex = StmtIndex(rhsRefValue);
		evResult = nextTProcessor->doesRsHold(lhsStmtIndex, rhsStmtIndex, pkbGetter);
	}
	return EvaluatedTable(evResult);
}

EvaluatedTable NextTInstruction::helperHandleOneInt(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType) {
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
		if (lhsRefType == PqlReferenceType::INTEGER) { /* e.g {7} if 6 is a Next* of some s2 (e.g. 7) */
			results = nextTProcessor->getUsingLeftStmtIndex(oneIntIndex, pkbGetter);
		} else { /* e.g {6} if some s1 (e.g. 6) is a Next* of 7 */
			results = nextTProcessor->getUsingRightStmtIndex(oneIntIndex, pkbGetter);
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

EvaluatedTable NextTInstruction::helperHandleTwoStmtsMaybeWildcard() {
	std::tuple<std::vector<int>, std::vector<int>> results;
	/* e.g. {1, 2}, {2, 3}, {3, 6} */
	Table PQLmap;
	results = nextTProcessor->getAll(pkbGetter);
	if (lhsRef.second == rhsRef.second) { /* Special case: Next(s1, s1) has a legitimate result */
		std::vector<Index> lhsResults = std::get<0>(results);
		std::vector<Index> rhsResults = std::get<1>(results);
		std::vector<Index> finalResults;
		for (size_t i = 0; i < lhsResults.size(); i++) {
			if (lhsResults[i] == rhsResults[i]) {
				finalResults.emplace_back(lhsResults[i]);
			}
		}
		PQLmap[lhsRef.second] = finalResults;
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

EvaluatedTable NextTInstruction::helperHandleTwoWildcards() {
	bool isEmptyTable = true;
	isEmptyTable = std::get<0>(nextTProcessor->getAll(pkbGetter)).empty();
	return EvaluatedTable(!isEmptyTable);
}

QueryScore NextTInstruction::getDifficultyScore() {
	return Instruction::getDifficultyScore() + 10;
}
