#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/PKBGetter.h"
#include "../source/PKB/PKBInserter.h"
#include "../source/QPS/Processors/NextTProcessor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestNextTProcessor) {
private:
	StmtIndex stmtIdx1 = { 1 };
	StmtIndex stmtIdx2 = { 2 };
	StmtIndex stmtIdx3 = { 3 };
	StmtIndex stmtIdx4 = { 4 };
	StmtIndex stmtIdx5 = { 5 };
	NextTProcessor* nextTProcessor;
	PKB pkb;
	PKBGetter* pkbGetter;
	PKBInserter* pkbInserter;

	TEST_METHOD_INITIALIZE(init) {
		NextTCache* nextTCache = new NextTCache();
		nextTProcessor = new NextTProcessor(nextTCache);
		pkb = PKB();
		pkbGetter = new PKBGetter(pkb);
		pkbInserter = new PKBInserter(pkb);
	}

public:

	void populateNoLoopLinear() {
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3);
	}

	void populateNoLoopBranched() {
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx3);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx4);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx5);
	}

	void populateLoopOneNode() {
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx1);
	}

	void populateLoopMidNodeToTopNode() {
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx1);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3);
	}

	void populateLoopEndNodeToTopNode() {
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx1);
	}

	void populateLoopEndNodeToMidNode() {
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx2);
	}

	void populateLoopMidAndEndNode() {
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx1);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx4);
	}

	void populateLoopAllNodes() {
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx3);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx1);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx1);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx2);
	}

	void populateDisjoint() {
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx4);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx4, stmtIdx5);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx5, stmtIdx4);
	}

	void populateNestedWhile() {
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx1);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx2);
	}

	void populateStronglyConnectedComponents() {
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx1);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx4);
		pkbInserter->insertRSInfo(RelationshipType::NEXT, stmtIdx4, stmtIdx5);
	}

	// ----------------- doesRsHold --------------------------------
	TEST_METHOD(doesRsHold_noLoopLinear) {
		populateNoLoopLinear();

		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx3, pkbGetter));
		Assert::IsFalse(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx1, pkbGetter));
		Assert::IsFalse(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx2, pkbGetter));
		Assert::IsFalse(nextTProcessor->doesRsHold(stmtIdx3, stmtIdx3, pkbGetter));
	};

	TEST_METHOD(doesRsHold_noLoopBranched) {
		populateNoLoopBranched();

		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx4, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx5, pkbGetter));

		Assert::IsFalse(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx3, pkbGetter));
		Assert::IsFalse(nextTProcessor->doesRsHold(stmtIdx3, stmtIdx2, pkbGetter));
		Assert::IsFalse(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx4, pkbGetter));
		Assert::IsFalse(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx5, pkbGetter));
	};

	TEST_METHOD(doesRsHold_loopOneNode) {
		populateLoopOneNode();

		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx1, pkbGetter));
	};

	TEST_METHOD(doesRsHold_loopMidNodeToTopNode) {
		populateLoopMidNodeToTopNode();

		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx3, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx1, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx2, pkbGetter));
		Assert::IsFalse(nextTProcessor->doesRsHold(stmtIdx3, stmtIdx3, pkbGetter));
	};

	TEST_METHOD(doesRsHold_loopEndNodeToTopNode) {
		populateLoopEndNodeToTopNode();

		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx3, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx3, stmtIdx2, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx1, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx2, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx3, stmtIdx3, pkbGetter));
	};

	TEST_METHOD(doesRsHold_loopEndNodeToMidNode) {
		populateLoopEndNodeToMidNode();

		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx3, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx2, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx3, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx3, stmtIdx2, pkbGetter));
	};

	TEST_METHOD(doesRsHold_loopMidAndEndNode) {
		populateLoopMidAndEndNode();

		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx3, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx4, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx3, stmtIdx2, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx3, stmtIdx4, pkbGetter));
	};

	TEST_METHOD(doesRsHold_loopAllNodes) {
		populateLoopAllNodes();
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx3, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx3, stmtIdx2, pkbGetter));
	};

	TEST_METHOD(doesRsHold_disjoint) {
		populateDisjoint();
		Assert::IsFalse(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx3, pkbGetter));
		Assert::IsFalse(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx4, pkbGetter));
		Assert::IsFalse(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx5, pkbGetter));
		Assert::IsFalse(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx3, pkbGetter));
		Assert::IsFalse(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx4, pkbGetter));
		Assert::IsFalse(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx5, pkbGetter));
	};

	TEST_METHOD(doesRsHold_nestedWhile) {
		populateNestedWhile();
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx1, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx2, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx3, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx1, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx2, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx3, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx3, stmtIdx1, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx3, stmtIdx2, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx3, stmtIdx3, pkbGetter));
	}

	TEST_METHOD(doesRsHold_stronglyConnectedComponents) {
		populateStronglyConnectedComponents();

		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx1, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx2, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx3, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx4, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx1, stmtIdx5, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx1, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx2, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx3, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx4, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx2, stmtIdx5, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx3, stmtIdx1, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx3, stmtIdx2, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx3, stmtIdx3, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx3, stmtIdx4, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx3, stmtIdx5, pkbGetter));
		Assert::IsTrue(nextTProcessor->doesRsHold(stmtIdx4, stmtIdx5, pkbGetter));
	}

	// ----------------- getUsingLeftStmtIndex --------------------------------
	TEST_METHOD(getUsingLeftStmtIndex_noLoopLinear) {
		populateNoLoopLinear();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx2, stmtIdx3} ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx1, pkbGetter));
	};

	TEST_METHOD(getUsingLeftStmtIndex_noLoopBranched) {
		populateNoLoopBranched();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx2, stmtIdx3, stmtIdx4, stmtIdx5} ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx4, stmtIdx5 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx3, pkbGetter));
	};

	TEST_METHOD(getUsingLeftStmtIndex_loopOneNode) {
		populateLoopOneNode();

		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx1 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx1, pkbGetter));
	};

	TEST_METHOD(getUsingLeftStmtIndex_loopMidNodeToTopNode) {
		populateLoopMidNodeToTopNode();

		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx2, stmtIdx1, stmtIdx3 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx1, stmtIdx3, stmtIdx2 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx3, pkbGetter));
	};

	TEST_METHOD(getUsingLeftStmtIndex_loopEndNodeToTopNode) {
		populateLoopEndNodeToTopNode();

		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx2, stmtIdx3, stmtIdx1 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx3, stmtIdx1, stmtIdx2 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx1, stmtIdx2, stmtIdx3 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx3, pkbGetter));
	};

	TEST_METHOD(getUsingLeftStmtIndex_loopEndNodeToMidNode) {
		populateLoopEndNodeToMidNode();

		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx2, stmtIdx3 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx3, stmtIdx2 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx2, stmtIdx3 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx3, pkbGetter));
	};

	TEST_METHOD(getUsingLeftStmtIndex_loopMidAndEndNode) {
		populateLoopMidAndEndNode();

		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx2, stmtIdx3, stmtIdx4, stmtIdx1 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx3, stmtIdx4, stmtIdx1, stmtIdx2 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx1, stmtIdx2, stmtIdx3, stmtIdx4 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx3, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx4, pkbGetter));
	};

	TEST_METHOD(getUsingLeftStmtIndex_loopAllNodes) {
		populateLoopAllNodes();

		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx2, stmtIdx3, stmtIdx1 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx1, stmtIdx3, stmtIdx2 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx1, stmtIdx2, stmtIdx3 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx3, pkbGetter));
	};

	TEST_METHOD(getUsingLeftStmtIndex_disjoint) {
		populateDisjoint();

		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx2 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx4, stmtIdx5 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx3, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx5, stmtIdx4 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx4, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx4, stmtIdx5 } ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx5, pkbGetter));
	};

	TEST_METHOD(getUsingLeftStmtIndex_nestedWhile) {
		populateNestedWhile();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx2, stmtIdx3, stmtIdx1} ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx3, stmtIdx1, stmtIdx2} ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx2, stmtIdx3, stmtIdx1} ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx3, pkbGetter));
	}

	TEST_METHOD(getUsingLeftStmtIndex_stronglyConnectedComponents) {
		populateStronglyConnectedComponents();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx2, stmtIdx4, stmtIdx3, stmtIdx5, stmtIdx1} ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx3, stmtIdx1, stmtIdx2, stmtIdx4, stmtIdx5} ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1, stmtIdx2, stmtIdx4, stmtIdx3, stmtIdx5} ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx3, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx5} ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx4, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			nextTProcessor->getUsingLeftStmtIndex(stmtIdx5, pkbGetter));
	}

	// ----------------- getUsingRightStmtIndex --------------------------------

	TEST_METHOD(getUsingRightStmtIndex_noLoopLinear) {
		populateNoLoopLinear();

		Assert::IsTrue(std::vector<StmtIndex> {} ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx1 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx2, stmtIdx1 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx3, pkbGetter));
	};

	TEST_METHOD(getUsingRightStmtIndex_noLoopBranched) {
		populateNoLoopBranched();

		Assert::IsTrue(std::vector<StmtIndex> {} ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx1 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx1 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx3, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx3, stmtIdx1 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx4, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx3, stmtIdx1 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx5, pkbGetter));
	};

	TEST_METHOD(getUsingRightStmtIndex_loopOneNode) {
		populateLoopOneNode();

		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx1 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx1, pkbGetter));
	};

	TEST_METHOD(getUsingRightStmtIndex_loopMidNodeToTopNode) {
		populateLoopMidNodeToTopNode();

		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx2, stmtIdx1 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx1, stmtIdx2 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx2, stmtIdx1 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx3, pkbGetter));
	};

	TEST_METHOD(getUsingRightStmtIndex_loopEndNodeToTopNode) {
		populateLoopEndNodeToTopNode();

		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx3, stmtIdx2, stmtIdx1 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx1, stmtIdx3, stmtIdx2 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx2, stmtIdx1, stmtIdx3 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx3, pkbGetter));
	};

	TEST_METHOD(getUsingRightStmtIndex_loopEndNodeToMidNode) {
		populateLoopEndNodeToMidNode();

		Assert::IsTrue(std::vector<StmtIndex> {} ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx1, stmtIdx3, stmtIdx2 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx2, stmtIdx1, stmtIdx3 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx3, pkbGetter));
	};

	TEST_METHOD(getUsingRightStmtIndex_loopMidAndEndNode) {
		populateLoopMidAndEndNode();

		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx3, stmtIdx2, stmtIdx1  } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx1, stmtIdx3, stmtIdx2 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx2, stmtIdx1, stmtIdx3 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx3, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx2, stmtIdx1, stmtIdx3 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx4, pkbGetter));
	};

	TEST_METHOD(getUsingRightStmtIndex_loopAllNodes) {
		populateLoopAllNodes();

		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx2, stmtIdx3, stmtIdx1 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx1, stmtIdx3, stmtIdx2 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx1, stmtIdx2, stmtIdx3 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx3, pkbGetter));
	};

	TEST_METHOD(getUsingRightStmtIndex_disjoint) {
		populateDisjoint();

		Assert::IsTrue(std::vector<StmtIndex> {} ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx1 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {} ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx3, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx3, stmtIdx5, stmtIdx4 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx4, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> { stmtIdx4, stmtIdx3, stmtIdx5 } ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx5, pkbGetter));
	};

	TEST_METHOD(getUsingRightStmtIndex_nestedWhile) {
		populateNestedWhile();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx2, stmtIdx1, stmtIdx3} ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1, stmtIdx3, stmtIdx2} ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx2, stmtIdx1, stmtIdx3} ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx3, pkbGetter));
	}

	TEST_METHOD(getUsingRightStmtIndex_stronglyConnectedComponents) {
		populateStronglyConnectedComponents();

		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx3, stmtIdx2, stmtIdx1} ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx1, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1, stmtIdx3, stmtIdx2} ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx2, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx2, stmtIdx1, stmtIdx3} ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx3, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx1, stmtIdx3, stmtIdx2} ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx4, pkbGetter));
		Assert::IsTrue(std::vector<StmtIndex> {stmtIdx4, stmtIdx1, stmtIdx3, stmtIdx2} ==
			nextTProcessor->getUsingRightStmtIndex(stmtIdx5, pkbGetter));
	}

	// ----------------- getAll --------------------------------

	TEST_METHOD(getAll_disjoint) {
		pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);
		pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		pkbInserter->insertStmt(StatementType::ASSIGN_TYPE);

		populateDisjoint();

		std::vector<StmtIndex> leftSynonyms = { stmtIdx1, stmtIdx3, stmtIdx3,
			stmtIdx4, stmtIdx4, stmtIdx5, stmtIdx5 };
		std::vector<StmtIndex> rightSynonyms = { stmtIdx2, stmtIdx4, stmtIdx5,
			stmtIdx5, stmtIdx4, stmtIdx4, stmtIdx5 };
		Assert::IsTrue(std::make_tuple(leftSynonyms, rightSynonyms) ==
			nextTProcessor->getAll(pkbGetter));
	};
	};
};
