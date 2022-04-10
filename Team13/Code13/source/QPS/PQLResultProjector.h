#pragma once

#include "EvaluatedTable.h"
#include "QPSCommons.h"
#include "ParsedQuery.h"
#include "../PKB/PKBGetter.h"

class PQLResultProjector {
private:
	EvaluatedTable& evTable;
	ParsedQuery& parsedQuery;
	PKBGetter* pkbGetter;
	std::vector<std::string> columnsProjected;
	std::unordered_map<std::string, EntityType> declarations;

	/* Helper method to project clauses that select boolean */
	std::list<std::string> projectBoolean();

	/* Helper methods to consolidate results of columns projected */
	std::list<std::string> projectColumns();

	std::string getColumnResults(int rowNum);

	/* Checks if table field has no values */
	bool resultTableHasEmptyColumn(Table& resultTable);

	/* Helper methods to get results of the columns pertaining to the entity name */
	std::string getSynonym(std::string entityName, int rowNum);
	std::string getProcName(std::string entityName, int rowNum);
	std::string getVarName(std::string entityName, int rowNum);
	std::string getConstant(std::string entityName, int rowNum);
	std::string getStmtNum(std::string entityName, int rowNum);

public:
	PQLResultProjector(EvaluatedTable& evTable, ParsedQuery& parsedQuery, PKBGetter* pkbGetter);

	/* Resolves the EvaluatedTable results into a list of strings to present the results of the query. */
	std::list<std::string> PQLResultProjector::resolveTableToResults();
};
