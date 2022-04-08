#pragma once

#include "../PKB/PKB.h"

class PKBGetter {
private:
	PKB* pkb;
public:
	PKBGetter(PKB* pkb);
	~PKBGetter();

	/* Main Entities */
	bool containsNameIdxEntity(EntityType entityType, std::string& entityName);
	std::string getNameIdxEntityName(EntityType entityType, NameIdxEntityIndex entityIdx);
	NameIdxEntityIndex getNameIdxEntityIndex(EntityType entityType, std::string entityName);
	std::vector<NameIdxEntityIndex> getAllNameIdxEntityInfo(EntityType entityType);

	std::vector<ConstValue> getAllConsts();

	bool containsStmt(StmtIndex stmtNo);
	std::vector<StmtIndex> getStmtIdxFromType(StatementType stmtType);
	StatementType getTypeFromStmtIdx(StmtIndex stmtIdx);
	std::vector<StmtIndex> getAllStmts();
	std::vector<StmtIndex> getAllContainerStmts();

	/* Other Entity related information */
	std::vector<StmtIndex> getStmtsFromProc(ProcIndex& procIdx);
	ProcIndex getProcFromStmt(StmtIndex stmtIdx);
	std::tuple<std::vector<ProcIndex>, std::vector<StmtIndex>> getAllProcStmts();

	/* Attributes */
	std::string getAttributeNameByStmtIdx(StmtIndex& stmtIdx);
	std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>>
		getEqualNameAttributes(EntityType leftEntityType, EntityType rightEntityType);
	std::vector<EntityAttributeRef> getEqualIntegerAttributes(EntityType leftEntityType, EntityType rightEntityType);
	std::vector<EntityAttributeRef> getEqualNameAttributesFromName(EntityType entityType, std::string& nameValue);
	bool hasEqualIntegerAttribute(EntityType entityType, ConstValue integerValue);

	/* Pattern */
	std::vector<StmtIndex> getAssignStmtsFromVarExprFullMatch(VarIndex varIdx, std::string& expression);
	std::vector<StmtIndex> getAssignStmtsFromVarExprPartialMatch(VarIndex varIdx, std::string& expression);
	std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> getAssignStmtsFromExprFullMatch(std::string& expression);
	std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> getAssignStmtsFromExprPartialMatch(std::string& expression);
	std::vector<StmtIndex> getAssignStmtsFromVar(VarIndex& varIdx);
	std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> getAllAssignPatternInfo();
	std::vector<StmtIndex> getPatternContainerStmtsFromVar(StatementType stmtType, VarIndex& varIndex);
	std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> getAllPatternContainerInfo(StatementType stmtType);

	/* Relationships */
	bool getRSContainsInfo(RelationshipType rsType, SynonymIndex leftIndex, SynonymIndex rightIndex);
	std::vector<SynonymIndex> getRSInfoFromLeftArg(RelationshipType rsType, SynonymIndex leftIndex);
	std::vector<SynonymIndex> getRSInfoFromRightArg(RelationshipType rsType, SynonymIndex rightIndex);
	std::tuple<std::vector<SynonymIndex>, std::vector<SynonymIndex>> getRSAllInfo(RelationshipType rsType);
};
