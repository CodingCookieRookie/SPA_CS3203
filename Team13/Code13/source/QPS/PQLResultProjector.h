#pragma once

#include "EvaluatedTable.h"
#include "QPSCommons.h"

enum class ProjectionType {
	single, tuple, boolean
};

class PQLResultProjector {
private:
	EvaluatedTable evaluatedTable;
	std::vector<std::string> columnsProjected;
	std::unordered_map<std::string, PqlEntityType> declarations;

	static ProjectionType getProjectionType(std::vector<PqlReference> attributesProjected);

public:
	/* Resolves the EvaluatedTable results into a list of strings to present the results of the query. */
	static std::list<std::string> PQLResultProjector::resolveTableToResults(
		EvaluatedTable evTable, std::vector<PqlReference> attributes,
		std::unordered_map<std::string, PqlEntityType> declarations);
};
