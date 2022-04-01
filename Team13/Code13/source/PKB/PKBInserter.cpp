#include "./PKBInserter.h"

PKBInserter::PKBInserter(PKB* pkb) {
	this->pkb = pkb;
}

PKBInserter::~PKBInserter() {}

void PKBInserter::insertToCFG(StmtIndex predecessor, StmtIndex successor) {
	pkb->cfg->insert(predecessor, successor);
}

std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> PKBInserter::getCFGTable() {
	return pkb->cfg->getCFGTable();
}
