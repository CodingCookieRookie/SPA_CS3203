#pragma once

#include "Instruction.h"

class PatternInstruction : public Instruction {
protected:
	std::string synonym;
	PqlReference entRef;
	PqlExpression expressionSpec;
	PKBGetter* pkbGetter;
	PatternInstruction(std::string synonym, PqlReference entRef, PKBGetter* pkbGetter);
	PatternInstruction(std::string synonym, PqlReference entRef, PqlExpression expressionSpec, PKBGetter* pkbGetter);
	std::unordered_set<std::string> getSynonyms() override;
};

class PatternAInstruction : public PatternInstruction {
public:
	PatternAInstruction(std::string synonym, PqlReference entRef, PqlExpression expressionSpec, PKBGetter* pkbGetter) :
		PatternInstruction(synonym, entRef, expressionSpec, pkbGetter) {};
	EvaluatedTable execute() override;
};

class PatternIInstruction : public PatternInstruction {
public:
	PatternIInstruction(std::string synonym, PqlReference entRef, PKBGetter* pkbGetter) :
		PatternInstruction(synonym, entRef, pkbGetter) {};
	EvaluatedTable execute() override;
};

class PatternWInstruction : public PatternInstruction {
public:
	PatternWInstruction(std::string synonym, PqlReference entRef, PKBGetter* pkbGetter) :
		PatternInstruction(synonym, entRef, pkbGetter) {};
	EvaluatedTable execute() override;
};
