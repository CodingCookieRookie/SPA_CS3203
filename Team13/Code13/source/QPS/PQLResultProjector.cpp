#include <list>
#include <string>
#include <unordered_set>

#include "../PKB/Entity.h"
#include "../QPS/Instruction.h"
#include "EvaluatedTable.h"
#include "PQLResultProjector.h"
#include "QPSCommons.h"

std::list<std::string> PQLResultProjector::resolveTableToResults(
	EvaluatedTable evaluatedTable, std::vector<PqlReference> attributes,
	std::unordered_map<std::string, PqlEntityType> declarations) {
	std::list<std::string> resList;
	std::unordered_map<std::string, std::vector<int>> resultTable = evaluatedTable.getTableRef();
	int numRow = evaluatedTable.getNumRow();

	/* 1. Check if Projecting Single Synonym, Tuple, or Boolean */
	ProjectionType projType = PQLResultProjector::getProjectionType(attributes);

	// 2. Projecting only selected columns

	/* procName, varName, value, stmtNum
	STRING: procedure.procName, call.procName,
	STRING: variable.varName, read.varName, print.varName:
	INTEGER: constant.value
	INTEGER: stmt.stmt#, read.stmt#, print.stmt#, call.stmt#,
	INTEGER: while.stmt#, if.stmt#, assign.stmt#: */

	/* TODO: DRY everything */
	if (projType == ProjectionType::boolean) {
		if (!resultTable.empty()) {
			resList.emplace_back("TRUE");
		} else {
			resList.emplace_back("FALSE");
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
			if (attributeType == PqlReferenceType::synonym) {
				if (isStatementEntity(declarations[entityName])
					|| declarations[entityName] == PqlEntityType::Constant) {
					value = std::to_string(resultTable[entityName][i]);
				} else if ((declarations[entityName] == PqlEntityType::Variable)) {
					value = Entity::getVarName(resultTable[entityName][i]);
				} else if ((declarations[entityName] == PqlEntityType::Procedure)) {
					value = Entity::getProcName(resultTable[entityName][i]);
				} else {}

			} else if (attributeType == PqlReferenceType::procName) {
				/* Assumption: there exists a column of p or cl */
				int index = resultTable[entityName][i]; /* index could be ProcIdx or StmtIdx */
				if ((declarations[entityName] == PqlEntityType::Procedure)) {
					value = Entity::getProcName(index);
				} else if ((declarations[entityName] == PqlEntityType::Call)) {
					value = Attribute::getAttributeNameByStmtIdx(index);
				}

			} else if (attributeType == PqlReferenceType::varName) {
				/* Assumption: there exists a column of v, r or pn */
				int index = resultTable[entityName][i]; /* index could be VarIdx or StmtIdx */
				if ((declarations[entityName] == PqlEntityType::Variable)) {
					value = Entity::getVarName(index);
				} else if ((declarations[entityName] == PqlEntityType::Read)) {
					value = Attribute::getAttributeNameByStmtIdx(index);
				} else if ((declarations[entityName] == PqlEntityType::Print)) {
					value = Attribute::getAttributeNameByStmtIdx(index);
				} else {}

			} else if (attributeType == PqlReferenceType::value) {
				/* Assumption: there exists a column of c */
				int index = resultTable[entityName][i]; /* index is the constant value itself */
				if ((declarations[entityName] == PqlEntityType::Constant)) {
					value = std::to_string(index);
				} else {}

			} else if (attributeType == PqlReferenceType::stmtNum) {
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

ProjectionType PQLResultProjector::getProjectionType(std::vector<PqlReference> attributesProjected) {
	if (attributesProjected.size() == 0) {
		return ProjectionType::boolean;
	} else if (attributesProjected.size() == 1) {
		return ProjectionType::single;
	} else if (attributesProjected.size() >= 1) {
		return ProjectionType::tuple;
	} else {
		throw EvaluatorException(EvaluatorException::PROJECTION_TYPE_ERROR);
	}
}
