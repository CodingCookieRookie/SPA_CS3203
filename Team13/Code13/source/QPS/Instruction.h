#pragma once

#include <unordered_map>
#include <vector>

#include "../Exception/SPAException.h"
#include "../PKB/ExpressionProcessor.h"
#include "../PKB/PKBGetter.h"
#include "EvaluatedTable.h"
#include "QPSCommons.h"
#include "../Common/Types.h"
#include "OnTheFlyRSProcessor.h"
#include "NextTProcessor.h"
#include "AffectsProcessor.h"
#include "AffectsTProcessor.h"

class Instruction {
public:
	virtual EvaluatedTable execute() = 0;
	virtual std::unordered_set<std::string> getSynonyms() = 0;
	virtual QueryScore getDifficultyScore() {
		return getSynonyms().size();
	};
};
