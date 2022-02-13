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

	TEST_METHOD(parse_matchRead_missingReadVarName_parserExceptionThrown) {
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

	TEST_METHOD(parse_matchRead_missingSemicolon_parserExceptionThrown) {
		const char* source = "   procedure proc  "
			"{ print x1; read vAliD123nAmE read vAliD123nAmEaGAIN ; } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchRead_tooManySemicolon_parserExceptionThrown) {
		const char* source = "   procedure proc  "
			"{ print x1; read vAliD123nAmE ; ; } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchPrint_missingPrintVarName_parserExceptionThrown) {
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

	TEST_METHOD(parse_matchPrint_missingSemicolon_parserExceptionThrown) {
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

	TEST_METHOD(parse_matchPrint_tooManySemicolon_parserExceptionThrown) {
		const char* source = "   procedure proc  "
			"{ print x1 ;; read vAliD123nAmE ; } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchStmt_invalidStmt_parserExceptionThrown) {
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

	TEST_METHOD(parse_matchProcedure_missingProcKeyword_parserExceptionThrown) {
		const char* source = "   proc procName  "
			"{ read x1; print y  ; } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_PROC_KEYWORD.c_str(), ex.what());
		}

		const char* source1 = "   Procedure procName  "
			"{ read x1; print y  ; } ";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_PROC_KEYWORD.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchProcedure_missingProcName_parserExceptionThrown) {
		const char* source = "   procedure  "
			"{ read x1; print y  ; } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_PROC_NAME.c_str(), ex.what());
		}

		const char* source1 = "   procedure !invalidProcName  "
			"{ read x1; print y  ; } ";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_PROC_NAME.c_str(), ex.what());
		}

		const char* source3 = "   procedure   ";
		auto wrapperFunc3 = [&source3] { Parser::parse(source3); };
		Assert::ExpectException<ParserException>(wrapperFunc3);
		try {
			Parser::parse(source3);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_PROC_NAME.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchProcedure_missingLeftCurly_parserExceptionThrown) {
		const char* source = "   procedure proc "
			" read x1; print y  ; } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_LEFT_CURLY.c_str(), ex.what());
		}

		const char* source1 = "   procedure validProcName }; ";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_LEFT_CURLY.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchProcedure_missingRightCurly_parserExceptionThrown) {
		const char* source = "   procedure proc "
			"  {read x1; print y  ;  ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchProcedure_tooManyLeftCurly_parserExceptionThrown) {
		const char* source = "   procedure proc "
			"  { {{ read x1; print y  ; } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchProcedure_tooManyRightCurly_parserExceptionThrown) {
		const char* source = "   procedure proc "
			"  {read x1; print y  ; }} } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_PROC_KEYWORD.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchProgram_noProc_parserExceptionThrown) {
		const char* source = "		 ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_PROG.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchProgram_multipleProcsHasMissingLeftCurly_parserExceptionThrown) {
		const char* source = "   procedure proc1  "
			"{ read x1; } "
			" \n procedure proc2  \n"
			" print y123;  \n "
			" read Y1Yy ; } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_LEFT_CURLY.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchAssign_noMathOp_success) {
		const char* source = "   procedure procedure123name \n "
			"{ flag    = 123;   count =    someVar123	; }  \n ";
		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(1), procNodes.size());
		Assert::AreEqual(std::string("procedure123name"), procNodes[0]->getProcName());

		/* Test statements */
		StmtLstNode* stmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> statements = stmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(2), statements.size());

		/* Test assign nodes*/
		// flag    = 123;
		AssignNode* assignNode1 = (AssignNode*)statements[0];
		Assert::AreEqual(std::string("flag"), assignNode1->getVarName());

		ExprNode* expr1 = assignNode1->getExpr();
		Assert::AreEqual(std::string("123"), expr1->getValue());
		Assert::IsTrue(expr1->getChildren().empty());

		// count =    someVar123	;
		AssignNode* assignNode2 = (AssignNode*)statements[1];
		Assert::AreEqual(std::string("count"), assignNode2->getVarName());

		ExprNode* expr2 = assignNode2->getExpr();
		Assert::AreEqual(std::string("someVar123"), expr2->getValue());
		Assert::IsTrue(expr2->getChildren().empty());
	}

	TEST_METHOD(parse_matchAssign_withMathOp_success) {
		const char* source = "   procedure procedure123name \n "
			"{ CenX = 9 + CenX; count = COUNT % 2   ;"
			" x = x + z * 5 ; "
			" y = y / w - 1; "
			" z = z + a123 / b0b - c  ;}";
		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(1), procNodes.size());
		Assert::AreEqual(std::string("procedure123name"), procNodes[0]->getProcName());

		/* Test statements */
		StmtLstNode* stmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> statements = stmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(5), statements.size());

		/* Test assign nodes*/
		// CenX = 9 + CenX;
		AssignNode* assignNode1 = (AssignNode*)statements[0];
		Assert::AreEqual(std::string("CenX"), assignNode1->getVarName());

		ExprNode* expr1 = assignNode1->getExpr();
		Assert::AreEqual(std::string("+"), expr1->getValue());
		std::vector<ExprNode*> children1 = expr1->getChildren();
		Assert::AreEqual(size_t(2), children1.size());
		Assert::AreEqual(std::string("9"), children1[0]->getValue());
		Assert::AreEqual(std::string("CenX"), children1[1]->getValue());

		// count = COUNT % 2   ;
		AssignNode* assignNode2 = (AssignNode*)statements[1];
		Assert::AreEqual(std::string("count"), assignNode2->getVarName());

		ExprNode* expr2 = assignNode2->getExpr();
		Assert::AreEqual(std::string("%"), expr2->getValue());
		std::vector<ExprNode*> children2 = expr2->getChildren();
		Assert::AreEqual(size_t(2), children2.size());
		Assert::AreEqual(std::string("COUNT"), children2[0]->getValue());
		Assert::AreEqual(std::string("2"), children2[1]->getValue());

		// x = x + z * 5 ;
		AssignNode* assignNode3 = (AssignNode*)statements[2];
		Assert::AreEqual(std::string("x"), assignNode3->getVarName());

		ExprNode* expr3 = assignNode3->getExpr();
		Assert::AreEqual(std::string("+"), expr3->getValue());
		std::vector<ExprNode*> children3 = expr3->getChildren();
		Assert::AreEqual(size_t(2), children3.size());
		Assert::AreEqual(std::string("x"), children3[0]->getValue());
		Assert::AreEqual(std::string("*"), children3[1]->getValue());

		std::vector<ExprNode*> zTimesFive = children3[1]->getChildren();
		Assert::AreEqual(size_t(2), zTimesFive.size());
		Assert::AreEqual(std::string("z"), zTimesFive[0]->getValue());
		Assert::AreEqual(std::string("5"), zTimesFive[1]->getValue());

		// y = y / w - 1;
		AssignNode* assignNode4 = (AssignNode*)statements[3];
		Assert::AreEqual(std::string("y"), assignNode4->getVarName());

		ExprNode* expr4 = assignNode4->getExpr();
		Assert::AreEqual(std::string("-"), expr4->getValue());
		std::vector<ExprNode*> children4 = expr4->getChildren();
		Assert::AreEqual(size_t(2), children4.size());
		Assert::AreEqual(std::string("/"), children4[0]->getValue());
		Assert::AreEqual(std::string("1"), children4[1]->getValue());

		std::vector<ExprNode*> yDividedByW = children4[0]->getChildren();
		Assert::AreEqual(size_t(2), yDividedByW.size());
		Assert::AreEqual(std::string("y"), yDividedByW[0]->getValue());
		Assert::AreEqual(std::string("w"), yDividedByW[1]->getValue());

		// z = z + a123 / b0b - c  ;
		AssignNode* assignNode5 = (AssignNode*)statements[4];
		Assert::AreEqual(std::string("z"), assignNode5->getVarName());

		ExprNode* expr5 = assignNode5->getExpr();
		Assert::AreEqual(std::string("-"), expr5->getValue());
		std::vector<ExprNode*> children5 = expr5->getChildren();
		Assert::AreEqual(size_t(2), children5.size());
		Assert::AreEqual(std::string("+"), children5[0]->getValue());
		Assert::AreEqual(std::string("c"), children5[1]->getValue());

		std::vector<ExprNode*> zPlusA123DividedByB0b = children5[0]->getChildren();
		Assert::AreEqual(size_t(2), zPlusA123DividedByB0b.size());
		Assert::AreEqual(std::string("z"), zPlusA123DividedByB0b[0]->getValue());
		Assert::AreEqual(std::string("/"), zPlusA123DividedByB0b[1]->getValue());

		std::vector<ExprNode*> a123DividedByB0b = zPlusA123DividedByB0b[1]->getChildren();
		Assert::AreEqual(size_t(2), a123DividedByB0b.size());
		Assert::AreEqual(std::string("a123"), a123DividedByB0b[0]->getValue());
		Assert::AreEqual(std::string("b0b"), a123DividedByB0b[1]->getValue());
	}

	TEST_METHOD(parse_matchAssign_withBracketedExpr_success) {
		const char* source = "   procedure procedure123name \n "
			"{ CenX = (9 + CenX); count = (((COUNT) % 2) )  ;"
			"   x = ((x + z) * 5 ) ; "
			" y = y / (((w - 1))); "
			" z = (( (z + a123) / (b0b - c) )) ;}";
		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(1), procNodes.size());
		Assert::AreEqual(std::string("procedure123name"), procNodes[0]->getProcName());

		/* Test statements */
		StmtLstNode* stmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> statements = stmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(5), statements.size());

		/* Test assign nodes*/
		// CenX = (9 + CenX);
		AssignNode* assignNode1 = (AssignNode*)statements[0];
		Assert::AreEqual(std::string("CenX"), assignNode1->getVarName());

		ExprNode* expr1 = assignNode1->getExpr();
		Assert::AreEqual(std::string("+"), expr1->getValue());
		std::vector<ExprNode*> children1 = expr1->getChildren();
		Assert::AreEqual(size_t(2), children1.size());
		Assert::AreEqual(std::string("9"), children1[0]->getValue());
		Assert::AreEqual(std::string("CenX"), children1[1]->getValue());

		// count = (COUNT % 2)   ;
		AssignNode* assignNode2 = (AssignNode*)statements[1];
		Assert::AreEqual(std::string("count"), assignNode2->getVarName());

		ExprNode* expr2 = assignNode2->getExpr();
		Assert::AreEqual(std::string("%"), expr2->getValue());
		std::vector<ExprNode*> children2 = expr2->getChildren();
		Assert::AreEqual(size_t(2), children2.size());
		Assert::AreEqual(std::string("COUNT"), children2[0]->getValue());
		Assert::AreEqual(std::string("2"), children2[1]->getValue());

		// x = (x + z) * 5 ;
		AssignNode* assignNode3 = (AssignNode*)statements[2];
		Assert::AreEqual(std::string("x"), assignNode3->getVarName());

		ExprNode* expr3 = assignNode3->getExpr();
		Assert::AreEqual(std::string("*"), expr3->getValue());
		std::vector<ExprNode*> children3 = expr3->getChildren();
		Assert::AreEqual(size_t(2), children3.size());
		Assert::AreEqual(std::string("+"), children3[0]->getValue());
		Assert::AreEqual(std::string("5"), children3[1]->getValue());

		std::vector<ExprNode*> xPlusZ = children3[0]->getChildren();
		Assert::AreEqual(size_t(2), xPlusZ.size());
		Assert::AreEqual(std::string("x"), xPlusZ[0]->getValue());
		Assert::AreEqual(std::string("z"), xPlusZ[1]->getValue());

		// y = y / (w - 1);
		AssignNode* assignNode4 = (AssignNode*)statements[3];
		Assert::AreEqual(std::string("y"), assignNode4->getVarName());

		ExprNode* expr4 = assignNode4->getExpr();
		Assert::AreEqual(std::string("/"), expr4->getValue());
		std::vector<ExprNode*> children4 = expr4->getChildren();
		Assert::AreEqual(size_t(2), children4.size());
		Assert::AreEqual(std::string("y"), children4[0]->getValue());
		Assert::AreEqual(std::string("-"), children4[1]->getValue());

		std::vector<ExprNode*> wMinusOne = children4[1]->getChildren();
		Assert::AreEqual(size_t(2), wMinusOne.size());
		Assert::AreEqual(std::string("w"), wMinusOne[0]->getValue());
		Assert::AreEqual(std::string("1"), wMinusOne[1]->getValue());

		// z = (z + a123) / (b0b - c)  ;
		AssignNode* assignNode5 = (AssignNode*)statements[4];
		Assert::AreEqual(std::string("z"), assignNode5->getVarName());

		ExprNode* expr5 = assignNode5->getExpr();
		Assert::AreEqual(std::string("/"), expr5->getValue());
		std::vector<ExprNode*> children5 = expr5->getChildren();
		Assert::AreEqual(size_t(2), children5.size());
		Assert::AreEqual(std::string("+"), children5[0]->getValue());
		Assert::AreEqual(std::string("-"), children5[1]->getValue());

		std::vector<ExprNode*> zPlusA123 = children5[0]->getChildren();
		Assert::AreEqual(size_t(2), zPlusA123.size());
		Assert::AreEqual(std::string("z"), zPlusA123[0]->getValue());
		Assert::AreEqual(std::string("a123"), zPlusA123[1]->getValue());

		std::vector<ExprNode*> b0bMinusC = children5[1]->getChildren();
		Assert::AreEqual(size_t(2), b0bMinusC.size());
		Assert::AreEqual(std::string("b0b"), b0bMinusC[0]->getValue());
		Assert::AreEqual(std::string("c"), b0bMinusC[1]->getValue());
	}

	TEST_METHOD(parse_matchAssign_missingSemicolon_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ CenX = (9 + CenX); count = (COUNT % 2)   ;"
			"   x = (x + z) * 5  "
			" y = y / (w - 1); "
			" z = (z + a123) / (b0b - c)  ;}";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchAssign_tooManySemicolon_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ CenX = (9 + CenX); count = (COUNT % 2)   ;"
			"   x = (x + z) * 5 ; ; "
			" y = y / (w - 1); "
			" z = (z + a123) / (b0b - c)  ;}";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}

		const char* source1 = "   procedure procedure123name \n "
			"{ CenX = (9 + CenX); count = (COUNT % 2)   ;"
			"   x = (x + z) * 5 ;  "
			" y = y / (w - 1); "
			" z = (z + a123) / (b0b - c)  ;;}";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchAssign_invalidInteger_lexerExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			" {  x = (x + 01) * 5 ; }";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<LexerException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (LexerException& ex) {
			Assert::AreEqual(LexerException::INVALID_INT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchAssign_incompleteExpr_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			" {  x = (x + 1) *  ; }";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}

		const char* source1 = "   procedure procedure123name \n "
			" {  x = (x  1) * 5 ; }";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}

		const char* source2 = "   procedure procedure123name \n "
			" {  x =  * 5 ; }";
		auto wrapperFunc2 = [&source2] { Parser::parse(source2); };
		Assert::ExpectException<ParserException>(wrapperFunc2);
		try {
			Parser::parse(source2);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchAssign_invalidFactor_parserExceptionThrown) {
		// '1' will be taken as a valid const_value, but "a" will be taken as invalid right bracket
		const char* source = "   procedure procedure123name \n "
			" {  x = (x + 1a) * 5 ; }";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}

		const char* source1 = "   procedure procedure123name \n "
			"{  y = !y / (w - 1); "
			" z = (z + a123) / (b0b - c)  ;}";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}

		// '0' will be taken as a valid const_value, but "a" will be taken as invalid right bracket
		const char* source2 = "   procedure procedure123name \n "
			"{  z = (z + 0a123) / (b0b - c)  ;}";
		auto wrapperFunc2 = [&source2] { Parser::parse(source2); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchAssign_tooManyLeftBracket_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ CenX = (9 + CenX); count = ( (COUNT % 2)   ;"
			"   x = (x + 1) * 5 ; "
			" y = y / (w - 1); "
			" z = (z + a123) / (b0b - c)  ;}";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}

		const char* source1 = "   procedure procedure123name \n "
			"{ CenX = (9 + CenX); count = (COUNT % 2)   ;"
			" y = y / ((w - 1); "
			" z = (z + a123) / (b0b - c)  ;}";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchAssign_tooManyRightBracket_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ CenX = (9 + CenX); count = (COUNT % 2)   ;"
			"   x = (x + 1) * 5 ) ; "
			" y = y / (w - 1); "
			" z = (z + a123) / (b0b - c)  ;}";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}

		const char* source1 = "   procedure procedure123name \n "
			"{ CenX = (9 + CenX); count = (COUNT % 2)   ;"
			" y = y / ((w - 1)); "
			" z =( (z + a123) / (b0b - c)  )  ) ;}";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchAssign_invalidOperand_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			" {  x = (x + 1) ! 5 ; }";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}

		const char* source1 = "   procedure procedure123name \n "
			"{  y = y / (w ( 1); "
			" z = (z + a123) / (b0b - c)  ;}";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}

		const char* source2 = "   procedure procedure123name \n "
			"{  z = (z + a123) / (b0b & c)  ;}";
		auto wrapperFunc2 = [&source2] { Parser::parse(source2); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}
	}
	};
}
