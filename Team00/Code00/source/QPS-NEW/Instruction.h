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
#include "../PKB/FollowsT.h"
#include "../PKB/Pattern.h"
#include "../PKB/Parent.h"
#include "../PKB/ParentT.h"
#include "../PKB/Pattern.h"
#include "../PKB/ExpressionProcessor.h"

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
	//enum class PqlReferenceType {
	//	synonym, wildcard, integer, ident
	//};

private:
	//RelationshipInstructionType type;
	PqlRelationshipType pqlRelationshipType;
	PqlReference lhsRef;
	PqlReference rhsRef;

	EvaluatedTable handleModifiesS() {
		// Modifies (a/r/s/a1, v) or Modifies(a/r/s/a1, "x") or Modifies (a/r/s/a1, _ ) 
		// Modifies (1, v)	=> true or Modifies (1, _ ) (under statement)
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
		PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Variable));
		std::tuple<std::vector<int>, std::vector<int>> allStmtVarInfos = Modifies::getAllStmtVarInfo();
		std::vector<int> allStmts;
		std::vector<int> varIndices;
		if (lhsRef.first == PqlReferenceType::synonym) {
			if (rhsRef.first == PqlReferenceType::ident) {
				VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
				allStmts = Modifies::getStatements(varIndex);
			}
			else {
				allStmts = std::get<0>(allStmtVarInfos);
			}
			PQLmap[lhsRef.second] = allStmts;
			if (rhsRef.first != PqlReferenceType::wildcard && rhsRef.first != PqlReferenceType::ident) {
				varIndices = std::get<1>(allStmtVarInfos);
				PQLmap[rhsRef.second] = varIndices;
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
		// Modifies (p/p1, v)	or Modifies (p/p1, _ )	Modifies (p/p1, "x" )
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Procedure));
		PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Variable));
		std::tuple<std::vector<int>, std::vector<int>> allProcVarInfos = Modifies::getAllProcVarInfo();
		std::vector<int> allStmts;
		std::vector<int> varIndices;
		if (lhsRef.first == PqlReferenceType::synonym) {
			if (rhsRef.first == PqlReferenceType::ident) {
				VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
				allStmts = Modifies::getStatements(varIndex);
			}
			else {
				allStmts = std::get<0>(allProcVarInfos);
			}
			PQLmap[lhsRef.second] = allStmts;
			if (rhsRef.first != PqlReferenceType::wildcard && rhsRef.first != PqlReferenceType::ident) {
				varIndices = std::get<1>(allProcVarInfos);
				PQLmap[rhsRef.second] = varIndices;
			}
		}
		else {
			std::cout << "Error in handleModifiesP\n";
		}
		return EvaluatedTable(PQLentities, PQLmap);
	}

	EvaluatedTable handleUsesS() {
		// Uses (a/r/s/a1, v) or Uses(a/r/s/a1, "x") or Uses (a/r/s/a1, _ )
		// Uses (1, v)	=> true or Uses (1, _ ) (under statement)
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
		PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Variable));
		std::tuple<std::vector<int>, std::vector<int>>  allStmtVarInfos = Uses::getAllStmtVarInfo();
		std::vector<int> allStmts;
		std::vector<int> varIndices;
		if (lhsRef.first == PqlReferenceType::synonym) {
			if (rhsRef.first == PqlReferenceType::ident) {
				VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
				allStmts = Uses::getStatements(varIndex);
			}
			else {
				allStmts = std::get<0>(allStmtVarInfos);
			}
			PQLmap[lhsRef.second] = allStmts;
			if (rhsRef.first != PqlReferenceType::wildcard && rhsRef.first != PqlReferenceType::ident) {
				varIndices = std::get<1>(allStmtVarInfos);
				PQLmap[rhsRef.second] = varIndices;
			}
		}
		else if (lhsRef.first == PqlReferenceType::integer) {
			int lhsRefValue = stoi(lhsRef.second);
			StmtIndex stmtIndex = { lhsRefValue };
			VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
			return EvaluatedTable(Uses::contains(stmtIndex, varIndex));
		}
		else {
			std::cout << "Error in handleModifiesS\n";
		}
		return EvaluatedTable(PQLentities, PQLmap);
	}

	EvaluatedTable handleUsesP() {
		// Uses (p/p1, v) or Uses (p/p1, "x") or Uses (p/p1, _ )	proc
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
		PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Variable));
		std::tuple<std::vector<int>, std::vector<int>>  allProcVarInfos = Uses::getAllProcVarInfo();
		std::vector<int> allStmts;
		std::vector<int> varIndices;
		if (lhsRef.first == PqlReferenceType::synonym) {
			if (rhsRef.first == PqlReferenceType::ident) {
				VarIndex varIndex = Entity::getVarIdx(rhsRef.second);
				allStmts = Uses::getStatements(varIndex);
			}
			else {
				allStmts = std::get<0>(allProcVarInfos);
			}
			PQLmap[lhsRef.second] = allStmts;
			if (rhsRef.first != PqlReferenceType::wildcard && rhsRef.first != PqlReferenceType::ident) {
				varIndices = std::get<1>(allProcVarInfos);
				PQLmap[rhsRef.second] = varIndices;
			}
		}
		else {
			std::cout << "Error in handleModifiesP\n";
		}
		return EvaluatedTable(PQLentities, PQLmap);
	}

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
			bool evResult = Follows::containsSuccessor(lhsStmtIndex, rhsStmtIndex);
			return EvaluatedTable(evResult); //e.g evResult == true, if 6 is followed by 7
			
		}
		// e.g Follows(6, s2), Follows(6, _)
		else if (lhsRef.first == PqlReferenceType::integer)
		{
			std::vector<int> results;
			int lhsRefValue = stoi(lhsRef.second); // might throw error if string value can't be converted to int
			StmtIndex lhsStmt = StmtIndex(lhsRefValue);
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
		// e.g. Follows(s1, 7), Follows(_, 7)
		else if (rhsRef.first == PqlReferenceType::integer)
		{
			std::vector<int> results;
			int rhsRefValue = stoi(rhsRef.second); //might throw error if string value can't be converted to int
			StmtIndex rhsStmt = StmtIndex(rhsRefValue);
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
		else if (!(lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard)) {
			//Assumption: Different synonym names (i.e. Follows(s1, s2), not Follows(s1, s1))
			std::tuple<std::vector<int>, std::vector<int>> results = Follows::getAllPredecessorSuccessorInfo();
			//e.g. {1, 2}, {2, 3}, {3, 6}
			std::unordered_map<std::string, PqlEntityType> PQLentities;
			std::unordered_map<std::string, std::vector<int>> PQLmap;

			if (lhsRef.first == PqlReferenceType::synonym) {
				PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
				PQLmap[lhsRef.second] = std::get<0>(results); // if RHS is wildcard, LHS may have duplicate values
			}
			if (rhsRef.first == PqlReferenceType::synonym) {
				PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));
				PQLmap[rhsRef.second] = std::get<1>(results); // if LHS is wildcard, RHS may have duplicate values
			}
			return EvaluatedTable(PQLentities, PQLmap);
		}
		// Follows(_, _)
		else {
			bool isEmptyTable = true;
			if (lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard) { // Follows(_, _)
				isEmptyTable = std::get<0>(Follows::getAllPredecessorSuccessorInfo()).empty();
			}
			// No Follows rs exists => isEmptyTable == true => EvTable.evResult == false (innerJoinMerge() can drop table)
			// Follows rs exists => isEmptyTable == false => EvTable.evResult == true (innerJoinMerge() can merge dummy table, preserving all rows)
			return EvaluatedTable(!isEmptyTable);
		}
	}

	EvaluatedTable handleFollowsT() {
		EvaluatedTable evTable;
		std::vector<StmtIndex> stmts = Entity::getAllStmts();

		// e.g Follows*(6, 7)
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
			bool evResult = FollowsT::containsSuccessor(lhsStmtIndex, rhsStmtIndex);
			return EvaluatedTable(evResult); //e.g evResult == true, if 6 is followed* by 7

		}
		// e.g Follows*(6, s2), Follows*(6, _)
		else if (lhsRef.first == PqlReferenceType::integer)
		{
			std::vector<int> results;
			int lhsRefValue = stoi(lhsRef.second); // might throw error if string value can't be converted to int
			StmtIndex lhsStmt = StmtIndex(lhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (FollowsT::containsSuccessor(lhsStmt, stmt)) {
					results.emplace_back(stmt.getIndex()); // e.g {7} because 6 is followed by 7
				}
			}
			std::unordered_map<std::string, PqlEntityType> PQLentities;
			PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));

			std::unordered_map<std::string, std::vector<int>> PQLmap;
			PQLmap[rhsRef.second] = results;

			return EvaluatedTable(PQLentities, PQLmap);

		}
		// e.g. Follows*(s1, 7), Follows*(_, 7)
		else if (rhsRef.first == PqlReferenceType::integer)
		{
			std::vector<int> results;
			int rhsRefValue = stoi(rhsRef.second); //might throw error if string value can't be converted to int
			StmtIndex rhsStmt = StmtIndex(rhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (FollowsT::containsSuccessor(stmt, rhsStmt)) {
					results.emplace_back(stmt.getIndex()); //e.g {3} because 3 is followed by 6
				}
			}
			std::unordered_map<std::string, PqlEntityType> PQLentities;
			PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));

			std::unordered_map<std::string, std::vector<int>> PQLmap;
			PQLmap[lhsRef.second] = results;

			return EvaluatedTable(PQLentities, PQLmap);
		}
		// Follows*(s1, s2), Follows*(s1, _), Follows*(_, s2)
		else if (!(lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard)) {
			//Assumption: Different synonym names (i.e. Follows(s1, s2), not Follows(s1, s1))
			std::tuple<std::vector<int>, std::vector<int>> results = FollowsT::getAllPredecessorSuccessorInfo();
			//e.g. {1, 2}, {2, 3}, {3, 6}
			std::unordered_map<std::string, PqlEntityType> PQLentities;
			std::unordered_map<std::string, std::vector<int>> PQLmap;

			if (lhsRef.first == PqlReferenceType::synonym) {
				PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
				PQLmap[lhsRef.second] = std::get<0>(results); // if RHS is wildcard, LHS may have duplicate values
			}
			if (rhsRef.first == PqlReferenceType::synonym) {
				PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));
				PQLmap[rhsRef.second] = std::get<1>(results); // if LHS is wildcard, RHS may have duplicate values
			}
			return EvaluatedTable(PQLentities, PQLmap);
		}
		// Follows*(_, _)
		else {
			bool isEmptyTable = true;
			if (lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard) { // Follows(_, _)
				isEmptyTable = std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).empty();
			}
			// No Follows rs exists => isEmptyTable == true => EvTable.evResult == false (innerJoinMerge() can drop table)
			// Follows rs exists => isEmptyTable == false => EvTable.evResult == true (innerJoinMerge() can merge dummy table, preserving all rows)
			return EvaluatedTable(!isEmptyTable);
		}
	}

	EvaluatedTable handleParent() {
		EvaluatedTable evTable;
		std::vector<StmtIndex> stmts = Entity::getAllStmts();

		// e.g Parent(6, 7)
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
			bool evResult = Parent::containsPredecessor(lhsStmtIndex, rhsStmtIndex);
			return EvaluatedTable(evResult); //e.g evResult == true, if 6 is a parent of 7

		}
		// e.g Parent(6, s2), Parent(6, _)
		else if (lhsRef.first == PqlReferenceType::integer)
		{
			std::vector<int> results;
			int lhsRefValue = stoi(lhsRef.second); // might throw error if string value can't be converted to int
			StmtIndex lhsStmt = StmtIndex(lhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (Parent::containsSuccessor(lhsStmt, stmt)) {
					results.emplace_back(stmt.getIndex()); // e.g {6} because 6 is a parent of 7
				}
			}
			std::unordered_map<std::string, PqlEntityType> PQLentities;
			PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));

			std::unordered_map<std::string, std::vector<int>> PQLmap;
			PQLmap[rhsRef.second] = results;

			return EvaluatedTable(PQLentities, PQLmap);

		}
		// e.g. Parent(s1, 7), Parent(_ 7)
		else if (rhsRef.first == PqlReferenceType::integer)
		{
			std::vector<int> results;
			std::string value = rhsRef.second;
			int rhsRefValue = std::stoi(value); //might throw error if string value can't be converted to int
			StmtIndex rhsStmt = StmtIndex(rhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (Parent::containsSuccessor(stmt, rhsStmt)) {
					results.emplace_back(stmt.getIndex()); //e.g {6} because 6 is a parent of 7
				}
			}
			std::unordered_map<std::string, PqlEntityType> PQLentities;
			PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));

			std::unordered_map<std::string, std::vector<int>> PQLmap;
			PQLmap[lhsRef.second] = results;

			return EvaluatedTable(PQLentities, PQLmap);
		}
		// Parent(s1, s2), Parent(s1, _), Parent(_, s2)
		else if (!( lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard)) {
			//Assumption: Different synonym names (i.e. Parent(s1, s2), not Parent(s1, s1))
			std::tuple<std::vector<int>, std::vector<int>> results = Parent::getAllPredecessorSuccessorInfo();
			//e.g. {1, 2}, {2, 3}, {3, 6}
			std::unordered_map<std::string, PqlEntityType> PQLentities;
			std::unordered_map<std::string, std::vector<int>> PQLmap;

			if (lhsRef.first == PqlReferenceType::synonym) {
				PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
				PQLmap[lhsRef.second] = std::get<0>(results); // if RHS is wildcard, LHS may have duplicate values
			}
			if (rhsRef.first == PqlReferenceType::synonym) {
				PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));
				PQLmap[rhsRef.second] = std::get<1>(results); // if LHS is wildcard, RHS may have duplicate values
			}
			return EvaluatedTable(PQLentities, PQLmap);
		}
		// Parent(_, _)
		else {
			bool isEmptyTable = true;
			if (lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard) {
				isEmptyTable = std::get<0>(Parent::getAllPredecessorSuccessorInfo()).empty();
			}
		    // No Parent rs exists => isEmptyTable == true => EvTable.evResult == false (innerJoinMerge() can drop table)
			// Parent rs exists => isEmptyTable == false => EvTable.evResult == true (innerJoinMerge() can merge dummy table, preserving all rows)
			return EvaluatedTable(!isEmptyTable);
		}
	}

	EvaluatedTable handleParentT() {
		EvaluatedTable evTable;
		std::vector<StmtIndex> stmts = Entity::getAllStmts();

		// e.g Parent*(6, 7)
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
			bool evResult = ParentT::containsPredecessor(lhsStmtIndex, rhsStmtIndex);
			return EvaluatedTable(evResult); //e.g evResult == true, if 6 is a parent of 7

		}
		// e.g Parent*(6, s2), Parent*(6, _)
		else if (lhsRef.first == PqlReferenceType::integer)
		{
			std::vector<int> results;
			int lhsRefValue = stoi(lhsRef.second); // might throw error if string value can't be converted to int
			StmtIndex lhsStmt = StmtIndex(lhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (ParentT::containsSuccessor(lhsStmt, stmt)) {
					results.emplace_back(stmt.getIndex()); // e.g {6} because 6 is a parent of 7
				}
			}
			std::unordered_map<std::string, PqlEntityType> PQLentities;
			PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));

			std::unordered_map<std::string, std::vector<int>> PQLmap;
			PQLmap[rhsRef.second] = results;

			return EvaluatedTable(PQLentities, PQLmap);

		}
		// e.g. Parent*(s1, 7), Parent*(_ 7)
		else if (rhsRef.first == PqlReferenceType::integer)
		{
			std::vector<int> results;
			int rhsRefValue = stoi(rhsRef.second); //might throw error if string value can't be converted to int
			StmtIndex rhsStmt = StmtIndex(rhsRefValue);
			for (StmtIndex stmt : stmts) {
				if (ParentT::containsSuccessor(stmt, rhsStmt)) {
					results.emplace_back(stmt.getIndex()); //e.g {6} because 6 is a parent of 7
				}
			}
			std::unordered_map<std::string, PqlEntityType> PQLentities;
			PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));

			std::unordered_map<std::string, std::vector<int>> PQLmap;
			PQLmap[lhsRef.second] = results;

			return EvaluatedTable(PQLentities, PQLmap);
		}
		// Parent*(s1, s2), Parent*(s1, _), Parent*(_, s2)
		else if (!(lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard)) {
			//Assumption: Different synonym names (i.e. Parent*(s1, s2), not Parent*(s1, s1))
			std::tuple<std::vector<int>, std::vector<int>> results = ParentT::getAllPredecessorSuccessorInfo();
			//e.g. {1, 2}, {2, 3}, {3, 6}
			std::unordered_map<std::string, PqlEntityType> PQLentities;
			std::unordered_map<std::string, std::vector<int>> PQLmap;

			if (lhsRef.first == PqlReferenceType::synonym) {
				PQLentities.insert(std::pair(lhsRef.second, PqlEntityType::Stmt));
				PQLmap[lhsRef.second] = std::get<0>(results); // if RHS is wildcard, LHS may have duplicate values
			}
			if (rhsRef.first == PqlReferenceType::synonym) {
				PQLentities.insert(std::pair(rhsRef.second, PqlEntityType::Stmt));
				PQLmap[rhsRef.second] = std::get<1>(results); // if LHS is wildcard, RHS may have duplicate values
			}
			return EvaluatedTable(PQLentities, PQLmap);
		}
		// Parent*(_, _)
		else {
			bool isEmptyTable = true;
			if (lhsRef.first == PqlReferenceType::wildcard && rhsRef.first == PqlReferenceType::wildcard) {
				isEmptyTable = std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).empty();
			}
			// No Parent rs exists => isEmptyTable == true => EvTable.evResult == false (innerJoinMerge() can drop table)
			// Parent rs exists => isEmptyTable == false => EvTable.evResult == true (innerJoinMerge() can merge dummy table, preserving all rows)
			return EvaluatedTable(!isEmptyTable);
		}
	}

