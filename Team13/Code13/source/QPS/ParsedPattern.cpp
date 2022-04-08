#include "ParsedPattern.h"

ParsedPattern::ParsedPattern(std::string& synonym, PqlReference entRef,
	PqlExpression expressionSpec, int numOfArgs)
	: ParsedPattern(synonym, PqlPatternType::PATTERN, entRef, expressionSpec, numOfArgs) {}

ParsedPattern::ParsedPattern(std::string& synonym, PqlPatternType patternType,
	PqlReference entRef, PqlExpression expressionSpec, int numOfArgs)
	: synonym(synonym), patternType(patternType), entRef(entRef),
	expressionSpec(expressionSpec), numOfArgs(numOfArgs) {}

Instruction* ParsedPattern::toInstruction(PKBGetter* pkbGetter) const {
	Instruction* instruction = nullptr;
	switch (patternType) {
	case PqlPatternType::PATTERN_A:
		instruction = new PatternAInstruction(synonym, entRef, expressionSpec, pkbGetter);
		break;
	case PqlPatternType::PATTERN_I:
		instruction = new PatternIInstruction(synonym, entRef, pkbGetter);
		break;
	case PqlPatternType::PATTERN_W:
		instruction = new PatternWInstruction(synonym, entRef, pkbGetter);
		break;
	default:
		break;
	}
	return instruction;
}

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
