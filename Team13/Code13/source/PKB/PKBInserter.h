#pragma once

#include "../PKB/PKB.h"

class PKBInserter {
private:
	PKB* pkb;
public:
	PKBInserter(PKB* pkb);
	~PKBInserter();

	/* Main Entities */
	NameIdxEntityIndex insertNameIdxEntity(EntityType entityType, std::string entityName);
	void insertConst(ConstValue constant);
	StmtIndex insertStmt(StatementType stmtType);
	StmtIndex insertStmt(StatementType stmtType, std::string& nameValue);

	/* Other Entity related information */
	void insertStmtFromProc(ProcIndex procIdx, StmtIndex stmtIdx);

	/* Attributes */
	void insertStmtByName(StmtIndex& stmtIdx, StatementType stmtType, std::string& nameValue);

	/* Pattern */
	void insertAssignInfo(VarIndex& varIdx, std::string& postFixExpression, StmtIndex& stmtIdx);
	void insertPatternContainer(StatementType stmtType, StmtIndex& stmtIdx, VarIndex& varIdx);

	/* Relationships */
	void insertRSInfo(RelationshipType rsType, SynonymIndex leftIndex, SynonymIndex rightIndex);
};
