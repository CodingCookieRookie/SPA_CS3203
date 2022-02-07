#pragma once
#include <vector>
#include <unordered_map>

#include "QPSCommons.h"
#include "../Exception/SPAException.h"

class ParsedQuery {
private:
	std::unordered_map<std::string, PQL_VARIABLE_TYPE> declarations;
	std::vector<std::string> columns;
public:

	/* Constructor for a ParsedQuery object */
	ParsedQuery(const std::vector<PQL_VARIABLE>& allDeclarations,
		const std::vector<std::string>& allColumns);


	std::unordered_map<std::string, PQL_VARIABLE_TYPE> getDeclarations();
	std::vector<std::string> getColumns();
};
