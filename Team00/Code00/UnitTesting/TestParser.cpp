#include "CppUnitTest.h"
#include "stdafx.h"

#include "SP/Parser.h"

#include <iostream>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestParser) {
public:
	TEST_METHOD(parse_oneProcedureReadPrint_success) {
		const char* source = "   procedure proc \n "
			"{ read x1; print y123;  \n "
			" read Y1Yy ;    } ";
		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(1), procNodes.size());
		Assert::AreEqual(std::string("proc"), procNodes[0]->getProcName());

		/* Test statements */
		StmtLstNode* stmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> statements = stmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(3), statements.size());

		ReadNode* readNode1 = (ReadNode*)statements[0];
		Assert::AreEqual(std::string("x1"), readNode1->getVarName());

		PrintNode* printNode = (PrintNode*)statements[1];
		Assert::AreEqual(std::string("y123"), printNode->getVarName());

		ReadNode* readNode2 = (ReadNode*)statements[2];
		Assert::AreEqual(std::string("Y1Yy"), readNode2->getVarName());
	}

	TEST_METHOD(parse_multipleProceduresReadPrint_success) {
		const char* source = "   procedure proc1  "
			"{ read x1; } "
			" \n procedure proc2 { \n"
			" print y123;  \n "
			" read Y1Yy ; } ";
		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(2), procNodes.size());
		Assert::AreEqual(std::string("proc1"), procNodes[0]->getProcName());
		Assert::AreEqual(std::string("proc2"), procNodes[1]->getProcName());

		/* Test statements in stmtLstNode1 */
		StmtLstNode* stmtLstNode1 = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> statements1 = stmtLstNode1->getStmtNodes();
		Assert::AreEqual(size_t(1), statements1.size());

		ReadNode* readNode1 = (ReadNode*)statements1[0];
		Assert::AreEqual(std::string("x1"), readNode1->getVarName());

		/* Test statements in stmtLstNode2 */
		StmtLstNode* stmtLstNode2 = procNodes[1]->getStmtLstNode();
		std::vector<StmtNode*> statements2 = stmtLstNode2->getStmtNodes();
		Assert::AreEqual(size_t(2), statements2.size());

		PrintNode* printNode = (PrintNode*)statements2[0];
		Assert::AreEqual(std::string("y123"), printNode->getVarName());

		ReadNode* readNode2 = (ReadNode*)statements2[1];
		Assert::AreEqual(std::string("Y1Yy"), readNode2->getVarName());
	}

	TEST_METHOD(parse_matchRead_missingReadVarName_parseExceptionThrown) {
		const char* source = "   procedure proc  "
			"{ print x1; read 0invalidName  ;} ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_VAR_NAME.c_str(), ex.what());
		}

		const char* source1 = "   procedure proc1  "
			"{ print x1;print y1 ; read  ";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_VAR_NAME.c_str(), ex.what());
		}

		const char* source2 = "   procedure proc1  "
			"{ print x1;print y1 ; read invalid_name; ";
		auto wrapperFunc2 = [&source2] { Parser::parse(source2); };
		Assert::ExpectException<ParserException>(wrapperFunc2);
		try {
			Parser::parse(source2);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchRead_missingSemicolon_parseExceptionThrown) {
		const char* source = "   procedure proc  "
			"{ print x1; read vAliD123nAmE  } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchPrint_missingPrintVarName_parseExceptionThrown) {
		const char* source = "   procedure proc  "
			"{ read x1; print 0invalidName  ;} ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_VAR_NAME.c_str(), ex.what());
		}

		const char* source1 = "   procedure proc1  "
			"{ read x1;read y1 ; print  ";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_VAR_NAME.c_str(), ex.what());
		}

		const char* source2 = "   procedure proc1  "
			"{ read x1;read y1 ; print invalid_name; ";
		auto wrapperFunc2 = [&source2] { Parser::parse(source2); };
		Assert::ExpectException<ParserException>(wrapperFunc2);
		try {
			Parser::parse(source2);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchPrint_missingSemicolon_parseExceptionThrown) {
		const char* source = "   procedure proc  "
			"{ read x1; print vAliD123nAmE  } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchStmt_invalidStmt_parseExceptionThrown) {
		const char* source = "   procedure proc  "
			"{ r3ad x1;  } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}

		const char* source1 = "   procedure proc { } ";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchStmt_missingClosingBracket_parseExceptionThrown) {
		const char* source = "   procedure proc  "
			"{ read x1; print y  ;  ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}
	};
}
