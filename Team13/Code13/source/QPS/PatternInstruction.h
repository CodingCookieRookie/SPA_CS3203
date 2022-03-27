#pragma once

#include "Instruction.h"

class PatternInstruction : public Instruction {
private:
	PqlPatternType pqlPatternType;

protected:
	std::string synonym;
	PqlReference entRef;
	PqlExpression expressionSpec;
	PatternInstruction(std::string synonym, PqlReference entRef);
	PatternInstruction(std::string synonym, PqlReference entRef, PqlExpression expressionSpec);
	PatternInstruction(std::string synonym, PqlPatternType pqlPatternType, PqlReference entRef, PqlExpression expressionSpec);
};

class PatternAInstruction : public PatternInstruction {
public:
	PatternAInstruction(std::string synonym, PqlReference entRef, PqlExpression expressionSpec) : PatternInstruction(synonym, entRef, expressionSpec) {};
	EvaluatedTable execute() override;
};

class PatternIInstruction : public PatternInstruction {
public:
	PatternIInstruction(std::string synonym, PqlReference entRef) : PatternInstruction(synonym, entRef) {};
	EvaluatedTable execute() override;
};

class PatternWInstruction : public PatternInstruction {
public:
	PatternWInstruction(std::string synonym, PqlReference entRef) : PatternInstruction(synonym, entRef) {};
	EvaluatedTable execute() override;
};
