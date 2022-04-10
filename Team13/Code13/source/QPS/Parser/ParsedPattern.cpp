#include "ParsedPattern.h"

const std::unordered_set<PqlReferenceType> ParsedPattern::ENT_REF =
	{ PqlReferenceType::SYNONYM, PqlReferenceType::WILDCARD, PqlReferenceType::IDENT };

const std::unordered_set<EntityType> ParsedPattern::VAR_ENTITIES = { EntityType::VARIABLE };

const std::unordered_map<EntityType, PqlPatternType>
ParsedPattern::VALID_PATTERN_SYN_DESIGN_ENTITIES = {
	{ EntityType::ASSIGN,	PqlPatternType::PATTERN_A },
	{ EntityType::WHILE,	PqlPatternType::PATTERN_W },
	{ EntityType::IF,		PqlPatternType::PATTERN_I },
};

const std::unordered_map<PqlPatternType, std::unordered_set<PqlReferenceType>>
ParsedPattern::VALID_PATTERN_ENT_REF = {
	{ PqlPatternType::PATTERN_A, ParsedPattern::ENT_REF },
	{ PqlPatternType::PATTERN_W, ParsedPattern::ENT_REF },
	{ PqlPatternType::PATTERN_I, ParsedPattern::ENT_REF },
};

const std::unordered_map<PqlPatternType, std::unordered_set<EntityType>> ParsedPattern::VALID_PATTERN_ENT_REF_ENTITIES = {
	{ PqlPatternType::PATTERN_A, ParsedPattern::VAR_ENTITIES },
	{ PqlPatternType::PATTERN_W, ParsedPattern::VAR_ENTITIES },
	{ PqlPatternType::PATTERN_I, ParsedPattern::VAR_ENTITIES },
};

const std::unordered_map<PqlPatternType, std::unordered_set<PqlExpressionType>>
ParsedPattern::VALID_PATTERN_EXPR_SPEC = {
	{ PqlPatternType::PATTERN_A, { PqlExpressionType::FULL, PqlExpressionType::PARTIAL, PqlExpressionType::WILDCARD } },
	{ PqlPatternType::PATTERN_W, { PqlExpressionType::WILDCARD } },
	{ PqlPatternType::PATTERN_I, { PqlExpressionType::WILDCARD } },
};

const std::unordered_map<PqlPatternType, int>
ParsedPattern::VALID_PATTERN_NUM_OF_ARGS = {
	{ PqlPatternType::PATTERN_A, 2 },
	{ PqlPatternType::PATTERN_W, 2 },
	{ PqlPatternType::PATTERN_I, 3 },
};

void ParsedPattern::validate(const std::unordered_map<std::string, EntityType>& declarationMap) {
	if (declarationMap.find(synonym) == declarationMap.end()) {
		throw QPSException(QPSException::VALIDATOR_ERROR);
	}
	EntityType synonymType = declarationMap.at(synonym);
	if (VALID_PATTERN_SYN_DESIGN_ENTITIES.find(synonymType) == VALID_PATTERN_SYN_DESIGN_ENTITIES.end()) {
		throw QPSException(QPSException::VALIDATOR_ERROR);
	}
	/* Reassign to a specific pattern type based on the type of the synonym */
	PqlPatternType reassignPatternType = VALID_PATTERN_SYN_DESIGN_ENTITIES.at(synonymType);
	/* Type-validate the first entRef */
	PqlReferenceType entRefType = entRef.first;
	if (VALID_PATTERN_ENT_REF.at(reassignPatternType).find(entRefType) == VALID_PATTERN_ENT_REF.at(reassignPatternType).end()) {
		throw QPSException(QPSException::VALIDATOR_ERROR);
	}
	/* If the entRef is a synonym, perform further type validation on the design entity */
	if (entRefType == PqlReferenceType::SYNONYM) {
		std::string& entRefName = entRef.second;
		if (declarationMap.find(entRefName) == declarationMap.end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		EntityType entRefEntity = declarationMap.at(entRefName);
		if (VALID_PATTERN_ENT_REF_ENTITIES.at(reassignPatternType).find(entRefEntity) == VALID_PATTERN_ENT_REF_ENTITIES.at(reassignPatternType).end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
	}
	/* Type-validate the expression-spec */
	PqlExpressionType expressionType = expressionSpec.first;
	if (VALID_PATTERN_EXPR_SPEC.at(reassignPatternType).find(expressionType) == VALID_PATTERN_EXPR_SPEC.at(reassignPatternType).end()) {
		throw QPSException(QPSException::VALIDATOR_ERROR);
	}
	/* Type-validate the number of arguments */
	if (VALID_PATTERN_NUM_OF_ARGS.at(reassignPatternType) != numOfArgs) {
		throw QPSException(QPSException::VALIDATOR_ERROR);
	}
	patternType = reassignPatternType;
}

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

Instruction* ParsedPattern::toInstruction(const std::unordered_map<std::string, EntityType>& declarationMap, PKBGetter* pkbGetter) {
	validate(declarationMap);

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
