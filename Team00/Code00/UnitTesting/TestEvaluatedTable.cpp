#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS-NEW/EvaluatedTable.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestEvaluatedTable) {
public:
	TEST_METHOD(innerJoinMerge_noCommonColumns_crossProduct) {
		std::unordered_map<std::string, PqlEntityType> leftEntities = {
			{"s1", PqlEntityType::Stmt},
			{"v1", PqlEntityType::Variable}
		};
		std::unordered_map<std::string, std::vector<int>> leftTable = {
			{"s1", { 1, 3 } },
			{"v1", { 2, 4 } },
		};
		EvaluatedTable leftEvTable(leftEntities, leftTable);

		std::unordered_map<std::string, PqlEntityType> rightEntities = {
			{"s2", PqlEntityType::Stmt},
			{"v2", PqlEntityType::Variable}
		};
		std::unordered_map<std::string, std::vector<int>> rightTable = {
			{"s2", { 5, 7, 9} },
			{"v2", { 6, 8, 10} },
		};
		EvaluatedTable rightEvTable(rightEntities, rightTable);

		EvaluatedTable mergedTable = leftEvTable.innerJoinMerge(rightEvTable);
		Assert::AreEqual(size_t(4), mergedTable.getEntities().size());
		std::unordered_map<std::string, PqlEntityType> entities = mergedTable.getEntities();
		Assert::IsTrue(entities.find(std::string("s1")) != entities.end());
		Assert::IsTrue(entities.find(std::string("v1")) != entities.end());
		Assert::IsTrue(entities.find(std::string("s2")) != entities.end());
		Assert::IsTrue(entities.find(std::string("v2")) != entities.end());
		Assert::AreEqual(size_t(6), mergedTable.getNumRow());
	}

	TEST_METHOD(innerJoinMerge_commonColumnExists_innerJoin) {
		std::unordered_map<std::string, PqlEntityType> leftEntities = {
			{"s", PqlEntityType::Stmt},
			{"v1", PqlEntityType::Variable}
		};
		std::unordered_map<std::string, std::vector<int>> leftTable = {
			{"s", { 1, 3, 4} },
			{"v1", { 2, 4, 4} },
		};
		EvaluatedTable leftEvTable(leftEntities, leftTable);

		std::unordered_map<std::string, PqlEntityType> rightEntities = {
			{"s", PqlEntityType::Stmt},
			{"v2", PqlEntityType::Variable}
		};
		std::unordered_map<std::string, std::vector<int>> rightTable = {
			{"s", { 1, 3, 5} },
			{"v2", { 6, 8, 10} },
		};
		EvaluatedTable rightEvTable(rightEntities, rightTable);

		EvaluatedTable mergedTable = leftEvTable.innerJoinMerge(rightEvTable);
		Assert::AreEqual(size_t(3), mergedTable.getEntities().size());
		std::unordered_map<std::string, PqlEntityType> entities = mergedTable.getEntities();
		Assert::IsTrue(entities.find(std::string("s")) != entities.end());
		Assert::IsTrue(entities.find(std::string("v1")) != entities.end());
		Assert::IsTrue(entities.find(std::string("v2")) != entities.end());
		Assert::AreEqual(size_t(2), mergedTable.getNumRow());
	}

	TEST_METHOD(innerJoinMerge_falseLHS_returnsEmptyTable) {
		EvaluatedTable leftEvTable(false);

		std::unordered_map<std::string, PqlEntityType> rightEntities = {
			{"s", PqlEntityType::Stmt},
			{"v2", PqlEntityType::Variable}
		};
		std::unordered_map<std::string, std::vector<int>> rightTable = {
			{"s", { 1, 3, 5} },
			{"v2", { 6, 8, 10} },
		};
		EvaluatedTable rightEvTable(rightEntities, rightTable);

		EvaluatedTable mergedTable = leftEvTable.innerJoinMerge(rightEvTable);
		Assert::AreEqual(size_t(0), mergedTable.getEntities().size());
		Assert::IsFalse(mergedTable.getEvResult());
	}

	TEST_METHOD(innerJoinMerge_falseRHS_returnsEmptyTable) {
		std::unordered_map<std::string, PqlEntityType> leftEntities = {
			{"s", PqlEntityType::Stmt},
			{"v1", PqlEntityType::Variable}
		};
		std::unordered_map<std::string, std::vector<int>> leftTable = {
			{"s", { 1, 3, 4} },
			{"v1", { 2, 4, 4} },
		};
		EvaluatedTable leftEvTable(leftEntities, leftTable);

		EvaluatedTable rightEvTable(false);

		EvaluatedTable mergedTable = leftEvTable.innerJoinMerge(rightEvTable);
		Assert::AreEqual(size_t(0), mergedTable.getEntities().size());
		Assert::IsFalse(mergedTable.getEvResult());
	}

	TEST_METHOD(innerJoinMerge_trueLHS_returnsRHS) {
		EvaluatedTable leftEvTable(true);

		std::unordered_map<std::string, PqlEntityType> rightEntities = {
			{"s", PqlEntityType::Stmt},
			{"v2", PqlEntityType::Variable}
		};
		std::unordered_map<std::string, std::vector<int>> rightTable = {
			{"s", { 1, 3, 5} },
			{"v2", { 6, 8, 10} },
		};
		EvaluatedTable rightEvTable(rightEntities, rightTable);

		EvaluatedTable mergedTable = leftEvTable.innerJoinMerge(rightEvTable);
		Assert::AreEqual(size_t(3), mergedTable.getNumRow());
		Assert::AreEqual(size_t(2), mergedTable.getEntities().size());
	}

	TEST_METHOD(innerJoinMerge_trueRHS_returnsLHS) {
		std::unordered_map<std::string, PqlEntityType> leftEntities = {
			{"s", PqlEntityType::Stmt},
			{"v1", PqlEntityType::Variable}
		};
		std::unordered_map<std::string, std::vector<int>> leftTable = {
			{"s", { 1, 3, 4} },
			{"v1", { 2, 4, 4} },
		};
		EvaluatedTable leftEvTable(leftEntities, leftTable);

		EvaluatedTable rightEvTable(true);

		EvaluatedTable mergedTable = leftEvTable.innerJoinMerge(rightEvTable);
		Assert::AreEqual(size_t(3), mergedTable.getNumRow());
		Assert::AreEqual(size_t(2), mergedTable.getEntities().size());
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
