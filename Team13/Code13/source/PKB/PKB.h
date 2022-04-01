#pragma once

#include "../PKB/CFG.h"

class PKB {
private:
	/*TODO: Add all PKB instances, along with their header files*/
	CFG* cfg;

public:
	PKB();
	~PKB();

	friend class PKBGetter;
	friend class PKBInserter;
};
