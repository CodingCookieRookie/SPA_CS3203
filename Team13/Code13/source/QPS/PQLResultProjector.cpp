#include <list>
#include <string>
#include <unordered_set>

#include "../QPS/Instruction.h"
#include "PQLResultProjector.h"

PQLResultProjector::PQLResultProjector(EvaluatedTable& evTable,
	ParsedQuery& parsedQuery, PKBGetter* pkbGetter) :
	evTable(evTable), parsedQuery(parsedQuery), pkbGetter(pkbGetter) {}

std::list<std::string> PQLResultProjector::resolveTableToResults() {
	/* 1. Differentiate Projecting Boolean and Projecting Columns */
	std::vector<PqlReference> attributes = parsedQuery.getAttributes();
	ProjectionType projType = ParsedQuery::getProjectionType(attributes);
	std::list<std::string> resList;
	if (projType == ProjectionType::BOOLEAN) {
		resList = PQLResultProjector::projectBoolean();
	} else {
		resList = PQLResultProjector::projectColumns();
	}
	/* 2. Filter to unique values */
	resList.unique();
	return resList;
}

std::list<std::string> PQLResultProjector::projectBoolean() {
	std::vector<PqlReference> attributes = parsedQuery.getAttributes();
	std::list<std::string> resList;
	bool resultTableHasEmptyColumn = PQLResultProjector::resultTableHasEmptyColumn(evTable.getTableRef());
	std::string boolResult = !evTable.getEvResult() || resultTableHasEmptyColumn ? "FALSE" : "TRUE";
	resList.emplace_back(boolResult);
	return resList;
}

bool PQLResultProjector::resultTableHasEmptyColumn(
	std::unordered_map<std::string, std::vector<int>> resultTable) {
	bool resultTableHasEmptyColumn = false; /* Default: No table at all, or filled table */
	for (std::pair<std::string, std::vector<int>> col : resultTable) {
		if ((col.second.empty())) {
			resultTableHasEmptyColumn = true;
			break;
		}
	}
	return resultTableHasEmptyColumn;
}

std::list<std::string> PQLResultProjector::projectColumns() {
	/* Attributes: procName, varName, value, stmtNum
	STRING: procedure.procName, call.procName,
	STRING: variable.varName, read.varName, print.varName:
	INTEGER: constant.value
	INTEGER: stmt.stmt#, read.stmt#, print.stmt#, call.stmt#,
	INTEGER: while.stmt#, if.stmt#, assign.stmt#: */
	int numRow = evTable.getNumRow();
	std::list<std::string> resList;
	for (int i = 0; i < numRow; i++) {	// for each row
		/* Handle each type of reference seperately
		e.g. read r; Select <r, r.varName> */
		resList.push_back(PQLResultProjector::getColumnResults(i));
	}
	return resList;
}

std::string PQLResultProjector::getColumnResults(int i) {
	std::string res = "";
	bool firstCol = true;
	std::vector<PqlReference> attributes = parsedQuery.getAttributes();
	for (const PqlReference& attribute : attributes) {
		std::string entityName = attribute.second;
		PqlReferenceType attributeType = attribute.first;
		std::string value = "";
		if (attributeType == PqlReferenceType::SYNONYM) {
			value = PQLResultProjector::getSynonym(entityName, i);
		} else if (attributeType == PqlReferenceType::PROC_NAME) {
			value = PQLResultProjector::getProcName(entityName, i);
		} else if (attributeType == PqlReferenceType::VAR_NAME) {
			value = PQLResultProjector::getVarName(entityName, i);
		} else if (attributeType == PqlReferenceType::VALUE) {
			value = PQLResultProjector::getConstant(entityName, i);
		} else if (attributeType == PqlReferenceType::STMT_NUM) {
			value = PQLResultProjector::getStmtNum(entityName, i);
		}
		res += firstCol ? value : " " + value;
		firstCol = false;
	}
	return res;
}

std::string PQLResultProjector::getSynonym(std::string entityName, int i) {
	std::unordered_map<std::string, EntityType> declarations = parsedQuery.getDeclarations();
	std::unordered_map<std::string, std::vector<int>>& resultTable = evTable.getTableRef();
	std::string value = "";
	if (isStatementEntity(declarations[entityName])
		|| declarations[entityName] == EntityType::CONSTANT) {
		value = std::to_string(resultTable[entityName][i]);
	} else if ((declarations[entityName] == EntityType::VARIABLE)) {
		value = pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, resultTable[entityName][i]);
	} else if ((declarations[entityName] == EntityType::PROCEDURE)) {
		value = pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, resultTable[entityName][i]);
	}
	return value;
}

std::string PQLResultProjector::getProcName(std::string entityName, int i) {
	std::unordered_map<std::string, EntityType> declarations = parsedQuery.getDeclarations();
	std::unordered_map<std::string, std::vector<int>>& resultTable = evTable.getTableRef();
	std::string value = "";
	/* Assumption: there exists a column of p or cl */
	int index = resultTable[entityName][i]; /* index could be ProcIdx or StmtIdx */
	if ((declarations[entityName] == EntityType::PROCEDURE)) {
		value = pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, index);
	} else if ((declarations[entityName] == EntityType::CALL)) {
		value = pkbGetter->getAttributeNameByStmtIdx(index);
	}
	return value;
}

std::string PQLResultProjector::getVarName(std::string entityName, int i) {
	std::unordered_map<std::string, EntityType> declarations = parsedQuery.getDeclarations();
	std::unordered_map<std::string, std::vector<int>>& resultTable = evTable.getTableRef();
	std::string value = "";
	/* Assumption: there exists a column of v, r or pn */
	int index = resultTable[entityName][i]; /* index could be VarIdx or StmtIdx */
	if ((declarations[entityName] == EntityType::VARIABLE)) {
		value = pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, index);
	} else if ((declarations[entityName] == EntityType::READ
		|| declarations[entityName] == EntityType::PRINT)) {
		value = pkbGetter->getAttributeNameByStmtIdx(index);
	}
	return value;
}

std::string PQLResultProjector::getConstant(std::string entityName, int i) {
	std::unordered_map<std::string, EntityType> declarations = parsedQuery.getDeclarations();
	std::unordered_map<std::string, std::vector<int>>& resultTable = evTable.getTableRef();
	std::string value = "";
	/* Assumption: there exists a column of c */
	int index = resultTable[entityName][i]; /* index is the constant value itself */
	if ((declarations[entityName] == EntityType::CONSTANT)) {
		value = std::to_string(index);
	}
	return value;
}

std::string PQLResultProjector::getStmtNum(std::string entityName, int i) {
	std::unordered_map<std::string, std::vector<int>>& resultTable = evTable.getTableRef();
	std::string value = "";
	/* Assumption: there exists a column of s, r, pn, cl, if, a */
	int index = resultTable[entityName][i]; /* index is StmtIdx */
	value = std::to_string(index);
	return value;
}
