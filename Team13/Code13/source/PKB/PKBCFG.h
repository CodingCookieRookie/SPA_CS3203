#pragma once

#include <vector>

#include "../SP/CFG.h"

class PKBCFG {
private:
	static std::vector<CFG> CFGTable;

public:
	static void insert(CFG& cfg);
	static std::vector<CFG> getCFGTable();
	static void performCleanUp();
};
