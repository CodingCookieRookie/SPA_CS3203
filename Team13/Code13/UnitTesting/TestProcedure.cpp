#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/PKB/Procedure.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestProcedure) {
private:
	std::string procName1 = "Peter";
	std::string procName2 = "Stephen";

	ProcIndex procIdx1 = ProcIndex(1);
	ProcIndex procIdx2 = ProcIndex(2);

public:
	TEST_METHOD(insertProc_containsProc_procExists) {
		Procedure procedure = Procedure();

		ProcIndex idx = procedure.insert(procName1);

		bool res = procedure.contains(procName1);
		Assert::IsTrue(res);
	}

	TEST_METHOD(insertProc_containsProc_procDoesNotExist) {
		Procedure procedure = Procedure();

		ProcIndex idx = procedure.insert(procName1);

		bool res = procedure.contains(procName2);
		Assert::IsFalse(res);
	}

	TEST_METHOD(insertProc_getProcName_differentProcs) {
		Procedure procedure = Procedure();

		ProcIndex idx1 = procedure.insert(procName1);
		ProcIndex idx2 = procedure.insert(procName2);

		std::string res1 = procedure.getName(procIdx1);
		std::string res2 = procedure.getName(procIdx2);
		Assert::AreEqual(procName1, res1);
		Assert::AreEqual(procName2, res2);
		Assert::IsTrue(procIdx1 == idx1);
		Assert::IsTrue(procIdx2 == idx2);
	}

	TEST_METHOD(insertProc_getProcIdx_differentProcs) {
		Procedure procedure = Procedure();

		ProcIndex idx1 = procedure.insert(procName1);
		ProcIndex idx2 = procedure.insert(procName2);

		ProcIndex res1 = procedure.getIndex(procName1);
		ProcIndex res2 = procedure.getIndex(procName2);
		Assert::IsTrue(procIdx1 == res1);
		Assert::IsTrue(procIdx2 == res2);
		Assert::IsTrue(procIdx1 == idx1);
		Assert::IsTrue(procIdx2 == idx2);
	}

	TEST_METHOD(insertProc_getProcIdx_sameProc) {
		Procedure procedure = Procedure();

		ProcIndex idx1 = procedure.insert(procName1);
		ProcIndex idx2 = procedure.insert(procName1);

		ProcIndex res = procedure.getIndex(procName1);
		Assert::IsTrue(procIdx1 == res);
		Assert::IsTrue(idx1 == res);
	}

	TEST_METHOD(insertProc_getAllProcs_differentProcs) {
		Procedure procedure = Procedure();

		std::vector<ProcIndex> expectedRes = { procIdx1 , procIdx2 };

		procedure.insert(procName1);
		procedure.insert(procName2);

		std::vector<ProcIndex> res = procedure.getAll();
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(insertProc_getAllProcs_sameProc) {
		Procedure procedure = Procedure();

		std::vector<ProcIndex> expectedRes = { procIdx1 };

		procedure.insert(procName1);
		procedure.insert(procName1);

		std::vector<ProcIndex> res = procedure.getAll();
		Assert::IsTrue(expectedRes == res);
	}
	};
}
