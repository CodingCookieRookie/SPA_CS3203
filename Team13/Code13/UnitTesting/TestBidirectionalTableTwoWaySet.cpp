#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "PKB/BidirectionalTable/BidirectionalTableTwoWaySet.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestBidirectionalTableTwoWaySet) {
public:
	TEST_METHOD(insert_contains_DifferentSynonymsRelationshipS_sameStmt) {
		StmtIndex stmtIdx = StmtIndex(1);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		VarIndex varIdx3 = VarIndex(3);

		BidirectionalTableTwoWaySet<VarIndex, StmtIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx);
		bidirectionalTable.insert(varIdx2, stmtIdx);
		bidirectionalTable.insert(varIdx3, stmtIdx);

		Assert::IsTrue(bidirectionalTable.contains(varIdx1, stmtIdx));
		Assert::IsTrue(bidirectionalTable.contains(varIdx2, stmtIdx));
		Assert::IsTrue(bidirectionalTable.contains(varIdx3, stmtIdx));
	}

	TEST_METHOD(insert_contains_DifferentSynonymsRelationshipS_differentStmt) {
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx3 = VarIndex(3);

		BidirectionalTableTwoWaySet<VarIndex, StmtIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx1);
		bidirectionalTable.insert(varIdx1, stmtIdx2);
		bidirectionalTable.insert(varIdx3, stmtIdx1);

		Assert::IsTrue(bidirectionalTable.contains(varIdx1, stmtIdx1));
		Assert::IsTrue(bidirectionalTable.contains(varIdx1, stmtIdx2));
		Assert::IsTrue(bidirectionalTable.contains(varIdx3, stmtIdx1));
		Assert::IsFalse(bidirectionalTable.contains(varIdx3, stmtIdx2));
	}

	TEST_METHOD(insert_contains_DifferentSynonymsRelationshipP_sameProc) {
		ProcIndex procIdx = ProcIndex(1);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		VarIndex varIdx3 = VarIndex(3);

		BidirectionalTableTwoWaySet<VarIndex, ProcIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<VarIndex, ProcIndex>();
		bidirectionalTable.insert(varIdx1, procIdx);
		bidirectionalTable.insert(varIdx2, procIdx);
		bidirectionalTable.insert(varIdx3, procIdx);

		Assert::IsTrue(bidirectionalTable.contains(varIdx1, procIdx));
		Assert::IsTrue(bidirectionalTable.contains(varIdx2, procIdx));
		Assert::IsTrue(bidirectionalTable.contains(varIdx3, procIdx));
	}

	TEST_METHOD(insert_contains_DifferentSynonymsRelationshipP_differentProc) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx3 = VarIndex(3);

		BidirectionalTableTwoWaySet<VarIndex, ProcIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<VarIndex, ProcIndex>();
		bidirectionalTable.insert(varIdx1, procIdx1);
		bidirectionalTable.insert(varIdx1, procIdx2);
		bidirectionalTable.insert(varIdx3, procIdx1);

		Assert::IsTrue(bidirectionalTable.contains(varIdx1, procIdx1));
		Assert::IsTrue(bidirectionalTable.contains(varIdx1, procIdx2));
		Assert::IsTrue(bidirectionalTable.contains(varIdx3, procIdx1));
		Assert::IsFalse(bidirectionalTable.contains(varIdx3, procIdx2));
	}

	TEST_METHOD(insert_getFromLeftArg_DifferentSynonymsRelationshipS_sameStmt) {
		StmtIndex stmtIdx = StmtIndex(1);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		VarIndex varIdx3 = VarIndex(3);

		std::vector<StmtIndex> expectedRes{ stmtIdx };

		BidirectionalTableTwoWaySet<VarIndex, StmtIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx);
		bidirectionalTable.insert(varIdx2, stmtIdx);
		bidirectionalTable.insert(varIdx3, stmtIdx);

		std::vector<StmtIndex> res1 = bidirectionalTable.getFromLeftArg(varIdx1);
		std::vector<StmtIndex> res2 = bidirectionalTable.getFromLeftArg(varIdx2);
		std::vector<StmtIndex> res3 = bidirectionalTable.getFromLeftArg(varIdx3);
		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);
		Assert::IsTrue(expectedRes == res3);
	}

	TEST_METHOD(insert_getFromLeftArg_DifferentSynonymsRelationshipS_differentStmt) {
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx3 = VarIndex(3);

		std::vector<StmtIndex> expectedRes1{ stmtIdx1 , stmtIdx2 };
		std::vector<StmtIndex> expectedRes2{ stmtIdx1 };

		BidirectionalTableTwoWaySet<VarIndex, StmtIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx1);
		bidirectionalTable.insert(varIdx1, stmtIdx2);
		bidirectionalTable.insert(varIdx3, stmtIdx1);

		std::vector<StmtIndex> res1 = bidirectionalTable.getFromLeftArg(varIdx1);
		std::vector<StmtIndex> res2 = bidirectionalTable.getFromLeftArg(varIdx3);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes2 == res2);
	}

	TEST_METHOD(insert_getFromLeftArg_DifferentSynonymsRelationshipP_sameProc) {
		ProcIndex procIdx = ProcIndex(1);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		VarIndex varIdx3 = VarIndex(3);

		std::vector<ProcIndex> expectedRes{ procIdx };

		BidirectionalTableTwoWaySet<VarIndex, ProcIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<VarIndex, ProcIndex>();
		bidirectionalTable.insert(varIdx1, procIdx);
		bidirectionalTable.insert(varIdx2, procIdx);
		bidirectionalTable.insert(varIdx3, procIdx);

		std::vector<ProcIndex> res1 = bidirectionalTable.getFromLeftArg(varIdx1);
		std::vector<ProcIndex> res2 = bidirectionalTable.getFromLeftArg(varIdx2);
		std::vector<ProcIndex> res3 = bidirectionalTable.getFromLeftArg(varIdx3);
		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);
		Assert::IsTrue(expectedRes == res3);
	}

	TEST_METHOD(insert_getFromLeftArg_DifferentSynonymsRelationshipP_differentProc) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx3 = VarIndex(3);

		std::vector<ProcIndex> expectedRes1{ procIdx2, procIdx1 };
		std::vector<ProcIndex> expectedRes2{ procIdx2 };

		BidirectionalTableTwoWaySet<VarIndex, ProcIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<VarIndex, ProcIndex>();
		bidirectionalTable.insert(varIdx1, procIdx2);
		bidirectionalTable.insert(varIdx1, procIdx1);
		bidirectionalTable.insert(varIdx3, procIdx2);

		std::vector<ProcIndex> res1 = bidirectionalTable.getFromLeftArg(varIdx1);
		std::vector<ProcIndex> res2 = bidirectionalTable.getFromLeftArg(varIdx3);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes2 == res2);
	}

	TEST_METHOD(insert_getFromRightArg_DifferentSynonymsRelationshipS_sameStmt) {
		StmtIndex stmtIdx = StmtIndex(1);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		VarIndex varIdx3 = VarIndex(3);

		std::vector<VarIndex> expectedRes{ varIdx1 , varIdx2, varIdx3 };

		BidirectionalTableTwoWaySet<VarIndex, StmtIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx);
		bidirectionalTable.insert(varIdx2, stmtIdx);
		bidirectionalTable.insert(varIdx3, stmtIdx);

		std::vector<VarIndex> res = bidirectionalTable.getFromRightArg(stmtIdx);
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getFromRightArg_DifferentSynonymsRelationshipS_differentStmt) {
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx3 = VarIndex(3);

		std::vector<VarIndex> expectedRes1{ varIdx1 , varIdx3 };
		std::vector<VarIndex> expectedRes2{ varIdx1 };

		BidirectionalTableTwoWaySet<VarIndex, StmtIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx1);
		bidirectionalTable.insert(varIdx1, stmtIdx2);
		bidirectionalTable.insert(varIdx3, stmtIdx1);

		std::vector<VarIndex> res1 = bidirectionalTable.getFromRightArg(stmtIdx1);
		std::vector<VarIndex> res2 = bidirectionalTable.getFromRightArg(stmtIdx2);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes2 == res2);
	}

	TEST_METHOD(insert_getFromRightArg_DifferentSynonymsRelationshipP_sameProc) {
		ProcIndex procIdx = ProcIndex(1);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		VarIndex varIdx3 = VarIndex(3);

		std::vector<VarIndex> expectedRes{ varIdx1, varIdx2, varIdx3 };

		BidirectionalTableTwoWaySet<VarIndex, ProcIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<VarIndex, ProcIndex>();
		bidirectionalTable.insert(varIdx1, procIdx);
		bidirectionalTable.insert(varIdx2, procIdx);
		bidirectionalTable.insert(varIdx3, procIdx);

		std::vector<VarIndex> res = bidirectionalTable.getFromRightArg(procIdx);
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getFromRightArg_DifferentSynonymsRelationshipP_differentProc) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx3 = VarIndex(3);

		std::vector<VarIndex> expectedRes1{ varIdx1 , varIdx3 };
		std::vector<VarIndex> expectedRes2{ varIdx1 };

		BidirectionalTableTwoWaySet<VarIndex, ProcIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<VarIndex, ProcIndex>();
		bidirectionalTable.insert(varIdx1, procIdx1);
		bidirectionalTable.insert(varIdx1, procIdx2);
		bidirectionalTable.insert(varIdx3, procIdx1);

		std::vector<VarIndex> res1 = bidirectionalTable.getFromRightArg(procIdx1);
		std::vector<VarIndex> res2 = bidirectionalTable.getFromRightArg(procIdx2);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes2 == res2);
	}

	TEST_METHOD(insert_getAll_DifferentSynonymsRelationshipS_sameStmt) {
		StmtIndex stmtIdx = StmtIndex(1);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		VarIndex varIdx3 = VarIndex(3);

		std::vector<VarIndex> expectedTList{ varIdx1 , varIdx2 , varIdx3 };
		std::vector<StmtIndex> expectedUList{ stmtIdx , stmtIdx, stmtIdx };
		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes =
			std::make_tuple(expectedTList, expectedUList);

		BidirectionalTableTwoWaySet<VarIndex, StmtIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx);
		bidirectionalTable.insert(varIdx2, stmtIdx);
		bidirectionalTable.insert(varIdx3, stmtIdx);

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_DifferentSynonymsRelationshipS_differentStmt) {
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx3 = VarIndex(3);

		std::vector<VarIndex> expectedTList{ varIdx1 , varIdx1 , varIdx3 };
		std::vector<StmtIndex> expectedUList{ stmtIdx1 , stmtIdx2, stmtIdx1 };
		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes =
			std::make_tuple(expectedTList, expectedUList);

		BidirectionalTableTwoWaySet<VarIndex, StmtIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx1);
		bidirectionalTable.insert(varIdx1, stmtIdx2);
		bidirectionalTable.insert(varIdx3, stmtIdx1);

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_DifferentSynonymsRelationshipP_sameProc) {
		ProcIndex procIdx = ProcIndex(1);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		VarIndex varIdx3 = VarIndex(3);

		std::vector<VarIndex> expectedTList{ varIdx1 , varIdx2 , varIdx3 };
		std::vector<ProcIndex> expectedUList{ procIdx , procIdx, procIdx };
		std::tuple<std::vector<VarIndex>, std::vector<ProcIndex>> expectedRes =
			std::make_tuple(expectedTList, expectedUList);

		BidirectionalTableTwoWaySet<VarIndex, ProcIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<VarIndex, ProcIndex>();
		bidirectionalTable.insert(varIdx1, procIdx);
		bidirectionalTable.insert(varIdx2, procIdx);
		bidirectionalTable.insert(varIdx3, procIdx);

		std::tuple<std::vector<VarIndex>, std::vector<ProcIndex>> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_DifferentSynonymsRelationshipP_differentProc) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);

		std::vector<VarIndex> expectedTList{ varIdx1 , varIdx1 , varIdx2 };
		std::vector<ProcIndex> expectedUList{ procIdx1 , procIdx2, procIdx1 };
		std::tuple<std::vector<VarIndex>, std::vector<ProcIndex>> expectedRes =
			std::make_tuple(expectedTList, expectedUList);

		BidirectionalTableTwoWaySet<VarIndex, ProcIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<VarIndex, ProcIndex>();
		bidirectionalTable.insert(varIdx1, procIdx1);
		bidirectionalTable.insert(varIdx1, procIdx2);
		bidirectionalTable.insert(varIdx2, procIdx1);

		std::tuple<std::vector<VarIndex>, std::vector<ProcIndex>> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_SameSynonymsRelationship_Calls) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		ProcIndex procIdx3 = ProcIndex(3);
		ProcIndex procIdx4 = ProcIndex(4);

		std::vector<ProcIndex> expectedTList{ procIdx1 , procIdx2 , procIdx2 };
		std::vector<ProcIndex> expectedUList{ procIdx2 , procIdx3, procIdx4 };
		std::tuple<std::vector<ProcIndex>, std::vector<ProcIndex>> expectedRes =
			std::make_tuple(expectedTList, expectedUList);

		BidirectionalTableTwoWaySet<ProcIndex, ProcIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<ProcIndex, ProcIndex>();
		bidirectionalTable.insert(procIdx1, procIdx2);
		bidirectionalTable.insert(procIdx2, procIdx3);
		bidirectionalTable.insert(procIdx2, procIdx4);

		std::tuple<std::vector<ProcIndex>, std::vector<ProcIndex>> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_SameSynonymsRelationship_Follows) {
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);

		std::vector<ProcIndex> expectedTList{ stmtIdx1 , stmtIdx2 , stmtIdx3 };
		std::vector<ProcIndex> expectedUList{ stmtIdx2 , stmtIdx3, stmtIdx4 };
		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> expectedRes =
			std::make_tuple(expectedTList, expectedUList);

		BidirectionalTableTwoWaySet<StmtIndex, StmtIndex> bidirectionalTable =
			BidirectionalTableTwoWaySet<StmtIndex, StmtIndex>();
		bidirectionalTable.insert(stmtIdx1, stmtIdx2);
		bidirectionalTable.insert(stmtIdx2, stmtIdx3);
		bidirectionalTable.insert(stmtIdx3, stmtIdx4);

		std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}
	};
}
