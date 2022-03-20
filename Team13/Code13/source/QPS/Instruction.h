#pragma once

#include <unordered_map>
#include <vector>

#include "../Exception/SPAException.h"
#include "../PKB/Entity.h"
#include "../PKB/ExpressionProcessor.h"
#include "../PKB/Follows.h"
#include "../PKB/FollowsT.h"
#include "../PKB/ModifiesS.h"
#include "../PKB/ModifiesP.h"
#include "../PKB/Parent.h"
#include "../PKB/ParentT.h"
#include "../PKB/Pattern.h"
#include "../PKB/UsesS.h"
#include "../PKB/UsesP.h"
#include "../PKB/Calls.h"
#include "../PKB/CallsT.h"
#include "../PKB/Next.h"
#include "EvaluatedTable.h"
#include "QPSCommons.h"

class Instruction {
public:
	virtual EvaluatedTable execute() = 0;
};
