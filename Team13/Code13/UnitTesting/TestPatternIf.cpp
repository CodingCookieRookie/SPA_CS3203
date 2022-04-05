#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/PatternContainerTypes.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPatternIf) {
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
	TEST_METHOD(insert_getIfStmtsFromVar_noStmts) {
		PatternIf patternIf = PatternIf();

		patternIf.insert(stmtIdx1, varIdx2);

		std::vector<StmtIndex> res1 = patternIf.getStmtsFromVar(varIdx1);

		Assert::IsTrue(0 == res1.size());
	}

	TEST_METHOD(insert_getIfStmtsFromVar_sameVariable_differentStmtIdx) {
		PatternIf patternIf = PatternIf();

		std::vector<StmtIndex> expectedRes{ 1, 2 };

		patternIf.insert(stmtIdx1, varIdx1);
		patternIf.insert(stmtIdx2, varIdx1);
		patternIf.insert(stmtIdx3, varIdx2);

		std::vector<StmtIndex> res1 = patternIf.getStmtsFromVar(varIdx1);

		Assert::IsTrue(expectedRes == res1);
	}

	TEST_METHOD(insert_getAllIfPatternInfo_singleIfStmt_singleControlVariable) {
		// 1. if (x = 1) then {
		// ...
		PatternIf patternIf = PatternIf();

		std::vector<StmtIndex> stmtIndices{ 1 };
		std::vector<VarIndex> varIndices{ 1 };

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes = std::make_tuple(varIndices, stmtIndices);

		patternIf.insert(stmtIdx1, varIdx1);

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res1 = patternIf.getAll();

		Assert::IsTrue(expectedRes == res1);
	}

	TEST_METHOD(insert_getAllIfPatternInfo_singleIfStmt_multipleControlVariables) {
		// 1. if ((!(number != x)) && ((number <= y) || (y == z))) then {
		// ...
		PatternIf patternIf = PatternIf();

		std::vector<StmtIndex> stmtIndices{ 1, 1, 1, 1 };
		std::vector<VarIndex> varIndices{ 1, 2, 3, 4 };

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes = std::make_tuple(varIndices, stmtIndices);

		patternIf.insert(stmtIdx1, varIdx1);
		patternIf.insert(stmtIdx1, varIdx2);
		patternIf.insert(stmtIdx1, varIdx3);
		patternIf.insert(stmtIdx1, varIdx4);

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res1 = patternIf.getAll();

		Assert::IsTrue(expectedRes == res1);
	}

	TEST_METHOD(insert_getAllIfPatternInfo_multipleIfStmts_multipleControlVariables) {
		// 1. if ((!(number != x)) && ((number <= y) || (y == z))) then {
		// 2.		if (x = y) then {
		// ...
		PatternIf patternIf = PatternIf();

		std::vector<StmtIndex> stmtIndices{ 1, 1, 2, 1, 2, 1 };
		std::vector<VarIndex> varIndices{ 1, 2, 2, 3, 3, 4 };

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes = std::make_tuple(varIndices, stmtIndices);

		patternIf.insert(stmtIdx1, varIdx1);
		patternIf.insert(stmtIdx1, varIdx2);
		patternIf.insert(stmtIdx1, varIdx3);
		patternIf.insert(stmtIdx1, varIdx4);
		patternIf.insert(stmtIdx2, varIdx2);
		patternIf.insert(stmtIdx2, varIdx3);

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res1 = patternIf.getAll();

		Assert::IsTrue(expectedRes == res1);
	}
	};
}
