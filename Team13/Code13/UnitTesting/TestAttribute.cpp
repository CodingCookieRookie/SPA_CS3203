#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/Common/Types.h"
#include "../source/PKB/Attribute.h"
#include "../source/PKB/Constant.h"
#include "../source/PKB/NameIndexEntity.h"
#include "../source/PKB/Procedure.h"
#include "../source/PKB/ProcedureStatementMapper.h"
#include "../source/PKB/Statement.h"
#include "../source/PKB/Variable.h"

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

	StatementType assignType = StatementType::ASSIGN_TYPE;
	StatementType whileType = StatementType::WHILE_TYPE;
	StatementType callType = StatementType::CALL_TYPE;
	StatementType readType = StatementType::READ_TYPE;
	StatementType printType = StatementType::PRINT_TYPE;
	StatementType ifType = StatementType::IF_TYPE;

	Attribute* attribute;
	Constant* constant;
	Procedure* procedure;
	Statement* statement;
	Variable* variable;

	TEST_METHOD_INITIALIZE(initTables) {
		attribute = new Attribute();
		constant = new Constant();
		procedure = new Procedure();
		statement = new Statement();
		variable = new Variable();
	}

public:

	TEST_METHOD(insertNameValue_getNameIdx_nameExists) {
		NameIndex nameIdx = attribute->insertNameValue(name1);
		NameIndex res = attribute->getNameIdx(name1);

		Assert::IsTrue(nameIdx == res);
		Assert::IsTrue(1 == res);
	}

	TEST_METHOD(insertNameValue_getNameIdx_nameDoesNotExist) {
		NameIndex nameIdx = attribute->insertNameValue(name2);
		NameIndex res = attribute->getNameIdx(name1);

		Assert::IsFalse(nameIdx == res);
	}

	TEST_METHOD(insertNameValue_getNameIdx_differentNames) {
		NameIndex nameIdx1 = attribute->insertNameValue(name1);
		NameIndex nameIdx2 = attribute->insertNameValue(name2);

		NameIndex res1 = attribute->getNameIdx(name1);
		NameIndex res2 = attribute->getNameIdx(name2);

		Assert::AreEqual(nameIdx1, res1);
		Assert::AreEqual(nameIdx2, res2);
		Assert::IsTrue(1 == res1);
		Assert::IsTrue(2 == res2);
	}

	TEST_METHOD(insertNameValue_getNameIdx_sameName) {
		NameIndex nameIdx1 = attribute->insertNameValue(name1);
		NameIndex nameIdx2 = attribute->insertNameValue(name1);

		NameIndex res = attribute->getNameIdx(name1);

		Assert::AreEqual(nameIdx1, res);
		Assert::AreEqual(nameIdx2, res);
		Assert::IsTrue(1 == res);
	}

	TEST_METHOD(insertVar_getNameIdx_nameExists) {
		VarIndex idx = variable->insert(varName1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(varName1);
		Assert::IsTrue(1 == nameIdx);
	}

	TEST_METHOD(insertVar_getNameIdx_nameDoesNotExist) {
		VarIndex idx = variable->insert(varName1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(varName2);
		Assert::IsTrue(0 == nameIdx);
	}

	TEST_METHOD(insertVar_getNameIdx_differentNames) {
		VarIndex idx1 = variable->insert(varName1, attribute);
		VarIndex idx2 = variable->insert(varName2, attribute);

		NameIndex nameIdx1 = attribute->getNameIdx(varName1);
		NameIndex nameIdx2 = attribute->getNameIdx(varName2);
		Assert::IsTrue(1 == nameIdx1);
		Assert::IsTrue(2 == nameIdx2);
	}

	TEST_METHOD(insertVar_getNameIdx_sameName) {
		VarIndex idx1 = variable->insert(varName1, attribute);
		VarIndex idx2 = variable->insert(varName1, attribute);

		NameIndex nameIdx1 = attribute->getNameIdx(varName1);
		NameIndex nameIdx2 = attribute->getNameIdx(varName2);
		Assert::IsTrue(1 == nameIdx1);
		Assert::IsFalse(2 == nameIdx2);
	}

	TEST_METHOD(insertProc_getNameIdx_nameExists) {
		ProcIndex idx = procedure->insert(procName1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(procName1);
		Assert::IsTrue(1 == nameIdx);
	}

	TEST_METHOD(insertProc_getNameIdx_nameDoesNotExist) {
		ProcIndex idx = procedure->insert(procName1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(procName2);
		Assert::IsTrue(0 == nameIdx);
	}

	TEST_METHOD(insertProc_getNameIdx_differentNames) {
		ProcIndex idx1 = procedure->insert(procName1, attribute);
		ProcIndex idx2 = procedure->insert(procName2, attribute);

		NameIndex nameIdx1 = attribute->getNameIdx(procName1);
		NameIndex nameIdx2 = attribute->getNameIdx(procName2);
		Assert::IsTrue(1 == nameIdx1);
		Assert::IsTrue(2 == nameIdx2);
	}

	TEST_METHOD(insertProc_getNameIdx_sameName) {
		ProcIndex idx1 = procedure->insert(procName1, attribute);
		ProcIndex idx2 = procedure->insert(procName1, attribute);

		NameIndex nameIdx1 = attribute->getNameIdx(procName1);
		NameIndex nameIdx2 = attribute->getNameIdx(procName2);
		Assert::IsTrue(1 == nameIdx1);
		Assert::IsFalse(2 == nameIdx2);
	}

	TEST_METHOD(insertCallStmt_getNameIdx_stmtExists) {
		StmtIndex stmtIdx = statement->insertStmt(callType, name1, attribute);

		NameIndex res = attribute->getNameIdx(name1);

		Assert::IsTrue(1 == res);
	}

	TEST_METHOD(insertCallStmt_getNameIdx_stmtDoesNotExist) {
		StmtIndex stmtIdx = statement->insertStmt(callType, name1, attribute);

		NameIndex res = attribute->getNameIdx(name2);

		Assert::IsTrue(0 == res);
	}

	TEST_METHOD(insertCallStmt_getNameIdx_multipleStmts_differentNames) {
		StmtIndex stmtIdx1 = statement->insertStmt(callType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(callType, name2, attribute);

		NameIndex res1 = attribute->getNameIdx(name1);
		NameIndex res2 = attribute->getNameIdx(name2);

		Assert::IsTrue(1 == res1);
		Assert::IsTrue(2 == res2);
	}

	TEST_METHOD(insertCallStmt_getNameIdx_multipleStmts_sameName) {
		StmtIndex stmtIdx1 = statement->insertStmt(callType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(callType, name1, attribute);

		NameIndex res1 = attribute->getNameIdx(name1);
		NameIndex res2 = attribute->getNameIdx(name2);

		Assert::IsTrue(1 == res1);
		Assert::IsFalse(2 == res2);
	}

	TEST_METHOD(insertReadStmt_getNameIdx_stmtExists) {
		StmtIndex stmtIdx = statement->insertStmt(readType, name1, attribute);

		NameIndex res = attribute->getNameIdx(name1);

		Assert::IsTrue(1 == res);
	}

	TEST_METHOD(insertReadStmt_getNameIdx_stmtDoesNotExist) {
		StmtIndex stmtIdx = statement->insertStmt(readType, name1, attribute);

		NameIndex res = attribute->getNameIdx(name2);

		Assert::IsTrue(0 == res);
	}

	TEST_METHOD(insertReadStmt_getNameIdx_multipleStmts_differentNames) {
		StmtIndex stmtIdx1 = statement->insertStmt(readType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(readType, name2, attribute);

		NameIndex res1 = attribute->getNameIdx(name1);
		NameIndex res2 = attribute->getNameIdx(name2);

		Assert::IsTrue(1 == res1);
		Assert::IsTrue(2 == res2);
	}

	TEST_METHOD(insertReadStmt_getNameIdx_multipleStmts_sameName) {
		StmtIndex stmtIdx1 = statement->insertStmt(readType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(readType, name1, attribute);

		NameIndex res1 = attribute->getNameIdx(name1);
		NameIndex res2 = attribute->getNameIdx(name2);

		Assert::IsTrue(1 == res1);
		Assert::IsFalse(2 == res2);
	}

	TEST_METHOD(insertPrintStmt_getNameIdx_stmtExists) {
		StmtIndex stmtIdx = statement->insertStmt(printType, name1, attribute);

		NameIndex res = attribute->getNameIdx(name1);

		Assert::IsTrue(1 == res);
	}

	TEST_METHOD(insertPrintStmt_getNameIdx_stmtDoesNotExist) {
		StmtIndex stmtIdx = statement->insertStmt(printType, name1, attribute);

		NameIndex res = attribute->getNameIdx(name2);

		Assert::IsTrue(0 == res);
	}

	TEST_METHOD(insertPrintStmt_getNameIdx_multipleStmts_differentNames) {
		StmtIndex stmtIdx1 = statement->insertStmt(printType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(printType, name2, attribute);

		NameIndex res1 = attribute->getNameIdx(name1);
		NameIndex res2 = attribute->getNameIdx(name2);

		Assert::IsTrue(1 == res1);
		Assert::IsTrue(2 == res2);
	}

	TEST_METHOD(insertPrintStmt_getNameIdx_multipleStmts_sameName) {
		StmtIndex stmtIdx1 = statement->insertStmt(printType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(printType, name1, attribute);

		NameIndex res1 = attribute->getNameIdx(name1);
		NameIndex res2 = attribute->getNameIdx(name2);

		Assert::IsTrue(1 == res1);
		Assert::IsFalse(2 == res2);
	}

	TEST_METHOD(insertStmt_getNameIdx_differentStmtTypes_differentNames) {
		StmtIndex stmtIdx1 = statement->insertStmt(callType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(printType, name2, attribute);
		StmtIndex stmtIdx3 = statement->insertStmt(readType, name3, attribute);

		NameIndex res1 = attribute->getNameIdx(name1);
		NameIndex res2 = attribute->getNameIdx(name2);
		NameIndex res3 = attribute->getNameIdx(name3);

		Assert::IsTrue(1 == res1);
		Assert::IsTrue(2 == res2);
		Assert::IsTrue(3 == res3);
	}

	TEST_METHOD(insertStmt_getNameIdx_differentStmtTypes_sameName) {
		StmtIndex stmtIdx1 = statement->insertStmt(callType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(printType, name1, attribute);
		StmtIndex stmtIdx3 = statement->insertStmt(readType, name1, attribute);

		NameIndex res1 = attribute->getNameIdx(name1);
		NameIndex res2 = attribute->getNameIdx(name2);
		NameIndex res3 = attribute->getNameIdx(name3);

		Assert::IsTrue(1 == res1);
		Assert::IsFalse(2 == res2);
		Assert::IsFalse(3 == res3);
	}

	TEST_METHOD(insertCallStmt_getAttributeNameByStmtIdx_singleStmt) {
		StmtIndex stmtIdx = statement->insertStmt(callType, name1, attribute);

		std::string res = attribute->getAttributeNameByStmtIdx(stmtIdx);

		Assert::IsTrue(name1 == res);
	}

	TEST_METHOD(insertCallStmt_getAttributeNameByStmtIdx_multipleStmts_differentNames) {
		StmtIndex stmtIdx1 = statement->insertStmt(callType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(callType, name2, attribute);

		std::string res1 = attribute->getAttributeNameByStmtIdx(stmtIdx1);
		std::string res2 = attribute->getAttributeNameByStmtIdx(stmtIdx2);

		Assert::IsTrue(name1 == res1);
		Assert::IsTrue(name2 == res2);
	}

	TEST_METHOD(insertCallStmt_getAttributeNameByStmtIdx_multipleStmts_sameName) {
		StmtIndex stmtIdx1 = statement->insertStmt(callType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(callType, name1, attribute);

		std::string res1 = attribute->getAttributeNameByStmtIdx(stmtIdx1);
		std::string res2 = attribute->getAttributeNameByStmtIdx(stmtIdx2);

		Assert::IsTrue(name1 == res1);
		Assert::IsTrue(name1 == res2);
	}

	TEST_METHOD(insertReadStmt_getAttributeNameByStmtIdx_singleStmt) {
		StmtIndex stmtIdx = statement->insertStmt(readType, name1, attribute);

		std::string res = attribute->getAttributeNameByStmtIdx(stmtIdx);

		Assert::IsTrue(name1 == res);
	}

	TEST_METHOD(insertReadStmt_getAttributeNameByStmtIdx_multipleStmts_differentNames) {
		StmtIndex stmtIdx1 = statement->insertStmt(readType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(readType, name2, attribute);

		std::string res1 = attribute->getAttributeNameByStmtIdx(stmtIdx1);
		std::string res2 = attribute->getAttributeNameByStmtIdx(stmtIdx2);

		Assert::IsTrue(name1 == res1);
		Assert::IsTrue(name2 == res2);
	}

	TEST_METHOD(insertReadStmt_getAttributeNameByStmtIdx_multipleStmts_sameName) {
		StmtIndex stmtIdx1 = statement->insertStmt(readType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(readType, name1, attribute);

		std::string res1 = attribute->getAttributeNameByStmtIdx(stmtIdx1);
		std::string res2 = attribute->getAttributeNameByStmtIdx(stmtIdx2);

		Assert::IsTrue(name1 == res1);
		Assert::IsTrue(name1 == res2);
	}

	TEST_METHOD(insertPrintStmt_getAttributeNameByStmtIdx_singleStmt) {
		StmtIndex stmtIdx = statement->insertStmt(printType, name1, attribute);

		std::string res = attribute->getAttributeNameByStmtIdx(stmtIdx);

		Assert::IsTrue(name1 == res);
	}

	TEST_METHOD(insertPrintStmt_getAttributeNameByStmtIdx_multipleStmts_differentNames) {
		StmtIndex stmtIdx1 = statement->insertStmt(printType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(printType, name2, attribute);

		std::string res1 = attribute->getAttributeNameByStmtIdx(stmtIdx1);
		std::string res2 = attribute->getAttributeNameByStmtIdx(stmtIdx2);

		Assert::IsTrue(name1 == res1);
		Assert::IsTrue(name2 == res2);
	}

	TEST_METHOD(insertPrintStmt_getAttributeNameByStmtIdx_multipleStmts_sameName) {
		StmtIndex stmtIdx1 = statement->insertStmt(printType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(printType, name1, attribute);

		std::string res1 = attribute->getAttributeNameByStmtIdx(stmtIdx1);
		std::string res2 = attribute->getAttributeNameByStmtIdx(stmtIdx2);

		Assert::IsTrue(name1 == res1);
		Assert::IsTrue(name1 == res2);
	}

	TEST_METHOD(insertStmt_getAttributeNameByStmtIdx_differentStmtTypes_differentNames) {
		StmtIndex stmtIdx1 = statement->insertStmt(callType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(printType, name2, attribute);
		StmtIndex stmtIdx3 = statement->insertStmt(readType, name3, attribute);

		std::string res1 = attribute->getAttributeNameByStmtIdx(stmtIdx1);
		std::string res2 = attribute->getAttributeNameByStmtIdx(stmtIdx2);
		std::string res3 = attribute->getAttributeNameByStmtIdx(stmtIdx3);

		Assert::IsTrue(name1 == res1);
		Assert::IsTrue(name2 == res2);
		Assert::IsTrue(name3 == res3);
	}

	TEST_METHOD(insertStmt_getAttributeNameByStmtIdx_differentStmtTypes_sameName) {
		StmtIndex stmtIdx1 = statement->insertStmt(callType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(printType, name1, attribute);
		StmtIndex stmtIdx3 = statement->insertStmt(readType, name1, attribute);

		std::string res1 = attribute->getAttributeNameByStmtIdx(stmtIdx1);
		std::string res2 = attribute->getAttributeNameByStmtIdx(stmtIdx2);
		std::string res3 = attribute->getAttributeNameByStmtIdx(stmtIdx3);

		Assert::IsTrue(name1 == res1);
		Assert::IsTrue(name1 == res2);
		Assert::IsTrue(name1 == res3);
	}

	TEST_METHOD(getEqualNameAttributes_oneEntity_sameEntityQueried) {
		ProcIndex procIdx = procedure->insert(procName1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(procName1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<EntityAttributeRef> expectedLeftEntityTypeIndices{ 1 };
		std::vector<EntityAttributeRef> expectedRightEntityTypeIndices{ 1 };
		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> expectedRes
			= std::make_tuple(expectedLeftEntityTypeIndices, expectedRightEntityTypeIndices);

		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> res
			= attribute->getEqualNameAttributes(EntityType::PROCEDURE, EntityType::PROCEDURE);
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(getEqualNameAttributes_oneEntity_differentEntityQueried) {
		ProcIndex procIdx = procedure->insert(procName1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(procName1);
		Assert::IsTrue(1 == nameIdx);

		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> res1
			= attribute->getEqualNameAttributes(EntityType::VARIABLE, EntityType::VARIABLE);
		std::vector<EntityAttributeRef> res1LeftEntityTypeIndices = std::get<0>(res1);
		std::vector<EntityAttributeRef> res1RightEntityTypeIndices = std::get<1>(res1);
		Assert::IsTrue(0 == res1LeftEntityTypeIndices.size());
		Assert::IsTrue(0 == res1RightEntityTypeIndices.size());

		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> res2
			= attribute->getEqualNameAttributes(EntityType::PROCEDURE, EntityType::VARIABLE);
		std::vector<EntityAttributeRef> res2LeftEntityTypeIndices = std::get<0>(res2);
		std::vector<EntityAttributeRef> res2RightEntityTypeIndices = std::get<1>(res2);
		Assert::IsTrue(0 == res2LeftEntityTypeIndices.size());
		Assert::IsTrue(0 == res2RightEntityTypeIndices.size());
	}

	TEST_METHOD(getEqualNameAttributes_sameEntityType_differentNames) {
		ProcIndex procIdx1 = procedure->insert(procName1, attribute);
		ProcIndex procIdx2 = procedure->insert(procName2, attribute);

		NameIndex nameIdx1 = attribute->getNameIdx(procName1);
		NameIndex nameIdx2 = attribute->getNameIdx(procName2);
		Assert::IsTrue(1 == nameIdx1);
		Assert::IsTrue(2 == nameIdx2);

		std::vector<EntityAttributeRef> expectedLeftEntityTypeIndices{ 1, 2 };
		std::vector<EntityAttributeRef> expectedRightEntityTypeIndices{ 1, 2 };
		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> expectedRes
			= std::make_tuple(expectedLeftEntityTypeIndices, expectedRightEntityTypeIndices);

		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> res
			= attribute->getEqualNameAttributes(EntityType::PROCEDURE, EntityType::PROCEDURE);
		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(getEqualNameAttributes_differentEntityTypes_sameName) {
		ProcIndex procIdx = procedure->insert(name1, attribute);
		VarIndex varIdx = variable->insert(name1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<EntityAttributeRef> expectedLeftEntityTypeIndices{ 1 };
		std::vector<EntityAttributeRef> expectedRightEntityTypeIndices{ 1 };
		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> expectedRes
			= std::make_tuple(expectedLeftEntityTypeIndices, expectedRightEntityTypeIndices);

		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> res1
			= attribute->getEqualNameAttributes(EntityType::PROCEDURE, EntityType::VARIABLE);
		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> res2
			= attribute->getEqualNameAttributes(EntityType::VARIABLE, EntityType::PROCEDURE);

		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);
	}

	TEST_METHOD(getEqualNameAttributes_differentEntityTypes_manyEntityTypes) {
		ProcIndex procIdx1 = procedure->insert(name2, attribute);
		ProcIndex procIdx2 = procedure->insert(name1, attribute);
		VarIndex varIdx = variable->insert(name1, attribute);
		StmtIndex stmtIdx = statement->insertStmt(callType, name1, attribute);

		NameIndex nameIdx1 = attribute->getNameIdx(name1);
		NameIndex nameIdx2 = attribute->getNameIdx(name2);
		Assert::IsTrue(2 == nameIdx1);
		Assert::IsTrue(1 == nameIdx2);

		std::vector<EntityAttributeRef> expectedLeftEntityTypeIndices1{ 2 };
		std::vector<EntityAttributeRef> expectedRightEntityTypeIndices1{ 1 };
		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> expectedRes1
			= std::make_tuple(expectedLeftEntityTypeIndices1, expectedRightEntityTypeIndices1);

		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> res1
			= attribute->getEqualNameAttributes(EntityType::PROCEDURE, EntityType::VARIABLE);
		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> res2
			= attribute->getEqualNameAttributes(EntityType::PROCEDURE, EntityType::CALL);

		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes1 == res2);

		std::vector<EntityAttributeRef> expectedLeftEntityTypeIndices2{ 1 };
		std::vector<EntityAttributeRef> expectedRightEntityTypeIndices2{ 2 };
		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> expectedRes2
			= std::make_tuple(expectedLeftEntityTypeIndices2, expectedRightEntityTypeIndices2);

		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> res3
			= attribute->getEqualNameAttributes(EntityType::VARIABLE, EntityType::PROCEDURE);
		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> res4
			= attribute->getEqualNameAttributes(EntityType::CALL, EntityType::PROCEDURE);

		Assert::IsTrue(expectedRes2 == res3);
		Assert::IsTrue(expectedRes2 == res4);

		std::vector<EntityAttributeRef> expectedLeftEntityTypeIndices3{ 1 };
		std::vector<EntityAttributeRef> expectedRightEntityTypeIndices3{ 1 };
		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> expectedRes3
			= std::make_tuple(expectedLeftEntityTypeIndices3, expectedRightEntityTypeIndices3);

		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> res5
			= attribute->getEqualNameAttributes(EntityType::CALL, EntityType::VARIABLE);
		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> res6
			= attribute->getEqualNameAttributes(EntityType::VARIABLE, EntityType::CALL);
		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> res7
			= attribute->getEqualNameAttributes(EntityType::CALL, EntityType::CALL);
		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> res8
			= attribute->getEqualNameAttributes(EntityType::VARIABLE, EntityType::VARIABLE);

		Assert::IsTrue(expectedRes3 == res5);
		Assert::IsTrue(expectedRes3 == res6);
		Assert::IsTrue(expectedRes3 == res7);
		Assert::IsTrue(expectedRes3 == res8);

		std::vector<EntityAttributeRef> expectedLeftEntityTypeIndices4{ 1, 2 };
		std::vector<EntityAttributeRef> expectedRightEntityTypeIndices4{ 1, 2 };
		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> expectedRes4
			= std::make_tuple(expectedLeftEntityTypeIndices4, expectedRightEntityTypeIndices4);

		std::tuple<std::vector<EntityAttributeRef>, std::vector<EntityAttributeRef>> res9
			= attribute->getEqualNameAttributes(EntityType::PROCEDURE, EntityType::PROCEDURE);

		Assert::IsTrue(expectedRes4 == res9);
	}

	TEST_METHOD(getEqualNameAttributesFromName_oneEntity_sameNameQueried_sameEntityQueried) {
		ProcIndex procIdx = procedure->insert(name1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<EntityAttributeRef> expectedRes{ 1 };

		std::vector<EntityAttributeRef> res = attribute->getEqualNameAttributesFromName(EntityType::PROCEDURE, name1);

		Assert::IsTrue(expectedRes == res);
	}

	TEST_METHOD(getEqualNameAttributesFromName_oneEntity_differentNameQueried_sameEntityQueried) {
		ProcIndex procIdx = procedure->insert(name1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<EntityAttributeRef> res = attribute->getEqualNameAttributesFromName(EntityType::PROCEDURE, name2);

		Assert::IsTrue(0 == res.size());
	}

	TEST_METHOD(getEqualNameAttributesFromName_oneEntity_sameNameQueried_differentEntityQueried) {
		ProcIndex procIdx = procedure->insert(name1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<EntityAttributeRef> res = attribute->getEqualNameAttributesFromName(EntityType::VARIABLE, name1);

		Assert::IsTrue(0 == res.size());
	}

	TEST_METHOD(getEqualNameAttributesFromName_oneEntity_differentNameQueried_differentEntityQueried) {
		ProcIndex procIdx = procedure->insert(name1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<EntityAttributeRef> res = attribute->getEqualNameAttributesFromName(EntityType::VARIABLE, name2);

		Assert::IsTrue(0 == res.size());
	}

	TEST_METHOD(getEqualNameAttributesFromName_manyEntityTypes_sameEntityType_sameName) {
		StmtIndex stmtIdx1 = statement->insertStmt(callType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(callType, name1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<EntityAttributeRef> expectedRes{ 1, 2 };

		std::vector<EntityAttributeRef> res1 = attribute->getEqualNameAttributesFromName(EntityType::CALL, name1);
		std::vector<EntityAttributeRef> res2 = attribute->getEqualNameAttributesFromName(EntityType::READ, name1);
		std::vector<EntityAttributeRef> res3 = attribute->getEqualNameAttributesFromName(EntityType::PRINT, name1);

		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(0 == res2.size());
		Assert::IsTrue(0 == res3.size());
	}

	TEST_METHOD(getEqualNameAttributesFromName_manyEntityTypes_differentEntityTypes_mixedNames) {
		StmtIndex stmtIdx1 = statement->insertStmt(callType, name2, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(callType, name1, attribute);
		StmtIndex stmtIdx3 = statement->insertStmt(readType, name1, attribute);
		StmtIndex stmtIdx4 = statement->insertStmt(printType, name1, attribute);

		NameIndex nameIdx1 = attribute->getNameIdx(name1);
		NameIndex nameIdx2 = attribute->getNameIdx(name2);
		Assert::IsTrue(2 == nameIdx1);
		Assert::IsTrue(1 == nameIdx2);

		std::vector<EntityAttributeRef> expectedRes1{ 2 };
		std::vector<EntityAttributeRef> res1 = attribute->getEqualNameAttributesFromName(EntityType::CALL, name1);
		Assert::IsTrue(expectedRes1 == res1);

		std::vector<EntityAttributeRef> expectedRes2{ 3 };

		std::vector<EntityAttributeRef> res2 = attribute->getEqualNameAttributesFromName(EntityType::READ, name1);
		Assert::IsTrue(expectedRes2 == res2);

		std::vector<EntityAttributeRef> expectedRes3{ 4 };
		std::vector<EntityAttributeRef> res3 = attribute->getEqualNameAttributesFromName(EntityType::PRINT, name1);
		Assert::IsTrue(expectedRes3 == res3);

		std::vector<EntityAttributeRef> expectedRes4{ 1 };
		std::vector<EntityAttributeRef> res4 = attribute->getEqualNameAttributesFromName(EntityType::CALL, name2);
		Assert::IsTrue(expectedRes4 == res4);

		std::vector<EntityAttributeRef> res5 = attribute->getEqualNameAttributesFromName(EntityType::READ, name2);
		std::vector<EntityAttributeRef> res6 = attribute->getEqualNameAttributesFromName(EntityType::PRINT, name2);
		Assert::IsTrue(0 == res5.size());
		Assert::IsTrue(0 == res6.size());
	}

	TEST_METHOD(getEqualIntegerAttributes_oneEntity_sameEntityQueried) {
		StmtIndex stmtIdx = statement->insertStmt(callType, name1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<EntityAttributeRef> expectedRes{ 1 };

		std::vector<EntityAttributeRef> res1 = attribute->getEqualIntegerAttributes(EntityType::CALL, EntityType::CALL, constant, statement);
		std::vector<EntityAttributeRef> res2 = attribute->getEqualIntegerAttributes(EntityType::STMT, EntityType::STMT, constant, statement);
		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);
	}

	TEST_METHOD(getEqualIntegerAttributes_oneEntity_differentEntityQueried) {
		StmtIndex stmtIdx = statement->insertStmt(readType, name1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		std::vector<EntityAttributeRef> expectedRes{ 1 };
		std::vector<EntityAttributeRef> res1 = attribute->getEqualIntegerAttributes(EntityType::READ, EntityType::STMT, constant, statement);
		std::vector<EntityAttributeRef> res2 = attribute->getEqualIntegerAttributes(EntityType::STMT, EntityType::READ, constant, statement);
		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);

		std::vector<EntityAttributeRef> res3 = attribute->getEqualIntegerAttributes(EntityType::READ, EntityType::CALL, constant, statement);
		std::vector<EntityAttributeRef> res4 = attribute->getEqualIntegerAttributes(EntityType::CALL, EntityType::STMT, constant, statement);
		Assert::IsTrue(0 == res3.size());
		Assert::IsTrue(0 == res4.size());
	}

	TEST_METHOD(getEqualIntegerAttributes_sameEntityType) {
		StmtIndex stmtIdx1 = statement->insertStmt(readType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(readType, name2, attribute);

		NameIndex nameIdx1 = attribute->getNameIdx(name1);
		NameIndex nameIdx2 = attribute->getNameIdx(name2);
		Assert::IsTrue(1 == nameIdx1);
		Assert::IsTrue(2 == nameIdx2);

		std::vector<EntityAttributeRef> expectedRes{ 1, 2 };
		std::vector<EntityAttributeRef> res1 = attribute->getEqualIntegerAttributes(EntityType::READ, EntityType::READ, constant, statement);
		std::vector<EntityAttributeRef> res2 = attribute->getEqualIntegerAttributes(EntityType::READ, EntityType::STMT, constant, statement);
		std::vector<EntityAttributeRef> res3 = attribute->getEqualIntegerAttributes(EntityType::STMT, EntityType::STMT, constant, statement);
		Assert::IsTrue(expectedRes == res1);
		Assert::IsTrue(expectedRes == res2);
		Assert::IsTrue(expectedRes == res3);
	}

	TEST_METHOD(getEqualIntegerAttributes_differentEntityTypes_onlyStmts) {
		StmtIndex stmtIdx1 = statement->insertStmt(readType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(callType, name2, attribute);
		StmtIndex stmtIdx3 = statement->insertStmt(printType, name2, attribute);

		NameIndex nameIdx1 = attribute->getNameIdx(name1);
		NameIndex nameIdx2 = attribute->getNameIdx(name2);
		Assert::IsTrue(1 == nameIdx1);
		Assert::IsTrue(2 == nameIdx2);

		std::vector<EntityAttributeRef> expectedRes1{ 1 };
		std::vector<EntityAttributeRef> res1 = attribute->getEqualIntegerAttributes(EntityType::READ, EntityType::READ, constant, statement);
		std::vector<EntityAttributeRef> res2 = attribute->getEqualIntegerAttributes(EntityType::READ, EntityType::STMT, constant, statement);
		std::vector<EntityAttributeRef> res3 = attribute->getEqualIntegerAttributes(EntityType::STMT, EntityType::READ, constant, statement);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes1 == res2);
		Assert::IsTrue(expectedRes1 == res3);

		std::vector<EntityAttributeRef> expectedRes2{ 2 };
		std::vector<EntityAttributeRef> res4 = attribute->getEqualIntegerAttributes(EntityType::CALL, EntityType::CALL, constant, statement);
		std::vector<EntityAttributeRef> res5 = attribute->getEqualIntegerAttributes(EntityType::CALL, EntityType::STMT, constant, statement);
		std::vector<EntityAttributeRef> res6 = attribute->getEqualIntegerAttributes(EntityType::STMT, EntityType::CALL, constant, statement);
		Assert::IsTrue(expectedRes2 == res4);
		Assert::IsTrue(expectedRes2 == res5);
		Assert::IsTrue(expectedRes2 == res6);

		std::vector<EntityAttributeRef> expectedRes3{ 3 };
		std::vector<EntityAttributeRef> res7 = attribute->getEqualIntegerAttributes(EntityType::PRINT, EntityType::PRINT, constant, statement);
		std::vector<EntityAttributeRef> res8 = attribute->getEqualIntegerAttributes(EntityType::PRINT, EntityType::STMT, constant, statement);
		std::vector<EntityAttributeRef> res9 = attribute->getEqualIntegerAttributes(EntityType::STMT, EntityType::PRINT, constant, statement);
		Assert::IsTrue(expectedRes3 == res7);
		Assert::IsTrue(expectedRes3 == res8);
		Assert::IsTrue(expectedRes3 == res9);

		std::vector<EntityAttributeRef> expectedRes4{ 1, 2, 3 };
		std::vector<EntityAttributeRef> res10 = attribute->getEqualIntegerAttributes(EntityType::STMT, EntityType::STMT, constant, statement);
		Assert::IsTrue(expectedRes4 == res10);

		std::vector<EntityAttributeRef> res11 = attribute->getEqualIntegerAttributes(EntityType::READ, EntityType::PRINT, constant, statement);
		std::vector<EntityAttributeRef> res12 = attribute->getEqualIntegerAttributes(EntityType::CALL, EntityType::READ, constant, statement);
		Assert::IsTrue(0 == res11.size());
		Assert::IsTrue(0 == res12.size());
	}

	TEST_METHOD(getEqualIntegerAttributes_differentEntityTypes_stmtsAndConsts) {
		constant->insertConst(2);
		constant->insertConst(9);
		StmtIndex stmtIdx1 = statement->insertStmt(assignType);
		StmtIndex stmtIdx2 = statement->insertStmt(ifType);
		StmtIndex stmtIdx3 = statement->insertStmt(whileType);

		std::vector<EntityAttributeRef> expectedRes1{ 1 };
		std::vector<EntityAttributeRef> res1 = attribute->getEqualIntegerAttributes(EntityType::ASSIGN, EntityType::ASSIGN, constant, statement);
		std::vector<EntityAttributeRef> res2 = attribute->getEqualIntegerAttributes(EntityType::ASSIGN, EntityType::STMT, constant, statement);
		std::vector<EntityAttributeRef> res3 = attribute->getEqualIntegerAttributes(EntityType::STMT, EntityType::ASSIGN, constant, statement);
		Assert::IsTrue(expectedRes1 == res1);
		Assert::IsTrue(expectedRes1 == res2);
		Assert::IsTrue(expectedRes1 == res3);

		std::vector<EntityAttributeRef> expectedRes2{ 2 };
		std::vector<EntityAttributeRef> res4 = attribute->getEqualIntegerAttributes(EntityType::IF, EntityType::IF, constant, statement);
		std::vector<EntityAttributeRef> res5 = attribute->getEqualIntegerAttributes(EntityType::IF, EntityType::STMT, constant, statement);
		std::vector<EntityAttributeRef> res6 = attribute->getEqualIntegerAttributes(EntityType::STMT, EntityType::IF, constant, statement);
		std::vector<EntityAttributeRef> res7 = attribute->getEqualIntegerAttributes(EntityType::STMT, EntityType::CONSTANT, constant, statement);
		std::vector<EntityAttributeRef> res8 = attribute->getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT, constant, statement);
		Assert::IsTrue(expectedRes2 == res4);
		Assert::IsTrue(expectedRes2 == res5);
		Assert::IsTrue(expectedRes2 == res6);
		Assert::IsTrue(expectedRes2 == res7);
		Assert::IsTrue(expectedRes2 == res8);

		std::vector<EntityAttributeRef> expectedRes3{ 3 };
		std::vector<EntityAttributeRef> res9 = attribute->getEqualIntegerAttributes(EntityType::WHILE, EntityType::WHILE, constant, statement);
		std::vector<EntityAttributeRef> res10 = attribute->getEqualIntegerAttributes(EntityType::WHILE, EntityType::STMT, constant, statement);
		std::vector<EntityAttributeRef> res11 = attribute->getEqualIntegerAttributes(EntityType::STMT, EntityType::WHILE, constant, statement);
		Assert::IsTrue(expectedRes3 == res9);
		Assert::IsTrue(expectedRes3 == res10);
		Assert::IsTrue(expectedRes3 == res11);

		std::vector<EntityAttributeRef> expectedRes4{ 1, 2, 3 };
		std::vector<EntityAttributeRef> res12 = attribute->getEqualIntegerAttributes(EntityType::STMT, EntityType::STMT, constant, statement);
		Assert::IsTrue(expectedRes4 == res12);

		std::vector<EntityAttributeRef> expectedRes5{ 2, 9 };
		std::vector<EntityAttributeRef> res13 = attribute->getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::CONSTANT, constant, statement);
		Assert::IsTrue(expectedRes5 == res13);

		std::vector<EntityAttributeRef> res14 = attribute->getEqualIntegerAttributes(EntityType::ASSIGN, EntityType::IF, constant, statement);
		std::vector<EntityAttributeRef> res15 = attribute->getEqualIntegerAttributes(EntityType::IF, EntityType::WHILE, constant, statement);
		std::vector<EntityAttributeRef> res16 = attribute->getEqualIntegerAttributes(EntityType::WHILE, EntityType::CONSTANT, constant, statement);
		Assert::IsTrue(0 == res14.size());
		Assert::IsTrue(0 == res15.size());
		Assert::IsTrue(0 == res16.size());
	}

	TEST_METHOD(hasEqualIntegerAttribute_oneEntity_sameIntQueried_sameEntityQueried) {
		StmtIndex stmtIdx = statement->insertStmt(readType, name1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		bool res1 = attribute->hasEqualIntegerAttribute(EntityType::READ, 1, constant, statement);
		bool res2 = attribute->hasEqualIntegerAttribute(EntityType::STMT, 1, constant, statement);

		Assert::IsTrue(res1);
		Assert::IsTrue(res2);
	}

	TEST_METHOD(hasEqualIntegerAttribute_oneEntity_differentIntQueried_sameEntityQueried) {
		StmtIndex stmtIdx = statement->insertStmt(readType, name1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		bool res1 = attribute->hasEqualIntegerAttribute(EntityType::READ, 2, constant, statement);
		bool res2 = attribute->hasEqualIntegerAttribute(EntityType::STMT, 2, constant, statement);

		Assert::IsFalse(res1);
		Assert::IsFalse(res2);
	}

	TEST_METHOD(hasEqualIntegerAttribute_oneEntity_sameIntQueried_differentEntityQueried) {
		StmtIndex stmtIdx = statement->insertStmt(readType, name1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		bool res1 = attribute->hasEqualIntegerAttribute(EntityType::ASSIGN, 1, constant, statement);
		bool res2 = attribute->hasEqualIntegerAttribute(EntityType::CONSTANT, 1, constant, statement);

		Assert::IsFalse(res1);
		Assert::IsFalse(res2);
	}

	TEST_METHOD(hasEqualIntegerAttribute_oneEntity_differentIntQueried_differentEntityQueried) {
		StmtIndex stmtIdx = statement->insertStmt(readType, name1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		bool res1 = attribute->hasEqualIntegerAttribute(EntityType::WHILE, 2, constant, statement);
		bool res2 = attribute->hasEqualIntegerAttribute(EntityType::CALL, 3, constant, statement);

		Assert::IsFalse(res1);
		Assert::IsFalse(res2);
	}

	TEST_METHOD(hasEqualIntegerAttribute_manyEntityTypes_sameEntityType) {
		StmtIndex stmtIdx1 = statement->insertStmt(callType, name1, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(callType, name1, attribute);

		NameIndex nameIdx = attribute->getNameIdx(name1);
		Assert::IsTrue(1 == nameIdx);

		bool res1 = attribute->hasEqualIntegerAttribute(EntityType::CALL, 1, constant, statement);
		bool res2 = attribute->hasEqualIntegerAttribute(EntityType::STMT, 1, constant, statement);
		bool res3 = attribute->hasEqualIntegerAttribute(EntityType::CALL, 2, constant, statement);
		bool res4 = attribute->hasEqualIntegerAttribute(EntityType::STMT, 2, constant, statement);

		Assert::IsTrue(res1);
		Assert::IsTrue(res2);
		Assert::IsTrue(res3);
		Assert::IsTrue(res4);
	}

	TEST_METHOD(hasEqualIntegerAttribute_manyEntityTypes_differentEntityTypes_mixedInt) {
		constant->insertConst(3);
		constant->insertConst(4);
		StmtIndex stmtIdx1 = statement->insertStmt(callType, name2, attribute);
		StmtIndex stmtIdx2 = statement->insertStmt(assignType);
		StmtIndex stmtIdx3 = statement->insertStmt(readType, name1, attribute);
		StmtIndex stmtIdx4 = statement->insertStmt(printType, name1, attribute);
		StmtIndex stmtIdx5 = statement->insertStmt(readType, name3, attribute);

		NameIndex nameIdx1 = attribute->getNameIdx(name1);
		NameIndex nameIdx2 = attribute->getNameIdx(name2);
		NameIndex nameIdx3 = attribute->getNameIdx(name3);
		Assert::IsTrue(2 == nameIdx1);
		Assert::IsTrue(1 == nameIdx2);
		Assert::IsTrue(3 == nameIdx3);

		bool res1 = attribute->hasEqualIntegerAttribute(EntityType::CALL, 1, constant, statement);
		bool res2 = attribute->hasEqualIntegerAttribute(EntityType::ASSIGN, 2, constant, statement);
		bool res3 = attribute->hasEqualIntegerAttribute(EntityType::READ, 3, constant, statement);
		bool res4 = attribute->hasEqualIntegerAttribute(EntityType::PRINT, 4, constant, statement);
		bool res5 = attribute->hasEqualIntegerAttribute(EntityType::READ, 5, constant, statement);
		bool res6 = attribute->hasEqualIntegerAttribute(EntityType::CONSTANT, 3, constant, statement);
		bool res7 = attribute->hasEqualIntegerAttribute(EntityType::CONSTANT, 4, constant, statement);
		bool res8 = attribute->hasEqualIntegerAttribute(EntityType::CONSTANT, 1, constant, statement);
		bool res9 = attribute->hasEqualIntegerAttribute(EntityType::CALL, 2, constant, statement);

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
	};
}
