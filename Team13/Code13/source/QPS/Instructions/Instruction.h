#pragma once

#include <unordered_map>
#include <vector>

#include "../../Common/Types.h"
#include "../../Exception/SPAException.h"
#include "../../PKB/Pattern/ExpressionProcessor.h"
#include "../../PKB/PKBGetter.h"
#include "../Processors/OnTheFlyRSProcessor.h"
#include "../Processors/NextTProcessor.h"
#include "../Processors/AffectsProcessor.h"
#include "../Processors/AffectsTProcessor.h"
#include "../Evaluator/EvaluatedTable.h"
#include "../QPSCommons.h"


class Instruction {
public:
	virtual EvaluatedTable execute() = 0;
	virtual std::unordered_set<std::string> getSynonyms() = 0;
	virtual QueryScore getDifficultyScore() {
		return getSynonyms().size();
	};
};
