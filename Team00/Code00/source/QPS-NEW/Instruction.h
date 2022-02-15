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
#include "../PKB/Follows.h"

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

	/* Main entry method for executing instruction based on PqlEntityType in Select-cl */
	EvaluatedTable execute() override {
		EvaluatedTable evTable;
		switch (pqlEntityType) {
		case PqlEntityType::Stmt:
			handleGetAllStmt(evTable, synonym);
			break;
		case PqlEntityType::Print:
			handleGetAllPrint(evTable, synonym);
			break;
		case PqlEntityType::Call:
			// TODO: PKB to add getAllCalls().
			break;
		case PqlEntityType::While:
			// TODO: PKB to add.
			break;
		case PqlEntityType::If:
			// TODO: PKB to add.
			break;
		case PqlEntityType::Assign:
			// TODO: PKB to add.
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

	void handleGetAllPrint(EvaluatedTable& evTable, std::string synonym) {
		// TODO: PKB to add getAllPrints().
		/*std::vector<int> results = Entity::getAllPrints();

		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(synonym, PqlEntityType::Print));

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
};

class RelationshipInstruction : public Instruction {
private:
	//RelationshipInstructionType type;
	PqlRelationshipType pqlRelationshipType;
	PqlReference lhsRef;
	PqlReference rhsRef;

	/*EvaluatedTable handleModifiesS() {
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
	}*/

	void handleFollows(EvaluatedTable& evTable) {

		
		if (lhsRef.first == PqlReferenceType::synonym || lhsRef.first == PqlReferenceType::wildcard) {
			std::vector<StmtIndex> stmts = Entity::getAllStmts();

			if (rhsRef.first == PqlReferenceType::integer) {
				std::vector<int> results;
				int rhsRefValue = stoi(rhsRef.second); //might throw error if string value can't be converted to int
				StmtIndex rhsStmt = stmts[rhsRefValue - 1];
				for (StmtIndex stmt : stmts) {
					if (Follows::containsSuccessor(stmt, rhsStmt)) {
						results.emplace_back(stmt.getIndex());
					}
				}
				std::unordered_map<std::string, PqlEntityType> PQLentities;
				PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));

				std::unordered_map<std::string, std::vector<int>> PQLmap;
				PQLmap[lhsRef.second] = results;

				evTable.setEntities(PQLentities);
				evTable.setTable(PQLmap);
				evTable.setNumRow(results.size());
			}
			else { //rhsRef is either a synonym or a wildcard

			}


		}
		else { //lhsRef is an integer
			if (rhsRef.first == PqlReferenceType::integer) { 

			}
			else { //rhsRef is either a synonym or a wildcard

			}

		}
	}

public:
	//enum class PqlRelationshipType {
	//	Follows, FollowsT, Parent, ParentT,
	//	UsesS, UsesP, ModifiesS, ModifiesP,
	//	Uses, Modifies
	//};
	RelationshipInstruction(PqlRelationshipType pqlRSType, PqlReference lhs, PqlReference rhs) :
		pqlRelationshipType(pqlRSType),lhsRef(lhs), rhsRef(rhs) {}


	EvaluatedTable execute() override {
		EvaluatedTable evTable;
		switch (pqlRelationshipType) {
		/*case PqlRelationshipType::ModifiesS :
			evTable = handleModifiesS();
			break;
		case PqlRelationshipType::UsesS:
			evTable = handleUsesS();
			break;*/
		case PqlRelationshipType::Follows:
			handleFollows(evTable);
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


