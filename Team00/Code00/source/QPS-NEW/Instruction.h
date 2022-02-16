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

	/* All handlers to set EvaluatedTable to results from PKB. Uses Pass by Reference on EvTable. */
	EvaluatedTable handleGetAllStmt(std::string synonym) {
		std::vector<StmtIndex> results = Entity::getAllStmts();
		
		std::vector<int> resultsToInt;
		for (StmtIndex result : results) {
			resultsToInt.emplace_back(result.getIndex());
		}

		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(synonym, PqlEntityType::Stmt));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[synonym] = resultsToInt;

		return EvaluatedTable(PQLentities, PQLmap);
	}

	EvaluatedTable handleGetAllStmtByType(std::string synonym, StatementType stmtType) {
		std::vector<StmtIndex> results = Entity::getStmtIdxFromType(stmtType);

		std::vector<int> resultsToInt;
		for (StmtIndex result : results) {
			resultsToInt.emplace_back(result.getIndex());
		}

		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(synonym, PqlEntityType::Stmt));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[synonym] = resultsToInt;

		return EvaluatedTable(PQLentities, PQLmap);
	}

	EvaluatedTable handleGetAllVar(std::string synonym) {
		// TODO: PKB to change getAllVars() to return VarIndex.
		std::vector<VarIndex> results = Entity::getAllVars();
		std::vector<int> resultsToInt;
		for (VarIndex result : results) {
			resultsToInt.emplace_back(result.getIndex());
		}

		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(synonym, PqlEntityType::Variable));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[synonym] = resultsToInt;

		return EvaluatedTable(PQLentities, PQLmap);
	}

	EvaluatedTable handleGetAllProc(std::string synonym) {
		// TODO: PKB to change getAllProcs() to return ProcIndex.
		std::vector<ProcIndex> results = Entity::getAllProcs();
		std::vector<int> resultsToInt;
		for (ProcIndex result : results) {
			resultsToInt.emplace_back(result.getIndex());
		}

		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(synonym, PqlEntityType::Procedure));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[synonym] = resultsToInt;

		return EvaluatedTable(PQLentities, PQLmap);
	}

	EvaluatedTable handleGetAllConst(std::string synonym) {
		std::vector<int> results = Entity::getAllConsts();

		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(synonym, PqlEntityType::Constant));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[synonym] = results;

		return EvaluatedTable(PQLentities, PQLmap);
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
			evTable = handleGetAllStmt(synonym);
			break;
		case PqlEntityType::Read:
			evTable = handleGetAllStmtByType(synonym, StatementType::readType);
			break;
		case PqlEntityType::Print:
			evTable = handleGetAllStmtByType(synonym, StatementType::printType);
			break;
		case PqlEntityType::Call:
			evTable = handleGetAllStmtByType(synonym, StatementType::callType);
			break;
		case PqlEntityType::While:
			evTable = handleGetAllStmtByType(synonym, StatementType::whileType);
			break;
		case PqlEntityType::If:
			evTable = handleGetAllStmtByType(synonym, StatementType::ifType);
			break;
		case PqlEntityType::Assign:
			evTable = handleGetAllStmtByType(synonym, StatementType::assignType);
			break;
		case PqlEntityType::Variable:
			evTable = handleGetAllVar(synonym);
			break;
		case PqlEntityType::Constant:
			evTable = handleGetAllProc(synonym);
			break;
		case PqlEntityType::Procedure:
			evTable = handleGetAllProc(synonym);
			break;
		}
		return evTable;
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

	EvaluatedTable handleFollows() {
		EvaluatedTable evTable;
		std::vector<StmtIndex> stmts = Entity::getAllStmts();
		
		// e.g Follows(6, 7)
		if (lhsRef.first == PqlReferenceType::integer && rhsRef.first == PqlReferenceType::integer) {
			
			StmtIndex lhsStmtIndex, rhsStmtIndex;
			for (StmtIndex stmt : stmts) {
				if (stmt.getIndex() == stoi(lhsRef.second)) {
					lhsStmtIndex = stmt;
				}
				if (stmt.getIndex() == stoi(rhsRef.second)) {
					rhsStmtIndex = stmt;
				}
			}
			bool isEmptyTable = Follows::containsSuccessor(lhsStmtIndex, rhsStmtIndex);
			return EvaluatedTable(isEmptyTable); //e.g isEmpty == True, if 6 is followed by 7
			
		}
		// e.g Follows(6, s2)
		else if (lhsRef.first == PqlReferenceType::integer && rhsRef.first != PqlReferenceType::wildcard)
		{
			std::vector<int> results;
			int lhsRefValue = stoi(lhsRef.second); // might throw error if string value can't be converted to int
			StmtIndex lhsStmt = stmts[lhsRefValue - 1];
			for (StmtIndex stmt : stmts) {
				if (Follows::containsSuccessor(lhsStmt, stmt)) {
					results.emplace_back(stmt.getIndex()); // e.g {7} because 6 is followed by 7
				}
			}
			std::unordered_map<std::string, PqlEntityType> PQLentities;
			PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));

			std::unordered_map<std::string, std::vector<int>> PQLmap;
			PQLmap[rhsRef.second] = results;

			return EvaluatedTable(PQLentities, PQLmap);
			
		} 
		// e.g. Follows(s1, 7)
		else if (rhsRef.first == PqlReferenceType::integer && lhsRef.first != PqlReferenceType::wildcard)
		{
			std::vector<int> results;
			int rhsRefValue = stoi(rhsRef.second); //might throw error if string value can't be converted to int
			StmtIndex rhsStmt = stmts[rhsRefValue - 1]; //check if off by 1
			for (StmtIndex stmt : stmts) {
				if (Follows::containsSuccessor(stmt, rhsStmt)) {
					results.emplace_back(stmt.getIndex()); //e.g {3} because 3 is followed by 6
				}
			}
			std::unordered_map<std::string, PqlEntityType> PQLentities;
			PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));

			std::unordered_map<std::string, std::vector<int>> PQLmap;
			PQLmap[lhsRef.second] = results;

			return EvaluatedTable(PQLentities, PQLmap);
		}
		// Follows(s1, s2), Follows(s1, _), Follows(_, s2)
		else if (lhsRef.first != PqlReferenceType::wildcard && rhsRef.first != PqlReferenceType::wildcard) {
			//Assumption: Different synonym names (i.e. Follows(s1, s2), not Follows(s1, s1))
			std::tuple<std::vector<int>, std::vector<int>> results = Follows::getAllPredecessorSuccessorInfo();
			//e.g. {1, 2}, {2, 3}, {3, 6}
			std::unordered_map<std::string, PqlEntityType> PQLentities;
			std::unordered_map<std::string, std::vector<int>> PQLmap;

			if (lhsRef.first == PqlReferenceType::synonym) {
				PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
				PQLmap[lhsRef.second] = std::get<0>(results); // if RHS is wildcard, LHS may have duplicate values
			}
			if (lhsRef.first == PqlReferenceType::synonym) {
				PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));
				PQLmap[rhsRef.second] = std::get<1>(results); // if LHS is wildcard, RHS may have duplicate values
			}
			return EvaluatedTable(PQLentities, PQLmap);
		}
		// Follows(_, _), or Follows(6, _), or Follows(_, 7)
		else {
			bool isEmptyTable = true;
			if (lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard) { // Follows(_, _)
				isEmptyTable = std::get<0>(Follows::getAllPredecessorSuccessorInfo()).empty();
			}
			StmtIndex lhsStmtIndex, rhsStmtIndex;
			if (lhsRef.first == PqlReferenceType::integer) { // e.g. Follows(6, _)
				for (StmtIndex stmt : stmts) {
					if (stmt.getIndex() == stoi(lhsRef.second)) {
						lhsStmtIndex = stmt;
						return EvaluatedTable(Follows::getSuccessorStmts(lhsStmtIndex).empty()); // False == Follows exists
					}
				}
			}
			else { // e.g. Follows(_, 7)
				for (StmtIndex stmt : stmts) {
					if (stmt.getIndex() == stoi(rhsRef.second)) {
						rhsStmtIndex = stmt;
						return EvaluatedTable(Follows::getSuccessorStmts(rhsStmtIndex).empty());
					}
				}
			}
			return EvaluatedTable(isEmptyTable);
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
			evTable = handleFollows();
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