public:
	//enum class PqlRelationshipType {
	//	Follows, FollowsT, Parent, ParentT,
	//	UsesS, UsesP, ModifiesS, ModifiesP,
	//	Uses, Modifies
	//};
	RelationshipInstruction(PqlRelationshipType pqlRSType, PqlReference lhs, PqlReference rhs) :
		pqlRelationshipType(pqlRSType), lhsRef(lhs), rhsRef(rhs) {}


	EvaluatedTable execute() override {
		EvaluatedTable evTable;
		switch (pqlRelationshipType) {
		case PqlRelationshipType::ModifiesS:
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
		case PqlRelationshipType::FollowsT:
			evTable = handleFollowsT();
			break;
		case PqlRelationshipType::Parent:
			evTable = handleParent();
			break;
		case PqlRelationshipType::ParentT:
			evTable = handleParentT();
			break;
	}


		return evTable;
	}

};


class PatternInstruction : public Instruction {
private:
	std::string synonym;
	PqlReference entRef;
	PqlExpression expressionSpec;

	bool isNumber(const std::string& s)
	{
		for (char const& ch : s) {
			if (std::isdigit(ch) == 0)
				return false;
		}
		return true;
	}

	bool containsWildCard(const std::string s)
	{
		return s.find("*") != std::string::npos;
	}

public:
	PatternInstruction::PatternInstruction(std::string synonym, PqlReference entRef, PqlExpression expressionSpec) : synonym(synonym), entRef(entRef), expressionSpec(expressionSpec) {}

