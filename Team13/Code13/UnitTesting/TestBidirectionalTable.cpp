#include "stdafx.h"
#include "CppUnitTest.h"

#include "PKB/BidirectionalTable.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestBidirectionalTable) {
public:
	TEST_METHOD(insert_contains_RS1S) {
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx3 = VarIndex(3);

		BidirectionalTable<VarIndex, StmtIndex> bidirectionalTable = BidirectionalTable<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx1);
		bidirectionalTable.insert(varIdx1, stmtIdx2);
		bidirectionalTable.insert(varIdx3, stmtIdx1);

		Assert::IsTrue(bidirectionalTable.contains(varIdx1, stmtIdx1));
		Assert::IsTrue(bidirectionalTable.contains(varIdx1, stmtIdx2));
		Assert::IsTrue(bidirectionalTable.contains(varIdx3, stmtIdx1));
		Assert::IsFalse(bidirectionalTable.contains(varIdx3, stmtIdx2));
	}

	TEST_METHOD(insert_contains_RS1P) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx3 = VarIndex(3);

		BidirectionalTable<VarIndex, ProcIndex> bidirectionalTable = BidirectionalTable<VarIndex, ProcIndex>();
		bidirectionalTable.insert(varIdx1, procIdx1);
		bidirectionalTable.insert(varIdx1, procIdx2);
		bidirectionalTable.insert(varIdx3, procIdx1);

		Assert::IsTrue(bidirectionalTable.contains(varIdx1, procIdx1));
		Assert::IsTrue(bidirectionalTable.contains(varIdx1, procIdx2));
		Assert::IsTrue(bidirectionalTable.contains(varIdx3, procIdx1));
		Assert::IsFalse(bidirectionalTable.contains(varIdx3, procIdx2));
	}

	TEST_METHOD(insert_getFromLeftArg_RS1S) {
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx3 = VarIndex(3);

		std::vector<StmtIndex> expectedRes1{ stmtIdx1 , stmtIdx2 };
		std::vector<StmtIndex> expectedRes2{ stmtIdx1 };

		BidirectionalTable<VarIndex, StmtIndex> bidirectionalTable = BidirectionalTable<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx1);
		bidirectionalTable.insert(varIdx1, stmtIdx2);
		bidirectionalTable.insert(varIdx3, stmtIdx1);

		std::vector<StmtIndex> res1 = bidirectionalTable.getFromLeftArg(varIdx1);
		std::vector<StmtIndex> res2 = bidirectionalTable.getFromLeftArg(varIdx3);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes2 == res2);
	}

	TEST_METHOD(insert_getFromLeftArg_RS1P) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx3 = VarIndex(3);

		std::vector<ProcIndex> expectedRes1{ procIdx2, procIdx1 };
		std::vector<ProcIndex> expectedRes2{ procIdx2 };

		BidirectionalTable<VarIndex, ProcIndex> bidirectionalTable = BidirectionalTable<VarIndex, ProcIndex>();
		bidirectionalTable.insert(varIdx1, procIdx2);
		bidirectionalTable.insert(varIdx1, procIdx1);
		bidirectionalTable.insert(varIdx3, procIdx2);

		std::vector<ProcIndex> res1 = bidirectionalTable.getFromLeftArg(varIdx1);
		std::vector<ProcIndex> res2 = bidirectionalTable.getFromLeftArg(varIdx3);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes2 == res2);
	}

	TEST_METHOD(insert_getFromRightArg_RS1S) {
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx3 = VarIndex(3);

		std::vector<VarIndex> expectedRes1{ varIdx1 , varIdx3 };
		std::vector<VarIndex> expectedRes2{ varIdx1 };

		BidirectionalTable<VarIndex, StmtIndex> bidirectionalTable = BidirectionalTable<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx1);
		bidirectionalTable.insert(varIdx1, stmtIdx2);
		bidirectionalTable.insert(varIdx3, stmtIdx1);

		std::vector<VarIndex> res1 = bidirectionalTable.getFromRightArg(stmtIdx1);
		std::vector<VarIndex> res2 = bidirectionalTable.getFromRightArg(stmtIdx2);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes2 == res2);
	}

	TEST_METHOD(insert_getFromRightArg_RS1P) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx3 = VarIndex(3);

		std::vector<VarIndex> expectedRes1{ varIdx1 , varIdx3 };
		std::vector<VarIndex> expectedRes2{ varIdx1 };

		BidirectionalTable<VarIndex, ProcIndex> bidirectionalTable = BidirectionalTable<VarIndex, ProcIndex>();
		bidirectionalTable.insert(varIdx1, procIdx1);
		bidirectionalTable.insert(varIdx1, procIdx2);
		bidirectionalTable.insert(varIdx3, procIdx1);

		std::vector<VarIndex> res1 = bidirectionalTable.getFromRightArg(procIdx1);
		std::vector<VarIndex> res2 = bidirectionalTable.getFromRightArg(procIdx2);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes2 == res2);
	}

	TEST_METHOD(insert_getAll_RS1S) {
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx3 = VarIndex(3);

		std::vector<VarIndex> expectedTList{ varIdx1 , varIdx1 , varIdx3 };
		std::vector<StmtIndex> expectedUList{ stmtIdx1 , stmtIdx2, stmtIdx1 };
		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes = std::make_tuple(expectedTList, expectedUList);

		BidirectionalTable<VarIndex, StmtIndex> bidirectionalTable = BidirectionalTable<VarIndex, StmtIndex>();
		bidirectionalTable.insert(varIdx1, stmtIdx1);
		bidirectionalTable.insert(varIdx1, stmtIdx2);
		bidirectionalTable.insert(varIdx3, stmtIdx1);

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_RS1P) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);

		std::vector<VarIndex> expectedTList{ varIdx1 , varIdx1 , varIdx2 };
		std::vector<ProcIndex> expectedUList{ procIdx1 , procIdx2, procIdx1 };
		std::tuple<std::vector<VarIndex>, std::vector<ProcIndex>> expectedRes = std::make_tuple(expectedTList, expectedUList);

		BidirectionalTable<VarIndex, ProcIndex> bidirectionalTable = BidirectionalTable<VarIndex, ProcIndex>();
		bidirectionalTable.insert(varIdx1, procIdx1);
		bidirectionalTable.insert(varIdx1, procIdx2);
		bidirectionalTable.insert(varIdx2, procIdx1);

		std::tuple<std::vector<VarIndex>, std::vector<ProcIndex>> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_RS2_Calls) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		ProcIndex procIdx3 = ProcIndex(3);
		ProcIndex procIdx4 = ProcIndex(4);

		std::vector<ProcIndex> expectedTList{ procIdx1 , procIdx2 , procIdx2 };
		std::vector<ProcIndex> expectedUList{ procIdx2 , procIdx3, procIdx4 };
		std::tuple<std::vector<ProcIndex>, std::vector<ProcIndex>> expectedRes = std::make_tuple(expectedTList, expectedUList);

		BidirectionalTable<ProcIndex, ProcIndex> bidirectionalTable = BidirectionalTable<ProcIndex, ProcIndex>();
		bidirectionalTable.insert(procIdx1, procIdx2);
		bidirectionalTable.insert(procIdx2, procIdx3);
		bidirectionalTable.insert(procIdx2, procIdx4);

		std::tuple<std::vector<ProcIndex>, std::vector<ProcIndex>> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}
	};
}
