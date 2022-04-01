#pragma once

#include "../PKB/PKB.h"

class PKBGetter {
private:
	PKB* pkb;
public:
	PKBGetter(PKB* pkb);
	~PKBGetter();

	/*TODO: Add public methods for QE, which should not contain any implementatin logic*/
};
