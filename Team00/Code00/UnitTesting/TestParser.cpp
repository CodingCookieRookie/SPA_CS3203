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

	TEST_METHOD(parse_matchWhile_oneRelExprCondExpr_success) {
		const char* source = "   procedure procedure123name \n "
			"{ while ( y + 2 > 5) "
			"{ flag    = 123;   count =    someVar123	; "
			" read flag ; print COUNT	; } "
			"} \n ";

		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(1), procNodes.size());
		Assert::AreEqual(std::string("procedure123name"), procNodes[0]->getProcName());

		/* Test statements */
		StmtLstNode* stmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> statements = stmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), statements.size());

		/* Test while stmt */
		WhileNode* whileNode = (WhileNode*)statements[0];
		ExprNode* gtOp = whileNode->getCondExpr();
		StmtLstNode* stmtLstNodeWhile = whileNode->getChildStmtLst()[0];

		/* Test cond expr */
		// y + 2 > 5
		Assert::AreEqual(std::string(">"), gtOp->getValue());
		Assert::IsTrue(ExprNodeValueType::relOperator == gtOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenGtOp = gtOp->getChildren();
		Assert::AreEqual(size_t(2), childrenGtOp.size());

		// y + 2
		ExprNode* plusOp = childrenGtOp[0];
		Assert::AreEqual(std::string("+"), plusOp->getValue());
		Assert::IsTrue(ExprNodeValueType::arithmeticOperator == plusOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenPlusOp = plusOp->getChildren();
		Assert::AreEqual(size_t(2), childrenPlusOp.size());
		Assert::AreEqual(std::string("y"), childrenPlusOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::varName == childrenPlusOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("2"), childrenPlusOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::constValue == childrenPlusOp[1]->getExprNodeValueType());

		// 5
		Assert::AreEqual(std::string("5"), childrenGtOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::constValue == childrenGtOp[1]->getExprNodeValueType());

		/* Test stmtLst in while container */
		std::vector<StmtNode*> stmtsInWhile = stmtLstNodeWhile->getStmtNodes();
		Assert::AreEqual(size_t(4), stmtsInWhile.size());

		// flag    = 123;
		AssignNode* assignNode1 = (AssignNode*)stmtsInWhile[0];
		Assert::AreEqual(std::string("flag"), assignNode1->getVarName());

		ExprNode* expr1 = assignNode1->getExpr();
		Assert::AreEqual(std::string("123"), expr1->getValue());
		Assert::IsTrue(expr1->getChildren().empty());

		// count =  ((  someVar123 )  )	;
		AssignNode* assignNode2 = (AssignNode*)stmtsInWhile[1];
		Assert::AreEqual(std::string("count"), assignNode2->getVarName());

		ExprNode* expr2 = assignNode2->getExpr();
		Assert::AreEqual(std::string("someVar123"), expr2->getValue());
		Assert::IsTrue(expr2->getChildren().empty());

		// read flag ;
		ReadNode* readNode = (ReadNode*)stmtsInWhile[2];
		Assert::AreEqual(std::string("flag"), readNode->getVarName());

		// print COUNT	;
		PrintNode* printNode = (PrintNode*)stmtsInWhile[3];
		Assert::AreEqual(std::string("COUNT"), printNode->getVarName());
	}

	TEST_METHOD(parse_matchWhile_NOTOpCondExpr_success) {
		const char* source = "   procedure procedure123name \n "
			"{ while (!(x >= 0)) "
			"{ flag    = 123;   count =    someVar123	; "
			" read flag ; print COUNT	; } "
			"} \n ";
		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(1), procNodes.size());
		Assert::AreEqual(std::string("procedure123name"), procNodes[0]->getProcName());

		/* Test statements */
		StmtLstNode* stmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> statements = stmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), statements.size());

		/* Test while stmt */
		WhileNode* whileNode = (WhileNode*)statements[0];
		ExprNode* notOp = whileNode->getCondExpr();
		StmtLstNode* stmtLstNodeWhile = whileNode->getChildStmtLst()[0];

		/* Test cond expr */
		Assert::AreEqual(std::string("!"), notOp->getValue());
		Assert::IsTrue(ExprNodeValueType::logicalOperator == notOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenNotOp = notOp->getChildren();
		Assert::AreEqual(size_t(1), childrenNotOp.size());

		ExprNode* gteOp = childrenNotOp[0];
		Assert::AreEqual(std::string(">="), gteOp->getValue());
		Assert::IsTrue(ExprNodeValueType::relOperator == gteOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenGteOp = gteOp->getChildren();
		Assert::AreEqual(size_t(2), childrenGteOp.size());

		Assert::AreEqual(std::string("x"), childrenGteOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::varName == childrenGteOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("0"), childrenGteOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::constValue == childrenGteOp[1]->getExprNodeValueType());

		/* Test stmtLst in while container */
		std::vector<StmtNode*> stmtsInWhile = stmtLstNodeWhile->getStmtNodes();
		Assert::AreEqual(size_t(4), stmtsInWhile.size());

		// flag    = 123;
		AssignNode* assignNode1 = (AssignNode*)stmtsInWhile[0];
		Assert::AreEqual(std::string("flag"), assignNode1->getVarName());

		ExprNode* expr1 = assignNode1->getExpr();
		Assert::AreEqual(std::string("123"), expr1->getValue());
		Assert::IsTrue(expr1->getChildren().empty());

		// count =  ((  someVar123 )  )	;
		AssignNode* assignNode2 = (AssignNode*)stmtsInWhile[1];
		Assert::AreEqual(std::string("count"), assignNode2->getVarName());

		ExprNode* expr2 = assignNode2->getExpr();
		Assert::AreEqual(std::string("someVar123"), expr2->getValue());
		Assert::IsTrue(expr2->getChildren().empty());

		// read flag ;
		ReadNode* readNode = (ReadNode*)stmtsInWhile[2];
		Assert::AreEqual(std::string("flag"), readNode->getVarName());

		// print COUNT	;
		PrintNode* printNode = (PrintNode*)stmtsInWhile[3];
		Assert::AreEqual(std::string("COUNT"), printNode->getVarName());
	}

	TEST_METHOD(parse_matchWhile_ANDOpCondExpr_success) {
		const char* source = "   procedure procedure123name \n "
			"{ while ( ( !(x >= 0) ) && ( y < 5 - z )    ) "
			"{ read fl123ag 	; } "
			"} \n ";

		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(1), procNodes.size());
		Assert::AreEqual(std::string("procedure123name"), procNodes[0]->getProcName());

		/* Test statements */
		StmtLstNode* stmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> statements = stmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), statements.size());

		/* Test while stmt */
		WhileNode* whileNode = (WhileNode*)statements[0];
		ExprNode* andOp = whileNode->getCondExpr();
		StmtLstNode* stmtLstNodeWhile = whileNode->getChildStmtLst()[0];

		/* Test cond expr */
		Assert::AreEqual(std::string("&&"), andOp->getValue());
		Assert::IsTrue(ExprNodeValueType::logicalOperator == andOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenAndOp = andOp->getChildren();
		Assert::AreEqual(size_t(2), childrenAndOp.size());

		// !(x >= 0)
		ExprNode* notOp = childrenAndOp[0];
		Assert::AreEqual(std::string("!"), notOp->getValue());
		Assert::IsTrue(ExprNodeValueType::logicalOperator == notOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenNotOp = notOp->getChildren();
		Assert::AreEqual(size_t(1), childrenNotOp.size());

		ExprNode* gteOp = childrenNotOp[0];
		Assert::AreEqual(std::string(">="), gteOp->getValue());
		Assert::IsTrue(ExprNodeValueType::relOperator == gteOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenGteOp = gteOp->getChildren();
		Assert::AreEqual(size_t(2), childrenGteOp.size());

		Assert::AreEqual(std::string("x"), childrenGteOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::varName == childrenGteOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("0"), childrenGteOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::constValue == childrenGteOp[1]->getExprNodeValueType());

		// ( y < 5 - z )
		ExprNode* ltOp = childrenAndOp[1];
		Assert::AreEqual(std::string("<"), ltOp->getValue());
		Assert::IsTrue(ExprNodeValueType::relOperator == ltOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenLtOp = ltOp->getChildren();
		Assert::AreEqual(size_t(2), childrenLtOp.size());

		Assert::AreEqual(std::string("y"), childrenLtOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::varName == childrenLtOp[0]->getExprNodeValueType());

		ExprNode* minusOp = childrenLtOp[1];
		Assert::AreEqual(std::string("-"), minusOp->getValue());
		Assert::IsTrue(ExprNodeValueType::arithmeticOperator == minusOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenMinusOp = minusOp->getChildren();
		Assert::AreEqual(size_t(2), childrenMinusOp.size());

		Assert::AreEqual(std::string("5"), childrenMinusOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::constValue == childrenMinusOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("z"), childrenMinusOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::varName == childrenMinusOp[1]->getExprNodeValueType());

		/* Test stmtLst in while container */
		std::vector<StmtNode*> stmtsInWhile = stmtLstNodeWhile->getStmtNodes();
		Assert::AreEqual(size_t(1), stmtsInWhile.size());

		// read fl123ag ;
		ReadNode* readNode = (ReadNode*)stmtsInWhile[0];
		Assert::AreEqual(std::string("fl123ag"), readNode->getVarName());
	}

	TEST_METHOD(parse_matchWhile_OROpCondExpr_success) {
		const char* source = "   procedure procedure123name \n "
			"{ while ( (x <= 0)  || \n"
			" (y * 1 == 5 - z)    ) "
			"{ read fl123ag 	; } "
			"} \n ";

		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(1), procNodes.size());
		Assert::AreEqual(std::string("procedure123name"), procNodes[0]->getProcName());

		/* Test statements */
		StmtLstNode* stmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> statements = stmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), statements.size());

		/* Test while stmt */
		WhileNode* whileNode = (WhileNode*)statements[0];
		ExprNode* orOp = whileNode->getCondExpr();
		StmtLstNode* stmtLstNodeWhile = whileNode->getChildStmtLst()[0];

		/* Test cond expr */
		Assert::AreEqual(std::string("||"), orOp->getValue());
		Assert::IsTrue(ExprNodeValueType::logicalOperator == orOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenOrOp = orOp->getChildren();
		Assert::AreEqual(size_t(2), childrenOrOp.size());

		// (x <= 0)
		ExprNode* lteOp = childrenOrOp[0];
		Assert::AreEqual(std::string("<="), lteOp->getValue());
		Assert::IsTrue(ExprNodeValueType::relOperator == lteOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenLteOp = lteOp->getChildren();
		Assert::AreEqual(size_t(2), childrenLteOp.size());

		Assert::AreEqual(std::string("x"), childrenLteOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::varName == childrenLteOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("0"), childrenLteOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::constValue == childrenLteOp[1]->getExprNodeValueType());

		// ( y * 1 == 5 - z )
		ExprNode* eqOp = childrenOrOp[1];
		Assert::AreEqual(std::string("=="), eqOp->getValue());
		Assert::IsTrue(ExprNodeValueType::relOperator == eqOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenEqOp = eqOp->getChildren();
		Assert::AreEqual(size_t(2), childrenEqOp.size());

		ExprNode* multiplyOp = childrenEqOp[0];
		Assert::AreEqual(std::string("*"), multiplyOp->getValue());
		Assert::IsTrue(ExprNodeValueType::arithmeticOperator == multiplyOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenMultiplyOp = multiplyOp->getChildren();
		Assert::AreEqual(size_t(2), childrenMultiplyOp.size());

		Assert::AreEqual(std::string("y"), childrenMultiplyOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::varName == childrenMultiplyOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("1"), childrenMultiplyOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::constValue == childrenMultiplyOp[1]->getExprNodeValueType());

		ExprNode* minusOp = childrenEqOp[1];
		Assert::AreEqual(std::string("-"), minusOp->getValue());
		Assert::IsTrue(ExprNodeValueType::arithmeticOperator == minusOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenMinusOp = minusOp->getChildren();
		Assert::AreEqual(size_t(2), childrenMinusOp.size());

		Assert::AreEqual(std::string("5"), childrenMinusOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::constValue == childrenMinusOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("z"), childrenMinusOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::varName == childrenMinusOp[1]->getExprNodeValueType());

		/* Test stmtLst in while container */
		std::vector<StmtNode*> stmtsInWhile = stmtLstNodeWhile->getStmtNodes();
		Assert::AreEqual(size_t(1), stmtsInWhile.size());

		// read fl123ag ;
		ReadNode* readNode = (ReadNode*)stmtsInWhile[0];
		Assert::AreEqual(std::string("fl123ag"), readNode->getVarName());
	}

	TEST_METHOD(parse_matchWhile_OROpNestedInNOTOpCondExpr_success) {
		const char* source = "   procedure procedure123name \n "
			"{ while ( !( (y * 1 == 5 - z) || (x != 0)  )    ) "
			"{ read fl123ag 	; } "
			"} \n ";

		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(1), procNodes.size());
		Assert::AreEqual(std::string("procedure123name"), procNodes[0]->getProcName());

		/* Test statements */
		StmtLstNode* stmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> statements = stmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), statements.size());

		/* Test while stmt */
		WhileNode* whileNode = (WhileNode*)statements[0];
		ExprNode* notOp = whileNode->getCondExpr();
		StmtLstNode* stmtLstNodeWhile = whileNode->getChildStmtLst()[0];

		/* Test cond expr */
		// !( (y * 1 == 5 - z) || (x != 0)  )
		Assert::AreEqual(std::string("!"), notOp->getValue());
		Assert::IsTrue(ExprNodeValueType::logicalOperator == notOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenNotOp = notOp->getChildren();
		Assert::AreEqual(size_t(1), childrenNotOp.size());

		ExprNode* orOp = childrenNotOp[0];
		Assert::AreEqual(std::string("||"), orOp->getValue());
		Assert::IsTrue(ExprNodeValueType::logicalOperator == orOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenOrOp = orOp->getChildren();
		Assert::AreEqual(size_t(2), childrenOrOp.size());

		// (y * 1 == 5 - z)
		ExprNode* eqOp = childrenOrOp[0];
		Assert::AreEqual(std::string("=="), eqOp->getValue());
		Assert::IsTrue(ExprNodeValueType::relOperator == eqOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenEqOp = eqOp->getChildren();
		Assert::AreEqual(size_t(2), childrenEqOp.size());

		ExprNode* multiplyOp = childrenEqOp[0];
		Assert::AreEqual(std::string("*"), multiplyOp->getValue());
		Assert::IsTrue(ExprNodeValueType::arithmeticOperator == multiplyOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenMultiplyOp = multiplyOp->getChildren();
		Assert::AreEqual(size_t(2), childrenMultiplyOp.size());

		Assert::AreEqual(std::string("y"), childrenMultiplyOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::varName == childrenMultiplyOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("1"), childrenMultiplyOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::constValue == childrenMultiplyOp[1]->getExprNodeValueType());

		ExprNode* minusOp = childrenEqOp[1];
		Assert::AreEqual(std::string("-"), minusOp->getValue());
		Assert::IsTrue(ExprNodeValueType::arithmeticOperator == minusOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenMinusOp = minusOp->getChildren();
		Assert::AreEqual(size_t(2), childrenMinusOp.size());

		Assert::AreEqual(std::string("5"), childrenMinusOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::constValue == childrenMinusOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("z"), childrenMinusOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::varName == childrenMinusOp[1]->getExprNodeValueType());

		// (x != 0)
		ExprNode* neqOp = childrenOrOp[1];
		Assert::AreEqual(std::string("!="), neqOp->getValue());
		Assert::IsTrue(ExprNodeValueType::relOperator == neqOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenNeqOp = neqOp->getChildren();
		Assert::AreEqual(size_t(2), childrenNeqOp.size());

		Assert::AreEqual(std::string("x"), childrenNeqOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::varName == childrenNeqOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("0"), childrenNeqOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::constValue == childrenNeqOp[1]->getExprNodeValueType());

		/* Test stmtLst in while container */
		std::vector<StmtNode*> stmtsInWhile = stmtLstNodeWhile->getStmtNodes();
		Assert::AreEqual(size_t(1), stmtsInWhile.size());

		// read fl123ag ;
		ReadNode* readNode = (ReadNode*)stmtsInWhile[0];
		Assert::AreEqual(std::string("fl123ag"), readNode->getVarName());
	}

	TEST_METHOD(parse_matchIf_oneRelExprCondExpr_success) {
		const char* source = "   procedure procedure123name \n "
			"{ if ( y == 5 * 2)  then "
			"{ read fl123ag \n	; cenX = 99 ; } "
			" else { print COUNT; } "
			"} \n ";

		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(1), procNodes.size());
		Assert::AreEqual(std::string("procedure123name"), procNodes[0]->getProcName());

		/* Test statements */
		StmtLstNode* stmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> statements = stmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), statements.size());

		/* Test if-else-then clause */
		IfNode* ifNode = (IfNode*)statements[0];
		Assert::IsTrue(StatementType::ifType == ifNode->getStmtType());
		std::vector<StmtLstNode*> childStmtLst = ifNode->getChildStmtLst();
		Assert::AreEqual(size_t(2), childStmtLst.size());

		/* Test cond expr */
		// ( y == 5 * 2)
		ExprNode* eqOp = ifNode->getCondExpr();
		Assert::AreEqual(std::string("=="), eqOp->getValue());
		Assert::IsTrue(ExprNodeValueType::relOperator == eqOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenEqOp = eqOp->getChildren();
		Assert::AreEqual(size_t(2), childrenEqOp.size());

		// y
		Assert::AreEqual(std::string("y"), childrenEqOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::varName == childrenEqOp[0]->getExprNodeValueType());

		// 5 * 2
		ExprNode* multOp = childrenEqOp[1];
		Assert::AreEqual(std::string("*"), multOp->getValue());
		Assert::IsTrue(ExprNodeValueType::arithmeticOperator == multOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenMultOp = multOp->getChildren();
		Assert::AreEqual(size_t(2), childrenMultOp.size());
		Assert::AreEqual(std::string("5"), childrenMultOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::constValue == childrenMultOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("2"), childrenMultOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::constValue == childrenMultOp[1]->getExprNodeValueType());

		/* Test then clause */
		// read fl123ag
		// cenX = 99
		StmtLstNode* stmtLstNodeThen = childStmtLst[0];
		std::vector<StmtNode*> stmtsThen = stmtLstNodeThen->getStmtNodes();
		Assert::AreEqual(size_t(2), stmtsThen.size());

		ReadNode* readNode = (ReadNode*)stmtsThen[0];
		Assert::AreEqual(std::string("fl123ag"), readNode->getVarName());

		AssignNode* assignNode = (AssignNode*)stmtsThen[1];
		Assert::AreEqual(std::string("cenX"), assignNode->getVarName());

		ExprNode* expr = assignNode->getExpr();
		Assert::AreEqual(std::string("99"), expr->getValue());
		Assert::IsTrue(expr->getChildren().empty());

		/* Test else clause */
		// print COUNT
		StmtLstNode* stmtLstNodeElse = childStmtLst[1];
		std::vector<StmtNode*> stmtsElse = stmtLstNodeElse->getStmtNodes();
		Assert::AreEqual(size_t(1), stmtsElse.size());

		PrintNode* printNode = (PrintNode*)stmtsElse[0];
		Assert::AreEqual(std::string("COUNT"), printNode->getVarName());
	}
	};
}
