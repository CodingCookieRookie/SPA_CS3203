#include "stdafx.h"
#include "CppUnitTest.h"

#include "PKB/PKBCFG.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPKBCFG) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		PKBCFG::performCleanUp();
	}

public:
	TEST_METHOD(insert_getCFGTable) {
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		std::set<StmtIndex> stmtIndices1 = { stmtIdx1 };
		std::set<StmtIndex> stmtIndices2 = { stmtIdx2 };
		CFGNode* head = new CFGNode(stmtIndices1);
		CFGNode* tail = new CFGNode(stmtIndices2);
		CFG cfg = CFG(head, tail);

		PKBCFG::insert(cfg);
		std::vector<CFG> res = PKBCFG::getCFGTable();
		Assert::IsTrue(1 == res.size());
	}

	TEST_METHOD(performCleanUp) {
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		std::set<StmtIndex> stmtIndices1 = { stmtIdx1 };
		std::set<StmtIndex> stmtIndices2 = { stmtIdx2 };
		CFGNode* head = new CFGNode(stmtIndices1);
		CFGNode* tail = new CFGNode(stmtIndices2);
		CFG cfg = CFG(head, tail);

		PKBCFG::insert(cfg);
		std::vector<CFG> res1 = PKBCFG::getCFGTable();
		Assert::IsTrue(1 == res1.size());

		PKBCFG::performCleanUp();

		std::vector<CFG> res2 = PKBCFG::getCFGTable();
		Assert::IsTrue(0 == res2.size());
	}
	};
}
