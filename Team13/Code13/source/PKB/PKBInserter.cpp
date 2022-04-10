#include "./PKBInserter.h"

PKBInserter::PKBInserter(PKB& pkb) :pkb(pkb) {}

PKBInserter::~PKBInserter() {}

NameIdxEntityIndex PKBInserter::insertNameIdxEntity(EntityType entityType, std::string entityName) {
	return pkb.nameIdxEntityToInstanceMap[entityType]->insert(entityName, pkb.attribute);
}

void PKBInserter::insertConst(ConstValue constant) {
	pkb.constant->insertConst(constant);
}

StmtIndex PKBInserter::insertStmt(StatementType stmtType) {
	return pkb.statement->insertStmt(stmtType);
}

StmtIndex PKBInserter::insertStmt(StatementType stmtType, std::string& nameValue) {
	return pkb.statement->insertStmt(stmtType, nameValue, pkb.attribute);
}

void PKBInserter::insertStmtFromProc(ProcIndex procIdx, StmtIndex stmtIdx) {
	return pkb.procStmtMapper->insertStmtFromProc(procIdx, stmtIdx);
}

void PKBInserter::insertStmtByName(StmtIndex& stmtIdx, StatementType stmtType, std::string& nameValue) {
	return pkb.attribute->insertStmtByName(stmtIdx, stmtType, nameValue);
}

void PKBInserter::insertAssignInfo(VarIndex& varIdx, std::string& postFixExpression, StmtIndex& stmtIdx) {
	return pkb.patternAssign->insertAssignInfo(varIdx, postFixExpression, stmtIdx);
}

void PKBInserter::insertPatternContainer(StatementType stmtType, StmtIndex& stmtIdx, VarIndex& varIdx) {
	pkb.containerEntityToInstanceMap[stmtType]->insert(stmtIdx, varIdx);
}

void PKBInserter::insertRSInfo(RelationshipType rsType, SynonymIndex leftIndex, SynonymIndex rightIndex) {
	pkb.rsToInstanceMap[rsType]->insert(leftIndex, rightIndex);
}
