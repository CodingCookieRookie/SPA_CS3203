#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "PKB/BidirectionalTable/BidirectionalIndexTable.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestBidirectionalIndexTable) {
public:
	TEST_METHOD(insert_checkIdx_procedure_differentElements) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		std::string procName1 = "main";
		std::string procName2 = "test";

		BidirectionalIndexTable<ProcIndex> bidirectionalTable = BidirectionalIndexTable<ProcIndex>();
		ProcIndex idx1 = bidirectionalTable.insert(procName1);
		ProcIndex idx2 = bidirectionalTable.insert(procName2);

		Assert::IsTrue(procIdx1 == idx1);
		Assert::IsTrue(procIdx2 == idx2);
	}

	TEST_METHOD(insert_checkIdx_procedure_sameElement) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		std::string procName = "main";

		BidirectionalIndexTable<ProcIndex> bidirectionalTable = BidirectionalIndexTable<ProcIndex>();
		ProcIndex idx1 = bidirectionalTable.insert(procName);
		ProcIndex idx2 = bidirectionalTable.insert(procName);

		Assert::IsTrue(procIdx1 == idx1);
		Assert::IsTrue(procIdx1 == idx2);
	}

	TEST_METHOD(insert_checkIdx_variable_differentElements) {
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		std::string varName1 = "x";
		std::string varName2 = "y";

		BidirectionalIndexTable<VarIndex> bidirectionalTable = BidirectionalIndexTable<VarIndex>();
		VarIndex idx1 = bidirectionalTable.insert(varName1);
		VarIndex idx2 = bidirectionalTable.insert(varName2);

		Assert::IsTrue(varIdx1 == idx1);
		Assert::IsTrue(varIdx2 == idx2);
	}

	TEST_METHOD(insert_checkIdx_variable_sameElement) {
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		std::string varName = "x";

		BidirectionalIndexTable<VarIndex> bidirectionalTable = BidirectionalIndexTable<VarIndex>();
		ProcIndex idx1 = bidirectionalTable.insert(varName);
		ProcIndex idx2 = bidirectionalTable.insert(varName);

		Assert::IsTrue(varIdx1 == idx1);
		Assert::IsTrue(varIdx1 == idx2);
	}

	TEST_METHOD(insert_checkIdx_name_differentElements) {
		NameIndex nameIdx1 = NameIndex(1);
		NameIndex nameIdx2 = NameIndex(2);
		std::string name1 = "procedure";
		std::string name2 = "variable";

		BidirectionalIndexTable<NameIndex> bidirectionalTable = BidirectionalIndexTable<NameIndex>();
		NameIndex idx1 = bidirectionalTable.insert(name1);
		NameIndex idx2 = bidirectionalTable.insert(name2);

		Assert::IsTrue(nameIdx1 == idx1);
		Assert::IsTrue(nameIdx2 == idx2);
	}

	TEST_METHOD(insert_checkIdx_name_sameElement) {
		NameIndex nameIdx1 = NameIndex(1);
		NameIndex nameIdx2 = NameIndex(2);
		std::string name = "procedure";

		BidirectionalIndexTable<NameIndex> bidirectionalTable = BidirectionalIndexTable<NameIndex>();
		ProcIndex idx1 = bidirectionalTable.insert(name);
		ProcIndex idx2 = bidirectionalTable.insert(name);

		Assert::IsTrue(nameIdx1 == idx1);
		Assert::IsTrue(nameIdx1 == idx2);
	}

	TEST_METHOD(insert_contains_procedure_differentElements) {
		std::string procName1 = "main";
		std::string procName2 = "test";

		BidirectionalIndexTable<ProcIndex> bidirectionalTable = BidirectionalIndexTable<ProcIndex>();
		bidirectionalTable.insert(procName1);
		bidirectionalTable.insert(procName2);

		Assert::IsTrue(bidirectionalTable.contains(procName1));
		Assert::IsTrue(bidirectionalTable.contains(procName2));
	}

	TEST_METHOD(insert_contains_procedure_sameElement) {
		std::string procName1 = "main";
		std::string procName2 = "test";

		BidirectionalIndexTable<ProcIndex> bidirectionalTable = BidirectionalIndexTable<ProcIndex>();
		bidirectionalTable.insert(procName1);
		bidirectionalTable.insert(procName1);

		Assert::IsTrue(bidirectionalTable.contains(procName1));
		Assert::IsFalse(bidirectionalTable.contains(procName2));
	}

	TEST_METHOD(insert_contains_variable_differentElements) {
		std::string varName1 = "x";
		std::string varName2 = "y";

		BidirectionalIndexTable<VarIndex> bidirectionalTable = BidirectionalIndexTable<VarIndex>();
		bidirectionalTable.insert(varName1);
		bidirectionalTable.insert(varName2);

		Assert::IsTrue(bidirectionalTable.contains(varName1));
		Assert::IsTrue(bidirectionalTable.contains(varName2));
	}

	TEST_METHOD(insert_contains_variable_sameElement) {
		std::string varName1 = "x";
		std::string varName2 = "y";

		BidirectionalIndexTable<VarIndex> bidirectionalTable = BidirectionalIndexTable<VarIndex>();
		bidirectionalTable.insert(varName1);
		bidirectionalTable.insert(varName1);

		Assert::IsTrue(bidirectionalTable.contains(varName1));
		Assert::IsFalse(bidirectionalTable.contains(varName2));
	}

	TEST_METHOD(insert_contains_name_differentElements) {
		std::string name1 = "procedure";
		std::string name2 = "variable";

		BidirectionalIndexTable<NameIndex> bidirectionalTable = BidirectionalIndexTable<NameIndex>();
		bidirectionalTable.insert(name1);
		bidirectionalTable.insert(name2);

		Assert::IsTrue(bidirectionalTable.contains(name1));
		Assert::IsTrue(bidirectionalTable.contains(name2));
	}

	TEST_METHOD(insert_contains_name_sameElement) {
		std::string name1 = "procedure";
		std::string name2 = "variable";

		BidirectionalIndexTable<NameIndex> bidirectionalTable = BidirectionalIndexTable<NameIndex>();
		bidirectionalTable.insert(name1);
		bidirectionalTable.insert(name1);

		Assert::IsTrue(bidirectionalTable.contains(name1));
		Assert::IsFalse(bidirectionalTable.contains(name2));
	}

	TEST_METHOD(insert_getStringFromIndex_procedure_differentElements) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		std::string procName1 = "main";
		std::string procName2 = "test";

		BidirectionalIndexTable<ProcIndex> bidirectionalTable = BidirectionalIndexTable<ProcIndex>();
		bidirectionalTable.insert(procName1);
		bidirectionalTable.insert(procName2);

		std::string res1 = bidirectionalTable.getStringFromIndex(procIdx1);
		std::string res2 = bidirectionalTable.getStringFromIndex(procIdx2);
		Assert::IsTrue(procName1 == res1);
		Assert::IsTrue(procName2 == res2);
	}

	TEST_METHOD(insert_getStringFromIndex_procedure_sameElement) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		std::string procName = "main";

		BidirectionalIndexTable<ProcIndex> bidirectionalTable = BidirectionalIndexTable<ProcIndex>();
		bidirectionalTable.insert(procName);
		bidirectionalTable.insert(procName);

		std::string res1 = bidirectionalTable.getStringFromIndex(procIdx1);
		std::string res2 = bidirectionalTable.getStringFromIndex(procIdx2);
		Assert::IsTrue(procName == res1);
		Assert::IsFalse(procName == res2);
	}

	TEST_METHOD(insert_getStringFromIndex_variable_differentElements) {
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		std::string varName1 = "x";
		std::string varName2 = "y";

		BidirectionalIndexTable<VarIndex> bidirectionalTable = BidirectionalIndexTable<VarIndex>();
		bidirectionalTable.insert(varName1);
		bidirectionalTable.insert(varName2);

		std::string res1 = bidirectionalTable.getStringFromIndex(varIdx1);
		std::string res2 = bidirectionalTable.getStringFromIndex(varIdx2);
		Assert::IsTrue(varName1 == res1);
		Assert::IsTrue(varName2 == res2);
	}

	TEST_METHOD(insert_getStringFromIndex_variable_sameElement) {
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		std::string varName = "x";

		BidirectionalIndexTable<VarIndex> bidirectionalTable = BidirectionalIndexTable<VarIndex>();
		bidirectionalTable.insert(varName);
		bidirectionalTable.insert(varName);

		std::string res1 = bidirectionalTable.getStringFromIndex(varIdx1);
		std::string res2 = bidirectionalTable.getStringFromIndex(varIdx2);
		Assert::IsTrue(varName == res1);
		Assert::IsFalse(varName == res2);
	}

	TEST_METHOD(insert_getStringFromIndex_name_differentElements) {
		NameIndex nameIdx1 = NameIndex(1);
		NameIndex nameIdx2 = NameIndex(2);
		std::string name1 = "procedure";
		std::string name2 = "variable";

		BidirectionalIndexTable<NameIndex> bidirectionalTable = BidirectionalIndexTable<NameIndex>();
		bidirectionalTable.insert(name1);
		bidirectionalTable.insert(name2);

		std::string res1 = bidirectionalTable.getStringFromIndex(nameIdx1);
		std::string res2 = bidirectionalTable.getStringFromIndex(nameIdx2);
		Assert::IsTrue(name1 == res1);
		Assert::IsTrue(name2 == res2);
	}

	TEST_METHOD(insert_getStringFromIndex_name_sameElement) {
		NameIndex nameIdx1 = NameIndex(1);
		NameIndex nameIdx2 = NameIndex(2);
		std::string name = "procedure";

		BidirectionalIndexTable<NameIndex> bidirectionalTable = BidirectionalIndexTable<NameIndex>();
		bidirectionalTable.insert(name);
		bidirectionalTable.insert(name);

		std::string res1 = bidirectionalTable.getStringFromIndex(nameIdx1);
		std::string res2 = bidirectionalTable.getStringFromIndex(nameIdx2);
		Assert::IsTrue(name == res1);
		Assert::IsFalse(name == res2);
	}

	TEST_METHOD(insert_getIndexFromString_procedure_differentElements) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		std::string procName1 = "main";
		std::string procName2 = "test";

		BidirectionalIndexTable<ProcIndex> bidirectionalTable = BidirectionalIndexTable<ProcIndex>();
		bidirectionalTable.insert(procName1);
		bidirectionalTable.insert(procName2);

		ProcIndex res1 = bidirectionalTable.getIndexFromString(procName1);
		ProcIndex res2 = bidirectionalTable.getIndexFromString(procName2);
		Assert::IsTrue(procIdx1 == res1);
		Assert::IsTrue(procIdx2 == res2);
	}

	TEST_METHOD(insert_getIndexFromString_procedure_sameElement) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		std::string procName1 = "main";
		std::string procName2 = "test";

		BidirectionalIndexTable<ProcIndex> bidirectionalTable = BidirectionalIndexTable<ProcIndex>();
		bidirectionalTable.insert(procName1);
		bidirectionalTable.insert(procName1);

		ProcIndex res1 = bidirectionalTable.getIndexFromString(procName1);
		ProcIndex res2 = bidirectionalTable.getIndexFromString(procName2);
		Assert::IsTrue(procIdx1 == res1);
		Assert::IsFalse(procIdx2 == res2);
	}

	TEST_METHOD(insert_getIndexFromString_variable_differentElements) {
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		std::string varName1 = "x";
		std::string varName2 = "y";

		BidirectionalIndexTable<VarIndex> bidirectionalTable = BidirectionalIndexTable<VarIndex>();
		bidirectionalTable.insert(varName1);
		bidirectionalTable.insert(varName2);

		ProcIndex res1 = bidirectionalTable.getIndexFromString(varName1);
		ProcIndex res2 = bidirectionalTable.getIndexFromString(varName2);
		Assert::IsTrue(varIdx1 == res1);
		Assert::IsTrue(varIdx2 == res2);
	}

	TEST_METHOD(insert_getIndexFromString_variable_sameElement) {
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		std::string varName1 = "x";
		std::string varName2 = "y";

		BidirectionalIndexTable<VarIndex> bidirectionalTable = BidirectionalIndexTable<VarIndex>();
		bidirectionalTable.insert(varName1);
		bidirectionalTable.insert(varName1);

		ProcIndex res1 = bidirectionalTable.getIndexFromString(varName1);
		ProcIndex res2 = bidirectionalTable.getIndexFromString(varName2);
		Assert::IsTrue(varIdx1 == res1);
		Assert::IsFalse(varIdx2 == res2);
	}

	TEST_METHOD(insert_getIndexFromString_name_differentElements) {
		NameIndex nameIdx1 = NameIndex(1);
		NameIndex nameIdx2 = NameIndex(2);
		std::string name1 = "procedure";
		std::string name2 = "variable";

		BidirectionalIndexTable<NameIndex> bidirectionalTable = BidirectionalIndexTable<NameIndex>();
		bidirectionalTable.insert(name1);
		bidirectionalTable.insert(name2);

		ProcIndex res1 = bidirectionalTable.getIndexFromString(name1);
		ProcIndex res2 = bidirectionalTable.getIndexFromString(name2);
		Assert::IsTrue(nameIdx1 == res1);
		Assert::IsTrue(nameIdx2 == res2);
	}

	TEST_METHOD(insert_getIndexFromString_name_sameElement) {
		NameIndex nameIdx1 = NameIndex(1);
		NameIndex nameIdx2 = NameIndex(2);
		std::string name1 = "procedure";
		std::string name2 = "variable";

		BidirectionalIndexTable<NameIndex> bidirectionalTable = BidirectionalIndexTable<NameIndex>();
		bidirectionalTable.insert(name1);
		bidirectionalTable.insert(name1);

		ProcIndex res1 = bidirectionalTable.getIndexFromString(name1);
		ProcIndex res2 = bidirectionalTable.getIndexFromString(name2);
		Assert::IsTrue(nameIdx1 == res1);
		Assert::IsFalse(nameIdx2 == res2);
	}

	TEST_METHOD(insert_getAll_procedure_differentElements) {
		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		std::string procName1 = "main";
		std::string procName2 = "test";

		std::vector<ProcIndex> expectedRes{ procIdx1 , procIdx2 };

		BidirectionalIndexTable<ProcIndex> bidirectionalTable = BidirectionalIndexTable<ProcIndex>();
		bidirectionalTable.insert(procName1);
		bidirectionalTable.insert(procName2);

		std::vector<ProcIndex> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_procedure_sameElement) {
		ProcIndex procIdx1 = ProcIndex(1);
		std::string procName = "main";

		std::vector<ProcIndex> expectedRes{ procIdx1 };

		BidirectionalIndexTable<ProcIndex> bidirectionalTable = BidirectionalIndexTable<ProcIndex>();
		bidirectionalTable.insert(procName);
		bidirectionalTable.insert(procName);

		std::vector<ProcIndex> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_variable_differentElements) {
		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		std::string varName1 = "x";
		std::string varName2 = "y";

		std::vector<ProcIndex> expectedRes{ varIdx1 , varIdx2 };

		BidirectionalIndexTable<VarIndex> bidirectionalTable = BidirectionalIndexTable<VarIndex>();
		bidirectionalTable.insert(varName1);
		bidirectionalTable.insert(varName2);

		std::vector<ProcIndex> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_variable_sameElement) {
		VarIndex varIdx1 = VarIndex(1);
		std::string varName = "x";

		std::vector<ProcIndex> expectedRes{ varIdx1 };

		BidirectionalIndexTable<VarIndex> bidirectionalTable = BidirectionalIndexTable<VarIndex>();
		bidirectionalTable.insert(varName);
		bidirectionalTable.insert(varName);

		std::vector<ProcIndex> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_name_differentElements) {
		NameIndex nameIdx1 = NameIndex(1);
		NameIndex nameIdx2 = NameIndex(2);
		std::string name1 = "procedure";
		std::string name2 = "variable";

		std::vector<ProcIndex> expectedRes{ nameIdx1 , nameIdx2 };

		BidirectionalIndexTable<NameIndex> bidirectionalTable = BidirectionalIndexTable<NameIndex>();
		bidirectionalTable.insert(name1);
		bidirectionalTable.insert(name2);

		std::vector<ProcIndex> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insert_getAll_name_sameElement) {
		NameIndex nameIdx1 = NameIndex(1);
		std::string name = "procedure";

		std::vector<ProcIndex> expectedRes{ nameIdx1 };

		BidirectionalIndexTable<NameIndex> bidirectionalTable = BidirectionalIndexTable<NameIndex>();
		bidirectionalTable.insert(name);
		bidirectionalTable.insert(name);

		std::vector<ProcIndex> res = bidirectionalTable.getAll();
		Assert::IsTrue(expectedRes == res);
	}
	};
}
