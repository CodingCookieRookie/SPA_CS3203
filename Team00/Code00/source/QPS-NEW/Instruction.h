#pragma once
#include <vector>
#include <unordered_map>

#include <iostream>
#include "QPSCommons.h"
#include "../Exception/SPAException.h"
#include "EvaluatedTable.h"
#include "../PKB/Entity.h"
#include "../PKB/Modifies.h"
#include "../PKB/Uses.h"

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
		std::tuple<std::vector<int>, std::vector<int>> allStmtVarInfos = Modifies::getAllStmtVarInfo();
		std::unordered_map<std::string, std::vector<int>> table;
		//Assumption: stmt's vector<int> is same size as var's vector<int>
		for (size_t i = 0; i < (std::get<0>(allStmtVarInfos).size()); i++) {
			int lhs = std::get<0>(allStmtVarInfos)[i];
			int rhs = std::get<1>(allStmtVarInfos)[i];
			table[lhsRefString].push_back(lhs);
			table[rhsRefString].push_back(rhs);
		}
		return EvaluatedTable(table);
	}

	EvaluatedTable handleUsesS() {
		EvaluatedTable newEvTable;
		std::tuple<std::vector<int>, std::vector<int>>  allStmtVarInfos = Uses::getAllStmtVarInfo();
		std::unordered_map<std::string, std::vector<int>> table;
		for (size_t i = 0; i < (std::get<0>(allStmtVarInfos).size()); i++) {
			int lhs = std::get<0>(allStmtVarInfos)[i];
			int rhs = std::get<1>(allStmtVarInfos)[i];
			table[lhsRefString].push_back(lhs);
			table[rhsRefString].push_back(rhs);
		}
		return EvaluatedTable(table);
	}

public:
	//enum class PqlRelationshipType {
	//	Follows, FollowsT, Parent, ParentT,
	//	UsesS, UsesP, ModifiesS, ModifiesP,
	//	Uses, Modifies
	//};
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
			evTable = handleUsesS();
			break;
		}
		return evTable;
	}

	//RelationshipInstructionType getType() {
	//	return type;
	//};

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
// For reference <To be removed after PatternInstruction done>
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


