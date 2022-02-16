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

class GetAllInstruction : public Instruction {
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

		return EvaluatedTable(PQLentities, PQLmap, results.size());
	}

	EvaluatedTable handleGetAllStmtByType(std::string synonym, StatementType stmtType) {
		// TODO: PKB to change return type of getStmtIdxFromType()
		std::vector<StmtIndex> results = Entity::getStmtIdxFromType(stmtType);

		std::vector<int> resultsToInt;
		for (StmtIndex result : results) {
			resultsToInt.emplace_back(result.getIndex());
		}

		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(synonym, PqlEntityType::Stmt));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[synonym] = resultsToInt;

		return EvaluatedTable(PQLentities, PQLmap, results.size());
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

		return EvaluatedTable(PQLentities, PQLmap, results.size());
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

		return EvaluatedTable(PQLentities, PQLmap, results.size());
	}

	EvaluatedTable handleGetAllConst(std::string synonym) {
		std::vector<int> results = Entity::getAllConsts();

		std::unordered_map<std::string, PqlEntityType> PQLentities;
		PQLentities.insert(std::pair(synonym, PqlEntityType::Constant));

		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLmap[synonym] = results;

		return EvaluatedTable(PQLentities, PQLmap, results.size());
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
	//enum class PqlReferenceType {
	//	synonym, wildcard, integer, ident
	//};

private:
	//RelationshipInstructionType type;
	PqlRelationshipType pqlRelationshipType;
	PqlReference lhsRef;
	PqlReference rhsRef;

	EvaluatedTable handleModifiesS() {
		// Modifies (a/r/s/a1, v) or Modifies (a/r/s/a1, _ )
		// Modifies (p/p1, v)	or Modifies (p/p1, _ )	proc
		// Modifies (1, v)	=> true or Modifies (1, _ ) (under statement)
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
		PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Variable));
		std::tuple<std::vector<int>, std::vector<int>> allStmtVarInfos = Modifies::getAllStmtVarInfo();
		if (lhsRef.first == PqlReferenceType::ident) {
			for (size_t i = 0; i < (std::get<0>(allStmtVarInfos).size()); i++) {
				int lhs = std::get<0>(allStmtVarInfos)[i];
				PQLmap[lhsRef.second].push_back(lhs);
				if (rhsRef.first != PqlReferenceType::wildcard) {
					int rhs = std::get<1>(allStmtVarInfos)[i];
					PQLmap[rhsRef.second].push_back(rhs);
				}
			}
		}
		else if (lhsRef.first == PqlReferenceType::integer) {
			int lhsRefValue = stoi(lhsRef.second);
			StmtIndex stmtIndex = { lhsRefValue };
			VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
			return EvaluatedTable(Modifies::contains(stmtIndex, varIndex));
		}
		else {
			std::cout << "Error in handleModifiesS\n";
		}
		return EvaluatedTable(PQLentities, PQLmap);
	}

	EvaluatedTable handleModifiesP() {
		// Modifies (a/r/s/a1, v) or Modifies (a/r/s/a1, _ )
		// Modifies (p/p1, v)	or Modifies (p/p1, _ )	proc
		// Modifies (1, v)	=> true or Modifies (1, _ ) (under statement)
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Procedure));
		PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Variable));
		std::tuple<std::vector<int>, std::vector<int>> allProcVarInfos = Modifies::getAllProcVarInfo();
		if (lhsRef.first == PqlReferenceType::ident) {
			for (size_t i = 0; i < (std::get<0>(allProcVarInfos).size()); i++) {
				int lhs = std::get<0>(allProcVarInfos)[i];
				PQLmap[lhsRef.second].push_back(lhs);
				if (rhsRef.first != PqlReferenceType::wildcard) {
					int rhs = std::get<1>(allProcVarInfos)[i];
					PQLmap[rhsRef.second].push_back(rhs);
				}
			}
		}
		else {
			std::cout << "Error in handleModifiesP\n";
		}
		return EvaluatedTable(PQLentities, PQLmap);
	}

	EvaluatedTable handleUsesS() {
		// Uses (a/r/s/a1, v) or Uses (a/r/s/a1, _ )
		// Uses (p/p1, v)	or Uses (p/p1, _ )	proc
		// Uses (1, v)	=> true or Uses (1, _ ) (under statement)
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
		PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Variable));
		std::tuple<std::vector<int>, std::vector<int>>  allStmtVarInfos = Uses::getAllStmtVarInfo();
		if (lhsRef.first == PqlReferenceType::ident) {
			for (size_t i = 0; i < (std::get<0>(allStmtVarInfos).size()); i++) {
				int lhs = std::get<0>(allStmtVarInfos)[i];
				PQLmap[lhsRef.second].push_back(lhs);
				if (rhsRef.first != PqlReferenceType::wildcard) {
					int rhs = std::get<1>(allStmtVarInfos)[i];
					PQLmap[rhsRef.second].push_back(rhs);
				}
			}
		}
		else if (lhsRef.first == PqlReferenceType::integer) {
			int lhsRefValue = stoi(lhsRef.second);
			StmtIndex stmtIndex = { lhsRefValue };
			VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
			return EvaluatedTable(Modifies::contains(stmtIndex, varIndex));
		}
		else {
			std::cout << "Error in handleModifiesS\n";
		}
		return EvaluatedTable(PQLentities, PQLmap);
	}

	EvaluatedTable handleUsesP() {
		// Uses (a/r/s/a1, v) or Uses (a/r/s/a1, _ )
		// Uses (p/p1, v)	or Uses (p/p1, _ )	proc
		// Uses (1, v)	=> true or Uses (1, _ ) (under statement)
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
		PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Variable));
		std::tuple<std::vector<int>, std::vector<int>>  allStmtVarInfos = Uses::getAllStmtVarInfo();
		if (lhsRef.first == PqlReferenceType::ident) {
			for (size_t i = 0; i < (std::get<0>(allStmtVarInfos).size()); i++) {
				int lhs = std::get<0>(allStmtVarInfos)[i];
				PQLmap[lhsRef.second].push_back(lhs);
				if (rhsRef.first != PqlReferenceType::wildcard) {
					int rhs = std::get<1>(allStmtVarInfos)[i];
					PQLmap[rhsRef.second].push_back(rhs);
				}
			}
		}
		else {
			std::cout << "Error in handleModifiesP\n";
		}
		return EvaluatedTable(PQLentities, PQLmap);
	}

	EvaluatedTable handleFollows() {
		EvaluatedTable evTable;
		// Follows(s1, ?), or Follows(_, ?)
		if (lhsRef.first == PqlReferenceType::synonym || lhsRef.first == PqlReferenceType::wildcard) {
			std::vector<StmtIndex> stmts = Entity::getAllStmts();

			// e.g. Follows(s1, 6), or Follows(_, 6)
			if (rhsRef.first == PqlReferenceType::integer) { 
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

				return EvaluatedTable(PQLentities, PQLmap, results.size());
			}
			// rhsRef is either a synonym or a wildcard (i.e. Follows(s1, s2), Follows(s1, _), Follows(_, s2), Follows(_, _))
			else { 
				//Assumption: Different synonym names (i.e. Follows(s1, s2), not Follows(s1, s1))
				std::tuple<std::vector<int>, std::vector<int>> results = Follows::getAllPredecessorSuccessorInfo();
			    //e.g. {1, 2}, {2, 3}, {3, 6}
				std::unordered_map<std::string, PqlEntityType> PQLentities;
				PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
				PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));

				std::unordered_map<std::string, std::vector<int>> PQLmap;
				PQLmap[lhsRef.second] = std::get<0>(results);
				PQLmap[rhsRef.second] = std::get<1>(results);

				return EvaluatedTable(PQLentities, PQLmap, std::get<0>(results).size());
			}
		}

		else { //lhsRef is an integer (e.g. Follows(6, ?))
			std::vector<StmtIndex> stmts = Entity::getAllStmts();

			// e.g. Follows(6, 7)
			if (rhsRef.first == PqlReferenceType::integer) { 
				StmtIndex lhsStmtIndex, rhsStmtIndex;
				for (StmtIndex stmt : stmts) {
					if (stmt.getIndex() == stoi(lhsRef.second)) {
						lhsStmtIndex = stmt;
					}
					if (stmt.getIndex() == stoi(rhsRef.second)) {
						rhsStmtIndex = stmt;
					}
				} 
				return EvaluatedTable(Follows::containsSuccessor(lhsStmtIndex, rhsStmtIndex));
				//e.g True, if 6 is follwed by 7
			}

			//rhsRef is either a synonym or a wildcard (e.g. Follows(6, s2), or Follows(6, _))
			else { 
				std::vector<int> results;
				int lhsRefValue = stoi(lhsRef.second); //might throw error if string value can't be converted to int
				StmtIndex lhsStmt = stmts[lhsRefValue - 1];
				for (StmtIndex stmt : stmts) {
					if (Follows::containsSuccessor(lhsStmt, stmt)) {
						results.emplace_back(stmt.getIndex()); //e.g {7} because 6 is followed by 7
					}
				}
				std::unordered_map<std::string, PqlEntityType> PQLentities;
				PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));

				std::unordered_map<std::string, std::vector<int>> PQLmap;
				PQLmap[rhsRef.second] = results;

				return EvaluatedTable(PQLentities, PQLmap, results.size());
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
		case PqlRelationshipType::ModifiesS :
			evTable = handleModifiesS();
			break;
		case PqlRelationshipType::ModifiesP:
			evTable = handleModifiesP();
			break;
		case PqlRelationshipType::UsesS:
			evTable = handleUsesS();
			break;
		case PqlRelationshipType::UsesP:
			evTable = handleUsesP();
			break;
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


