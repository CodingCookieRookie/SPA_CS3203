#pragma once

#include "Clause.h"

class ParsedPattern : public Clause {
private:
	std::string synonym;
	PqlPatternType patternType;
	PqlReference entRef;
	PqlExpression expressionSpec;
	int numOfArgs;
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
};
