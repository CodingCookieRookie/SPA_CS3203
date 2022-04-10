#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS/EvaluatedTable.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestEvaluatedTable) {
public:
	TEST_METHOD(innerJoinMerge_noCommonColumns_crossProduct) {
		Table leftTable = {
			{"s1", { 1, 3 } },
			{"v1", { 2, 4 } },
		};
		EvaluatedTable leftEvTable(leftTable);

		Table rightTable = {
			{"s2", { 5, 7, 9} },
			{"v2", { 6, 8, 10} },
		};
		EvaluatedTable rightEvTable(rightTable);

		EvaluatedTable mergedTable = leftEvTable.innerJoinMerge(rightEvTable);
		Assert::AreEqual(size_t(4), mergedTable.getTableRef().size());
		Table table = mergedTable.getTableRef();
		Assert::IsTrue(table.find(std::string("s1")) != table.end());
		Assert::IsTrue(table.find(std::string("v1")) != table.end());
		Assert::IsTrue(table.find(std::string("s2")) != table.end());
		Assert::IsTrue(table.find(std::string("v2")) != table.end());
		Assert::AreEqual(size_t(6), mergedTable.getNumRow());
	}

	TEST_METHOD(innerJoinMerge_commonColumnExists_innerJoin) {
		Table leftTable = {
			{"s", { 1, 3, 4} },
			{"v1", { 2, 4, 4} },
		};
		EvaluatedTable leftEvTable(leftTable);

		Table rightTable = {
			{"s", { 1, 3, 5} },
			{"v2", { 6, 8, 10} },
		};
		EvaluatedTable rightEvTable(rightTable);

		EvaluatedTable mergedTable = leftEvTable.innerJoinMerge(rightEvTable);
		Assert::AreEqual(size_t(3), mergedTable.getTableRef().size());
		Table table = mergedTable.getTableRef();
		Assert::IsTrue(table.find(std::string("s")) != table.end());
		Assert::IsTrue(table.find(std::string("v1")) != table.end());
		Assert::IsTrue(table.find(std::string("v2")) != table.end());
		Assert::AreEqual(size_t(2), mergedTable.getNumRow());
	}

	TEST_METHOD(innerJoinMerge_falseLHS_returnsEmptyTable) {
		EvaluatedTable leftEvTable(false);

		Table rightTable = {
			{"s", { 1, 3, 5} },
			{"v2", { 6, 8, 10} },
		};
		EvaluatedTable rightEvTable(rightTable);

		EvaluatedTable mergedTable = leftEvTable.innerJoinMerge(rightEvTable);
		Assert::AreEqual(size_t(0), mergedTable.getTableRef().size());
		Assert::IsFalse(mergedTable.getEvResult());
	}

	TEST_METHOD(innerJoinMerge_falseRHS_returnsEmptyTable) {
		Table leftTable = {
			{"s", { 1, 3, 4} },
			{"v1", { 2, 4, 4} },
		};
		EvaluatedTable leftEvTable(leftTable);

		EvaluatedTable rightEvTable(false);

		EvaluatedTable mergedTable = leftEvTable.innerJoinMerge(rightEvTable);
		Assert::AreEqual(size_t(0), mergedTable.getTableRef().size());
		Assert::IsFalse(mergedTable.getEvResult());
	}

	TEST_METHOD(innerJoinMerge_trueLHS_returnsRHS) {
		EvaluatedTable leftEvTable(true);

		Table rightTable = {
			{"s", { 1, 3, 5} },
			{"v2", { 6, 8, 10} },
		};
		EvaluatedTable rightEvTable(rightTable);

		EvaluatedTable mergedTable = leftEvTable.innerJoinMerge(rightEvTable);
		Assert::AreEqual(size_t(3), mergedTable.getNumRow());
		Assert::AreEqual(size_t(2), mergedTable.getTableRef().size());
	}

	TEST_METHOD(innerJoinMerge_trueRHS_returnsLHS) {
		Table leftTable = {
			{"s", { 1, 3, 4} },
			{"v1", { 2, 4, 4} },
		};
		EvaluatedTable leftEvTable(leftTable);

		EvaluatedTable rightEvTable(true);

		EvaluatedTable mergedTable = leftEvTable.innerJoinMerge(rightEvTable);
		Assert::AreEqual(size_t(3), mergedTable.getNumRow());
		Assert::AreEqual(size_t(2), mergedTable.getTableRef().size());
	}

	TEST_METHOD(innerJoinMerge_trueLHSAndRHS_returnsTrue) {
		EvaluatedTable leftEvTable(true);

		EvaluatedTable rightEvTable(true);

		EvaluatedTable mergedTable = leftEvTable.innerJoinMerge(rightEvTable);
		Assert::AreEqual(size_t(0), mergedTable.getNumRow());
		Assert::IsTrue(mergedTable.getEvResult());
	}
	};
}
