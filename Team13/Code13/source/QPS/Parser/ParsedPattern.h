#pragma once

#include "../Instructions/PatternInstruction/PatternInstruction.h"

class ParsedPattern {
private:
	static const std::unordered_set<PqlReferenceType> ENT_REF;
	static const std::unordered_set<EntityType> VAR_ENTITIES;

	/* Tables for type-validating the pattern clause */
	static const std::unordered_map<EntityType, PqlPatternType> VALID_PATTERN_SYN_DESIGN_ENTITIES;
	static const std::unordered_map<PqlPatternType, std::unordered_set<PqlReferenceType>> VALID_PATTERN_ENT_REF;
	static const std::unordered_map<PqlPatternType, std::unordered_set<EntityType>> VALID_PATTERN_ENT_REF_ENTITIES;
	static const std::unordered_map<PqlPatternType, std::unordered_set<PqlExpressionType>> VALID_PATTERN_EXPR_SPEC;
	static const std::unordered_map<PqlPatternType, int> VALID_PATTERN_NUM_OF_ARGS;

	std::string synonym;
	PqlPatternType patternType;
	PqlReference entRef;
	PqlExpression expressionSpec;
	int numOfArgs;

	void validate(const std::unordered_map<std::string, EntityType>& declarationMap);
public:
	ParsedPattern(std::string& synonym, PqlReference entRef,
		PqlExpression expressionSpec, int numOfArgs);
	ParsedPattern(std::string& synonym, PqlPatternType patternType,
		PqlReference entRef, PqlExpression expressionSpec, int numOfArgs);
	Instruction* toInstruction(PKBGetter* pkbGetter) const;
	std::string getSynonym() const;
	PqlPatternType getPatternType() const;
	PqlReference getEntRef() const;
	PqlExpression getExpression() const;
	int getNumOfArgs() const;

	Instruction* toInstruction(const std::unordered_map<std::string, EntityType>& declarationMap, PKBGetter* pkbGetter);
};