	EvaluatedTable handlePatterns() {
		// Pattern a(v, "_x_") or Pattern a(v, "_123_") or Pattern a("x", "_x_")
		// Pattern a(v, *) or Pattern a("x", *)
		// Pattern a(*, "_x_") 
		// Pattern a(*, *)		
		std::unordered_map<std::string, PqlEntityType> PQLentities;
		std::unordered_map<std::string, std::vector<int>> PQLmap;
		PQLentities.insert(std::pair(synonym, PqlEntityType::Assign));
		if (isNumber(entRef.second)) {
			PQLentities.insert(std::pair(entRef.second, PqlEntityType::Constant));
		}
		else {
			PQLentities.insert(std::pair(entRef.second, PqlEntityType::Variable));
		}
		std::tuple<std::vector<int>, std::vector<int>> allPatternStmtInfo;
		if (expressionSpec.first == PqlExpressionType::full) {
			allPatternStmtInfo = Pattern::getStmtsFromPattern(expressionSpec.second, false);
		}
		else if (expressionSpec.first == PqlExpressionType::partial) {
			// currently only has this for iteration 1
			allPatternStmtInfo = Pattern::getStmtsFromPattern(expressionSpec.second, true);
		}
		else if (expressionSpec.first == PqlExpressionType::wildcard) {
			allPatternStmtInfo = Pattern::getAllAssignStmtVarsPatternInfo();
		}
		else {
			std::cout << "Invalid expression type";
		}
		if (entRef.first == PqlReferenceType::synonym) {
			std::vector<int> allStmts;
			std::vector<int> varIndices;
			allStmts = std::get<0>(allPatternStmtInfo);
			varIndices = std::get<1>(allPatternStmtInfo);
			PQLmap[synonym] = allStmts;
			PQLmap[entRef.second] = varIndices;
		}
		else if (entRef.first == PqlReferenceType::ident) {
			// Pattern a("x", "_x_")
			if (Entity::containsVar(entRef.second)) {
				VarIndex varIndex = Entity::getVarIdx(entRef.second);
				std::vector<int> allStmts;
				allStmts = Pattern::getStmtsFromVarPattern(varIndex, expressionSpec.second, true);
				PQLmap[synonym] = allStmts;
			} 
		}
		else if (entRef.first == PqlReferenceType::wildcard) {
			if (containsWildCard(expressionSpec.second)) {
				return EvaluatedTable(true);
			}
			std::vector<int> allStmts;
			allStmts = std::get<0>(allPatternStmtInfo);
			PQLmap[synonym] = allStmts;
		}
		return EvaluatedTable(PQLentities, PQLmap);
	}

	EvaluatedTable execute() override {
		EvaluatedTable evTable = handlePatterns();
		return evTable;
	}
};