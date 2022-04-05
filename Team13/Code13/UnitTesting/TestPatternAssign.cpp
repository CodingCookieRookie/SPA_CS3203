#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/PatternAssign.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPatternAssign) {
private:
	std::string varName1 = "future";
	std::string varName2 = "present";
	std::string varName3 = "past";

	std::string postFix1 = " a b + ";
	std::string postFix2 = " 5 b * c % d / deab % ";
	std::string postFix3 = " a b * ";
	std::string postFix4 = " c a b * + ";

	VarIndex varIdx1 = VarIndex(1);
	VarIndex varIdx2 = VarIndex(2);
	VarIndex varIdx3 = VarIndex(3);
	VarIndex varIdx4 = VarIndex(4);

	StmtIndex stmtIdx1 = StmtIndex(1);
	StmtIndex stmtIdx2 = StmtIndex(2);
	StmtIndex stmtIdx3 = StmtIndex(3);
	StmtIndex stmtIdx4 = StmtIndex(4);
	StmtIndex stmtIdx5 = StmtIndex(5);

public:
	TEST_METHOD(insertAssignInfo_getAssignStmtsFromVar_sameVariable_differentStmtIdx) {
		PatternAssign patternAssign = PatternAssign();

		std::vector<StmtIndex> expectedRes{ 1, 2, 3 };

		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx2);
		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx3);

		std::vector<StmtIndex> res = patternAssign.getAssignStmtsFromVar(varIdx1);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromVar_sameVariable_differentStmtIdx_repeatedStmtIdx) {
		PatternAssign patternAssign = PatternAssign();

		std::vector<StmtIndex> expectedRes{ 1, 2, 3 };

		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx2);
		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx3);
		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx2);
		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx3);
		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx3);

		std::vector<StmtIndex> res = patternAssign.getAssignStmtsFromVar(varIdx1);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromVar_differentVariables_differentStmtIdx_repeatedStmtIdx) {
		PatternAssign patternAssign = PatternAssign();

		std::vector<StmtIndex> expectedRes{ 2, 1 };

		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		patternAssign.insertAssignInfo(varIdx2, postFix1, stmtIdx2);
		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx3);
		patternAssign.insertAssignInfo(varIdx2, postFix1, stmtIdx1);
		patternAssign.insertAssignInfo(varIdx2, postFix1, stmtIdx1);

		std::vector<StmtIndex> res = patternAssign.getAssignStmtsFromVar(varIdx2);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromVarExpr_sameExpression_exactExpression) {
		PatternAssign patternAssign = PatternAssign();

		std::vector<StmtIndex> expectedRes{ 1, 2 };

		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx2);
		patternAssign.insertAssignInfo(varIdx2, postFix1, stmtIdx3);
		patternAssign.insertAssignInfo(varIdx2, postFix1, stmtIdx2);

		std::vector<StmtIndex> res = patternAssign.getAssignStmtsFromVarExprFullMatch(varIdx1, postFix1);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromVarExpr_differentExpression_repeatedStmtIdx_exactExpression) {
		PatternAssign patternAssign = PatternAssign();

		std::vector<StmtIndex> expectedRes{ 1 };

		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		patternAssign.insertAssignInfo(varIdx1, postFix2, stmtIdx2);
		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		patternAssign.insertAssignInfo(varIdx2, postFix1, stmtIdx3);
		patternAssign.insertAssignInfo(varIdx2, postFix2, stmtIdx2);

		std::vector<StmtIndex> res = patternAssign.getAssignStmtsFromVarExprFullMatch(varIdx1, postFix1);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromVarExpr_sameExpression_subExpression_subExpressionQuery) {
		PatternAssign patternAssign = PatternAssign();

		std::vector<StmtIndex> expectedRes{ 1, 2 };

		patternAssign.insertAssignInfo(varIdx1, postFix4, stmtIdx1);
		patternAssign.insertAssignInfo(varIdx1, postFix4, stmtIdx2);
		patternAssign.insertAssignInfo(varIdx2, postFix4, stmtIdx3);

		/* postFix3 is a subexpression of postFix4 */
		std::vector<StmtIndex> res1 = patternAssign.getAssignStmtsFromVarExprPartialMatch(varIdx1, postFix3);
		std::vector<StmtIndex> res2 = patternAssign.getAssignStmtsFromVarExprPartialMatch(varIdx1, postFix4);

		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromVarExpr_sameExpression_subExpression_nonSubExpressionQuery) {
		PatternAssign patternAssign = PatternAssign();

		std::vector<StmtIndex> expectedRes{ 1 };

		patternAssign.insertAssignInfo(varIdx1, postFix4, stmtIdx1);
		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx2);
		patternAssign.insertAssignInfo(varIdx2, postFix4, stmtIdx3);

		/* postFix1 is a not subexpression of postFix4 */
		std::vector<StmtIndex> res = patternAssign.getAssignStmtsFromVarExprPartialMatch(varIdx1, postFix4);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromVarExpr_differentExpression_subExpression) {
		PatternAssign patternAssign = PatternAssign();

		std::vector<StmtIndex> expectedRes{ 1, 2 };

		patternAssign.insertAssignInfo(varIdx1, postFix4, stmtIdx1);
		patternAssign.insertAssignInfo(varIdx1, postFix4, stmtIdx2);
		patternAssign.insertAssignInfo(varIdx2, postFix4, stmtIdx3);

		/* postFix3 is a subexpression of postFix4 */
		std::vector<StmtIndex> res1 = patternAssign.getAssignStmtsFromVarExprPartialMatch(varIdx1, postFix3);
		std::vector<StmtIndex> res2 = patternAssign.getAssignStmtsFromVarExprPartialMatch(varIdx1, postFix4);

		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);

		/* Add subexpression postFix3 */
		expectedRes.push_back(3);
		patternAssign.insertAssignInfo(varIdx1, postFix3, stmtIdx3);

		std::vector<StmtIndex> res3 = patternAssign.getAssignStmtsFromVarExprPartialMatch(varIdx1, postFix3);
		std::vector<StmtIndex> res4 = patternAssign.getAssignStmtsFromVarExprPartialMatch(varIdx1, postFix4);

		Assert::IsTrue(expectedRes == res3);
		Assert::IsFalse(expectedRes == res4);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromExpr_sameExpression_exactExpression) {
		PatternAssign patternAssign = PatternAssign();

		std::vector<StmtIndex> stmtIndices{ 1, 2, 3 };
		std::vector<VarIndex> varIndices{ 1, 1, 2 };

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes = std::make_tuple(varIndices, stmtIndices);

		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx2);
		patternAssign.insertAssignInfo(varIdx2, postFix1, stmtIdx3);

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res = patternAssign.getAssignStmtsFromExprFullMatch(postFix1);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromExpr_differentExpression_exactExpression) {
		PatternAssign patternAssign = PatternAssign();

		std::vector<StmtIndex> stmtIndices{ 2, 3 };
		std::vector<VarIndex> varIndices{ 1, 2 };

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes = std::make_tuple(varIndices, stmtIndices);

		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		patternAssign.insertAssignInfo(varIdx1, postFix2, stmtIdx2);
		patternAssign.insertAssignInfo(varIdx2, postFix1, stmtIdx3);
		patternAssign.insertAssignInfo(varIdx2, postFix2, stmtIdx3);

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res = patternAssign.getAssignStmtsFromExprFullMatch(postFix2);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromExpr_sameExpression_subExpression_subExpressionQuery) {
		PatternAssign patternAssign = PatternAssign();

		std::vector<StmtIndex> stmtIndices{ 1, 2, 3 };
		std::vector<VarIndex> varIndices{ 1, 1, 2 };

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes = std::make_tuple(varIndices, stmtIndices);

		patternAssign.insertAssignInfo(varIdx1, postFix4, stmtIdx1);
		patternAssign.insertAssignInfo(varIdx1, postFix4, stmtIdx2);
		patternAssign.insertAssignInfo(varIdx2, postFix4, stmtIdx3);

		/* postFix3 is a subexpression of postFix4 */
		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res1 = patternAssign.getAssignStmtsFromExprPartialMatch(postFix3);
		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res2 = patternAssign.getAssignStmtsFromExprPartialMatch(postFix4);
		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromExpr_sameExpression_subExpression_nonSubExpressionQuery) {
		PatternAssign patternAssign = PatternAssign();

		std::vector<StmtIndex> stmtIndices{ 2 };
		std::vector<VarIndex> varIndices{ 3 };

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes = std::make_tuple(varIndices, stmtIndices);

		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx2);
		patternAssign.insertAssignInfo(varIdx2, postFix1, stmtIdx3);
		patternAssign.insertAssignInfo(varIdx3, postFix4, stmtIdx2);

		/* postFix1 is a not subexpression of postFix4 */
		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res = patternAssign.getAssignStmtsFromExprPartialMatch(postFix4);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromExpr_differentExpression_subExpression) {
		PatternAssign patternAssign = PatternAssign();

		std::vector<StmtIndex> stmtIndices{ 1, 2, 4 };
		std::vector<VarIndex> varIndices{ 1, 1, 2 };

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes = std::make_tuple(varIndices, stmtIndices);

		patternAssign.insertAssignInfo(varIdx1, postFix4, stmtIdx1);
		patternAssign.insertAssignInfo(varIdx1, postFix4, stmtIdx2);
		patternAssign.insertAssignInfo(varIdx1, postFix2, stmtIdx3);
		patternAssign.insertAssignInfo(varIdx2, postFix4, stmtIdx4);

		/* postFix3 is a subexpression of postFix4 */
		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res1 = patternAssign.getAssignStmtsFromExprPartialMatch(postFix3);
		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res2 = patternAssign.getAssignStmtsFromExprPartialMatch(postFix4);
		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);

		/* Add subexpression postFix3 */
		stmtIndices.insert(stmtIndices.begin(), 5);
		varIndices.insert(varIndices.begin(), 3);
		expectedRes = std::make_tuple(varIndices, stmtIndices);

		patternAssign.insertAssignInfo(varIdx3, postFix3, stmtIdx5);

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res3 = patternAssign.getAssignStmtsFromExprPartialMatch(postFix3);
		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res4 = patternAssign.getAssignStmtsFromExprPartialMatch(postFix4);

		Assert::IsTrue(expectedRes == res3);
		Assert::IsFalse(expectedRes == res4);
	}

	TEST_METHOD(insertAssignInfo_getAllAssignPatternInfo) {
		PatternAssign patternAssign = PatternAssign();

		std::vector<StmtIndex> stmtIndices{ 1, 2, 3, 3 };
		std::vector<VarIndex> varIndices{ 1, 1, 1, 2 };

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> expectedRes = std::make_tuple(varIndices, stmtIndices);

		patternAssign.insertAssignInfo(varIdx1, postFix4, stmtIdx1);
		patternAssign.insertAssignInfo(varIdx1, postFix1, stmtIdx2);
		patternAssign.insertAssignInfo(varIdx1, postFix2, stmtIdx3);
		patternAssign.insertAssignInfo(varIdx2, postFix4, stmtIdx3);

		std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> res = patternAssign.getAllAssignPatternInfo();

		Assert::IsTrue(expectedRes == res);
	}
	};
}
