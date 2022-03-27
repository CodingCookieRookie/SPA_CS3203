#include "stdafx.h"
#include "CppUnitTest.h"

#include "PKB/CFG.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestCFG) {
public:
	TEST_METHOD(insert_getCFGTable) {
		CFG* cfg = new CFG();

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);

		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> expectedCfg;
		expectedCfg[stmtIdx1].insert(stmtIdx2);

		cfg->insert(stmtIdx1, stmtIdx2);
		std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> res = cfg->getCFGTable();
		Assert::IsTrue(expectedCfg == res);
		Assert::IsTrue(size_t(1) == res.size());
	}
	};
}
