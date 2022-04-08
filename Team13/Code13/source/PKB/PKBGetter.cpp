#include "./PKBGetter.h"

PKBGetter::PKBGetter(PKB* pkb) {
	this->pkb = pkb;
}

PKBGetter::~PKBGetter() {}

bool PKBGetter::containsNameIdxEntity(EntityType entityType, std::string& entityName) {
	return pkb->nameIdxEntityToInstanceMap[entityType]->contains(entityName);
}

std::string PKBGetter::getNameIdxEntityName(EntityType entityType, NameIdxEntityIndex entityIdx) {
	return pkb->nameIdxEntityToInstanceMap[entityType]->getName(entityIdx);
}

NameIdxEntityIndex PKBGetter::getNameIdxEntityIndex(EntityType entityType, std::string entityName) {
	return pkb->nameIdxEntityToInstanceMap[entityType]->getIndex(entityName);
}

std::vector<NameIdxEntityIndex> PKBGetter::getAllNameIdxEntityInfo(EntityType entityType) {
	return pkb->nameIdxEntityToInstanceMap[entityType]->getAll();
}

std::vector<ConstValue> PKBGetter::getAllConsts() {
	return pkb->constant->getAllConsts();
}

bool PKBGetter::containsStmt(StmtIndex stmtNo) {
	return pkb->statement->containsStmt(stmtNo);
}

std::vector<StmtIndex> PKBGetter::getStmtIdxFromType(StatementType stmtType) {
	return pkb->statement->getStmtIdxFromType(stmtType);
}

StatementType PKBGetter::getTypeFromStmtIdx(StmtIndex stmtIdx) {
	return pkb->statement->getTypeFromStmtIdx(stmtIdx);
};

std::vector<StmtIndex> PKBGetter::getAllStmts() {
	return pkb->statement->getAllStmts();
}

std::vector<StmtIndex> PKBGetter::getAllContainerStmts() {
	return pkb->statement->getAllContainerStmts();
}

std::vector<StmtIndex> PKBGetter::getStmtsFromProc(ProcIndex& procIdx) {
	return pkb->procStmtMapper->getStmtsFromProc(procIdx);
}

ProcIndex PKBGetter::getProcFromStmt(StmtIndex stmtIdx) {
	return pkb->procStmtMapper->getProcFromStmt(stmtIdx);
};

std::tuple<std::vector<ProcIndex>, std::vector<StmtIndex>> PKBGetter::getAllProcStmts() {
	return pkb->procStmtMapper->getAllProcStmts();
}

std::string PKBGetter::getAttributeNameByStmtIdx(StmtIndex& stmtIdx) {
	return pkb->attribute->getAttributeNameByStmtIdx(stmtIdx);
}

std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> PKBGetter::getEqualNameAttributes(
	EntityType leftEntityType, EntityType rightEntityType) {
	return pkb->attribute->getEqualNameAttributes(leftEntityType, rightEntityType);
}

std::vector<EntityAttributeRef> PKBGetter::getEqualIntegerAttributes(
	EntityType leftEntityType, EntityType rightEntityType) {
	return pkb->attribute->getEqualIntegerAttributes(leftEntityType, rightEntityType, pkb->constant, pkb->statement);
}

std::vector<EntityAttributeRef> PKBGetter::getEqualNameAttributesFromName(
	EntityType entityType, std::string& nameValue) {
	return pkb->attribute->getEqualNameAttributesFromName(entityType, nameValue);
}

bool PKBGetter::hasEqualIntegerAttribute(EntityType entityType, ConstValue integerValue) {
	return pkb->attribute->hasEqualIntegerAttribute(entityType, integerValue, pkb->constant, pkb->statement);
}

std::vector<StmtIndex> PKBGetter::getAssignStmtsFromVarExprFullMatch(VarIndex varIdx, std::string& expression) {
	return pkb->patternAssign->getAssignStmtsFromVarExprFullMatch(varIdx, expression);
}

std::vector<StmtIndex> PKBGetter::getAssignStmtsFromVarExprPartialMatch(VarIndex varIdx, std::string& expression) {
	return pkb->patternAssign->getAssignStmtsFromVarExprPartialMatch(varIdx, expression);
}

std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> PKBGetter::getAssignStmtsFromExprFullMatch(std::string& expression) {
	return pkb->patternAssign->getAssignStmtsFromExprFullMatch(expression);
}

std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> PKBGetter::getAssignStmtsFromExprPartialMatch(std::string& expression) {
	return pkb->patternAssign->getAssignStmtsFromExprPartialMatch(expression);
}

std::vector<StmtIndex> PKBGetter::getAssignStmtsFromVar(VarIndex& varIdx) {
	return pkb->patternAssign->getAssignStmtsFromVar(varIdx);
}

std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> PKBGetter::getAllAssignPatternInfo() {
	return pkb->patternAssign->getAllAssignPatternInfo();
}

std::vector<StmtIndex> PKBGetter::getPatternContainerStmtsFromVar(StatementType stmtType, VarIndex& varIndex) {
	return pkb->containerEntityToInstanceMap[stmtType]->getStmtsFromVar(varIndex);
}
std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> PKBGetter::getAllPatternContainerInfo(StatementType stmtType) {
	return pkb->containerEntityToInstanceMap[stmtType]->getAll();
}

bool PKBGetter::getRSContainsInfo(RelationshipType rsType, SynonymIndex leftIndex, SynonymIndex rightIndex) {
	return pkb->rsToInstanceMap[rsType]->contains(leftIndex, rightIndex);
}

std::vector<SynonymIndex> PKBGetter::getRSInfoFromLeftArg(RelationshipType rsType, SynonymIndex leftIndex) {
	return pkb->rsToInstanceMap[rsType]->getFromLeftArg(leftIndex);
}

std::vector<SynonymIndex> PKBGetter::getRSInfoFromRightArg(RelationshipType rsType, SynonymIndex rightIndex) {
	return pkb->rsToInstanceMap[rsType]->getFromRightArg(rightIndex);
}

std::tuple<std::vector<SynonymIndex>, std::vector<SynonymIndex>>
PKBGetter::getRSAllInfo(RelationshipType rsType) {
	return pkb->rsToInstanceMap[rsType]->getAllInfo();
}
