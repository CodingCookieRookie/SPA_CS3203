#include "PatternInstruction.h"

PatternInstruction::PatternInstruction(
	std::string synonym, PqlReference entRef) :
	synonym(synonym), entRef(entRef) {}

PatternInstruction::PatternInstruction(
	std::string synonym, PqlReference entRef, PqlExpression expressionSpec) :
	synonym(synonym), entRef(entRef), expressionSpec(expressionSpec) {}

PatternInstruction::PatternInstruction(
	std::string synonym, PqlPatternType pqlPatternType,
	PqlReference entRef, PqlExpression expressionSpec) :
	synonym(synonym), pqlPatternType(pqlPatternType),
	entRef(entRef), expressionSpec(expressionSpec) {}

EvaluatedTable PatternAInstruction::execute() {
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	std::vector<int> allStmts;
	switch (entRef.first) {
	case PqlReferenceType::IDENT:
		if (Entity::containsVar(entRef.second)) {
			VarIndex varIndex = Entity::getVarIdx(entRef.second);
			switch (expressionSpec.first) {
			case PqlExpressionType::FULL:
				allStmts = Pattern::getAssignStmtsFromVarExprFullMatch(varIndex, expressionSpec.second);
				break;
			case PqlExpressionType::PARTIAL:
				allStmts = Pattern::getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
				break;
			case PqlExpressionType::WILDCARD:
				allStmts = Pattern::getAssignStmtsFromVar(varIndex);
				break;
			default:
				throw EvaluatorException(EvaluatorException::PATTERN_ERROR);
				break;
			}
		}
		break;
	default:
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo;
		switch (expressionSpec.first) {
		case PqlExpressionType::FULL:
			allPatternStmtInfo = Pattern::getAssignStmtsFromExprFullMatch(expressionSpec.second);
			break;
		case PqlExpressionType::PARTIAL:
			allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(expressionSpec.second);
			break;
		case PqlExpressionType::WILDCARD:
			allPatternStmtInfo = Pattern::getAllAssignPatternInfo();
			break;
		default:
			throw EvaluatorException(EvaluatorException::PATTERN_ERROR);
			break;
		}
		if (entRef.first == PqlReferenceType::SYNONYM) {
			std::vector<int> varIndices = std::get<1>(allPatternStmtInfo);
			PQLmap[entRef.second] = varIndices;
		}
		allStmts = std::get<0>(allPatternStmtInfo);
	}
	PQLmap[synonym] = allStmts;
	return EvaluatedTable(PQLmap);
}

EvaluatedTable PatternIInstruction::execute() {
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	std::vector<int> allStmts;
	switch (entRef.first) {
	case PqlReferenceType::IDENT:
		if (Entity::containsVar(entRef.second)) {
			VarIndex varIndex = Entity::getVarIdx(entRef.second);
			allStmts = Pattern::getIfStmtsFromVar(varIndex);
		}
		break;
	default:
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo;
		allPatternStmtInfo = Pattern::getAllIfPatternInfo();
		if (entRef.first == PqlReferenceType::SYNONYM) {
			std::vector<int> varIndices = std::get<1>(allPatternStmtInfo);
			PQLmap[entRef.second] = varIndices;
		}
		allStmts = std::get<0>(allPatternStmtInfo);
	}
	PQLmap[synonym] = allStmts;
	return EvaluatedTable(PQLmap);
}

EvaluatedTable PatternWInstruction::execute() {
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	std::vector<int> allStmts;
	switch (entRef.first) {
	case PqlReferenceType::IDENT:
		if (Entity::containsVar(entRef.second)) {
			VarIndex varIndex = Entity::getVarIdx(entRef.second);
			allStmts = Pattern::getWhileStmtsFromVar(varIndex);
		}
		break;
	default:
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo;
		allPatternStmtInfo = Pattern::getAllWhilePatternInfo();
		if (entRef.first == PqlReferenceType::SYNONYM) {
			std::vector<int> varIndices = std::get<1>(allPatternStmtInfo);
			PQLmap[entRef.second] = varIndices;
		}
		allStmts = std::get<0>(allPatternStmtInfo);
	}
	PQLmap[synonym] = allStmts;
	return EvaluatedTable(PQLmap);
}
