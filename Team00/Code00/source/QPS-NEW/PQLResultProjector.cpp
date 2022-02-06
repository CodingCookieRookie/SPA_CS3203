#include "PQLResultProjector.h"
#include "EvaluatedTable.h"
#include <string>
#include <list>
#include <unordered_set>

std::list<std::string> resolveTableToResults(EvaluatedTable evaluatedTable) {
	std::unordered_map<PQL_VARIABLE_TYPE, std::vector<VALUE>>* table = evaluatedTable.getTableRef();
	//unordered_map<PQL_VARIABLE_TYPE, std::vector<VALUE>>* 
	std::list<std::string> resList;
	for (auto it : *table) {
		std::vector<VALUE> values = it.second;
		std::string res = "";
		for (int i = 0; i < values.size(); i++) {
			res += values.at(i);
			if (i != values.size() - 1) {
				res += " ";
			}
		}
		resList.push_back(res);
	}
}
