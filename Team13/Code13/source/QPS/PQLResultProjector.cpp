#include <list>
#include <string>
#include <unordered_set>

#include "../QPS/Instruction.h"
#include "EvaluatedTable.h"
#include "PQLResultProjector.h"
#include "QPSCommons.h"

PQLResultProjector::PQLResultProjector(EvaluatedTable& evTable, ParsedQuery& parsedQuery, PKBGetter* pkbGetter) :
	evTable(evTable), parsedQuery(parsedQuery), pkbGetter(pkbGetter) {}

std::list<std::string> PQLResultProjector::resolveTableToResults() {
	std::vector<PqlReference> attributes = parsedQuery.getAttributes();
	std::unordered_map<std::string, EntityType> declarations = parsedQuery.getDeclarations();
	std::list<std::string> resList;
	std::unordered_map<std::string, std::vector<int>> resultTable = evTable.getTableRef();
	int numRow = evTable.getNumRow();

	/* 1. Check if Projecting Single Synonym, Tuple, or Boolean */
	ProjectionType projType = ParsedQuery::getProjectionType(attributes);

	/* 2. To check table-emptiness: the table can have rows but no values in them */
	bool isNoValuesInResultTable = EvaluatedTable::isNoValuesInResultTable(resultTable);

	/* 3. Projecting only selected columns */

	/* procName, varName, value, stmtNum
	STRING: procedure.procName, call.procName,
	STRING: variable.varName, read.varName, print.varName:
	INTEGER: constant.value
	INTEGER: stmt.stmt#, read.stmt#, print.stmt#, call.stmt#,
	INTEGER: while.stmt#, if.stmt#, assign.stmt#: */

	if (projType == ProjectionType::BOOLEAN) {
		if (isNoValuesInResultTable) {
			resList.emplace_back("FALSE");
		} else {
			resList.emplace_back("TRUE");
		}
		return resList;
	}

	for (int i = 0; i < numRow; i++) {	// for each row
		std::string res = "";
		bool firstCol = true;
		/* Handle each type of reference seperately
		e.g. read r; Select <r, r.varName> */
		for (const PqlReference& attribute : attributes) {
			std::string entityName = attribute.second;
			PqlReferenceType attributeType = attribute.first;
			std::string value = "";
			if (attributeType == PqlReferenceType::SYNONYM) {
				if (isStatementEntity(declarations[entityName])
					|| declarations[entityName] == EntityType::CONSTANT) {
					value = std::to_string(resultTable[entityName][i]);
				} else if ((declarations[entityName] == EntityType::VARIABLE)) {
					value = pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, resultTable[entityName][i]);
				} else if ((declarations[entityName] == EntityType::PROCEDURE)) {
					value = pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, resultTable[entityName][i]);
				} else {}
			} else if (attributeType == PqlReferenceType::PROC_NAME) {
				/* Assumption: there exists a column of p or cl */
				int index = resultTable[entityName][i]; /* index could be ProcIdx or StmtIdx */
				if ((declarations[entityName] == EntityType::PROCEDURE)) {
					value = pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, index);
				} else if ((declarations[entityName] == EntityType::CALL)) {
					value = pkbGetter->getAttributeNameByStmtIdx(index);
				}
			} else if (attributeType == PqlReferenceType::VAR_NAME) {
				/* Assumption: there exists a column of v, r or pn */
				int index = resultTable[entityName][i]; /* index could be VarIdx or StmtIdx */
				if ((declarations[entityName] == EntityType::VARIABLE)) {
					value = pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, index);
				} else if ((declarations[entityName] == EntityType::READ)) {
					value = pkbGetter->getAttributeNameByStmtIdx(index);
				} else if ((declarations[entityName] == EntityType::PRINT)) {
					value = pkbGetter->getAttributeNameByStmtIdx(index);
				} else {}
			} else if (attributeType == PqlReferenceType::VALUE) {
				/* Assumption: there exists a column of c */
				int index = resultTable[entityName][i]; /* index is the constant value itself */
				if ((declarations[entityName] == EntityType::CONSTANT)) {
					value = std::to_string(index);
				} else {}
			} else if (attributeType == PqlReferenceType::STMT_NUM) {
				/* Assumption: there exists a column of s, r, pn, cl, if, a */
				int index = resultTable[entityName][i]; /* index is StmtIdx */
				value = std::to_string(index);
			}

			if (!firstCol) {
				res += " ";
			}
			res += value;
			firstCol = false;
		}
		resList.push_back(res);
	}

	// 3. Filter to unique values
	resList.unique();

	return resList;
}
