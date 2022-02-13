#pragma once
#include <vector>
#include <unordered_map>

#include <iostream>
#include "QPSCommons.h"
#include "../Exception/SPAException.h"
#include "EvaluatedTable.h"
#include "../PKB/Entity.h"
#include "../PKB/Modifies.h"

class Instruction {
//protected:	//-> Use protected if need any shared fields
public:
	virtual EvaluatedTable execute() = 0;
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

	EvaluatedTable execute() override {
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
	//RelationshipInstructionType type;
	PqlRelationshipType pqlRelationshipType;
	std::string lhsRefString;
	std::string rhsRefString;

	EvaluatedTable handleModifiesS() {
		EvaluatedTable newEvTable;
		VarIndex varIndex = Entity::getVarIdx(rhsRefString);
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> allModifiesStmts = Modifies::getStatements(varIndex);	// (s,v) -> ({1 1 2}, {3, 4, 4}) 
		std::vector<std::tuple<StmtIndex, VarIndex>> allStmtVarInfos = Modifies::getAllStmtVarInfo();
		std::cout << "handled ModifiesS\n";
		std::unordered_map<std::string, std::vector<int>> table;
		for (int i = 0; i < allStmtVarInfos.size(); i++) {
			std::tuple<StmtIndex, VarIndex> pair = allStmtVarInfos.at(i);
			table[lhsRefString].push_back(std::get<0>(pair).getIndex());
			table[rhsRefString].push_back(std::get<1>(pair).index);
			std::cout << "test\n";
			std::cout << std::get<0>(pair).getIndex() << "\n";
			std::cout << std::get<1>(pair).index << "\n";
		}
		return EvaluatedTable(table);
	}

public:
	//enum class PqlRelationshipType {
	//	Follows, FollowsT, Parent, ParentT,
	//	UsesS, UsesP, ModifiesS, ModifiesP,
	//	Uses, Modifies
	//};
	//RelationshipInstruction::RelationshipInstruction(PqlRelationshipType pqlRelationshipType, std::string lhsString, std::string rhsRefString) : pqlRelationshipType(pqlRelationshipType), lhsRefString(lhsRefString), rhsRefString(rhsRefString) {}
	RelationshipInstruction(PqlRelationshipType pqlRSType, std::string lhsString, std::string rhsString) {
		pqlRelationshipType = pqlRSType;
		lhsRefString = lhsString;
		rhsRefString = rhsString;
	}

	EvaluatedTable execute() override {
		EvaluatedTable evTable;
		switch (pqlRelationshipType) {
		case PqlRelationshipType::ModifiesS :
			evTable = handleModifiesS();
			break;
		case PqlRelationshipType::UsesS:

			break;
		}
		return evTable;
	}

	//RelationshipInstructionType getType() {
	//	return type;
	//};

	RelationshipInstruction* getClassType() {
		return this;
	};
};

class PatternInstruction : public Instruction {
private:
	std::string synonym;
    PqlReference entRef;
    PqlExpression expressionSpec;

public:

	PatternInstruction::PatternInstruction(std::string synonym, PqlReference entRef, PqlExpression expressionSpec) : synonym(synonym), entRef(entRef), expressionSpec(expressionSpec) {}

	EvaluatedTable execute(Instruction* relationshipInstruction) {
		EvaluatedTable evTable;
		return evTable;
	}
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


