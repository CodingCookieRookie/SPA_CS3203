#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "../source/PKB/Attribute.h"
#include "../source/PKB/Entity.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestAttribute) {
private:
	std::string name1 = "procedure";
	std::string name2 = "variable";
	std::string name3 = "constant";

	std::string varName1 = "future";
	std::string varName2 = "present";

	std::string procName1 = "Peter";
	std::string procName2 = "Stephen";

	StatementType assignType = StatementType::assignType;
	StatementType whileType = StatementType::whileType;
	StatementType callType = StatementType::callType;
	StatementType readType = StatementType::readType;
	StatementType printType = StatementType::printType;
	StatementType ifType = StatementType::ifType;

	TEST_METHOD_CLEANUP(cleanUpAttributeInfo) {
		Attribute::performCleanUp();
		Entity::performCleanUp();
	}

public:

	TEST_METHOD(insertNameValue_getNameIdx_nameExists) {
		int nameIdx = Attribute::insertNameValue(name1);
		int res = Attribute::getNameIdx(name1);

		Assert::IsTrue(nameIdx == res);
		Assert::IsTrue(1 == res);
	}

	TEST_METHOD(insertNameValue_getNameIdx_nameDoesNotExist) {
		int nameIdx = Attribute::insertNameValue(name2);
		int res = Attribute::getNameIdx(name1);

		Assert::IsFalse(nameIdx == res);
	}

	TEST_METHOD(insertNameValue_getNameIdx_differentNames) {
		int nameIdx1 = Attribute::insertNameValue(name1);
		int nameIdx2 = Attribute::insertNameValue(name2);

		int res1 = Attribute::getNameIdx(name1);
		int res2 = Attribute::getNameIdx(name2);

		Assert::AreEqual(nameIdx1, res1);
		Assert::AreEqual(nameIdx2, res2);
		Assert::IsTrue(1 == res1);
		Assert::IsTrue(2 == res2);
	}

	TEST_METHOD(insertNameValue_getNameIdx_sameName) {
		int nameIdx1 = Attribute::insertNameValue(name1);
		int nameIdx2 = Attribute::insertNameValue(name1);

		int res = Attribute::getNameIdx(name1);

		Assert::AreEqual(nameIdx1, res);
		Assert::AreEqual(nameIdx2, res);
		Assert::IsTrue(1 == res);
	}

	TEST_METHOD(insertVar_getNameIdx_nameExists) {
		VarIndex idx = Entity::insertVar(varName1);

		int nameIdx = Attribute::getNameIdx(varName1);
		Assert::IsTrue(1 == nameIdx);
	}

	TEST_METHOD(insertVar_getNameIdx_nameDoesNotExist) {
		VarIndex idx = Entity::insertVar(varName1);

		int nameIdx = Attribute::getNameIdx(varName2);
		Assert::IsTrue(0 == nameIdx);
	}

	TEST_METHOD(insertVar_getNameIdx_differentNames) {
		VarIndex idx1 = Entity::insertVar(varName1);
		VarIndex idx2 = Entity::insertVar(varName2);

		int nameIdx1 = Attribute::getNameIdx(varName1);
		int nameIdx2 = Attribute::getNameIdx(varName2);
		Assert::IsTrue(1 == nameIdx1);
		Assert::IsTrue(2 == nameIdx2);
	}

	TEST_METHOD(insertVar_getNameIdx_sameName) {
		VarIndex idx1 = Entity::insertVar(varName1);
		VarIndex idx2 = Entity::insertVar(varName1);

		int nameIdx1 = Attribute::getNameIdx(varName1);
		int nameIdx2 = Attribute::getNameIdx(varName2);
		Assert::IsTrue(1 == nameIdx1);
		Assert::IsFalse(2 == nameIdx2);
	}

	TEST_METHOD(insertProc_getNameIdx_nameExists) {
		ProcIndex idx = Entity::insertProc(procName1);

		int nameIdx = Attribute::getNameIdx(procName1);
		Assert::IsTrue(1 == nameIdx);
	}

	TEST_METHOD(insertProc_getNameIdx_nameDoesNotExist) {
		ProcIndex idx = Entity::insertProc(procName1);

		int nameIdx = Attribute::getNameIdx(procName2);
		Assert::IsTrue(0 == nameIdx);
	}

	TEST_METHOD(insertProc_getNameIdx_differentNames) {
		ProcIndex idx1 = Entity::insertProc(procName1);
		ProcIndex idx2 = Entity::insertProc(procName2);

		int nameIdx1 = Attribute::getNameIdx(procName1);
		int nameIdx2 = Attribute::getNameIdx(procName2);
		Assert::IsTrue(1 == nameIdx1);
		Assert::IsTrue(2 == nameIdx2);
	}

	TEST_METHOD(insertProc_getNameIdx_sameName) {
		ProcIndex idx1 = Entity::insertProc(procName1);
		ProcIndex idx2 = Entity::insertProc(procName1);

		int nameIdx1 = Attribute::getNameIdx(procName1);
		int nameIdx2 = Attribute::getNameIdx(procName2);
		Assert::IsTrue(1 == nameIdx1);
		Assert::IsFalse(2 == nameIdx2);
	}

	TEST_METHOD(insertCallStmt_getNameIdx_stmtExists) {
		StmtIndex stmtIdx = Entity::insertStmt(callType, name1);

		int res = Attribute::getNameIdx(name1);

		Assert::IsTrue(1 == res);
	}

	TEST_METHOD(insertCallStmt_getNameIdx_stmtDoesNotExist) {
		StmtIndex stmtIdx = Entity::insertStmt(callType, name1);

		int res = Attribute::getNameIdx(name2);

		Assert::IsTrue(0 == res);
	}

	TEST_METHOD(insertCallStmt_getNameIdx_multipleStmts_differentNames) {
		StmtIndex stmtIdx1 = Entity::insertStmt(callType, name1);
		StmtIndex stmtIdx2 = Entity::insertStmt(callType, name2);

		int res1 = Attribute::getNameIdx(name1);
		int res2 = Attribute::getNameIdx(name2);

		Assert::IsTrue(1 == res1);
		Assert::IsTrue(2 == res2);
	}

	TEST_METHOD(insertCallStmt_getNameIdx_multipleStmts_sameName) {
		StmtIndex stmtIdx1 = Entity::insertStmt(callType, name1);
		StmtIndex stmtIdx2 = Entity::insertStmt(callType, name1);

		int res1 = Attribute::getNameIdx(name1);
		int res2 = Attribute::getNameIdx(name2);

		Assert::IsTrue(1 == res1);
		Assert::IsFalse(2 == res2);
	}

	TEST_METHOD(insertReadStmt_getNameIdx_stmtExists) {
		StmtIndex stmtIdx = Entity::insertStmt(readType, name1);

		int res = Attribute::getNameIdx(name1);

		Assert::IsTrue(1 == res);
	}

	TEST_METHOD(insertReadStmt_getNameIdx_stmtDoesNotExist) {
		StmtIndex stmtIdx = Entity::insertStmt(readType, name1);

		int res = Attribute::getNameIdx(name2);

		Assert::IsTrue(0 == res);
	}

	TEST_METHOD(insertReadStmt_getNameIdx_multipleStmts_differentNames) {
		StmtIndex stmtIdx1 = Entity::insertStmt(readType, name1);
		StmtIndex stmtIdx2 = Entity::insertStmt(readType, name2);

		int res1 = Attribute::getNameIdx(name1);
		int res2 = Attribute::getNameIdx(name2);

		Assert::IsTrue(1 == res1);
		Assert::IsTrue(2 == res2);
	}

	TEST_METHOD(insertReadStmt_getNameIdx_multipleStmts_sameName) {
		StmtIndex stmtIdx1 = Entity::insertStmt(readType, name1);
		StmtIndex stmtIdx2 = Entity::insertStmt(readType, name1);

		int res1 = Attribute::getNameIdx(name1);
		int res2 = Attribute::getNameIdx(name2);

		Assert::IsTrue(1 == res1);
		Assert::IsFalse(2 == res2);
	}

	TEST_METHOD(insertPrintStmt_getNameIdx_stmtExists) {
		StmtIndex stmtIdx = Entity::insertStmt(printType, name1);

		int res = Attribute::getNameIdx(name1);

		Assert::IsTrue(1 == res);
	}

	TEST_METHOD(insertPrintStmt_getNameIdx_stmtDoesNotExist) {
		StmtIndex stmtIdx = Entity::insertStmt(printType, name1);

		int res = Attribute::getNameIdx(name2);

		Assert::IsTrue(0 == res);
	}

	TEST_METHOD(insertPrintStmt_getNameIdx_multipleStmts_differentNames) {
		StmtIndex stmtIdx1 = Entity::insertStmt(printType, name1);
		StmtIndex stmtIdx2 = Entity::insertStmt(printType, name2);

		int res1 = Attribute::getNameIdx(name1);
		int res2 = Attribute::getNameIdx(name2);

		Assert::IsTrue(1 == res1);
		Assert::IsTrue(2 == res2);
	}

	TEST_METHOD(insertPrintStmt_getNameIdx_multipleStmts_sameName) {
		StmtIndex stmtIdx1 = Entity::insertStmt(printType, name1);
		StmtIndex stmtIdx2 = Entity::insertStmt(printType, name1);

		int res1 = Attribute::getNameIdx(name1);
		int res2 = Attribute::getNameIdx(name2);

		Assert::IsTrue(1 == res1);
		Assert::IsFalse(2 == res2);
	}

	TEST_METHOD(insertStmt_getNameIdx_differentStmtTypes_differentNames) {
		StmtIndex stmtIdx1 = Entity::insertStmt(callType, name1);
		StmtIndex stmtIdx2 = Entity::insertStmt(printType, name2);
		StmtIndex stmtIdx3 = Entity::insertStmt(readType, name3);

		int res1 = Attribute::getNameIdx(name1);
		int res2 = Attribute::getNameIdx(name2);
		int res3 = Attribute::getNameIdx(name3);

		Assert::IsTrue(1 == res1);
		Assert::IsTrue(2 == res2);
		Assert::IsTrue(3 == res3);
	}

	TEST_METHOD(insertStmt_getNameIdx_differentStmtTypes_sameName) {
		StmtIndex stmtIdx1 = Entity::insertStmt(callType, name1);
		StmtIndex stmtIdx2 = Entity::insertStmt(printType, name1);
		StmtIndex stmtIdx3 = Entity::insertStmt(readType, name1);

		int res1 = Attribute::getNameIdx(name1);
		int res2 = Attribute::getNameIdx(name2);
		int res3 = Attribute::getNameIdx(name3);

		Assert::IsTrue(1 == res1);
		Assert::IsFalse(2 == res2);
		Assert::IsFalse(3 == res3);
	}

	TEST_METHOD(getEqualNameAttributes_oneEntity_sameEntityQueried) {
		ProcIndex procIdx = Entity::insertProc(procName1);

		int nameIdx = Attribute::getNameIdx(procName1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<int> expectedLeftEntityTypeIndices{ 1 };
		std::vector<int> expectedRightEntityTypeIndices{ 1 };
		std::tuple<std::vector<int>, std::vector<int>> expectedRes = std::make_tuple(expectedLeftEntityTypeIndices, expectedRightEntityTypeIndices);

		std::tuple<std::vector<int>, std::vector<int>> res = Attribute::getEqualNameAttributes(PqlEntityType::Procedure, PqlEntityType::Procedure);
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(getEqualNameAttributes_oneEntity_differentEntityQueried) {
		ProcIndex procIdx = Entity::insertProc(procName1);

		int nameIdx = Attribute::getNameIdx(procName1);
		Assert::IsTrue(1 == nameIdx);

		std::tuple<std::vector<int>, std::vector<int>> res1 = Attribute::getEqualNameAttributes(PqlEntityType::Variable, PqlEntityType::Variable);
		std::vector<int> res1LeftEntityTypeIndices = std::get<0>(res1);
		std::vector<int> res1RightEntityTypeIndices = std::get<1>(res1);
		Assert::IsTrue(0 == res1LeftEntityTypeIndices.size());
		Assert::IsTrue(0 == res1RightEntityTypeIndices.size());

		std::tuple<std::vector<int>, std::vector<int>> res2 = Attribute::getEqualNameAttributes(PqlEntityType::Procedure, PqlEntityType::Variable);
		std::vector<int> res2LeftEntityTypeIndices = std::get<0>(res2);
		std::vector<int> res2RightEntityTypeIndices = std::get<1>(res2);
		Assert::IsTrue(0 == res2LeftEntityTypeIndices.size());
		Assert::IsTrue(0 == res2RightEntityTypeIndices.size());
	}

	TEST_METHOD(getEqualNameAttributes_sameEntityType_differentNames) {
		ProcIndex procIdx1 = Entity::insertProc(procName1);
		ProcIndex procIdx2 = Entity::insertProc(procName2);

		int nameIdx1 = Attribute::getNameIdx(procName1);
		int nameIdx2 = Attribute::getNameIdx(procName2);
		Assert::IsTrue(1 == nameIdx1);
		Assert::IsTrue(2 == nameIdx2);

		std::vector<int> expectedLeftEntityTypeIndices{ 1, 2 };
		std::vector<int> expectedRightEntityTypeIndices{ 1, 2 };
		std::tuple<std::vector<int>, std::vector<int>> expectedRes = std::make_tuple(expectedLeftEntityTypeIndices, expectedRightEntityTypeIndices);

		std::tuple<std::vector<int>, std::vector<int>> res = Attribute::getEqualNameAttributes(PqlEntityType::Procedure, PqlEntityType::Procedure);
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(getEqualNameAttributes_differentEntityTypes_sameName) {
		ProcIndex procIdx = Entity::insertProc(name1);
		VarIndex varIdx = Entity::insertVar(name1);

		int nameIdx = Attribute::getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<int> expectedLeftEntityTypeIndices{ 1 };
		std::vector<int> expectedRightEntityTypeIndices{ 1 };
		std::tuple<std::vector<int>, std::vector<int>> expectedRes = std::make_tuple(expectedLeftEntityTypeIndices, expectedRightEntityTypeIndices);

		std::tuple<std::vector<int>, std::vector<int>> res1 = Attribute::getEqualNameAttributes(PqlEntityType::Procedure, PqlEntityType::Variable);
		std::tuple<std::vector<int>, std::vector<int>> res2 = Attribute::getEqualNameAttributes(PqlEntityType::Variable, PqlEntityType::Procedure);

		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);
	}

	TEST_METHOD(getEqualNameAttributes_differentEntityTypes_manyEntityTypes) {
		ProcIndex procIdx1 = Entity::insertProc(name2);
		ProcIndex procIdx2 = Entity::insertProc(name1);
		VarIndex varIdx = Entity::insertVar(name1);
		StmtIndex stmtIdx = Entity::insertStmt(callType, name1);

		int nameIdx1 = Attribute::getNameIdx(name1);
		int nameIdx2 = Attribute::getNameIdx(name2);
		Assert::IsTrue(2 == nameIdx1);
		Assert::IsTrue(1 == nameIdx2);

		std::vector<int> expectedLeftEntityTypeIndices1{ 2 };
		std::vector<int> expectedRightEntityTypeIndices1{ 1 };
		std::tuple<std::vector<int>, std::vector<int>> expectedRes1 = std::make_tuple(expectedLeftEntityTypeIndices1, expectedRightEntityTypeIndices1);

		std::tuple<std::vector<int>, std::vector<int>> res1 = Attribute::getEqualNameAttributes(PqlEntityType::Procedure, PqlEntityType::Variable);
		std::tuple<std::vector<int>, std::vector<int>> res2 = Attribute::getEqualNameAttributes(PqlEntityType::Procedure, PqlEntityType::Call);

		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes1 == res2);

		std::vector<int> expectedLeftEntityTypeIndices2{ 1 };
		std::vector<int> expectedRightEntityTypeIndices2{ 2 };
		std::tuple<std::vector<int>, std::vector<int>> expectedRes2 = std::make_tuple(expectedLeftEntityTypeIndices2, expectedRightEntityTypeIndices2);

		std::tuple<std::vector<int>, std::vector<int>> res3 = Attribute::getEqualNameAttributes(PqlEntityType::Variable, PqlEntityType::Procedure);
		std::tuple<std::vector<int>, std::vector<int>> res4 = Attribute::getEqualNameAttributes(PqlEntityType::Call, PqlEntityType::Procedure);

		Assert::IsTrue(expectedRes2 == res3);
		Assert::IsTrue(expectedRes2 == res4);

		std::vector<int> expectedLeftEntityTypeIndices3{ 1 };
		std::vector<int> expectedRightEntityTypeIndices3{ 1 };
		std::tuple<std::vector<int>, std::vector<int>> expectedRes3 = std::make_tuple(expectedLeftEntityTypeIndices3, expectedRightEntityTypeIndices3);

		std::tuple<std::vector<int>, std::vector<int>> res5 = Attribute::getEqualNameAttributes(PqlEntityType::Call, PqlEntityType::Variable);
		std::tuple<std::vector<int>, std::vector<int>> res6 = Attribute::getEqualNameAttributes(PqlEntityType::Variable, PqlEntityType::Call);
		std::tuple<std::vector<int>, std::vector<int>> res7 = Attribute::getEqualNameAttributes(PqlEntityType::Call, PqlEntityType::Call);
		std::tuple<std::vector<int>, std::vector<int>> res8 = Attribute::getEqualNameAttributes(PqlEntityType::Variable, PqlEntityType::Variable);

		Assert::IsTrue(expectedRes3 == res5);
		Assert::IsTrue(expectedRes3 == res6);
		Assert::IsTrue(expectedRes3 == res7);
		Assert::IsTrue(expectedRes3 == res8);

		std::vector<int> expectedLeftEntityTypeIndices4{ 1, 2 };
		std::vector<int> expectedRightEntityTypeIndices4{ 1, 2 };
		std::tuple<std::vector<int>, std::vector<int>> expectedRes4 = std::make_tuple(expectedLeftEntityTypeIndices4, expectedRightEntityTypeIndices4);

		std::tuple<std::vector<int>, std::vector<int>> res9 = Attribute::getEqualNameAttributes(PqlEntityType::Procedure, PqlEntityType::Procedure);

		Assert::IsTrue(expectedRes4 == res9);
	}

	TEST_METHOD(getEqualNameAttributesFromName_oneEntity_sameNameQueried_sameEntityQueried) {
		ProcIndex procIdx = Entity::insertProc(name1);

		int nameIdx = Attribute::getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<int> expectedRes{ 1 };

		std::vector<int> res = Attribute::getEqualNameAttributesFromName(PqlEntityType::Procedure, name1);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(getEqualNameAttributesFromName_oneEntity_differentNameQueried_sameEntityQueried) {
		ProcIndex procIdx = Entity::insertProc(name1);

		int nameIdx = Attribute::getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<int> res = Attribute::getEqualNameAttributesFromName(PqlEntityType::Procedure, name2);

		Assert::IsTrue(0 == res.size());
	}

	TEST_METHOD(getEqualNameAttributesFromName_oneEntity_sameNameQueried_differentEntityQueried) {
		ProcIndex procIdx = Entity::insertProc(name1);

		int nameIdx = Attribute::getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<int> res = Attribute::getEqualNameAttributesFromName(PqlEntityType::Variable, name1);

		Assert::IsTrue(0 == res.size());
	}

	TEST_METHOD(getEqualNameAttributesFromName_oneEntity_differentNameQueried_differentEntityQueried) {
		ProcIndex procIdx = Entity::insertProc(name1);

		int nameIdx = Attribute::getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<int> res = Attribute::getEqualNameAttributesFromName(PqlEntityType::Variable, name2);

		Assert::IsTrue(0 == res.size());
	}

	TEST_METHOD(getEqualNameAttributesFromName_manyEntityTypes_sameEntityType_sameName) {
		StmtIndex stmtIdx1 = Entity::insertStmt(callType, name1);
		StmtIndex stmtIdx2 = Entity::insertStmt(callType, name1);

		int nameIdx = Attribute::getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<int> expectedRes{ 1, 2 };

		std::vector<int> res1 = Attribute::getEqualNameAttributesFromName(PqlEntityType::Call, name1);
		std::vector<int> res2 = Attribute::getEqualNameAttributesFromName(PqlEntityType::Read, name1);
		std::vector<int> res3 = Attribute::getEqualNameAttributesFromName(PqlEntityType::Print, name1);

		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(0 == res2.size());
		Assert::IsTrue(0 == res3.size());
	}

	TEST_METHOD(getEqualNameAttributesFromName_manyEntityTypes_differentEntityTypes_mixedNames) {
		StmtIndex stmtIdx1 = Entity::insertStmt(callType, name2);
		StmtIndex stmtIdx2 = Entity::insertStmt(callType, name1);
		StmtIndex stmtIdx3 = Entity::insertStmt(readType, name1);
		StmtIndex stmtIdx4 = Entity::insertStmt(printType, name1);

		int nameIdx1 = Attribute::getNameIdx(name1);
		int nameIdx2 = Attribute::getNameIdx(name2);
		Assert::IsTrue(2 == nameIdx1);
		Assert::IsTrue(1 == nameIdx2);

		std::vector<int> expectedRes1{ 2 };
		std::vector<int> res1 = Attribute::getEqualNameAttributesFromName(PqlEntityType::Call, name1);
		Assert::IsTrue(expectedRes1 == res1);

		std::vector<int> expectedRes2{ 3 };
		std::vector<int> res2 = Attribute::getEqualNameAttributesFromName(PqlEntityType::Read, name1);
		Assert::IsTrue(expectedRes2 == res2);

		std::vector<int> expectedRes3{ 4 };
		std::vector<int> res3 = Attribute::getEqualNameAttributesFromName(PqlEntityType::Print, name1);
		Assert::IsTrue(expectedRes3 == res3);

		std::vector<int> expectedRes4{ 1 };
		std::vector<int> res4 = Attribute::getEqualNameAttributesFromName(PqlEntityType::Call, name2);
		Assert::IsTrue(expectedRes4 == res4);

		std::vector<int> res5 = Attribute::getEqualNameAttributesFromName(PqlEntityType::Read, name2);
		std::vector<int> res6 = Attribute::getEqualNameAttributesFromName(PqlEntityType::Print, name2);
		Assert::IsTrue(0 == res5.size());
		Assert::IsTrue(0 == res6.size());
	}

	TEST_METHOD(getEqualIntegerAttributes_oneEntity_sameEntityQueried) {
		StmtIndex stmtIdx = Entity::insertStmt(callType, name1);

		int nameIdx = Attribute::getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<int> expectedRes{ 1 };

		std::vector<int> res1 = Attribute::getEqualIntegerAttributes(PqlEntityType::Call, PqlEntityType::Call);
		std::vector<int> res2 = Attribute::getEqualIntegerAttributes(PqlEntityType::Stmt, PqlEntityType::Stmt);
		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);
	}

	TEST_METHOD(getEqualIntegerAttributes_oneEntity_differentEntityQueried) {
		StmtIndex stmtIdx = Entity::insertStmt(readType, name1);

		int nameIdx = Attribute::getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<int> expectedRes{ 1 };
		std::vector<int> res1 = Attribute::getEqualIntegerAttributes(PqlEntityType::Read, PqlEntityType::Stmt);
		std::vector<int> res2 = Attribute::getEqualIntegerAttributes(PqlEntityType::Stmt, PqlEntityType::Read);
		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);

		std::vector<int> res3 = Attribute::getEqualIntegerAttributes(PqlEntityType::Read, PqlEntityType::Call);
		std::vector<int> res4 = Attribute::getEqualIntegerAttributes(PqlEntityType::Call, PqlEntityType::Stmt);
		Assert::IsTrue(0 == res3.size());
		Assert::IsTrue(0 == res4.size());
	}

	TEST_METHOD(getEqualIntegerAttributes_sameEntityType) {
		StmtIndex stmtIdx1 = Entity::insertStmt(readType, name1);
		StmtIndex stmtIdx2 = Entity::insertStmt(readType, name2);

		int nameIdx1 = Attribute::getNameIdx(name1);
		int nameIdx2 = Attribute::getNameIdx(name2);
		Assert::IsTrue(1 == nameIdx1);
		Assert::IsTrue(2 == nameIdx2);

		std::vector<int> expectedRes{ 1, 2 };
		std::vector<int> res1 = Attribute::getEqualIntegerAttributes(PqlEntityType::Read, PqlEntityType::Read);
		std::vector<int> res2 = Attribute::getEqualIntegerAttributes(PqlEntityType::Read, PqlEntityType::Stmt);
		std::vector<int> res3 = Attribute::getEqualIntegerAttributes(PqlEntityType::Stmt, PqlEntityType::Stmt);
		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);
		Assert::IsTrue(expectedRes == res3);
	}

	TEST_METHOD(getEqualIntegerAttributes_differentEntityTypes_onlyStmts) {
		StmtIndex stmtIdx1 = Entity::insertStmt(readType, name1);
		StmtIndex stmtIdx2 = Entity::insertStmt(callType, name2);
		StmtIndex stmtIdx3 = Entity::insertStmt(printType, name2);

		int nameIdx1 = Attribute::getNameIdx(name1);
		int nameIdx2 = Attribute::getNameIdx(name2);
		Assert::IsTrue(1 == nameIdx1);
		Assert::IsTrue(2 == nameIdx2);

		std::vector<int> expectedRes1{ 1 };
		std::vector<int> res1 = Attribute::getEqualIntegerAttributes(PqlEntityType::Read, PqlEntityType::Read);
		std::vector<int> res2 = Attribute::getEqualIntegerAttributes(PqlEntityType::Read, PqlEntityType::Stmt);
		std::vector<int> res3 = Attribute::getEqualIntegerAttributes(PqlEntityType::Stmt, PqlEntityType::Read);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes1 == res2);
		Assert::IsTrue(expectedRes1 == res3);

		std::vector<int> expectedRes2{ 2 };
		std::vector<int> res4 = Attribute::getEqualIntegerAttributes(PqlEntityType::Call, PqlEntityType::Call);
		std::vector<int> res5 = Attribute::getEqualIntegerAttributes(PqlEntityType::Call, PqlEntityType::Stmt);
		std::vector<int> res6 = Attribute::getEqualIntegerAttributes(PqlEntityType::Stmt, PqlEntityType::Call);
		Assert::IsTrue(expectedRes2 == res4);
		Assert::IsTrue(expectedRes2 == res5);
		Assert::IsTrue(expectedRes2 == res6);

		std::vector<int> expectedRes3{ 3 };
		std::vector<int> res7 = Attribute::getEqualIntegerAttributes(PqlEntityType::Print, PqlEntityType::Print);
		std::vector<int> res8 = Attribute::getEqualIntegerAttributes(PqlEntityType::Print, PqlEntityType::Stmt);
		std::vector<int> res9 = Attribute::getEqualIntegerAttributes(PqlEntityType::Stmt, PqlEntityType::Print);
		Assert::IsTrue(expectedRes3 == res7);
		Assert::IsTrue(expectedRes3 == res8);
		Assert::IsTrue(expectedRes3 == res9);

		std::vector<int> expectedRes4{ 1, 2, 3 };
		std::vector<int> res10 = Attribute::getEqualIntegerAttributes(PqlEntityType::Stmt, PqlEntityType::Stmt);
		Assert::IsTrue(expectedRes4 == res10);

		std::vector<int> res11 = Attribute::getEqualIntegerAttributes(PqlEntityType::Read, PqlEntityType::Print);
		std::vector<int> res12 = Attribute::getEqualIntegerAttributes(PqlEntityType::Call, PqlEntityType::Read);
		Assert::IsTrue(0 == res11.size());
		Assert::IsTrue(0 == res12.size());
	}

	TEST_METHOD(getEqualIntegerAttributes_differentEntityTypes_stmtsAndConsts) {
		Entity::insertConst(2);
		Entity::insertConst(9);
		StmtIndex stmtIdx1 = Entity::insertStmt(assignType);
		StmtIndex stmtIdx2 = Entity::insertStmt(ifType);
		StmtIndex stmtIdx3 = Entity::insertStmt(whileType);

		std::vector<int> expectedRes1{ 1 };
		std::vector<int> res1 = Attribute::getEqualIntegerAttributes(PqlEntityType::Assign, PqlEntityType::Assign);
		std::vector<int> res2 = Attribute::getEqualIntegerAttributes(PqlEntityType::Assign, PqlEntityType::Stmt);
		std::vector<int> res3 = Attribute::getEqualIntegerAttributes(PqlEntityType::Stmt, PqlEntityType::Assign);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes1 == res2);
		Assert::IsTrue(expectedRes1 == res3);

		std::vector<int> expectedRes2{ 2 };
		std::vector<int> res4 = Attribute::getEqualIntegerAttributes(PqlEntityType::If, PqlEntityType::If);
		std::vector<int> res5 = Attribute::getEqualIntegerAttributes(PqlEntityType::If, PqlEntityType::Stmt);
		std::vector<int> res6 = Attribute::getEqualIntegerAttributes(PqlEntityType::Stmt, PqlEntityType::If);
		std::vector<int> res7 = Attribute::getEqualIntegerAttributes(PqlEntityType::Stmt, PqlEntityType::Constant);
		std::vector<int> res8 = Attribute::getEqualIntegerAttributes(PqlEntityType::Constant, PqlEntityType::Stmt);
		Assert::IsTrue(expectedRes2 == res4);
		Assert::IsTrue(expectedRes2 == res5);
		Assert::IsTrue(expectedRes2 == res6);
		Assert::IsTrue(expectedRes2 == res7);
		Assert::IsTrue(expectedRes2 == res8);

		std::vector<int> expectedRes3{ 3 };
		std::vector<int> res9 = Attribute::getEqualIntegerAttributes(PqlEntityType::While, PqlEntityType::While);
		std::vector<int> res10 = Attribute::getEqualIntegerAttributes(PqlEntityType::While, PqlEntityType::Stmt);
		std::vector<int> res11 = Attribute::getEqualIntegerAttributes(PqlEntityType::Stmt, PqlEntityType::While);
		Assert::IsTrue(expectedRes3 == res9);
		Assert::IsTrue(expectedRes3 == res10);
		Assert::IsTrue(expectedRes3 == res11);

		std::vector<int> expectedRes4{ 1, 2, 3 };
		std::vector<int> res12 = Attribute::getEqualIntegerAttributes(PqlEntityType::Stmt, PqlEntityType::Stmt);
		Assert::IsTrue(expectedRes4 == res12);

		std::vector<int> expectedRes5{ 2, 9 };
		std::vector<int> res13 = Attribute::getEqualIntegerAttributes(PqlEntityType::Constant, PqlEntityType::Constant);
		Assert::IsTrue(expectedRes5 == res13);

		std::vector<int> res14 = Attribute::getEqualIntegerAttributes(PqlEntityType::Assign, PqlEntityType::If);
		std::vector<int> res15 = Attribute::getEqualIntegerAttributes(PqlEntityType::If, PqlEntityType::While);
		std::vector<int> res16 = Attribute::getEqualIntegerAttributes(PqlEntityType::While, PqlEntityType::Constant);
		Assert::IsTrue(0 == res14.size());
		Assert::IsTrue(0 == res15.size());
		Assert::IsTrue(0 == res16.size());
	}

	TEST_METHOD(hasEqualIntegerAttribute_oneEntity_sameIntQueried_sameEntityQueried) {
		StmtIndex stmtIdx = Entity::insertStmt(readType, name1);

		int nameIdx = Attribute::getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		bool res1 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Read, 1);
		bool res2 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Stmt, 1);

		Assert::IsTrue(res1);
		Assert::IsTrue(res2);
	}

	TEST_METHOD(hasEqualIntegerAttribute_oneEntity_differentIntQueried_sameEntityQueried) {
		StmtIndex stmtIdx = Entity::insertStmt(readType, name1);

		int nameIdx = Attribute::getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		bool res1 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Read, 2);
		bool res2 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Stmt, 2);

		Assert::IsFalse(res1);
		Assert::IsFalse(res2);
	}

	TEST_METHOD(hasEqualIntegerAttribute_oneEntity_sameIntQueried_differentEntityQueried) {
		StmtIndex stmtIdx = Entity::insertStmt(readType, name1);

		int nameIdx = Attribute::getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		bool res1 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Assign, 1);
		bool res2 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Constant, 1);

		Assert::IsFalse(res1);
		Assert::IsFalse(res2);
	}

	TEST_METHOD(hasEqualIntegerAttribute_oneEntity_differentIntQueried_differentEntityQueried) {
		StmtIndex stmtIdx = Entity::insertStmt(readType, name1);

		int nameIdx = Attribute::getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		bool res1 = Attribute::hasEqualIntegerAttribute(PqlEntityType::While, 2);
		bool res2 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Call, 3);

		Assert::IsFalse(res1);
		Assert::IsFalse(res2);
	}

	TEST_METHOD(hasEqualIntegerAttribute_manyEntityTypes_sameEntityType) {
		StmtIndex stmtIdx1 = Entity::insertStmt(callType, name1);
		StmtIndex stmtIdx2 = Entity::insertStmt(callType, name1);

		int nameIdx = Attribute::getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		bool res1 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Call, 1);
		bool res2 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Stmt, 1);
		bool res3 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Call, 2);
		bool res4 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Stmt, 2);

		Assert::IsTrue(res1);
		Assert::IsTrue(res2);
		Assert::IsTrue(res3);
		Assert::IsTrue(res4);
	}

	TEST_METHOD(hasEqualIntegerAttribute_manyEntityTypes_differentEntityTypes_mixedInt) {
		Entity::insertConst(3);
		Entity::insertConst(4);
		StmtIndex stmtIdx1 = Entity::insertStmt(callType, name2);
		StmtIndex stmtIdx2 = Entity::insertStmt(assignType);
		StmtIndex stmtIdx3 = Entity::insertStmt(readType, name1);
		StmtIndex stmtIdx4 = Entity::insertStmt(printType, name1);
		StmtIndex stmtIdx5 = Entity::insertStmt(readType, name3);

		int nameIdx1 = Attribute::getNameIdx(name1);
		int nameIdx2 = Attribute::getNameIdx(name2);
		int nameIdx3 = Attribute::getNameIdx(name3);
		Assert::IsTrue(2 == nameIdx1);
		Assert::IsTrue(1 == nameIdx2);
		Assert::IsTrue(3 == nameIdx3);

		bool res1 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Call, 1);
		bool res2 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Assign, 2);
		bool res3 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Read, 3);
		bool res4 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Print, 4);
		bool res5 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Read, 5);
		bool res6 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Constant, 3);
		bool res7 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Constant, 4);
		bool res8 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Constant, 1);
		bool res9 = Attribute::hasEqualIntegerAttribute(PqlEntityType::Call, 2);

		Assert::IsTrue(res1);
		Assert::IsTrue(res2);
		Assert::IsTrue(res3);
		Assert::IsTrue(res4);
		Assert::IsTrue(res5);
		Assert::IsTrue(res6);
		Assert::IsTrue(res7);
		Assert::IsFalse(res8);
		Assert::IsFalse(res9);
	}

	TEST_METHOD(performCleanUp) {
		Attribute::insertNameValue(name1);
		Attribute::insertNameValue(name2);
		int res = Attribute::getNameIdx(name2);
		Assert::IsTrue(2 == res);

		Attribute::performCleanUp();

		Attribute::insertNameValue(name3);
		int res2 = Attribute::getNameIdx(name3);
		int res3 = Attribute::getNameIdx(name1);
		Assert::IsTrue(1 == res2);
		Assert::IsFalse(1 == res3);
	}
	};
}
