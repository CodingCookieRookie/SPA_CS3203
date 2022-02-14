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
	PqlEntityType pqlEntityType;
	std::string synonym;

public:
	/* Constructor for a GetAllInstruction object */
	GetAllInstruction(PqlEntityType type, std::string synonym) : pqlEntityType(type), synonym(synonym) {}

	/* Getter for type */
	PqlEntityType getType() {
		return pqlEntityType;
	};

	/* Getter for synonym */
	std::string getSynonym() {
		return synonym;
	};

	EvaluatedTable execute() override {
		EvaluatedTable evTable;
		switch (pqlEntityType) {
		case PqlEntityType::Stmt:

			handleGetAllStmt(evTable, synonym);
			break;
		case PqlEntityType::Print:

			break;
		case PqlEntityType::Call:

			break;
		case PqlEntityType::While:

			break;
		case PqlEntityType::If:

			break;
		case PqlEntityType::Assign:

			break;
		case PqlEntityType::Variable:

			break;
		case PqlEntityType::Constant:

			break;
		case PqlEntityType::Procedure:

			break;
		}
		return evTable;
	}

	void handleGetAllStmt(EvaluatedTable& evTable, std::string synonym) {
		std::vector<StmtIndex> results = Entity::getAllStmts();
		std::vector<int> resultsToInt;
		for (StmtIndex result : results) {
			resultsToInt.emplace_back(result.getIndex());
		}

		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(synonym, PqlEntityType::Stmt));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[synonym] = resultsToInt;

		evTable.setEntities(PQLentities);
		evTable.setTable(PQLmap);
		evTable.setNumRow(results.size());
	}
};

class RelationshipInstruction : public Instruction {
private:
	//RelationshipInstructionType type;
	PqlRelationshipType pqlRelationshipType;
	std::string lhsRefString;
	std::string rhsRefString;

	EvaluatedTable handleModifiesS() {
		EvaluatedTable newEvTable;
		std::vector<std::tuple<StmtIndex, VarIndex>> allStmtVarInfos = Modifies::getAllStmtVarInfo();
		std::unordered_map<std::string, std::vector<int>> table;
		for (int i = 0; i < allStmtVarInfos.size(); i++) {
			std::tuple<StmtIndex, VarIndex> pair = allStmtVarInfos.at(i);
			table[lhsRefString].push_back(std::get<0>(pair).getIndex());
			table[rhsRefString].push_back(std::get<1>(pair).index);
		}
		return EvaluatedTable(table);
	}

	EvaluatedTable handleUsesS() {
		EvaluatedTable newEvTable;
		std::vector<std::tuple<StmtIndex, VarIndex>> allStmtVarInfos = Uses::getAllStmtVarInfo();
		std::unordered_map<std::string, std::vector<int>> table;
		for (int i = 0; i < allStmtVarInfos.size(); i++) {
			std::tuple<StmtIndex, VarIndex> pair = allStmtVarInfos.at(i);
			table[lhsRefString].push_back(std::get<0>(pair).getIndex());
			table[rhsRefString].push_back(std::get<1>(pair).index);
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


