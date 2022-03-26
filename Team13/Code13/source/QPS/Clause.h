#pragma once

#include "Instruction.h"
#include "QPSCommons.h"
#include "WithInstruction.h"
#include "ModifiesInstruction.h"
#include "UsesInstruction.h"

class Clause {
public:
	virtual Instruction* toInstruction() {
		return nullptr;
	}
};

class ParsedRelationship : public Clause {
private:
	PqlRelationshipType relationshipType;
	PqlReference lhsRef;
	PqlReference rhsRef;
public:
	ParsedRelationship(PqlRelationshipType relationshipType,
		PqlReference lhsRef, PqlReference rhsRef);
	PqlRelationshipType getRelationshipType() const;
	PqlReference getLhs() const;
	PqlReference getRhs() const;
	Instruction* toInstruction() const;
};

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
	std::string getSynonym() const;
	PqlPatternType getPatternType() const;
	PqlReference getEntRef() const;
	PqlExpression getExpression() const;
	int getNumOfArgs() const;
};

class ParsedWith : public Clause {
private:
	PqlReference lhs;
	PqlReference rhs;
	EntityType lhsEntity;
	EntityType rhsEntity;
	PqlAttributeType attribType;
public:
	ParsedWith(PqlReference lhs, PqlReference rhs);
	ParsedWith(PqlReference lhs, PqlReference rhs, EntityType lhsEntity, EntityType rhsEntity, PqlAttributeType attribType);
	PqlReference getLhs() const;
	PqlReference getRhs() const;
	PqlAttributeType getAttribType() const;
	Instruction* toInstruction() const;
};
