#pragma once
#include <vector>
#include <unordered_map>

#include "QPSCommons.h"
#include "../Exception/SPAException.h"
#include "EvaluatedTable.h"
#include "../PKB/Entity.h"

class Instruction {
//protected:	//-> Use protected if need any shared fields
public:
	virtual EvaluatedTable execute(Instruction* instruction) = 0;
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

	EvaluatedTable execute(Instruction* getAllInstruction) {
		EvaluatedTable evTable;

		return evTable;
	}

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
	RelationshipInstructionType type;
	PqlRelationshipType pqlRelationshipType;
	std::string lhsRefString;
	std::string rhsRefString;

public:

	RelationshipInstruction::RelationshipInstruction(PqlRelationshipType pqlRelationshipType, std::string lhsString, std::string rhsRefString) : pqlRelationshipType(pqlRelationshipType), lhsRefString(lhsRefString), rhsRefString(rhsRefString) {}

	EvaluatedTable execute(Instruction* relationshipInstruction) {
		EvaluatedTable evTable;
		switch (pqlRelationshipType) {
		case PqlRelationshipType::ModifiesS :
			std::vector<StmtIndex> allStmts = Entity::getAllStmts();
			break;
		}
		
		return evTable;
	}

	RelationshipInstructionType getType() {
		return type;
	};

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


