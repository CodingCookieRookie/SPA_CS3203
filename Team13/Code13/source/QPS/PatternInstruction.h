#pragma once

#include "Instruction.h"

class PatternInstruction : public Instruction {
private:
	std::string synonym;
	PqlPatternType pqlPatternType;
	PqlReference entRef;
	PqlExpression expressionSpec;

public:
	PatternInstruction(std::string synonym, PqlPatternType pqlPatternType, PqlReference entRef, PqlExpression expressionSpec);
	EvaluatedTable handlePatternA();
	EvaluatedTable handlePatternI();
	EvaluatedTable handlePatternW();
	EvaluatedTable execute() override;
};
