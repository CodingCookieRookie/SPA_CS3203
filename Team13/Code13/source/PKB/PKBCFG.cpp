#include "PKBCFG.h"

std::vector<CFG> PKBCFG::CFGTable;

void PKBCFG::insert(CFG& cfg) {
	CFGTable.push_back(cfg);
}

std::vector<CFG> PKBCFG::getCFGTable() {
	std::vector<CFG> res;

	for (auto& cfg : CFGTable) {
		res.push_back(cfg);
	}

	return res;
}

void PKBCFG::performCleanUp() {
	CFGTable = {};
}
