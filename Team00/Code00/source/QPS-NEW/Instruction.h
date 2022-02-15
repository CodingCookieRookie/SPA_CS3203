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

	/* All handlers to set EvaluatedTable to results from PKB. Uses Pass by Reference on EvTable. */
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

	void handleGetAllStmtByType(EvaluatedTable& evTable, std::string synonym, StatementType stmtType) {
		// TODO: PKB to change return type of getStmtIdxFromType()
		/*std::vector<StmtIndex> results = Entity::getStmtIdxFromType(stmtType);

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
		evTable.setNumRow(results.size());*/
	}



	void handleGetAllVar(EvaluatedTable& evTable, std::string synonym) {
		// TODO: PKB to change getAllVars() to return VarIndex.
		/*std::vector<VarIndex> results = Entity::getAllVars();
		std::vector<int> resultsToInt;
		for (VarIndex result : results) {
			resultsToInt.emplace_back(result.getIndex());
		}

		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(synonym, PqlEntityType::Variable));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[synonym] = resultsToInt;

		evTable.setEntities(PQLentities);
		evTable.setTable(PQLmap);
		evTable.setNumRow(results.size());*/
	}

	void handleGetAllProc(EvaluatedTable& evTable, std::string synonym) {
		// TODO: PKB to change getAllProcs() to return ProcIndex.
		/*std::vector<VarIndex> results = Entity::getAllVars();
		std::vector<int> resultsToInt;
		for (VarIndex result : results) {
			resultsToInt.emplace_back(result.getIndex());
		}

		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(synonym, PqlEntityType::Procedure));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[synonym] = resultsToInt;

		evTable.setEntities(PQLentities);
		evTable.setTable(PQLmap);
		evTable.setNumRow(results.size());*/
	}

	void handleGetAllConst(EvaluatedTable& evTable, std::string synonym) {
		std::vector<int> results = Entity::getAllConsts();

		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(synonym, PqlEntityType::Constant));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[synonym] = results;

		evTable.setEntities(PQLentities);
		evTable.setTable(PQLmap);
		evTable.setNumRow(results.size());
	}

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

	/* Main entry method for executing instruction based on PqlEntityType in Select-cl */
	EvaluatedTable execute() override {
		EvaluatedTable evTable;
		switch (pqlEntityType) {
		case PqlEntityType::Stmt:
			handleGetAllStmt(evTable, synonym);
			break;
		case PqlEntityType::Read:
			handleGetAllStmtByType(evTable, synonym, StatementType::readType);
			break;
		case PqlEntityType::Print:
			handleGetAllStmtByType(evTable, synonym, StatementType::printType);
			break;
		case PqlEntityType::Call:
			handleGetAllStmtByType(evTable, synonym, StatementType::callType);
			break;
		case PqlEntityType::While:
			handleGetAllStmtByType(evTable, synonym, StatementType::whileType);
			break;
		case PqlEntityType::If:
			handleGetAllStmtByType(evTable, synonym, StatementType::ifType);
			break;
		case PqlEntityType::Assign:
			handleGetAllStmtByType(evTable, synonym, StatementType::assignType);
			break;
		case PqlEntityType::Variable:
			handleGetAllVar(evTable, synonym);
			break;
		case PqlEntityType::Constant:
			handleGetAllProc(evTable, synonym);
			break;
		case PqlEntityType::Procedure:
			handleGetAllProc(evTable, synonym);
			break;
		}
		return evTable;
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


