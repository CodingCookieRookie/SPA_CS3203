#pragma once

#include "../PKB/PKB.h"

class PKBInserter {
private:
	PKB* pkb;
public:
	PKBInserter(PKB* pkb);
	~PKBInserter();

	/*TODO: Add public methods for DE, which should not contain any implementatin logic*/
};
