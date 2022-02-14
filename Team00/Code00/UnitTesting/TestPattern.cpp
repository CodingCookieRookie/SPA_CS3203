#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/common/Types.h"
#include "../source/PKB/Pattern.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPattern) {
private:
	std::string varName1 = "future";
	std::string varName2 = "present";
	std::string varName3 = "past";

	std::string postFix1 = "a b +";
	std::string postFix2 = "5 b *c %d /deab %";
	std::string postFix3 = "a b *";
	std::string postFix4 = "c a b * +";

	VarIndex varIdx1 = VarIndex(1);
	VarIndex varIdx2 = VarIndex(2);
	VarIndex varIdx3 = VarIndex(3);

	StmtIndex stmtIdx1 = StmtIndex(1);
	StmtIndex stmtIdx2 = StmtIndex(2);
	StmtIndex stmtIdx3 = StmtIndex(3);

	TEST_METHOD_CLEANUP(cleanUpPatternInfo) {
		Pattern::performCleanUp();
	}

public:
	TEST_METHOD(insertPostFixInfo_getStmtsFromVarPattern_fromVar_sameVariable_differentStmtIdx) {
		std::vector<int> expectedRes;
		expectedRes.push_back(1);
		expectedRes.push_back(2);
		expectedRes.push_back(3);

		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx2);
		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx3);

		std::vector<int> res = Pattern::getStmtsFromVarPattern(varIdx1);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertPostFixInfo_getStmtsFromVarPattern_fromVar_sameVariable_differentStmtIdx_repeatedStmtIdx) {
		std::vector<int> expectedRes;
		expectedRes.push_back(1);
		expectedRes.push_back(2);
		expectedRes.push_back(3);

		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx2);
		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx3);
		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx2);
		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx3);
		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx3);

		std::vector<int> res = Pattern::getStmtsFromVarPattern(varIdx1);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertPostFixInfo_getStmtsFromVarPattern_fromVar_differentVariables_differentStmtIdx_repeatedStmtIdx) {
		std::vector<int> expectedRes;
		expectedRes.push_back(2);
		expectedRes.push_back(1);

		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertPostFixInfo(varIdx2, postFix1, stmtIdx2);
		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx3);
		Pattern::insertPostFixInfo(varIdx2, postFix1, stmtIdx1);
		Pattern::insertPostFixInfo(varIdx2, postFix1, stmtIdx1);

		std::vector<int> res = Pattern::getStmtsFromVarPattern(varIdx2);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertPostFixInfo_getStmtsFromVarPattern_fromExpression_sameExpression_exactExpression) {
		std::vector<int> expectedRes;
		expectedRes.push_back(1);
		expectedRes.push_back(2);

		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx2);
		Pattern::insertPostFixInfo(varIdx2, postFix1, stmtIdx3);
		Pattern::insertPostFixInfo(varIdx2, postFix1, stmtIdx2);

		std::vector<int> res = Pattern::getStmtsFromVarPattern(varIdx1, postFix1, false);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertPostFixInfo_getStmtsFromVarPattern_fromExpression_differentExpression_repeatedStmtIdx_exactExpression) {
		std::vector<int> expectedRes;
		expectedRes.push_back(1);

		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertPostFixInfo(varIdx1, postFix2, stmtIdx2);
		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertPostFixInfo(varIdx2, postFix1, stmtIdx3);
		Pattern::insertPostFixInfo(varIdx2, postFix2, stmtIdx2);

		std::vector<int> res = Pattern::getStmtsFromVarPattern(varIdx1, postFix1, false);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertPostFixInfo_getStmtsFromVarPattern_fromExpression_sameExpression_subExpression_subExpressionQuery) {
		std::vector<int> expectedRes;
		expectedRes.push_back(1);
		expectedRes.push_back(2);

		Pattern::insertPostFixInfo(varIdx1, postFix4, stmtIdx1);
		Pattern::insertPostFixInfo(varIdx1, postFix4, stmtIdx2);
		Pattern::insertPostFixInfo(varIdx2, postFix4, stmtIdx3);

		// postFix3 is a subexpression of postFix4
		std::vector<int> res1 = Pattern::getStmtsFromVarPattern(varIdx1, postFix3, true);
		std::vector<int> res2 = Pattern::getStmtsFromVarPattern(varIdx1, postFix4, true);

		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);
	}

	TEST_METHOD(insertPostFixInfo_getStmtsFromVarPattern_fromExpression_sameExpression_subExpression_nonSubExpressionQuery) {
		std::vector<int> expectedRes;
		expectedRes.push_back(1);

		Pattern::insertPostFixInfo(varIdx1, postFix4, stmtIdx1);
		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx2);
		Pattern::insertPostFixInfo(varIdx2, postFix4, stmtIdx3);

		// postFix1 is a not subexpression of postFix4
		std::vector<int> res = Pattern::getStmtsFromVarPattern(varIdx1, postFix4, true);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertPostFixInfo_getStmtsFromVarPattern_fromExpression_differentExpression_subExpression) {
		std::vector<int> expectedRes;
		expectedRes.push_back(1);
		expectedRes.push_back(2);

		Pattern::insertPostFixInfo(varIdx1, postFix4, stmtIdx1);
		Pattern::insertPostFixInfo(varIdx1, postFix4, stmtIdx2);
		Pattern::insertPostFixInfo(varIdx2, postFix4, stmtIdx3);

		// postFix3 is a subexpression of postFix4
		std::vector<int> res1 = Pattern::getStmtsFromVarPattern(varIdx1, postFix3, true);
		std::vector<int> res2 = Pattern::getStmtsFromVarPattern(varIdx1, postFix4, true);

		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);

		// Add subexpression postFix3
		expectedRes.push_back(3);
		Pattern::insertPostFixInfo(varIdx1, postFix3, stmtIdx3);

		std::vector<int> res3 = Pattern::getStmtsFromVarPattern(varIdx1, postFix3, true);
		std::vector<int> res4 = Pattern::getStmtsFromVarPattern(varIdx1, postFix4, true);

		Assert::IsTrue(expectedRes == res3);
		Assert::IsFalse(expectedRes == res4);
	}

	TEST_METHOD(insertPostFixInfo_getStmtsFromPattern_sameExpression_exactExpression) {
		std::vector<int> stmtIndices;
		std::vector<int> varIndices;

		stmtIndices.push_back(1);
		stmtIndices.push_back(2);
		stmtIndices.push_back(3);
		varIndices.push_back(1);
		varIndices.push_back(1);
		varIndices.push_back(2);

		std::tuple<std::vector<int>, std::vector<int>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx2);
		Pattern::insertPostFixInfo(varIdx2, postFix1, stmtIdx3);

		std::tuple<std::vector<int>, std::vector<int>> res = Pattern::getStmtsFromPattern(postFix1, false);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertPostFixInfo_getStmtsFromPattern_differentExpression_exactExpression) {
		std::vector<int> stmtIndices;
		std::vector<int> varIndices;

		stmtIndices.push_back(2);
		stmtIndices.push_back(3);
		varIndices.push_back(1);
		varIndices.push_back(2);

		std::tuple<std::vector<int>, std::vector<int>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertPostFixInfo(varIdx1, postFix2, stmtIdx2);
		Pattern::insertPostFixInfo(varIdx2, postFix1, stmtIdx3);
		Pattern::insertPostFixInfo(varIdx2, postFix2, stmtIdx3);

		std::tuple<std::vector<int>, std::vector<int>> res = Pattern::getStmtsFromPattern(postFix2, false);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertPostFixInfo_getStmtsFromPattern_sameExpression_subExpression_subExpressionQuery) {
		std::vector<int> stmtIndices;
		std::vector<int> varIndices;

		stmtIndices.push_back(1);
		stmtIndices.push_back(2);
		stmtIndices.push_back(3);
		varIndices.push_back(1);
		varIndices.push_back(1);
		varIndices.push_back(2);

		std::tuple<std::vector<int>, std::vector<int>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertPostFixInfo(varIdx1, postFix4, stmtIdx1);
		Pattern::insertPostFixInfo(varIdx1, postFix4, stmtIdx2);
		Pattern::insertPostFixInfo(varIdx2, postFix4, stmtIdx3);

		// postFix3 is a subexpression of postFix4
		std::tuple<std::vector<int>, std::vector<int>> res1 = Pattern::getStmtsFromPattern(postFix3, true);
		std::tuple<std::vector<int>, std::vector<int>> res2 = Pattern::getStmtsFromPattern(postFix4, true);
		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);
	}

	TEST_METHOD(insertPostFixInfo_getStmtsFromPattern_sameExpression_subExpression_nonSubExpressionQuery) {
		std::vector<int> stmtIndices;
		std::vector<int> varIndices;

		stmtIndices.push_back(2);
		varIndices.push_back(3);

		std::tuple<std::vector<int>, std::vector<int>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx1);
		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx2);
		Pattern::insertPostFixInfo(varIdx2, postFix1, stmtIdx3);
		Pattern::insertPostFixInfo(varIdx3, postFix4, stmtIdx2);

		// postFix1 is a not subexpression of postFix4
		std::tuple<std::vector<int>, std::vector<int>> res = Pattern::getStmtsFromPattern(postFix4, true);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertPostFixInfo_getStmtsFromPattern_differentExpression_subExpression) {
		std::vector<int> stmtIndices;
		std::vector<int> varIndices;

		stmtIndices.push_back(1);
		stmtIndices.push_back(2);
		stmtIndices.push_back(3);
		varIndices.push_back(1);
		varIndices.push_back(1);
		varIndices.push_back(2);

		std::tuple<std::vector<int>, std::vector<int>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertPostFixInfo(varIdx1, postFix4, stmtIdx1);
		Pattern::insertPostFixInfo(varIdx1, postFix4, stmtIdx2);
		Pattern::insertPostFixInfo(varIdx1, postFix2, stmtIdx3);
		Pattern::insertPostFixInfo(varIdx2, postFix4, stmtIdx3);

		// postFix3 is a subexpression of postFix4
		std::tuple<std::vector<int>, std::vector<int>> res1 = Pattern::getStmtsFromPattern(postFix3, true);
		std::tuple<std::vector<int>, std::vector<int>> res2 = Pattern::getStmtsFromPattern(postFix4, true);
		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);

		// Add subexpression postFix3
		stmtIndices.insert(stmtIndices.begin(), 3);
		varIndices.insert(varIndices.begin(), 3);
		expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertPostFixInfo(varIdx3, postFix3, stmtIdx3);

		std::tuple<std::vector<int>, std::vector<int>> res3 = Pattern::getStmtsFromPattern(postFix3, true);
		std::tuple<std::vector<int>, std::vector<int>> res4 = Pattern::getStmtsFromPattern(postFix4, true);

		Assert::IsTrue(expectedRes == res3);
		Assert::IsFalse(expectedRes == res4);
	}

	TEST_METHOD(insertPostFixInfo_getAllAssignStmtVarsPatternInfo) {
		std::vector<int> stmtIndices;
		std::vector<int> varIndices;

		stmtIndices.push_back(1);
		stmtIndices.push_back(2);
		stmtIndices.push_back(3);
		stmtIndices.push_back(3);
		varIndices.push_back(1);
		varIndices.push_back(1);
		varIndices.push_back(1);
		varIndices.push_back(2);

		std::tuple<std::vector<int>, std::vector<int>> expectedRes = std::make_tuple(stmtIndices, varIndices);

		Pattern::insertPostFixInfo(varIdx1, postFix4, stmtIdx1);
		Pattern::insertPostFixInfo(varIdx1, postFix1, stmtIdx2);
		Pattern::insertPostFixInfo(varIdx1, postFix2, stmtIdx3);
		Pattern::insertPostFixInfo(varIdx2, postFix4, stmtIdx3);

		std::tuple<std::vector<int>, std::vector<int>> res = Pattern::getAllAssignStmtVarsPatternInfo();

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(performCleanUp_VarMap) {
		Pattern::insertPostFixInfo(varIdx1, postFix4, stmtIdx1);
		Pattern::insertPostFixInfo(varIdx2, postFix1, stmtIdx2);
		Pattern::insertPostFixInfo(varIdx1, postFix2, stmtIdx3);

		std::vector<int> res1 = Pattern::getStmtsFromVarPattern(varIdx1);
		Assert::IsTrue(res1.size() == 2);

		Pattern::performCleanUp();

		std::vector<int> res2 = Pattern::getStmtsFromVarPattern(varIdx1);
		Assert::IsTrue(res2.size() == 0);
	}
	};
}
