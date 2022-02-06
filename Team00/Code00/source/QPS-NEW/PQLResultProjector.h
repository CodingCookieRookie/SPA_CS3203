#pragma once
#include "EvaluatedTable.h"

class PQLResultProjector
{
public:
	std::list<std::string> resolveTable(EvaluatedTable evaluatedTable);
};
