#include "ParsedPattern.h"

ParsedPattern::ParsedPattern(std::string& synonym, PqlReference entRef,
	PqlExpression expressionSpec, int numOfArgs)
	: ParsedPattern(synonym, PqlPatternType::Pattern, entRef, expressionSpec, numOfArgs) {}

ParsedPattern::ParsedPattern(std::string& synonym, PqlPatternType patternType,
	PqlReference entRef, PqlExpression expressionSpec, int numOfArgs)
	: synonym(synonym), patternType(patternType), entRef(entRef),
	expressionSpec(expressionSpec), numOfArgs(numOfArgs) {}

std::string ParsedPattern::getSynonym() const {
	return synonym;
}

PqlPatternType ParsedPattern::getPatternType() const {
	return patternType;
}

PqlReference ParsedPattern::getEntRef() const {
	return entRef;
}

PqlExpression ParsedPattern::getExpression() const {
	return expressionSpec;
}

int ParsedPattern::getNumOfArgs() const {
	return numOfArgs;
}
