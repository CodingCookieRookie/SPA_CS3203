#pragma once

#include "../PKB/PKB.h"

class PKBInserter {
private:
	PKB* pkb;
public:
	PKBInserter(PKB* pkb);
	~PKBInserter();

	/*TODO: Add public methods for DE, which should not contain any implementatin logic*/
	void insertToCFG(StmtIndex predecessor, StmtIndex successor);
	std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> getCFGTable();
};
