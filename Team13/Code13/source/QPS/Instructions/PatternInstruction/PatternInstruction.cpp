#include "PatternInstruction.h"

PatternInstruction::PatternInstruction(
	std::string synonym, PqlReference entRef, PKBGetter* pkbGetter) :
	synonym(synonym), entRef(entRef), pkbGetter(pkbGetter) {}

PatternInstruction::PatternInstruction(
	std::string synonym, PqlReference entRef, PqlExpression expressionSpec, PKBGetter* pkbGetter) :
	synonym(synonym), entRef(entRef), expressionSpec(expressionSpec), pkbGetter(pkbGetter) {}

std::unordered_set<std::string> PatternInstruction::getSynonyms() {
	std::unordered_set<std::string> results{ synonym };
	if (QPSCommons::isSynonymRef(entRef)) {
		results.insert(entRef.second);
	}
	return results;
}

EvaluatedTable PatternAInstruction::execute() {
	Table PQLmap;
	std::vector<Index> allStmts;
	switch (entRef.first) {
	case PqlReferenceType::IDENT:
		if (pkbGetter->containsNameIdxEntity(EntityType::VARIABLE, entRef.second)) {
			VarIndex varIndex = pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, entRef.second);
			switch (expressionSpec.first) {
			case PqlExpressionType::FULL:
				allStmts = pkbGetter->getAssignStmtsFromVarExprFullMatch(varIndex, expressionSpec.second);
				break;
			case PqlExpressionType::PARTIAL:
				allStmts = pkbGetter->getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
				break;
			case PqlExpressionType::WILDCARD:
				allStmts = pkbGetter->getAssignStmtsFromVar(varIndex);
				break;
			default:
				throw EvaluatorException(EvaluatorException::PATTERN_ERROR);
				break;
			}
		}
		break;
	default:
		std::tuple<std::vector<Index>, std::vector<Index>> allPatternStmtInfo;
		switch (expressionSpec.first) {
		case PqlExpressionType::FULL:
			allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprFullMatch(expressionSpec.second);
			break;
		case PqlExpressionType::PARTIAL:
			allPatternStmtInfo = pkbGetter->getAssignStmtsFromExprPartialMatch(expressionSpec.second);
			break;
		case PqlExpressionType::WILDCARD:
			allPatternStmtInfo = pkbGetter->getAllAssignPatternInfo();
			break;
		default:
			throw EvaluatorException(EvaluatorException::PATTERN_ERROR);
			break;
		}
		if (entRef.first == PqlReferenceType::SYNONYM) {
			std::vector<Index> varIndices = std::get<0>(allPatternStmtInfo);
			PQLmap[entRef.second] = varIndices;
		}
		allStmts = std::get<1>(allPatternStmtInfo);
	}
	PQLmap[synonym] = allStmts;
	return EvaluatedTable(PQLmap);
}

EvaluatedTable PatternIInstruction::execute() {
	Table PQLmap;
	std::vector<Index> allStmts;
	switch (entRef.first) {
	case PqlReferenceType::IDENT:
		if (pkbGetter->containsNameIdxEntity(EntityType::VARIABLE, entRef.second)) {
			VarIndex varIndex = pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, entRef.second);
			allStmts = pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIndex);
		}
		break;
	default:
		std::tuple<std::vector<Index>, std::vector<Index>> allPatternStmtInfo;
		allPatternStmtInfo = pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE);
		if (entRef.first == PqlReferenceType::SYNONYM) {
			std::vector<Index> varIndices = std::get<0>(allPatternStmtInfo);
			PQLmap[entRef.second] = varIndices;
		}
		allStmts = std::get<1>(allPatternStmtInfo);
	}
	PQLmap[synonym] = allStmts;
	return EvaluatedTable(PQLmap);
}

EvaluatedTable PatternWInstruction::execute() {
	Table PQLmap;
	std::vector<Index> allStmts;
	switch (entRef.first) {
	case PqlReferenceType::IDENT:
		if (pkbGetter->containsNameIdxEntity(EntityType::VARIABLE, entRef.second)) {
			VarIndex varIndex = pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, entRef.second);
			allStmts = pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIndex);
		}
		break;
	default:
		std::tuple<std::vector<Index>, std::vector<Index>> allPatternStmtInfo;
		allPatternStmtInfo = pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE);
		if (entRef.first == PqlReferenceType::SYNONYM) {
			std::vector<Index> varIndices = std::get<0>(allPatternStmtInfo);
			PQLmap[entRef.second] = varIndices;
		}
		allStmts = std::get<1>(allPatternStmtInfo);
	}
	PQLmap[synonym] = allStmts;
	return EvaluatedTable(PQLmap);
}
