#include "PQLResultProjector.h"
#include "EvaluatedTable.h"
#include <string>
#include <unordered_set>

std::list<std::string> resolveTable(EvaluatedTable evaluatedTable) {
	// we kind of need to check whether we want the entities or values or both to be returned right?
	// Maybe we can add this state in evaluatedTable so evaluatedTable only contains what we want?
	
	//std::unordered_set<std::string> entities = evaluatedTable.getEvaluatedTable();
}
