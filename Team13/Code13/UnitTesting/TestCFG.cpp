#include "stdafx.h"
#include "CppUnitTest.h"

#include "SP/CFG.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestCFG) {
private:
	StmtIndex stmtIdx1 = StmtIndex(1);
	StmtIndex stmtIdx2 = StmtIndex(2);
	StmtIndex stmtIdx3 = StmtIndex(3);

public:
	TEST_METHOD(initCFG_getHead) {
		std::set<StmtIndex> stmtIndices1 = { stmtIdx1 , stmtIdx2 };
		std::set<StmtIndex> stmtIndices2 = { stmtIdx3 };
		CFGNode* expectedHeadNode = new CFGNode(stmtIndices1);
		CFGNode* expectedTailNode = new CFGNode(stmtIndices2);

		CFG cfg = CFG(expectedHeadNode, expectedTailNode);

		CFGNode* headNode = cfg.getHead();
		Assert::IsTrue(expectedHeadNode == headNode);
	}

	TEST_METHOD(initCFG_getTail) {
		std::set<StmtIndex> stmtIndices1 = { stmtIdx1 , stmtIdx2 };
		std::set<StmtIndex> stmtIndices2 = { stmtIdx3 };
		CFGNode* expectedHeadNode = new CFGNode(stmtIndices1);
		CFGNode* expectedTailNode = new CFGNode(stmtIndices2);

		CFG cfg = CFG(expectedHeadNode, expectedTailNode);

		CFGNode* tailNode = cfg.getTail();
		Assert::IsTrue(expectedTailNode == tailNode);
	}

	TEST_METHOD(initCFG_addToEnd) {
		std::set<StmtIndex> stmtIndices1 = { stmtIdx1 };
		std::set<StmtIndex> stmtIndices2 = { stmtIdx2 };
		std::set<StmtIndex> stmtIndices3 = { stmtIdx3 };

		CFGNode* expectedHeadNode = new CFGNode(stmtIndices1);
		CFGNode* expectedMidNode = new CFGNode(stmtIndices2);
		CFGNode* expectedTailNode = new CFGNode(stmtIndices3);

		CFG cfg = CFG(expectedHeadNode, expectedMidNode);
		cfg.addToEnd(expectedTailNode);

		CFGNode* headNode = cfg.getHead();
		Assert::IsTrue(expectedHeadNode == headNode);

		CFGNode* tailNode = cfg.getTail();
		Assert::IsTrue(expectedTailNode == tailNode);
	}
	};
}
