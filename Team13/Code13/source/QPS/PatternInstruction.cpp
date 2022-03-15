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
	if (entRef.first == PqlReferenceType::ident) {
		/* Pattern a("x", _"x"_) or a("x", _"123"_) or Pattern a("x", _) */
		if (Entity::containsVar(entRef.second)) {
			VarIndex varIndex = Entity::getVarIdx(entRef.second);
			if (expressionSpec.first == PqlExpressionType::partial) {
				allStmts = Pattern::getAssignStmtsFromVarExprPartialMatch(varIndex, expressionSpec.second);
			} else if (expressionSpec.first == PqlExpressionType::wildcard) {
				allStmts = Pattern::getAssignStmtsFromVar(varIndex);
			} else if (expressionSpec.first == PqlExpressionType::full) {
				allStmts = Pattern::getAssignStmtsFromVarExprFullMatch(varIndex, expressionSpec.second);
			}
		}
	} else {
		/* Pattern a(v, _"x"_) or a(v, _"123"_) or Pattern a(v, _) */
		/* Pattern a(_, _"x"_) or a(_, _"123"_) or Pattern a(_, _) */
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo;
		if (expressionSpec.first == PqlExpressionType::full) {
			allPatternStmtInfo = Pattern::getAssignStmtsFromExprFullMatch(expressionSpec.second);
		} else if (expressionSpec.first == PqlExpressionType::partial) {
			/* currently only has this for iteration 1 */
			allPatternStmtInfo = Pattern::getAssignStmtsFromExprPartialMatch(expressionSpec.second);
		} else if (expressionSpec.first == PqlExpressionType::wildcard) {
			allPatternStmtInfo = Pattern::getAllAssignPatternInfo();
		} else {
			throw EvaluatorException(EvaluatorException::PATTERN_ERROR);
		}
		if (entRef.first == PqlReferenceType::synonym) {
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
	return EvaluatedTable(PQLentities, PQLmap);
}

EvaluatedTable PatternInstruction::handlePatternW() {
	std::unordered_map<std::string, PqlEntityType> PQLentities;
	std::unordered_map<std::string, std::vector<int>> PQLmap;
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
