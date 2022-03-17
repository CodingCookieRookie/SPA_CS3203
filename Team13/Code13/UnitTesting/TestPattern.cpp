#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "../source/PKB/Pattern.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPattern) {
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

	TEST_METHOD_CLEANUP(cleanUpPatternInfo) {
		Pattern::performCleanUp();
	}

public:
	TEST_METHOD(insertAssignInfo_getAssignStmtsFromVar_sameVariable_differentStmtIdx) {
		std::vector<StmtIndex> expectedRes{ 1, 2, 3 };

		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx2);
		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx3);

		std::vector<StmtIndex> res = Pattern::getAssignStmtsFromVar(varIdx1);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromVar_sameVariable_differentStmtIdx_repeatedStmtIdx) {
		std::vector<StmtIndex> expectedRes{ 1, 2, 3 };

		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx2);
		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx3);
		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx2);
		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx3);
		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx3);

		std::vector<StmtIndex> res = Pattern::getAssignStmtsFromVar(varIdx1);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromVar_differentVariables_differentStmtIdx_repeatedStmtIdx) {
		std::vector<StmtIndex> expectedRes{ 2, 1 };

		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertAssignInfo(varIdx2, postFix1, stmtIdx2);
		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx3);
		Pattern::insertAssignInfo(varIdx2, postFix1, stmtIdx1);
		Pattern::insertAssignInfo(varIdx2, postFix1, stmtIdx1);

		std::vector<StmtIndex> res = Pattern::getAssignStmtsFromVar(varIdx2);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromVarExpr_sameExpression_exactExpression) {
		std::vector<StmtIndex> expectedRes{ 1, 2 };

		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx2);
		Pattern::insertAssignInfo(varIdx2, postFix1, stmtIdx3);
		Pattern::insertAssignInfo(varIdx2, postFix1, stmtIdx2);

		std::vector<StmtIndex> res = Pattern::getAssignStmtsFromVarExprFullMatch(varIdx1, postFix1);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromVarExpr_differentExpression_repeatedStmtIdx_exactExpression) {
		std::vector<StmtIndex> expectedRes{ 1 };

		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertAssignInfo(varIdx1, postFix2, stmtIdx2);
		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertAssignInfo(varIdx2, postFix1, stmtIdx3);
		Pattern::insertAssignInfo(varIdx2, postFix2, stmtIdx2);

		std::vector<StmtIndex> res = Pattern::getAssignStmtsFromVarExprFullMatch(varIdx1, postFix1);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromVarExpr_sameExpression_subExpression_subExpressionQuery) {
		std::vector<StmtIndex> expectedRes{ 1, 2 };

		Pattern::insertAssignInfo(varIdx1, postFix4, stmtIdx1);
		Pattern::insertAssignInfo(varIdx1, postFix4, stmtIdx2);
		Pattern::insertAssignInfo(varIdx2, postFix4, stmtIdx3);

		/* postFix3 is a subexpression of postFix4 */
		std::vector<StmtIndex> res1 = Pattern::getAssignStmtsFromVarExprPartialMatch(varIdx1, postFix3);
		std::vector<StmtIndex> res2 = Pattern::getAssignStmtsFromVarExprPartialMatch(varIdx1, postFix4);

		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromVarExpr_sameExpression_subExpression_nonSubExpressionQuery) {
		std::vector<StmtIndex> expectedRes{ 1 };

		Pattern::insertAssignInfo(varIdx1, postFix4, stmtIdx1);
		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx2);
		Pattern::insertAssignInfo(varIdx2, postFix4, stmtIdx3);

		/* postFix1 is a not subexpression of postFix4 */
		std::vector<StmtIndex> res = Pattern::getAssignStmtsFromVarExprPartialMatch(varIdx1, postFix4);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromVarExpr_differentExpression_subExpression) {
		std::vector<StmtIndex> expectedRes{ 1, 2 };

		Pattern::insertAssignInfo(varIdx1, postFix4, stmtIdx1);
		Pattern::insertAssignInfo(varIdx1, postFix4, stmtIdx2);
		Pattern::insertAssignInfo(varIdx2, postFix4, stmtIdx3);

		/* postFix3 is a subexpression of postFix4 */
		std::vector<StmtIndex> res1 = Pattern::getAssignStmtsFromVarExprPartialMatch(varIdx1, postFix3);
		std::vector<StmtIndex> res2 = Pattern::getAssignStmtsFromVarExprPartialMatch(varIdx1, postFix4);

		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);

		/* Add subexpression postFix3 */
		expectedRes.push_back(3);
		Pattern::insertAssignInfo(varIdx1, postFix3, stmtIdx3);

		std::vector<StmtIndex> res3 = Pattern::getAssignStmtsFromVarExprPartialMatch(varIdx1, postFix3);
		std::vector<StmtIndex> res4 = Pattern::getAssignStmtsFromVarExprPartialMatch(varIdx1, postFix4);

		Assert::IsTrue(expectedRes == res3);
		Assert::IsFalse(expectedRes == res4);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromExpr_sameExpression_exactExpression) {
		std::vector<StmtIndex> stmtIndices{ 1, 2, 3 };
		std::vector<VarIndex> varIndices{ 1, 1, 2 };

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx2);
		Pattern::insertAssignInfo(varIdx2, postFix1, stmtIdx3);

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res = Pattern::getAssignStmtsFromExprFullMatch(postFix1);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromExpr_differentExpression_exactExpression) {
		std::vector<StmtIndex> stmtIndices{ 2, 3 };
		std::vector<VarIndex> varIndices{ 1, 2 };

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertAssignInfo(varIdx1, postFix2, stmtIdx2);
		Pattern::insertAssignInfo(varIdx2, postFix1, stmtIdx3);
		Pattern::insertAssignInfo(varIdx2, postFix2, stmtIdx3);

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res = Pattern::getAssignStmtsFromExprFullMatch(postFix2);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromExpr_sameExpression_subExpression_subExpressionQuery) {
		std::vector<StmtIndex> stmtIndices{ 1, 2, 3 };
		std::vector<VarIndex> varIndices{ 1, 1, 2 };

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertAssignInfo(varIdx1, postFix4, stmtIdx1);
		Pattern::insertAssignInfo(varIdx1, postFix4, stmtIdx2);
		Pattern::insertAssignInfo(varIdx2, postFix4, stmtIdx3);

		/* postFix3 is a subexpression of postFix4 */
		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res1 = Pattern::getAssignStmtsFromExprPartialMatch(postFix3);
		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res2 = Pattern::getAssignStmtsFromExprPartialMatch(postFix4);
		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromExpr_sameExpression_subExpression_nonSubExpressionQuery) {
		std::vector<StmtIndex> stmtIndices{ 2 };
		std::vector<VarIndex> varIndices{ 3 };

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx2);
		Pattern::insertAssignInfo(varIdx2, postFix1, stmtIdx3);
		Pattern::insertAssignInfo(varIdx3, postFix4, stmtIdx2);

		/* postFix1 is a not subexpression of postFix4 */
		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res = Pattern::getAssignStmtsFromExprPartialMatch(postFix4);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertAssignInfo_getAssignStmtsFromExpr_differentExpression_subExpression) {
		std::vector<StmtIndex> stmtIndices{ 1, 2, 3 };
		std::vector<VarIndex> varIndices{ 1, 1, 2 };

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertAssignInfo(varIdx1, postFix4, stmtIdx1);
		Pattern::insertAssignInfo(varIdx1, postFix4, stmtIdx2);
		Pattern::insertAssignInfo(varIdx1, postFix2, stmtIdx3);
		Pattern::insertAssignInfo(varIdx2, postFix4, stmtIdx3);

		/* postFix3 is a subexpression of postFix4 */
		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res1 = Pattern::getAssignStmtsFromExprPartialMatch(postFix3);
		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res2 = Pattern::getAssignStmtsFromExprPartialMatch(postFix4);
		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);

		/* Add subexpression postFix3 */
		stmtIndices.insert(stmtIndices.begin(), 3);
		varIndices.insert(varIndices.begin(), 3);
		expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertAssignInfo(varIdx3, postFix3, stmtIdx3);

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res3 = Pattern::getAssignStmtsFromExprPartialMatch(postFix3);
		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res4 = Pattern::getAssignStmtsFromExprPartialMatch(postFix4);

		Assert::IsTrue(expectedRes == res3);
		Assert::IsFalse(expectedRes == res4);
	}

	TEST_METHOD(insertIfInfo_getIfStmtsFromVar_noStmts) {
		Pattern::insertIfInfo(stmtIdx1, varIdx2);

		std::vector<StmtIndex> res1 = Pattern::getIfStmtsFromVar(varIdx1);

		Assert::IsTrue(0 == res1.size());

		std::vector<StmtIndex> res2 = Pattern::getWhileStmtsFromVar(varIdx1);
		Assert::IsTrue(0 == res2.size());
	}

	TEST_METHOD(insertIfInfo_getIfStmtsFromVar_sameVariable_differentStmtIdx) {
		std::vector<StmtIndex> expectedRes{ 1, 2 };

		Pattern::insertIfInfo(stmtIdx1, varIdx1);
		Pattern::insertIfInfo(stmtIdx2, varIdx1);
		Pattern::insertIfInfo(stmtIdx3, varIdx2);

		std::vector<StmtIndex> res1 = Pattern::getIfStmtsFromVar(varIdx1);

		Assert::IsTrue(expectedRes == res1);

		std::vector<StmtIndex> res2 = Pattern::getWhileStmtsFromVar(varIdx1);
		Assert::IsTrue(0 == res2.size());
	}

	TEST_METHOD(insertWhileInfo_getWhileStmtsFromVar_noStmts) {
		Pattern::insertWhileInfo(stmtIdx1, varIdx2);

		std::vector<StmtIndex> res1 = Pattern::getWhileStmtsFromVar(varIdx1);

		Assert::IsTrue(0 == res1.size());

		std::vector<StmtIndex> res2 = Pattern::getIfStmtsFromVar(varIdx1);
		Assert::IsTrue(0 == res2.size());
	}

	TEST_METHOD(insertWhileInfo_getWhileStmtsFromVar_sameVariable_differentStmtIdx) {
		std::vector<StmtIndex> expectedRes{ 1, 2 };

		Pattern::insertWhileInfo(stmtIdx1, varIdx1);
		Pattern::insertWhileInfo(stmtIdx2, varIdx1);
		Pattern::insertWhileInfo(stmtIdx3, varIdx2);

		std::vector<StmtIndex> res1 = Pattern::getWhileStmtsFromVar(varIdx1);

		Assert::IsTrue(expectedRes == res1);

		std::vector<StmtIndex> res2 = Pattern::getIfStmtsFromVar(varIdx1);
		Assert::IsTrue(0 == res2.size());
	}

	TEST_METHOD(insertAssignInfo_getAllAssignPatternInfo) {
		std::vector<StmtIndex> stmtIndices{ 1, 2, 3, 3 };
		std::vector<VarIndex> varIndices{ 1, 1, 1, 2 };

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertAssignInfo(varIdx1, postFix4, stmtIdx1);
		Pattern::insertAssignInfo(varIdx1, postFix1, stmtIdx2);
		Pattern::insertAssignInfo(varIdx1, postFix2, stmtIdx3);
		Pattern::insertAssignInfo(varIdx2, postFix4, stmtIdx3);

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res = Pattern::getAllAssignPatternInfo();

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertIfInfo_getAllIfPatternInfo_singleIfStmt_singleControlVariable) {
		// 1. if (x = 1) then {
		// ...
		std::vector<StmtIndex> stmtIndices{ 1 };
		std::vector<VarIndex> varIndices{ 1 };

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertIfInfo(stmtIdx1, varIdx1);

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res1 = Pattern::getAllIfPatternInfo();

		Assert::IsTrue(expectedRes == res1);

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res2 = Pattern::getAllWhilePatternInfo();
		Assert::IsTrue(0 == std::get<0>(res2).size());
	}

	TEST_METHOD(insertIfInfo_getAllIfPatternInfo_singleIfStmt_multipleControlVariables) {
		// 1. if ((!(number != x)) && ((number <= y) || (y == z))) then {
		// ...
		std::vector<StmtIndex> stmtIndices{ 1, 1, 1, 1 };
		std::vector<VarIndex> varIndices{ 1, 2, 3, 4 };

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertIfInfo(stmtIdx1, varIdx1);
		Pattern::insertIfInfo(stmtIdx1, varIdx2);
		Pattern::insertIfInfo(stmtIdx1, varIdx3);
		Pattern::insertIfInfo(stmtIdx1, varIdx4);

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res1 = Pattern::getAllIfPatternInfo();

		Assert::IsTrue(expectedRes == res1);

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res2 = Pattern::getAllWhilePatternInfo();
		Assert::IsTrue(0 == std::get<0>(res2).size());
	}

	TEST_METHOD(insertIfInfo_getAllIfPatternInfo_multipleIfStmts_multipleControlVariables) {
		// 1. if ((!(number != x)) && ((number <= y) || (y == z))) then {
		// 2.		if (x = y) then {
		// ...
		std::vector<StmtIndex> stmtIndices{ 1, 1, 2, 1, 2, 1 };
		std::vector<VarIndex> varIndices{ 1, 2, 2, 3, 3, 4 };

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertIfInfo(stmtIdx1, varIdx1);
		Pattern::insertIfInfo(stmtIdx1, varIdx2);
		Pattern::insertIfInfo(stmtIdx1, varIdx3);
		Pattern::insertIfInfo(stmtIdx1, varIdx4);
		Pattern::insertIfInfo(stmtIdx2, varIdx2);
		Pattern::insertIfInfo(stmtIdx2, varIdx3);

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res1 = Pattern::getAllIfPatternInfo();

		Assert::IsTrue(expectedRes == res1);

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res2 = Pattern::getAllWhilePatternInfo();
		Assert::IsTrue(0 == std::get<0>(res2).size());
	}

	TEST_METHOD(insertWhileInfo_getAllWhilePatternInfo_singleIfStmt_singleControlVariable) {
		// 1. while (x = 1) {
		// ...
		std::vector<StmtIndex> stmtIndices{ 1 };
		std::vector<VarIndex> varIndices{ 1 };

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertWhileInfo(stmtIdx1, varIdx1);

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res1 = Pattern::getAllWhilePatternInfo();

		Assert::IsTrue(expectedRes == res1);

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res2 = Pattern::getAllIfPatternInfo();
		Assert::IsTrue(0 == std::get<0>(res2).size());
	}

	TEST_METHOD(insertWhileInfo_getAllWhilePatternInfo_singleIfStmt_multipleControlVariables) {
		// 1. while ((!(number != x)) && ((number <= y) || (y == z))) {
		// ...
		std::vector<StmtIndex> stmtIndices{ 1, 1, 1, 1 };
		std::vector<VarIndex> varIndices{ 1, 2, 3, 4 };

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertWhileInfo(stmtIdx1, varIdx1);
		Pattern::insertWhileInfo(stmtIdx1, varIdx2);
		Pattern::insertWhileInfo(stmtIdx1, varIdx3);
		Pattern::insertWhileInfo(stmtIdx1, varIdx4);

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res1 = Pattern::getAllWhilePatternInfo();

		Assert::IsTrue(expectedRes == res1);

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res2 = Pattern::getAllIfPatternInfo();
		Assert::IsTrue(0 == std::get<0>(res2).size());
	}

	TEST_METHOD(insertWhileInfo_getAllWhilePatternInfo_multipleIfStmts_multipleControlVariables) {
		// 1. while ((!(number != x)) && ((number <= y) || (y == z))) {
		// 2.		while (x = y) {
		// ...
		std::vector<StmtIndex> stmtIndices{ 1, 1, 2, 1, 2, 1 };
		std::vector<VarIndex> varIndices{ 1, 2, 2, 3, 3, 4 };

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertWhileInfo(stmtIdx1, varIdx1);
		Pattern::insertWhileInfo(stmtIdx1, varIdx2);
		Pattern::insertWhileInfo(stmtIdx1, varIdx3);
		Pattern::insertWhileInfo(stmtIdx1, varIdx4);
		Pattern::insertWhileInfo(stmtIdx2, varIdx2);
		Pattern::insertWhileInfo(stmtIdx2, varIdx3);

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res1 = Pattern::getAllWhilePatternInfo();

		Assert::IsTrue(expectedRes == res1);

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res2 = Pattern::getAllIfPatternInfo();
		Assert::IsTrue(0 == std::get<0>(res2).size());
	}

	TEST_METHOD(insertIfWhileVarInfo_getAllIfWhilePatternInfo_multipleIfWhileStmts_multipleControlVariables) {
		// 1. if ((!(number != x)) && (number <= y)) then {
		// 2.		while (x = y) {
		// 3.             if (z = 0) {
		// ...
		std::vector<StmtIndex> ifStmtIndices{ 1, 1, 1, 3 };
		std::vector<VarIndex> ifVarIndices{ 1, 2, 3, 4 };
		std::vector<StmtIndex> whileStmtIndices{ 2, 2 };
		std::vector<VarIndex> whileVarIndices{ 2, 3 };

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> expectedRes1 = std::make_tuple(ifStmtIndices, ifVarIndices);
		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> expectedRes2 = std::make_tuple(whileStmtIndices, whileVarIndices);

		Pattern::insertIfInfo(stmtIdx1, varIdx1);
		Pattern::insertIfInfo(stmtIdx1, varIdx2);
		Pattern::insertIfInfo(stmtIdx1, varIdx3);
		Pattern::insertWhileInfo(stmtIdx2, varIdx2);
		Pattern::insertWhileInfo(stmtIdx2, varIdx3);
		Pattern::insertIfInfo(stmtIdx3, varIdx4);

		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res1 = Pattern::getAllIfPatternInfo();
		std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> res2 = Pattern::getAllWhilePatternInfo();

		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes2 == res2);
	}

	TEST_METHOD(performCleanUp) {
		Pattern::insertAssignInfo(varIdx1, postFix4, stmtIdx1);
		Pattern::insertAssignInfo(varIdx2, postFix1, stmtIdx2);
		Pattern::insertAssignInfo(varIdx1, postFix2, stmtIdx3);

		std::vector<StmtIndex> res1 = Pattern::getAssignStmtsFromVar(varIdx1);
		Assert::IsTrue(res1.size() == 2);

		Pattern::performCleanUp();

		std::vector<StmtIndex> res2 = Pattern::getAssignStmtsFromVar(varIdx1);
		Assert::IsTrue(res2.size() == 0);
	}
	};
}
