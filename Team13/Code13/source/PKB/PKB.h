#pragma once

#include "./PatternAssign.h"
#include "./PatternContainer.h"
#include "./PatternContainerTypes.h"

class PKB {
private:
	/*TODO: Add all PKB instances, along with their header files*/

	/* Main Entities */

	/* Pattern */
	PatternAssign patternAssign;
	std::unordered_map<EntityType, PatternContainer> containerEntityToPatternMap;

public:
	PKB();
	~PKB();

	friend class PKBGetter;
	friend class PKBInserter;
};
