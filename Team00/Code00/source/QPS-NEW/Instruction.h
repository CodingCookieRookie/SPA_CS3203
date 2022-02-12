#pragma once
#include <vector>
#include <unordered_map>

#include "QPSCommons.h"
#include "../Exception/SPAException.h"

class Instruction {
//protected:	//-> Use protected if need any shared fields
public:
	Instruction getClassType();
};

class GetAllInstruction : public Instruction{
private:
	GetAllInstructionType type;
	std::vector<std::string> arguments; // has to be generalised

public:

	/* Constructor for an Instruction object */
	GetAllInstruction(GetAllInstructionType type) : type(type) {}

	/* Constructor for an Instruction object */
	GetAllInstruction(GetAllInstructionType type, std::vector<std::string> arguments) : type(type), arguments(arguments) {}

	///* TODO: To generalise. Executes instruction by calling the PKB */
	//void execute(Instruction& instr);

	/* Getter for type */
	GetAllInstructionType getType() {
		return type;
	};

	GetAllInstruction* getClassType() {
		return this;
	}

	/* Getter for arguments */
	std::vector<std::string> getArgs() {
		return arguments;
	};

};

class RelationshipInstruction : public Instruction {
private:
	int relationshipType;
	std::string lhsString;
	std::string rhsRefString;
public:
	RelationshipInstruction::RelationshipInstruction(int relationshipType, std::string lhsString, std::string rhsRefString)
	{
		relationshipType = relationshipType;
		lhsString = lhsString;
		rhsRefString = rhsRefString;
	}


	/*RelationshipInstructionType getType() {
		return 
	};*/

	RelationshipInstruction* getClassType() {
		return this;
	};
};
//class ParsedRelationship {
//private:
//    PqlRelationshipType relationshipType;
//    PqlReference lhsRef;
//    PqlReference rhsRef;
//public:
//    ParsedRelationship(PqlRelationshipType relationshipType,
//        PqlReference lhsRef, PqlReference rhsRef);
//    ParsedRelationship();
//    PqlRelationshipType getRelationshipType();
//    PqlReference getLhs() const;
//    PqlReference getRhs() const;
//};
//
//class ParsedPattern {
//private:
//    std::string synonym;
//    PqlReference entRef;
//    PqlExpression expressionSpec;
//public:
//    ParsedPattern(std::string& synonym,
//        PqlReference entRef, PqlExpression expressionSpec);
//    ParsedPattern();
//    std::string getSynonym() const;
//    PqlReference getEntRef() const;
//    PqlExpression getExpression() const;
//};


