#include "stdafx.h"
#include "CppUnitTest.h"

#include "SP/CFGNode.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestCFGNode) {
private:
	StmtIndex stmtIdx1 = StmtIndex(1);
	StmtIndex stmtIdx2 = StmtIndex(2);
	StmtIndex stmtIdx3 = StmtIndex(3);

public:
	TEST_METHOD(initEmptyNode_getStmtIndices) {
		CFGNode* node = new CFGNode();

		std::unordered_set<StmtIndex> res = node->getStmtIndices();
		Assert::IsTrue(0 == res.size());
	}

	TEST_METHOD(initEmptyNode_getNextNodes) {
		CFGNode* node = new CFGNode();

		std::unordered_set<CFGNode*> res = node->getNextNodes();
		Assert::IsTrue(0 == res.size());
	}

	TEST_METHOD(emptyNode_addNext_getNextNodes) {
		std::unordered_set<StmtIndex> stmtIndices = { stmtIdx1 };
		CFGNode* nextNode = new CFGNode(stmtIndices);

		std::unordered_set<CFGNode*> expectedRes = { nextNode };

		CFGNode* node = new CFGNode();
		node->addNext(nextNode);

		std::unordered_set<CFGNode*> res = node->getNextNodes();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(initNode_getStmtIndices) {
		std::unordered_set<StmtIndex> expectedRes = { stmtIdx1 , stmtIdx2 };

		std::unordered_set<StmtIndex> stmtIndices = { stmtIdx1 , stmtIdx2 };
		CFGNode* node = new CFGNode(stmtIndices);

		std::unordered_set<StmtIndex> res = node->getStmtIndices();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(initNode_getNextNodes) {
		std::unordered_set<CFGNode*> expectedRes = { };

		std::unordered_set<StmtIndex> stmtIndices = { stmtIdx1 , stmtIdx2 };
		CFGNode* node = new CFGNode(stmtIndices);

		std::unordered_set<CFGNode*> res = node->getNextNodes();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(addNext_getNextNodes) {
		std::unordered_set<StmtIndex> stmtIndices1 = { stmtIdx2 };
		std::unordered_set<StmtIndex> stmtIndices2 = { stmtIdx3 };
		CFGNode* nextNode1 = new CFGNode(stmtIndices1);
		CFGNode* nextNode2 = new CFGNode(stmtIndices2);

		std::unordered_set<CFGNode*> expectedRes = { nextNode1, nextNode2 };

		std::unordered_set<StmtIndex> stmtIndices = { stmtIdx1 };
		CFGNode* node = new CFGNode(stmtIndices);
		node->addNext(nextNode1);
		node->addNext(nextNode2);

		std::unordered_set<CFGNode*> res = node->getNextNodes();
		Assert::IsTrue(expectedRes == res);
	}
	};
}
