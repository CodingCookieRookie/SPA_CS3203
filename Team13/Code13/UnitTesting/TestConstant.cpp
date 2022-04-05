#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/Constant.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestConstant) {
public:

	TEST_METHOD(insertConst_getAllConsts_differentConsts) {
		Constant constant = Constant();

		std::vector<ConstValue> expectedRes = { 1, 2 };

		constant.insertConst(1);
		constant.insertConst(2);

		std::vector<ConstValue> res = constant.getAllConsts();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertConst_getAllConsts_sameConst) {
		Constant constant = Constant();

		std::vector<ConstValue> expectedRes = { 1 };

		constant.insertConst(1);
		constant.insertConst(1);

		std::vector<ConstValue> res = constant.getAllConsts();
		Assert::IsTrue(expectedRes == res);
	}
	};
}
