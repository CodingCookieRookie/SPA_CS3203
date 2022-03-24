#include "PatternInstruction.h"

PatternInstruction::PatternInstruction(
	std::string synonym, PqlPatternType pqlPatternType,
	PqlReference entRef, PqlExpression expressionSpec) :
	synonym(synonym), pqlPatternType(pqlPatternType),
	entRef(entRef), expressionSpec(expressionSpec) {}

EvaluatedTable PatternInstruction::handlePatternA() {
	std::unordered_map<std::string, PqlEntityType> PQLentities;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	std::vector<int> allStmts;
	PQLentities.insert(std::pair(synonym, PqlEntityType::Assign));
	switch (entRef.first) {
	case PqlReferenceType::Ident:
		if (Entity::containsVar(entRef.second)) {
			VarIndex varIndex = Entity::getVarIdx(entRef.second);
			switch (expressionSpec.first) {
			case PqlExpressionType::Full:
				allStmts = Pattern::getAssignStmtsFromVarExprFullMatch(varIndex, expressionSpec.second);
				break;
			case PqlExpressionType::Partial:
				allStmts = Pattern::getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
				break;
			case PqlExpressionType::Wildcard:
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
		case PqlExpressionType::Full:
			allPatternStmtInfo = Pattern::getAssignStmtsFromExprFullMatch(expressionSpec.second);
			break;
		case PqlExpressionType::Partial:
			allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(expressionSpec.second);
			break;
		case PqlExpressionType::Wildcard:
			allPatternStmtInfo = Pattern::getAllAssignPatternInfo();
			break;
		default:
			throw EvaluatorException(EvaluatorException::PATTERN_ERROR);
			break;
		}
		if (entRef.first == PqlReferenceType::Synonym) {
			std::vector<int> varIndices = std::get<1>(allPatternStmtInfo);
			PQLmap[entRef.second] = varIndices;
			PQLentities.insert(std::pair(entRef.second, PqlEntityType::Variable));
		}
		allStmts = std::get<0>(allPatternStmtInfo);
	}
	PQLmap[synonym] = allStmts;
	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable PatternInstruction::handlePatternI() {
	std::unordered_map<std::string, PqlEntityType> PQLentities;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	std::vector<int> allStmts;
	PQLentities.insert(std::pair(synonym, PqlEntityType::If));
	switch (entRef.first) {
	case PqlReferenceType::Ident:
		if (Entity::containsVar(entRef.second)) {
			VarIndex varIndex = Entity::getVarIdx(entRef.second);
			allStmts = Pattern::getIfStmtsFromVar(varIndex);
		}
		break;
	default:
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo;
		allPatternStmtInfo = Pattern::getAllIfPatternInfo();
		if (entRef.first == PqlReferenceType::Synonym) {
			std::vector<int> varIndices = std::get<1>(allPatternStmtInfo);
			PQLmap[entRef.second] = varIndices;
			PQLentities.insert(std::pair(entRef.second, PqlEntityType::Variable));
		}
		allStmts = std::get<0>(allPatternStmtInfo);
	}
	PQLmap[synonym] = allStmts;
	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable PatternInstruction::handlePatternW() {
	std::unordered_map<std::string, PqlEntityType> PQLentities;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	std::vector<int> allStmts;
	PQLentities.insert(std::pair(synonym, PqlEntityType::While));
	switch (entRef.first) {
	case PqlReferenceType::Ident:
		if (Entity::containsVar(entRef.second)) {
			VarIndex varIndex = Entity::getVarIdx(entRef.second);
			allStmts = Pattern::getWhileStmtsFromVar(varIndex);
		}
		break;
	default:
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo;
		allPatternStmtInfo = Pattern::getAllWhilePatternInfo();
		if (entRef.first == PqlReferenceType::Synonym) {
			std::vector<int> varIndices = std::get<1>(allPatternStmtInfo);
			PQLmap[entRef.second] = varIndices;
			PQLentities.insert(std::pair(entRef.second, PqlEntityType::Variable));
		}
		allStmts = std::get<0>(allPatternStmtInfo);
	}
	PQLmap[synonym] = allStmts;
	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable PatternInstruction::execute() {
	EvaluatedTable evTable;
	switch (pqlPatternType) {
	case PqlPatternType::Pattern:
		evTable = handlePatternA();
		break;
	case PqlPatternType::PatternA:
		evTable = handlePatternA();
		break;
	case PqlPatternType::PatternI:
		evTable = handlePatternI();
		break;
	case PqlPatternType::PatternW:
		evTable = handlePatternW();
		break;
	}
	return evTable;
}
