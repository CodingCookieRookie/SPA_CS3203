#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/PatternContainerTypes.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPatternWhile) {
private:
	VarIndex varIdx1 = VarIndex(1);
	VarIndex varIdx2 = VarIndex(2);
	VarIndex varIdx3 = VarIndex(3);
	VarIndex varIdx4 = VarIndex(4);

	StmtIndex stmtIdx1 = StmtIndex(1);
	StmtIndex stmtIdx2 = StmtIndex(2);
	StmtIndex stmtIdx3 = StmtIndex(3);
	StmtIndex stmtIdx4 = StmtIndex(4);

public:
	TEST_METHOD(insert_getWhileStmtsFromVar_noStmts) {
		PatternWhile patternWhile = PatternWhile();

		patternWhile.insert(stmtIdx1, varIdx2);

		std::vector<StmtIndex> res1 = patternWhile.getStmtsFromVar(varIdx1);

		Assert::IsTrue(0 == res1.size());
	}

	TEST_METHOD(insert_getWhileStmtsFromVar_sameVariable_differentStmtIdx) {
		PatternWhile patternWhile = PatternWhile();

		std::vector<StmtIndex> expectedRes{ 1, 2 };

		patternWhile.insert(stmtIdx1, varIdx1);
		patternWhile.insert(stmtIdx2, varIdx1);
		patternWhile.insert(stmtIdx3, varIdx2);

		std::vector<StmtIndex> res1 = patternWhile.getStmtsFromVar(varIdx1);

		Assert::IsTrue(expectedRes == res1);
	}

	TEST_METHOD(insert_getAll_singleIfStmt_singleControlVariable) {
		// 1. while (x = 1) {
		// ...
		PatternWhile patternWhile = PatternWhile();

		std::vector<StmtIndex> stmtIndices{ 1 };
		std::vector<VarIndex> varIndices{ 1 };

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes = std::make_tuple(varIndices, stmtIndices);

		patternWhile.insert(stmtIdx1, varIdx1);

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res1 = patternWhile.getAll();

		Assert::IsTrue(expectedRes == res1);
	}

	TEST_METHOD(insert_getAll_singleIfStmt_multipleControlVariables) {
		// 1. while ((!(number != x)) && ((number <= y) || (y == z))) {
		// ...
		PatternWhile patternWhile = PatternWhile();

		std::vector<StmtIndex> stmtIndices{ 1, 1, 1, 1 };
		std::vector<VarIndex> varIndices{ 1, 2, 3, 4 };

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes = std::make_tuple(varIndices, stmtIndices);

		patternWhile.insert(stmtIdx1, varIdx1);
		patternWhile.insert(stmtIdx1, varIdx2);
		patternWhile.insert(stmtIdx1, varIdx3);
		patternWhile.insert(stmtIdx1, varIdx4);

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res1 = patternWhile.getAll();

		Assert::IsTrue(expectedRes == res1);
	}

	TEST_METHOD(insert_getAll_multipleIfStmts_multipleControlVariables) {
		// 1. while ((!(number != x)) && ((number <= y) || (y == z))) {
		// 2.		while (x = y) {
		// ...
		PatternWhile patternWhile = PatternWhile();

		std::vector<StmtIndex> stmtIndices{ 1, 1, 2, 1, 2, 1 };
		std::vector<VarIndex> varIndices{ 1, 2, 2, 3, 3, 4 };

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes = std::make_tuple(varIndices, stmtIndices);

		patternWhile.insert(stmtIdx1, varIdx1);
		patternWhile.insert(stmtIdx1, varIdx2);
		patternWhile.insert(stmtIdx1, varIdx3);
		patternWhile.insert(stmtIdx1, varIdx4);
		patternWhile.insert(stmtIdx2, varIdx2);
		patternWhile.insert(stmtIdx2, varIdx3);

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res1 = patternWhile.getAll();

		Assert::IsTrue(expectedRes == res1);
	}
	};
}
