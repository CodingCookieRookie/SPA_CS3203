#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/common/Types.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestTypes) {
private:
	int entityTypeOverloadedFunction(VarIndex varIdx) {
		return 1;
	}

	int entityTypeOverloadedFunction(ProcIndex procIdx) {
		return 2;
	}

	int entityTypeOverloadedFunction(StmtIndex stmtIdx) {
		return 3;
	}

public:

	TEST_METHOD(overloadedFunction_differentTypeArguments) {
		struct VarIndex varIdx = VarIndex(1);
		struct ProcIndex procIdx = ProcIndex(1);
		struct StmtIndex stmtIdx = StmtIndex(1);

		int varRes = entityTypeOverloadedFunction(varIdx);
		int procRes = entityTypeOverloadedFunction(procIdx);
		int stmtRes = entityTypeOverloadedFunction(stmtIdx);

		Assert::AreEqual(1, varRes);
		Assert::AreEqual(2, procRes);
		Assert::AreEqual(3, stmtRes);
	}

	TEST_METHOD(uniqueEntityTypes) {
		Assert::IsFalse(typeid(VarIndex) == typeid(ProcIndex));
		Assert::IsFalse(typeid(StmtIndex) == typeid(ProcIndex));
		Assert::IsFalse(typeid(StmtIndex) == typeid(VarIndex));
		Assert::IsFalse(typeid(VarIndex) == typeid(ProcIndex));
		Assert::IsFalse(typeid(StmtIndex) == typeid(ProcIndex));
		Assert::IsFalse(typeid(StmtIndex) == typeid(VarIndex));
	}

	TEST_METHOD(uniqueStatementTypes) {
		Assert::IsFalse(StatementType::assignType == StatementType::whileType);
	}
	};
}
