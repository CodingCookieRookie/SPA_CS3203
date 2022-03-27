#include "CppUnitTest.h"
#include "stdafx.h"

#include <vector>

#include "SP/Parser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestParser) {
public:
	TEST_METHOD(parse_oneProcedure_ReadPrintAssignWhileIf_manyWhitespaces_varNameReservedKeywords_noNesting_success) {
		const char* source = "   procedure aRelativelyLongProcedureName \n "
			"{ read read; print y123;  \n "
			" x = (y + 1) * 3	; "
			" while ( (1 >= x ) || (! (while == 2147483647	)) ){ read print	; } "
			"if ( 2 > 2147483648) \n then \n"
			" { Re123ad = ALLUPPERCASE;}else{ if = 5; }"
			" } ";
		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(1), procNodes.size());
		Assert::AreEqual(std::string("aRelativelyLongProcedureName"), procNodes[0]->getProcName());

		/* Test statements */
		StmtLstNode* stmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> statements = stmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(5), statements.size());

		/* Test read stmt */
		/* read read; */
		ReadNode* readNode = (ReadNode*)statements[0];
		Assert::IsTrue(StatementType::READ_TYPE == readNode->getStmtType());
		Assert::AreEqual(std::string("read"), *readNode->getModifiesVars().begin());

		/* Test print stmt */
		/* print y123; */
		PrintNode* printNode = (PrintNode*)statements[1];
		Assert::IsTrue(StatementType::PRINT_TYPE == printNode->getStmtType());
		Assert::AreEqual(std::string("y123"), *printNode->getUsesVars().begin());

		/* Test assign stmt */
		/* x = (y + 1) * 3 */
		AssignNode* assignNode = (AssignNode*)statements[2];
		Assert::IsTrue(StatementType::ASSIGN_TYPE == assignNode->getStmtType());
		Assert::AreEqual(std::string("x"), *assignNode->getModifiesVars().begin());
		ExprNode* multOp = assignNode->getExpr();
		Assert::AreEqual(std::string("*"), multOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == multOp->getExprNodeValueType());

		/* (y + 1) */
		std::vector<ExprNode*> multOpChildren = multOp->getChildren();
		Assert::AreEqual(size_t(2), multOpChildren.size());
		ExprNode* plusOp = multOpChildren[0];
		Assert::AreEqual(std::string("+"), plusOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == plusOp->getExprNodeValueType());
		std::vector<ExprNode*> plusOpChildren = plusOp->getChildren();
		Assert::AreEqual(size_t(2), plusOpChildren.size());
		Assert::AreEqual(std::string("y"), plusOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == plusOpChildren[0]->getExprNodeValueType());
		Assert::IsTrue(plusOpChildren[0]->getChildren().empty());
		Assert::AreEqual(std::string("1"), plusOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == plusOpChildren[1]->getExprNodeValueType());
		Assert::IsTrue(plusOpChildren[1]->getChildren().empty());

		/* 3 */
		ExprNode* three = multOpChildren[1];
		Assert::AreEqual(std::string("3"), three->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == three->getExprNodeValueType());
		Assert::IsTrue(three->getChildren().empty());

		/* Test while stmt */
		/* while ( (1 >= x ) || (! (while == 2147483647	)) ){ read print	; } */
		WhileNode* whileNode = (WhileNode*)statements[3];
		Assert::IsTrue(StatementType::WHILE_TYPE == whileNode->getStmtType());
		ExprNode* orOp = whileNode->getCondExpr();
		StmtLstNode* whileStmtLstNode = whileNode->getChildStmtLst()[0];

		/* Test while cond expr */
		Assert::AreEqual(std::string("||"), orOp->getValue());
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == orOp->getExprNodeValueType());
		std::vector<ExprNode*> orOpChildren = orOp->getChildren();
		Assert::AreEqual(size_t(2), orOpChildren.size());

		/* (1 >= x) */
		ExprNode* gteOp = orOpChildren[0];
		Assert::AreEqual(std::string(">="), gteOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == gteOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenGteOp = gteOp->getChildren();
		Assert::AreEqual(size_t(2), childrenGteOp.size());
		Assert::AreEqual(std::string("1"), childrenGteOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == childrenGteOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("x"), childrenGteOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == childrenGteOp[1]->getExprNodeValueType());

		/* (! (while == 2147483647	)) */
		ExprNode* notOp = orOpChildren[1];
		Assert::AreEqual(std::string("!"), notOp->getValue());
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == notOp->getExprNodeValueType());
		std::vector<ExprNode*> notOpChildren = notOp->getChildren();
		Assert::AreEqual(size_t(1), notOpChildren.size());

		/* (while == 2147483647	) */
		ExprNode* eqOp = notOpChildren[0];
		Assert::AreEqual(std::string("=="), eqOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == eqOp->getExprNodeValueType());
		std::vector<ExprNode*> eqOpChildren = eqOp->getChildren();
		Assert::AreEqual(size_t(2), eqOpChildren.size());

		Assert::AreEqual(std::string("while"), eqOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == eqOpChildren[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("2147483647"), eqOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == eqOpChildren[1]->getExprNodeValueType());

		/* Test stmtLst in while container */
		/* read print	; */
		std::vector<StmtNode*> stmtsInWhile = whileStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), stmtsInWhile.size());
		ReadNode* whileReadNode = (ReadNode*)stmtsInWhile[0];
		Assert::IsTrue(StatementType::READ_TYPE == whileReadNode->getStmtType());
		Assert::AreEqual(std::string("print"), *whileReadNode->getModifiesVars().begin());

		/* Test if stmt */
		/*if ( 2 > 2147483648) \n then \n
			 { Re123ad = ALLUPPERCASE;}else{ if = 5; }*/
		IfNode* ifNode = (IfNode*)statements[4];
		Assert::IsTrue(StatementType::IF_TYPE == ifNode->getStmtType());
		std::vector<StmtLstNode*> childStmtLst = ifNode->getChildStmtLst();
		Assert::AreEqual(size_t(2), childStmtLst.size());

		/* Test cond expr */
		/* ( 2 > 2147483648) */
		ExprNode* gtOp = ifNode->getCondExpr();
		Assert::AreEqual(std::string(">"), gtOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == gtOp->getExprNodeValueType());
		std::vector<ExprNode*> gtOpChildren = gtOp->getChildren();
		Assert::AreEqual(size_t(2), gtOpChildren.size());
		Assert::AreEqual(std::string("2"), gtOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == gtOpChildren[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("2147483648"), gtOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == gtOpChildren[1]->getExprNodeValueType());

		/* Test then clause */
		/* Re123ad = ALLUPPERCASE; */
		StmtLstNode* thenStmtLstNode = childStmtLst[0];
		std::vector<StmtNode*> thenStmts = thenStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), thenStmts.size());
		AssignNode* thenAssignNode = (AssignNode*)thenStmts[0];
		Assert::IsTrue(StatementType::ASSIGN_TYPE == thenAssignNode->getStmtType());
		Assert::AreEqual(std::string("Re123ad"), *thenAssignNode->getModifiesVars().begin());
		ExprNode* allUpperCase = thenAssignNode->getExpr();
		Assert::AreEqual(std::string("ALLUPPERCASE"), allUpperCase->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == allUpperCase->getExprNodeValueType());
		Assert::IsTrue(allUpperCase->getChildren().empty());

		/* Test else clause */
		/* if = 5 */
		StmtLstNode* elseStmtLstNode = childStmtLst[1];
		std::vector<StmtNode*> elseStmts = elseStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), elseStmts.size());
		AssignNode* elseAssignNode = (AssignNode*)elseStmts[0];
		Assert::IsTrue(StatementType::ASSIGN_TYPE == elseAssignNode->getStmtType());
		Assert::AreEqual(std::string("if"), *elseAssignNode->getModifiesVars().begin());
		ExprNode* five = elseAssignNode->getExpr();
		Assert::AreEqual(std::string("5"), five->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == five->getExprNodeValueType());
		Assert::IsTrue(five->getChildren().empty());
	}

	TEST_METHOD(parse_multipleProcedures_syntacticallyValid_semanticallyValid_success) {
		const char* source =
			" procedure proc1 {read proc1;}"
			"   procedure aRelativelyLongProcedureName \n "
			"{ read read; print y123;  \n "
			" x = (y + 1) * 3	; "
			" while ( (1 >= x ) || (! (while == 2147483647	)) ){ read print	; } "
			"if ( 2 > 2147483648) \n then \n"
			" { Re123ad = ALLUPPERCASE;}else{ if = 5; }"
			" } "
			" procedure proc3 {read=read;}";
		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(3), procNodes.size());

		/* 1st procedure */
		/* procedure proc1 {read proc1;} */
		Assert::AreEqual(std::string("proc1"), procNodes[0]->getProcName());
		StmtLstNode* proc1StmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> proc1Stmts = proc1StmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), proc1Stmts.size());
		ReadNode* proc1ReadNode = (ReadNode*)proc1Stmts[0];
		Assert::IsTrue(StatementType::READ_TYPE == proc1ReadNode->getStmtType());
		Assert::AreEqual(std::string("proc1"), *proc1ReadNode->getModifiesVars().begin());

		/* 2nd procedure */
		/* procedure aRelativelyLongProcedureName ... */
		Assert::AreEqual(std::string("aRelativelyLongProcedureName"), procNodes[1]->getProcName());

		/* Test statements in 2nd procedure */
		StmtLstNode* proc2StmtLstNode = procNodes[1]->getStmtLstNode();
		std::vector<StmtNode*> proc2Stmts = proc2StmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(5), proc2Stmts.size());

		/* Test read stmt */
		/* read read; */
		ReadNode* readNode = (ReadNode*)proc2Stmts[0];
		Assert::IsTrue(StatementType::READ_TYPE == readNode->getStmtType());
		Assert::AreEqual(std::string("read"), *readNode->getModifiesVars().begin());

		/* Test print stmt */
		/* print y123; */
		PrintNode* printNode = (PrintNode*)proc2Stmts[1];
		Assert::IsTrue(StatementType::PRINT_TYPE == printNode->getStmtType());
		Assert::AreEqual(std::string("y123"), *printNode->getUsesVars().begin());

		/* Test assign stmt */
		/* x = (y + 1) * 3 */
		AssignNode* assignNode = (AssignNode*)proc2Stmts[2];
		Assert::IsTrue(StatementType::ASSIGN_TYPE == assignNode->getStmtType());
		Assert::AreEqual(std::string("x"), *assignNode->getModifiesVars().begin());
		ExprNode* multOp = assignNode->getExpr();
		Assert::AreEqual(std::string("*"), multOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == multOp->getExprNodeValueType());

		/* (y + 1) */
		std::vector<ExprNode*> multOpChildren = multOp->getChildren();
		Assert::AreEqual(size_t(2), multOpChildren.size());
		ExprNode* plusOp = multOpChildren[0];
		Assert::AreEqual(std::string("+"), plusOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == plusOp->getExprNodeValueType());
		std::vector<ExprNode*> plusOpChildren = plusOp->getChildren();
		Assert::AreEqual(size_t(2), plusOpChildren.size());
		Assert::AreEqual(std::string("y"), plusOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == plusOpChildren[0]->getExprNodeValueType());
		Assert::IsTrue(plusOpChildren[0]->getChildren().empty());
		Assert::AreEqual(std::string("1"), plusOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == plusOpChildren[1]->getExprNodeValueType());
		Assert::IsTrue(plusOpChildren[1]->getChildren().empty());

		/* 3 */
		ExprNode* three = multOpChildren[1];
		Assert::AreEqual(std::string("3"), three->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == three->getExprNodeValueType());
		Assert::IsTrue(three->getChildren().empty());

		/* Test while stmt */
		/* while ( (1 >= x ) || (! (while == 2147483647	)) ){ read print	; } */
		WhileNode* whileNode = (WhileNode*)proc2Stmts[3];
		Assert::IsTrue(StatementType::WHILE_TYPE == whileNode->getStmtType());
		ExprNode* orOp = whileNode->getCondExpr();
		StmtLstNode* whileStmtLstNode = whileNode->getChildStmtLst()[0];

		/* Test while cond expr */
		Assert::AreEqual(std::string("||"), orOp->getValue());
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == orOp->getExprNodeValueType());
		std::vector<ExprNode*> orOpChildren = orOp->getChildren();
		Assert::AreEqual(size_t(2), orOpChildren.size());

		/* (1 >= x) */
		ExprNode* gteOp = orOpChildren[0];
		Assert::AreEqual(std::string(">="), gteOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == gteOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenGteOp = gteOp->getChildren();
		Assert::AreEqual(size_t(2), childrenGteOp.size());
		Assert::AreEqual(std::string("1"), childrenGteOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == childrenGteOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("x"), childrenGteOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == childrenGteOp[1]->getExprNodeValueType());

		/* (! (while == 2147483647	)) */
		ExprNode* notOp = orOpChildren[1];
		Assert::AreEqual(std::string("!"), notOp->getValue());
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == notOp->getExprNodeValueType());
		std::vector<ExprNode*> notOpChildren = notOp->getChildren();
		Assert::AreEqual(size_t(1), notOpChildren.size());

		/* (while == 2147483647	) */
		ExprNode* eqOp = notOpChildren[0];
		Assert::AreEqual(std::string("=="), eqOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == eqOp->getExprNodeValueType());
		std::vector<ExprNode*> eqOpChildren = eqOp->getChildren();
		Assert::AreEqual(size_t(2), eqOpChildren.size());

		Assert::AreEqual(std::string("while"), eqOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == eqOpChildren[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("2147483647"), eqOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == eqOpChildren[1]->getExprNodeValueType());

		/* Test stmtLst in while container */
		/* read print	; */
		std::vector<StmtNode*> stmtsInWhile = whileStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), stmtsInWhile.size());
		ReadNode* whileReadNode = (ReadNode*)stmtsInWhile[0];
		Assert::IsTrue(StatementType::READ_TYPE == whileReadNode->getStmtType());
		Assert::AreEqual(std::string("print"), *whileReadNode->getModifiesVars().begin());

		/* Test if stmt */
		/*if ( 2 > 2147483648) \n then \n
			 { Re123ad = ALLUPPERCASE;}else{ if = 5; }*/
		IfNode* ifNode = (IfNode*)proc2Stmts[4];
		Assert::IsTrue(StatementType::IF_TYPE == ifNode->getStmtType());
		std::vector<StmtLstNode*> childStmtLst = ifNode->getChildStmtLst();
		Assert::AreEqual(size_t(2), childStmtLst.size());

		/* Test cond expr */
		/* ( 2 > 2147483648) */
		ExprNode* gtOp = ifNode->getCondExpr();
		Assert::AreEqual(std::string(">"), gtOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == gtOp->getExprNodeValueType());
		std::vector<ExprNode*> gtOpChildren = gtOp->getChildren();
		Assert::AreEqual(size_t(2), gtOpChildren.size());
		Assert::AreEqual(std::string("2"), gtOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == gtOpChildren[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("2147483648"), gtOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == gtOpChildren[1]->getExprNodeValueType());

		/* Test then clause */
		/* Re123ad = ALLUPPERCASE; */
		StmtLstNode* thenStmtLstNode = childStmtLst[0];
		std::vector<StmtNode*> thenStmts = thenStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), thenStmts.size());
		AssignNode* thenAssignNode = (AssignNode*)thenStmts[0];
		Assert::IsTrue(StatementType::ASSIGN_TYPE == thenAssignNode->getStmtType());
		Assert::AreEqual(std::string("Re123ad"), *thenAssignNode->getModifiesVars().begin());
		ExprNode* allUpperCase = thenAssignNode->getExpr();
		Assert::AreEqual(std::string("ALLUPPERCASE"), allUpperCase->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == allUpperCase->getExprNodeValueType());
		Assert::IsTrue(allUpperCase->getChildren().empty());

		/* Test else clause */
		/* if = 5 */
		StmtLstNode* elseStmtLstNode = childStmtLst[1];
		std::vector<StmtNode*> elseStmts = elseStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), elseStmts.size());
		AssignNode* elseAssignNode = (AssignNode*)elseStmts[0];
		Assert::IsTrue(StatementType::ASSIGN_TYPE == elseAssignNode->getStmtType());
		Assert::AreEqual(std::string("if"), *elseAssignNode->getModifiesVars().begin());
		ExprNode* five = elseAssignNode->getExpr();
		Assert::AreEqual(std::string("5"), five->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == five->getExprNodeValueType());
		Assert::IsTrue(five->getChildren().empty());

		/* 3rd procedure */
		/* procedure proc3 {read=read;} */
		Assert::AreEqual(std::string("proc3"), procNodes[2]->getProcName());
		StmtLstNode* proc3StmtLstNode = procNodes[2]->getStmtLstNode();
		std::vector<StmtNode*> proc3Stmts = proc3StmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), proc3Stmts.size());
		AssignNode* proc3AssignNode = (AssignNode*)proc3Stmts[0];
		Assert::IsTrue(StatementType::ASSIGN_TYPE == proc3AssignNode->getStmtType());
		Assert::AreEqual(std::string("read"), *proc3AssignNode->getModifiesVars().begin());
		ExprNode* proc3Expr = proc3AssignNode->getExpr();
		Assert::AreEqual(std::string("read"), proc3Expr->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == proc3Expr->getExprNodeValueType());
		Assert::IsTrue(proc3Expr->getChildren().empty());
	}

	TEST_METHOD(parse_multipleProcedures_syntacticallyValid_semanticallyInvalid_success) {
		/* Note that the parser doesn't do any semantic check, thus the duplicated proc name is allowed. */
		const char* source =
			" procedure proc1 {read proc1;}"
			"   procedure aRelativelyLongProcedureName \n "
			"{ read read; print y123;  \n "
			" x = (y + 1) * 3	; "
			" while ( (1 >= x ) || (! (while == 2147483647	)) ){ read print	; } "
			"if ( 2 > 2147483648) \n then \n"
			" { Re123ad = ALLUPPERCASE;}else{ if = 5; }"
			" } "
			" procedure proc1 {read=read;}";
		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(3), procNodes.size());

		/* 1st procedure */
		/* procedure proc1 {read proc1;} */
		Assert::AreEqual(std::string("proc1"), procNodes[0]->getProcName());
		StmtLstNode* proc1StmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> proc1Stmts = proc1StmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), proc1Stmts.size());
		ReadNode* proc1ReadNode = (ReadNode*)proc1Stmts[0];
		Assert::IsTrue(StatementType::READ_TYPE == proc1ReadNode->getStmtType());
		Assert::AreEqual(std::string("proc1"), *proc1ReadNode->getModifiesVars().begin());

		/* 2nd procedure */
		/* procedure aRelativelyLongProcedureName ... */
		Assert::AreEqual(std::string("aRelativelyLongProcedureName"), procNodes[1]->getProcName());

		/* Test statements in 2nd procedure */
		StmtLstNode* proc2StmtLstNode = procNodes[1]->getStmtLstNode();
		std::vector<StmtNode*> proc2Stmts = proc2StmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(5), proc2Stmts.size());

		/* Test read stmt */
		/* read read; */
		ReadNode* readNode = (ReadNode*)proc2Stmts[0];
		Assert::IsTrue(StatementType::READ_TYPE == readNode->getStmtType());
		Assert::AreEqual(std::string("read"), *readNode->getModifiesVars().begin());

		/* Test print stmt */
		/* print y123; */
		PrintNode* printNode = (PrintNode*)proc2Stmts[1];
		Assert::IsTrue(StatementType::PRINT_TYPE == printNode->getStmtType());
		Assert::AreEqual(std::string("y123"), *printNode->getUsesVars().begin());

		/* Test assign stmt */
		/* x = (y + 1) * 3 */
		AssignNode* assignNode = (AssignNode*)proc2Stmts[2];
		Assert::IsTrue(StatementType::ASSIGN_TYPE == assignNode->getStmtType());
		Assert::AreEqual(std::string("x"), *assignNode->getModifiesVars().begin());
		ExprNode* multOp = assignNode->getExpr();
		Assert::AreEqual(std::string("*"), multOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == multOp->getExprNodeValueType());

		/* (y + 1) */
		std::vector<ExprNode*> multOpChildren = multOp->getChildren();
		Assert::AreEqual(size_t(2), multOpChildren.size());
		ExprNode* plusOp = multOpChildren[0];
		Assert::AreEqual(std::string("+"), plusOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == plusOp->getExprNodeValueType());
		std::vector<ExprNode*> plusOpChildren = plusOp->getChildren();
		Assert::AreEqual(size_t(2), plusOpChildren.size());
		Assert::AreEqual(std::string("y"), plusOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == plusOpChildren[0]->getExprNodeValueType());
		Assert::IsTrue(plusOpChildren[0]->getChildren().empty());
		Assert::AreEqual(std::string("1"), plusOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == plusOpChildren[1]->getExprNodeValueType());
		Assert::IsTrue(plusOpChildren[1]->getChildren().empty());

		/* 3 */
		ExprNode* three = multOpChildren[1];
		Assert::AreEqual(std::string("3"), three->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == three->getExprNodeValueType());
		Assert::IsTrue(three->getChildren().empty());

		/* Test while stmt */
		/* while ( (1 >= x ) || (! (while == 2147483647	)) ){ read print	; } */
		WhileNode* whileNode = (WhileNode*)proc2Stmts[3];
		Assert::IsTrue(StatementType::WHILE_TYPE == whileNode->getStmtType());
		ExprNode* orOp = whileNode->getCondExpr();
		StmtLstNode* whileStmtLstNode = whileNode->getChildStmtLst()[0];

		/* Test while cond expr */
		Assert::AreEqual(std::string("||"), orOp->getValue());
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == orOp->getExprNodeValueType());
		std::vector<ExprNode*> orOpChildren = orOp->getChildren();
		Assert::AreEqual(size_t(2), orOpChildren.size());

		/* (1 >= x) */
		ExprNode* gteOp = orOpChildren[0];
		Assert::AreEqual(std::string(">="), gteOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == gteOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenGteOp = gteOp->getChildren();
		Assert::AreEqual(size_t(2), childrenGteOp.size());
		Assert::AreEqual(std::string("1"), childrenGteOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == childrenGteOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("x"), childrenGteOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == childrenGteOp[1]->getExprNodeValueType());

		/* (! (while == 2147483647	)) */
		ExprNode* notOp = orOpChildren[1];
		Assert::AreEqual(std::string("!"), notOp->getValue());
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == notOp->getExprNodeValueType());
		std::vector<ExprNode*> notOpChildren = notOp->getChildren();
		Assert::AreEqual(size_t(1), notOpChildren.size());

		/* (while == 2147483647	) */
		ExprNode* eqOp = notOpChildren[0];
		Assert::AreEqual(std::string("=="), eqOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == eqOp->getExprNodeValueType());
		std::vector<ExprNode*> eqOpChildren = eqOp->getChildren();
		Assert::AreEqual(size_t(2), eqOpChildren.size());

		Assert::AreEqual(std::string("while"), eqOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == eqOpChildren[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("2147483647"), eqOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == eqOpChildren[1]->getExprNodeValueType());

		/* Test stmtLst in while container */
		/* read print	; */
		std::vector<StmtNode*> stmtsInWhile = whileStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), stmtsInWhile.size());
		ReadNode* whileReadNode = (ReadNode*)stmtsInWhile[0];
		Assert::IsTrue(StatementType::READ_TYPE == whileReadNode->getStmtType());
		Assert::AreEqual(std::string("print"), *whileReadNode->getModifiesVars().begin());

		/* Test if stmt */
		/*if ( 2 > 2147483648) \n then \n
			 { Re123ad = ALLUPPERCASE;}else{ if = 5; }*/
		IfNode* ifNode = (IfNode*)proc2Stmts[4];
		Assert::IsTrue(StatementType::IF_TYPE == ifNode->getStmtType());
		std::vector<StmtLstNode*> childStmtLst = ifNode->getChildStmtLst();
		Assert::AreEqual(size_t(2), childStmtLst.size());

		/* Test cond expr */
		/* ( 2 > 2147483648) */
		ExprNode* gtOp = ifNode->getCondExpr();
		Assert::AreEqual(std::string(">"), gtOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == gtOp->getExprNodeValueType());
		std::vector<ExprNode*> gtOpChildren = gtOp->getChildren();
		Assert::AreEqual(size_t(2), gtOpChildren.size());
		Assert::AreEqual(std::string("2"), gtOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == gtOpChildren[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("2147483648"), gtOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == gtOpChildren[1]->getExprNodeValueType());

		/* Test then clause */
		/* Re123ad = ALLUPPERCASE; */
		StmtLstNode* thenStmtLstNode = childStmtLst[0];
		std::vector<StmtNode*> thenStmts = thenStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), thenStmts.size());
		AssignNode* thenAssignNode = (AssignNode*)thenStmts[0];
		Assert::IsTrue(StatementType::ASSIGN_TYPE == thenAssignNode->getStmtType());
		Assert::AreEqual(std::string("Re123ad"), *thenAssignNode->getModifiesVars().begin());
		ExprNode* allUpperCase = thenAssignNode->getExpr();
		Assert::AreEqual(std::string("ALLUPPERCASE"), allUpperCase->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == allUpperCase->getExprNodeValueType());
		Assert::IsTrue(allUpperCase->getChildren().empty());

		/* Test else clause */
		/* if = 5 */
		StmtLstNode* elseStmtLstNode = childStmtLst[1];
		std::vector<StmtNode*> elseStmts = elseStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), elseStmts.size());
		AssignNode* elseAssignNode = (AssignNode*)elseStmts[0];
		Assert::IsTrue(StatementType::ASSIGN_TYPE == elseAssignNode->getStmtType());
		Assert::AreEqual(std::string("if"), *elseAssignNode->getModifiesVars().begin());
		ExprNode* five = elseAssignNode->getExpr();
		Assert::AreEqual(std::string("5"), five->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == five->getExprNodeValueType());
		Assert::IsTrue(five->getChildren().empty());

		/* 3rd procedure */
		/* procedure proc1 {read=read;} */
		/* Note that the parser doesn't do any semantic check, thus the duplicated proc name is allowed. */
		Assert::AreEqual(std::string("proc1"), procNodes[2]->getProcName());
		StmtLstNode* proc3StmtLstNode = procNodes[2]->getStmtLstNode();
		std::vector<StmtNode*> proc3Stmts = proc3StmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), proc3Stmts.size());
		AssignNode* proc3AssignNode = (AssignNode*)proc3Stmts[0];
		Assert::IsTrue(StatementType::ASSIGN_TYPE == proc3AssignNode->getStmtType());
		Assert::AreEqual(std::string("read"), *proc3AssignNode->getModifiesVars().begin());
		ExprNode* proc3Expr = proc3AssignNode->getExpr();
		Assert::AreEqual(std::string("read"), proc3Expr->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == proc3Expr->getExprNodeValueType());
		Assert::IsTrue(proc3Expr->getChildren().empty());
	}

	TEST_METHOD(parse_multipleProcedures_syntacticallyInvalid_parserExceptionThrown) {
		const char* source = "procedure proc {read=read;}"
			"   procedure 1procName  "
			"{ read x1; print y  ; } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_PROC_NAME.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_multipleProcedures_missingProcKeyword_parserExceptionThrown) {
		const char* source = "procedure proc {read=read;}"
			"   proc procName  "
			"{ read x1; print y  ; } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_PROC_KEYWORD.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchRead_missingReadVarName_parserExceptionThrown) {
		const char* source = "   procedure proc1  "
			"{ print x1;print y1 ; read } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_VAR_NAME.c_str(), ex.what());
		}

		const char* source1 = "   procedure proc1  "
			"{ print x1;print y1 ; read		; print V; }";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_VAR_NAME.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchRead_invalidReadVarName_parserExceptionThrown) {
		const char* source = "   procedure proc  "
			"{ print x1; read 1invalidName ; } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_VAR_NAME.c_str(), ex.what());
		}

		const char* source1 = "   procedure proc1  "
			"{ print x1;print y1 ; read invalid_name; }";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchRead_missingSemicolon_parserExceptionThrown) {
		const char* source = "   procedure proc  "
			"{ print x1; read vAliD123nAmE read vAliD123nAmEaGAIN ;}  ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}

		const char* source1 = "   procedure proc  "
			"{ print x1; read vAliD123nAmE ; read vAliD123nAmEaGAIN  } ";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
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

		const char* source1 = "   procedure proc  "
			"{ print x1; read vAliD123nAmE ;; read x1 ; }";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchPrint_missingPrintVarName_parserExceptionThrown) {
		const char* source = "   procedure proc1  "
			"{ read x1;read y1 ; print  }";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_VAR_NAME.c_str(), ex.what());
		}

		const char* source1 = "   procedure proc1  "
			"{ read x1;read y1 ; print  ; read x; }";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_VAR_NAME.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchPrint_invalidPrintVarName_parserExceptionThrown) {
		const char* source = "   procedure proc  "
			"{ read x1; print 0invalidName ; } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_VAR_NAME.c_str(), ex.what());
		}

		const char* source1 = "   procedure proc1  "
			"{ read x1;read y1 ; print invalid_name; read y1	;}";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
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

		const char* source1 = "   procedure proc  "
			"{ read x1; print vAliD123nAmE  print p ; } ";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
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

		const char* source1 = "   procedure proc  "
			"{ print x1 ; read vAliD123nAmE ; ;; }";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchStmt_invalidStmtOneStmt_parserExceptionThrown) {
		const char* source = "   procedure proc  "
			"{ r3ad x1;  } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchStmt_invalidStmtInMultipleStmts_parserExceptionThrown) {
		const char* source = "   procedure proc  "
			"{ read x1; print yadayadayada  ; assign 1; x = 123	;  } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
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

	TEST_METHOD(parse_matchProcedure_missingStmtLst_parserExceptionThrown) {
		const char* source = "   procedure procName  "
			"{ } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
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
		/* flag    = 123; */
		AssignNode* assignNode1 = (AssignNode*)statements[0];
		Assert::AreEqual(std::string("flag"), *assignNode1->getModifiesVars().begin());

		ExprNode* expr1 = assignNode1->getExpr();
		Assert::AreEqual(std::string("123"), expr1->getValue());
		Assert::IsTrue(expr1->getChildren().empty());

		/* count =    someVar123	; */
		AssignNode* assignNode2 = (AssignNode*)statements[1];
		Assert::AreEqual(std::string("count"), *assignNode2->getModifiesVars().begin());

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
		/* CenX = 9 + CenX; */
		AssignNode* assignNode1 = (AssignNode*)statements[0];
		Assert::AreEqual(std::string("CenX"), *assignNode1->getModifiesVars().begin());

		ExprNode* expr1 = assignNode1->getExpr();
		Assert::AreEqual(std::string("+"), expr1->getValue());
		std::vector<ExprNode*> children1 = expr1->getChildren();
		Assert::AreEqual(size_t(2), children1.size());
		Assert::AreEqual(std::string("9"), children1[0]->getValue());
		Assert::AreEqual(std::string("CenX"), children1[1]->getValue());

		/* count = COUNT % 2   ; */
		AssignNode* assignNode2 = (AssignNode*)statements[1];
		Assert::AreEqual(std::string("count"), *assignNode2->getModifiesVars().begin());

		ExprNode* expr2 = assignNode2->getExpr();
		Assert::AreEqual(std::string("%"), expr2->getValue());
		std::vector<ExprNode*> children2 = expr2->getChildren();
		Assert::AreEqual(size_t(2), children2.size());
		Assert::AreEqual(std::string("COUNT"), children2[0]->getValue());
		Assert::AreEqual(std::string("2"), children2[1]->getValue());

		/* x = x + z * 5 ; */
		AssignNode* assignNode3 = (AssignNode*)statements[2];
		Assert::AreEqual(std::string("x"), *assignNode3->getModifiesVars().begin());

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

		/* y = y / w - 1; */
		AssignNode* assignNode4 = (AssignNode*)statements[3];
		Assert::AreEqual(std::string("y"), *assignNode4->getModifiesVars().begin());

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

		/* z = z + a123 / b0b - c  ; */
		AssignNode* assignNode5 = (AssignNode*)statements[4];
		Assert::AreEqual(std::string("z"), *assignNode5->getModifiesVars().begin());

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
		/* CenX = (9 + CenX); */
		AssignNode* assignNode1 = (AssignNode*)statements[0];
		Assert::AreEqual(std::string("CenX"), *assignNode1->getModifiesVars().begin());

		ExprNode* expr1 = assignNode1->getExpr();
		Assert::AreEqual(std::string("+"), expr1->getValue());
		std::vector<ExprNode*> children1 = expr1->getChildren();
		Assert::AreEqual(size_t(2), children1.size());
		Assert::AreEqual(std::string("9"), children1[0]->getValue());
		Assert::AreEqual(std::string("CenX"), children1[1]->getValue());

		/* count = (COUNT % 2)   ; */
		AssignNode* assignNode2 = (AssignNode*)statements[1];
		Assert::AreEqual(std::string("count"), *assignNode2->getModifiesVars().begin());

		ExprNode* expr2 = assignNode2->getExpr();
		Assert::AreEqual(std::string("%"), expr2->getValue());
		std::vector<ExprNode*> children2 = expr2->getChildren();
		Assert::AreEqual(size_t(2), children2.size());
		Assert::AreEqual(std::string("COUNT"), children2[0]->getValue());
		Assert::AreEqual(std::string("2"), children2[1]->getValue());

		/* x = (x + z) * 5 ; */
		AssignNode* assignNode3 = (AssignNode*)statements[2];
		Assert::AreEqual(std::string("x"), *assignNode3->getModifiesVars().begin());

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

		/* y = y / (w - 1); */
		AssignNode* assignNode4 = (AssignNode*)statements[3];
		Assert::AreEqual(std::string("y"), *assignNode4->getModifiesVars().begin());

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

		/* z = (z + a123) / (b0b - c)  ; */
		AssignNode* assignNode5 = (AssignNode*)statements[4];
		Assert::AreEqual(std::string("z"), *assignNode5->getModifiesVars().begin());

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

	TEST_METHOD(parse_matchAssign_withVeryNestedExpr_success) {
		/*
									/
							+				-
						z		a123	b0b		-
											*		%
										  c   10 ALL  +
													 z  *
													   1 0
		*/
		const char* source = "   procedure procedure123name \n "
			"{ z = (( (z + a123) / (b0b - (c * 10 - ALL %( Z + (1*0)) )	) )) ;}";
		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(1), procNodes.size());
		Assert::AreEqual(std::string("procedure123name"), procNodes[0]->getProcName());

		/* Test statements */
		StmtLstNode* stmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> statements = stmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), statements.size());

		/* Test assign nodes*/
		/* z = (( (z + a123) / (b0b - (c * 10 - ALL %( Z + (1*0)) )	) )) ; */
		AssignNode* assignNode = (AssignNode*)statements[0];
		Assert::IsTrue(StatementType::ASSIGN_TYPE == assignNode->getStmtType());
		Assert::AreEqual(std::string("z"), *assignNode->getModifiesVars().begin());

		ExprNode* divideOp = assignNode->getExpr();
		Assert::AreEqual(std::string("/"), divideOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == divideOp->getExprNodeValueType());
		std::vector<ExprNode*> divideOpchildren = divideOp->getChildren();
		Assert::AreEqual(size_t(2), divideOpchildren.size());

		/* (z + a123) */
		ExprNode* plusOp = divideOpchildren[0];
		Assert::AreEqual(std::string("+"), plusOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == plusOp->getExprNodeValueType());
		std::vector<ExprNode*> plusOpChildren = plusOp->getChildren();
		Assert::AreEqual(size_t(2), plusOpChildren.size());
		Assert::AreEqual(std::string("z"), plusOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == plusOpChildren[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("a123"), plusOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == plusOpChildren[1]->getExprNodeValueType());

		/* (b0b - (c * 10 - ALL %( Z + (1*0)) ) */
		ExprNode* minusOp = divideOpchildren[1];
		Assert::AreEqual(std::string("-"), minusOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == minusOp->getExprNodeValueType());
		std::vector<ExprNode*> minusOpChildren = minusOp->getChildren();
		Assert::AreEqual(size_t(2), minusOpChildren.size());

		/* b0b */
		Assert::AreEqual(std::string("b0b"), minusOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == minusOpChildren[0]->getExprNodeValueType());

		/* (c * 10 - ALL %( Z + (1*0)) ) */
		ExprNode* minusOp2 = minusOpChildren[1];
		Assert::AreEqual(std::string("-"), minusOp2->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == minusOp2->getExprNodeValueType());
		std::vector<ExprNode*> minusOp2Children = minusOp2->getChildren();
		Assert::AreEqual(size_t(2), minusOp2Children.size());

		/* c * 10 */
		ExprNode* multOp = minusOp2Children[0];
		Assert::AreEqual(std::string("*"), multOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == multOp->getExprNodeValueType());
		std::vector<ExprNode*> multOpChildren = multOp->getChildren();
		Assert::AreEqual(size_t(2), multOpChildren.size());
		Assert::AreEqual(std::string("c"), multOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == multOpChildren[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("10"), multOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == multOpChildren[1]->getExprNodeValueType());

		/* ALL %( Z + (1*0) ) */
		ExprNode* modOp = minusOp2Children[1];
		Assert::AreEqual(std::string("%"), modOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == modOp->getExprNodeValueType());
		std::vector<ExprNode*> modOpChildren = modOp->getChildren();
		Assert::AreEqual(size_t(2), modOpChildren.size());

		/* ALL */
		Assert::AreEqual(std::string("ALL"), modOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == modOpChildren[0]->getExprNodeValueType());

		/* ( Z + (1*0) ) */
		ExprNode* plusOp2 = modOpChildren[1];
		Assert::AreEqual(std::string("+"), plusOp2->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == plusOp2->getExprNodeValueType());
		std::vector<ExprNode*> plusOp2Children = plusOp2->getChildren();
		Assert::AreEqual(size_t(2), plusOp2Children.size());

		/* Z */
		Assert::AreEqual(std::string("Z"), plusOp2Children[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == plusOp2Children[0]->getExprNodeValueType());

		/* (1*0) */
		ExprNode* multOp2 = plusOp2Children[1];
		Assert::AreEqual(std::string("*"), multOp2->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == multOp2->getExprNodeValueType());
		std::vector<ExprNode*> multOp2Children = multOp2->getChildren();
		Assert::AreEqual(size_t(2), multOp2Children.size());
		Assert::AreEqual(std::string("1"), multOp2Children[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == multOp2Children[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("0"), multOp2Children[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == multOp2Children[1]->getExprNodeValueType());
	}

	TEST_METHOD(parse_matchAssign_withReservedKeywords_success) {
		const char* source = "   procedure procedure123name \n "
			"{ if = 9 + read; read = print % 2   ;"
			" while = x + z * 5 ; "
			" print = y / w - 1; "
			" then = z + a123 / b0b - c  ;}";
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
		/* if = 9 + read; */
		AssignNode* assignNode1 = (AssignNode*)statements[0];
		Assert::AreEqual(std::string("if"), *assignNode1->getModifiesVars().begin());

		ExprNode* expr1 = assignNode1->getExpr();
		Assert::AreEqual(std::string("+"), expr1->getValue());
		std::vector<ExprNode*> children1 = expr1->getChildren();
		Assert::AreEqual(size_t(2), children1.size());
		Assert::AreEqual(std::string("9"), children1[0]->getValue());
		Assert::AreEqual(std::string("read"), children1[1]->getValue());

		/* read = print % 2   ; */
		AssignNode* assignNode2 = (AssignNode*)statements[1];
		Assert::AreEqual(std::string("read"), *assignNode2->getModifiesVars().begin());

		ExprNode* expr2 = assignNode2->getExpr();
		Assert::AreEqual(std::string("%"), expr2->getValue());
		std::vector<ExprNode*> children2 = expr2->getChildren();
		Assert::AreEqual(size_t(2), children2.size());
		Assert::AreEqual(std::string("print"), children2[0]->getValue());
		Assert::AreEqual(std::string("2"), children2[1]->getValue());

		/* while = x + z * 5 ; */
		AssignNode* assignNode3 = (AssignNode*)statements[2];
		Assert::AreEqual(std::string("while"), *assignNode3->getModifiesVars().begin());

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

		/* print = y / w - 1; */
		AssignNode* assignNode4 = (AssignNode*)statements[3];
		Assert::AreEqual(std::string("print"), *assignNode4->getModifiesVars().begin());

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

		/* then = z + a123 / b0b - c  ; */
		AssignNode* assignNode5 = (AssignNode*)statements[4];
		Assert::AreEqual(std::string("then"), *assignNode5->getModifiesVars().begin());

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

	TEST_METHOD(parse_matchAssign_missingVarname_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{    = (x + z) * 5  }";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchAssign_invalidVarname_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{  !invalid_var_name  = (x + z) * 5 ; }";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchAssign_missingEqualSign_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{  x   (x + z) * 5 ; }";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchAssign_missingExpr_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{  x  =  ; }";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}
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

	TEST_METHOD(parse_matchAssign_missingFactor_parserExceptionThrown) {
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
			" {  x =  * 5 ; }";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchAssign_invalidFactor_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			" {  x = (x + 1) * ! ; }";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}

		const char* source1 = "   procedure procedure123name \n "
			" {  x = ^ * 5 ; }";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}

		const char* source2 = "   procedure procedure123name \n "
			" {  x = (x + 1a) * 5 ; }";
		auto wrapperFunc2 = [&source2] { Parser::parse(source2); };
		Assert::ExpectException<ParserException>(wrapperFunc2);
		try {
			Parser::parse(source2);
		} catch (ParserException& ex) {
			/* '1' will be taken as a valid const_value, but "a" will be taken as invalid right bracket */
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}

		const char* source3 = "   procedure procedure123name \n "
			"{  y = !y / (w - 1); "
			" z = (z + a123) / (b0b - c)  ;}";
		auto wrapperFunc3 = [&source3] { Parser::parse(source3); };
		Assert::ExpectException<ParserException>(wrapperFunc3);
		try {
			Parser::parse(source3);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}

		const char* source4 = "   procedure procedure123name \n "
			"{  z = (z + 0a123) / (b0b - c)  ;}";
		auto wrapperFunc4 = [&source4] { Parser::parse(source4); };
		Assert::ExpectException<ParserException>(wrapperFunc4);
		try {
			Parser::parse(source4);
		} catch (ParserException& ex) {
			/* '0' will be taken as a valid const_value, but "a" will be taken as invalid right bracket */
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchAssign_missingOp_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			" {  x = (x  1) * 5 ; }";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_EXPR.c_str(), ex.what());
		}

		const char* source1 = "   procedure procedure123name \n "
			" {  x = (x + 1)  5 ; }";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			/* Since '5' is not a term or expr operator, "x = (x + 1)" is regarded as a valid stmt.
			Thus, the parser is expecting a semicolon next. */
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchAssign_invalidOp_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			" {  x = (x + 1) ? 4 ; }";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			/* Since '?' is not a term or expr operator, "x = (x + 1)" is regarded as a valid stmt.
			Thus, the parser is expecting a semicolon next. */
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

		const char* source3 = "   procedure procedure123name \n "
			" {  x = 2 @ 5 ; }";
		auto wrapperFunc3 = [&source3] { Parser::parse(source3); };
		Assert::ExpectException<ParserException>(wrapperFunc3);
		try {
			Parser::parse(source3);
		} catch (ParserException& ex) {
			/* Since '&' is not a term or expr operator, "x = 2" is regarded as a valid stmt.
				Thus, the parser is expecting a semicolon next. */
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
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
		/* y + 2 > 5 */
		Assert::AreEqual(std::string(">"), gtOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == gtOp->getExprNodeValueType());
		std::vector<ExprNode*> gtOpChildren = gtOp->getChildren();
		Assert::AreEqual(size_t(2), gtOpChildren.size());

		/* y + 2 */
		ExprNode* plusOp = gtOpChildren[0];
		Assert::AreEqual(std::string("+"), plusOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == plusOp->getExprNodeValueType());
		std::vector<ExprNode*> plusOpChildren = plusOp->getChildren();
		Assert::AreEqual(size_t(2), plusOpChildren.size());
		Assert::AreEqual(std::string("y"), plusOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == plusOpChildren[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("2"), plusOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == plusOpChildren[1]->getExprNodeValueType());

		/* 5 */
		Assert::AreEqual(std::string("5"), gtOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == gtOpChildren[1]->getExprNodeValueType());

		/* Test stmtLst in while container */
		std::vector<StmtNode*> stmtsInWhile = stmtLstNodeWhile->getStmtNodes();
		Assert::AreEqual(size_t(4), stmtsInWhile.size());

		/* flag    = 123; */
		AssignNode* assignNode1 = (AssignNode*)stmtsInWhile[0];
		Assert::AreEqual(std::string("flag"), *assignNode1->getModifiesVars().begin());

		ExprNode* expr1 = assignNode1->getExpr();
		Assert::AreEqual(std::string("123"), expr1->getValue());
		Assert::IsTrue(expr1->getChildren().empty());

		/* count =  ((  someVar123 )  )	; */
		AssignNode* assignNode2 = (AssignNode*)stmtsInWhile[1];
		Assert::AreEqual(std::string("count"), *assignNode2->getModifiesVars().begin());

		ExprNode* expr2 = assignNode2->getExpr();
		Assert::AreEqual(std::string("someVar123"), expr2->getValue());
		Assert::IsTrue(expr2->getChildren().empty());

		/* read flag ; */
		ReadNode* readNode = (ReadNode*)stmtsInWhile[2];
		Assert::AreEqual(std::string("flag"), *readNode->getModifiesVars().begin());

		/* print COUNT	; */
		PrintNode* printNode = (PrintNode*)stmtsInWhile[3];
		Assert::AreEqual(std::string("COUNT"), *printNode->getUsesVars().begin());
	}

	TEST_METHOD(parse_matchWhile_oneRelExprCondExpr_veryNestedExpr_success) {
		const char* source = "   procedure procedure123name \n "
			"{ while ( y + 2 > (b0b - (c * 10 - ALL %( Z + (1*0)) )	)) "
			"{ flag    = 123;   } "
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
		Assert::IsTrue(StatementType::WHILE_TYPE == whileNode->getStmtType());
		ExprNode* gtOp = whileNode->getCondExpr();
		StmtLstNode* stmtLstNodeWhile = whileNode->getChildStmtLst()[0];

		/* Test cond expr */
		/* ( y + 2 > (b0b - (c * 10 - ALL %( Z + (1*0)) )	))  */
		Assert::AreEqual(std::string(">"), gtOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == gtOp->getExprNodeValueType());
		std::vector<ExprNode*> gtOpChildren = gtOp->getChildren();
		Assert::AreEqual(size_t(2), gtOpChildren.size());

		/* y + 2 */
		ExprNode* plusOp = gtOpChildren[0];
		Assert::AreEqual(std::string("+"), plusOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == plusOp->getExprNodeValueType());
		std::vector<ExprNode*> plusOpChildren = plusOp->getChildren();
		Assert::AreEqual(size_t(2), plusOpChildren.size());
		Assert::AreEqual(std::string("y"), plusOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == plusOpChildren[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("2"), plusOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == plusOpChildren[1]->getExprNodeValueType());

		/* (b0b - (c * 10 - ALL %( Z + (1*0)) ) */
		ExprNode* minusOp = gtOpChildren[1];
		Assert::AreEqual(std::string("-"), minusOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == minusOp->getExprNodeValueType());
		std::vector<ExprNode*> minusOpChildren = minusOp->getChildren();
		Assert::AreEqual(size_t(2), minusOpChildren.size());

		/* b0b */
		Assert::AreEqual(std::string("b0b"), minusOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == minusOpChildren[0]->getExprNodeValueType());

		/* (c * 10 - ALL %( Z + (1*0)) ) */
		ExprNode* minusOp2 = minusOpChildren[1];
		Assert::AreEqual(std::string("-"), minusOp2->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == minusOp2->getExprNodeValueType());
		std::vector<ExprNode*> minusOp2Children = minusOp2->getChildren();
		Assert::AreEqual(size_t(2), minusOp2Children.size());

		/* c * 10 */
		ExprNode* multOp = minusOp2Children[0];
		Assert::AreEqual(std::string("*"), multOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == multOp->getExprNodeValueType());
		std::vector<ExprNode*> multOpChildren = multOp->getChildren();
		Assert::AreEqual(size_t(2), multOpChildren.size());
		Assert::AreEqual(std::string("c"), multOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == multOpChildren[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("10"), multOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == multOpChildren[1]->getExprNodeValueType());

		/* ALL %( Z + (1*0) ) */
		ExprNode* modOp = minusOp2Children[1];
		Assert::AreEqual(std::string("%"), modOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == modOp->getExprNodeValueType());
		std::vector<ExprNode*> modOpChildren = modOp->getChildren();
		Assert::AreEqual(size_t(2), modOpChildren.size());

		/* ALL */
		Assert::AreEqual(std::string("ALL"), modOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == modOpChildren[0]->getExprNodeValueType());

		/* ( Z + (1*0) ) */
		ExprNode* plusOp2 = modOpChildren[1];
		Assert::AreEqual(std::string("+"), plusOp2->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == plusOp2->getExprNodeValueType());
		std::vector<ExprNode*> plusOp2Children = plusOp2->getChildren();
		Assert::AreEqual(size_t(2), plusOp2Children.size());

		/* Z */
		Assert::AreEqual(std::string("Z"), plusOp2Children[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == plusOp2Children[0]->getExprNodeValueType());

		/* (1*0) */
		ExprNode* multOp2 = plusOp2Children[1];
		Assert::AreEqual(std::string("*"), multOp2->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == multOp2->getExprNodeValueType());
		std::vector<ExprNode*> multOp2Children = multOp2->getChildren();
		Assert::AreEqual(size_t(2), multOp2Children.size());
		Assert::AreEqual(std::string("1"), multOp2Children[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == multOp2Children[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("0"), multOp2Children[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == multOp2Children[1]->getExprNodeValueType());

		/* Test stmtLst in while container */
		std::vector<StmtNode*> stmtsInWhile = stmtLstNodeWhile->getStmtNodes();
		Assert::AreEqual(size_t(1), stmtsInWhile.size());

		/* flag    = 123; */
		AssignNode* assignNode = (AssignNode*)stmtsInWhile[0];
		Assert::IsTrue(StatementType::ASSIGN_TYPE == assignNode->getStmtType());
		Assert::AreEqual(std::string("flag"), *assignNode->getModifiesVars().begin());

		ExprNode* expr = assignNode->getExpr();
		Assert::AreEqual(std::string("123"), expr->getValue());
		Assert::IsTrue(expr->getChildren().empty());
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
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == notOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenNotOp = notOp->getChildren();
		Assert::AreEqual(size_t(1), childrenNotOp.size());

		ExprNode* gteOp = childrenNotOp[0];
		Assert::AreEqual(std::string(">="), gteOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == gteOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenGteOp = gteOp->getChildren();
		Assert::AreEqual(size_t(2), childrenGteOp.size());

		Assert::AreEqual(std::string("x"), childrenGteOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == childrenGteOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("0"), childrenGteOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == childrenGteOp[1]->getExprNodeValueType());

		/* Test stmtLst in while container */
		std::vector<StmtNode*> stmtsInWhile = stmtLstNodeWhile->getStmtNodes();
		Assert::AreEqual(size_t(4), stmtsInWhile.size());

		/* flag    = 123; */
		AssignNode* assignNode1 = (AssignNode*)stmtsInWhile[0];
		Assert::AreEqual(std::string("flag"), *assignNode1->getModifiesVars().begin());

		ExprNode* expr1 = assignNode1->getExpr();
		Assert::AreEqual(std::string("123"), expr1->getValue());
		Assert::IsTrue(expr1->getChildren().empty());

		/* count =  ((  someVar123 )  )	; */
		AssignNode* assignNode2 = (AssignNode*)stmtsInWhile[1];
		Assert::AreEqual(std::string("count"), *assignNode2->getModifiesVars().begin());

		ExprNode* expr2 = assignNode2->getExpr();
		Assert::AreEqual(std::string("someVar123"), expr2->getValue());
		Assert::IsTrue(expr2->getChildren().empty());

		/* read flag ; */
		ReadNode* readNode = (ReadNode*)stmtsInWhile[2];
		Assert::AreEqual(std::string("flag"), *readNode->getModifiesVars().begin());

		/* print COUNT	; */
		PrintNode* printNode = (PrintNode*)stmtsInWhile[3];
		Assert::AreEqual(std::string("COUNT"), *printNode->getUsesVars().begin());
	}

	TEST_METHOD(parse_matchWhile_nestedNOTOpCondExpr_success) {
		const char* source = "   procedure procedure123name \n "
			"{ while (!(	!(x >= 0)	)	) "
			"{ flag    = 123;  } "
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
		Assert::IsTrue(StatementType::WHILE_TYPE == whileNode->getStmtType());
		StmtLstNode* stmtLstNodeWhile = whileNode->getChildStmtLst()[0];

		/* Test cond expr */
		/* Outer NOT cond */
		ExprNode* outerNotOp = whileNode->getCondExpr();
		Assert::AreEqual(std::string("!"), outerNotOp->getValue());
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == outerNotOp->getExprNodeValueType());
		std::vector<ExprNode*> outerNotOpChildren = outerNotOp->getChildren();
		Assert::AreEqual(size_t(1), outerNotOpChildren.size());

		/* Inner NOT cond */
		ExprNode* innerNotOp = outerNotOpChildren[0];
		Assert::AreEqual(std::string("!"), innerNotOp->getValue());
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == innerNotOp->getExprNodeValueType());
		std::vector<ExprNode*> innerNotOpChildren = innerNotOp->getChildren();
		Assert::AreEqual(size_t(1), innerNotOpChildren.size());

		/* (x >= 0) */
		ExprNode* gteOp = innerNotOpChildren[0];
		Assert::AreEqual(std::string(">="), gteOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == gteOp->getExprNodeValueType());
		std::vector<ExprNode*> gteOpChildren = gteOp->getChildren();
		Assert::AreEqual(size_t(2), gteOpChildren.size());
		Assert::AreEqual(std::string("x"), gteOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == gteOpChildren[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("0"), gteOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == gteOpChildren[1]->getExprNodeValueType());

		/* Test stmtLst in while container */
		std::vector<StmtNode*> stmtsInWhile = stmtLstNodeWhile->getStmtNodes();
		Assert::AreEqual(size_t(1), stmtsInWhile.size());

		/* flag    = 123; */
		AssignNode* assignNode = (AssignNode*)stmtsInWhile[0];
		Assert::IsTrue(StatementType::ASSIGN_TYPE == assignNode->getStmtType());
		Assert::AreEqual(std::string("flag"), *assignNode->getModifiesVars().begin());
		ExprNode* expr = assignNode->getExpr();
		Assert::AreEqual(std::string("123"), expr->getValue());
		Assert::IsTrue(expr->getChildren().empty());
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
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == andOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenAndOp = andOp->getChildren();
		Assert::AreEqual(size_t(2), childrenAndOp.size());

		/* !(x >= 0)*/
		ExprNode* notOp = childrenAndOp[0];
		Assert::AreEqual(std::string("!"), notOp->getValue());
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == notOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenNotOp = notOp->getChildren();
		Assert::AreEqual(size_t(1), childrenNotOp.size());

		ExprNode* gteOp = childrenNotOp[0];
		Assert::AreEqual(std::string(">="), gteOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == gteOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenGteOp = gteOp->getChildren();
		Assert::AreEqual(size_t(2), childrenGteOp.size());

		Assert::AreEqual(std::string("x"), childrenGteOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == childrenGteOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("0"), childrenGteOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == childrenGteOp[1]->getExprNodeValueType());

		/* ( y < 5 - z ) */
		ExprNode* ltOp = childrenAndOp[1];
		Assert::AreEqual(std::string("<"), ltOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == ltOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenLtOp = ltOp->getChildren();
		Assert::AreEqual(size_t(2), childrenLtOp.size());

		Assert::AreEqual(std::string("y"), childrenLtOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == childrenLtOp[0]->getExprNodeValueType());

		ExprNode* minusOp = childrenLtOp[1];
		Assert::AreEqual(std::string("-"), minusOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == minusOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenMinusOp = minusOp->getChildren();
		Assert::AreEqual(size_t(2), childrenMinusOp.size());

		Assert::AreEqual(std::string("5"), childrenMinusOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == childrenMinusOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("z"), childrenMinusOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == childrenMinusOp[1]->getExprNodeValueType());

		/* Test stmtLst in while container */
		std::vector<StmtNode*> stmtsInWhile = stmtLstNodeWhile->getStmtNodes();
		Assert::AreEqual(size_t(1), stmtsInWhile.size());

		/* read fl123ag ; */
		ReadNode* readNode = (ReadNode*)stmtsInWhile[0];
		Assert::AreEqual(std::string("fl123ag"), *readNode->getModifiesVars().begin());
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
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == orOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenOrOp = orOp->getChildren();
		Assert::AreEqual(size_t(2), childrenOrOp.size());

		/* (x <= 0) */
		ExprNode* lteOp = childrenOrOp[0];
		Assert::AreEqual(std::string("<="), lteOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == lteOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenLteOp = lteOp->getChildren();
		Assert::AreEqual(size_t(2), childrenLteOp.size());

		Assert::AreEqual(std::string("x"), childrenLteOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == childrenLteOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("0"), childrenLteOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == childrenLteOp[1]->getExprNodeValueType());

		/* ( y * 1 == 5 - z )*/
		ExprNode* eqOp = childrenOrOp[1];
		Assert::AreEqual(std::string("=="), eqOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == eqOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenEqOp = eqOp->getChildren();
		Assert::AreEqual(size_t(2), childrenEqOp.size());

		ExprNode* multiplyOp = childrenEqOp[0];
		Assert::AreEqual(std::string("*"), multiplyOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == multiplyOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenMultiplyOp = multiplyOp->getChildren();
		Assert::AreEqual(size_t(2), childrenMultiplyOp.size());

		Assert::AreEqual(std::string("y"), childrenMultiplyOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == childrenMultiplyOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("1"), childrenMultiplyOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == childrenMultiplyOp[1]->getExprNodeValueType());

		ExprNode* minusOp = childrenEqOp[1];
		Assert::AreEqual(std::string("-"), minusOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == minusOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenMinusOp = minusOp->getChildren();
		Assert::AreEqual(size_t(2), childrenMinusOp.size());

		Assert::AreEqual(std::string("5"), childrenMinusOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == childrenMinusOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("z"), childrenMinusOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == childrenMinusOp[1]->getExprNodeValueType());

		/* Test stmtLst in while container */
		std::vector<StmtNode*> stmtsInWhile = stmtLstNodeWhile->getStmtNodes();
		Assert::AreEqual(size_t(1), stmtsInWhile.size());

		/* read fl123ag ; */
		ReadNode* readNode = (ReadNode*)stmtsInWhile[0];
		Assert::AreEqual(std::string("fl123ag"), *readNode->getModifiesVars().begin());
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
		/* !( (y * 1 == 5 - z) || (x != 0)  )*/
		Assert::AreEqual(std::string("!"), notOp->getValue());
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == notOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenNotOp = notOp->getChildren();
		Assert::AreEqual(size_t(1), childrenNotOp.size());

		ExprNode* orOp = childrenNotOp[0];
		Assert::AreEqual(std::string("||"), orOp->getValue());
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == orOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenOrOp = orOp->getChildren();
		Assert::AreEqual(size_t(2), childrenOrOp.size());

		/* (y * 1 == 5 - z)*/
		ExprNode* eqOp = childrenOrOp[0];
		Assert::AreEqual(std::string("=="), eqOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == eqOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenEqOp = eqOp->getChildren();
		Assert::AreEqual(size_t(2), childrenEqOp.size());

		ExprNode* multiplyOp = childrenEqOp[0];
		Assert::AreEqual(std::string("*"), multiplyOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == multiplyOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenMultiplyOp = multiplyOp->getChildren();
		Assert::AreEqual(size_t(2), childrenMultiplyOp.size());

		Assert::AreEqual(std::string("y"), childrenMultiplyOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == childrenMultiplyOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("1"), childrenMultiplyOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == childrenMultiplyOp[1]->getExprNodeValueType());

		ExprNode* minusOp = childrenEqOp[1];
		Assert::AreEqual(std::string("-"), minusOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == minusOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenMinusOp = minusOp->getChildren();
		Assert::AreEqual(size_t(2), childrenMinusOp.size());

		Assert::AreEqual(std::string("5"), childrenMinusOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == childrenMinusOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("z"), childrenMinusOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == childrenMinusOp[1]->getExprNodeValueType());

		// (x != 0)
		ExprNode* neqOp = childrenOrOp[1];
		Assert::AreEqual(std::string("!="), neqOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == neqOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenNeqOp = neqOp->getChildren();
		Assert::AreEqual(size_t(2), childrenNeqOp.size());

		Assert::AreEqual(std::string("x"), childrenNeqOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == childrenNeqOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("0"), childrenNeqOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == childrenNeqOp[1]->getExprNodeValueType());

		/* Test stmtLst in while container */
		std::vector<StmtNode*> stmtsInWhile = stmtLstNodeWhile->getStmtNodes();
		Assert::AreEqual(size_t(1), stmtsInWhile.size());

		/* read fl123ag ; */
		ReadNode* readNode = (ReadNode*)stmtsInWhile[0];
		Assert::AreEqual(std::string("fl123ag"), *readNode->getModifiesVars().begin());
	}

	TEST_METHOD(parse_matchWhile_veryNestedCondExpr_success) {
		const char* source = "   procedure procedure123name \n "
			"{ while ( !(   ((ALLUPPERCASE > 123 + (4567*0)) && (!(1-2147483647 % 3 != MiXeDcAsE)) ) ||   (1<999 / 888)	 )    ) "
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
		Assert::IsTrue(StatementType::WHILE_TYPE == whileNode->getStmtType());
		ExprNode* notOp = whileNode->getCondExpr();
		StmtLstNode* stmtLstNodeWhile = whileNode->getChildStmtLst()[0];

		/* Test cond expr */
		/*  !(   ((ALLUPPERCASE > 123 + (4567*0)) && (!(1-2147483647 % 3 != MiXeDcAsE)) ) ||   (1<999 / 888)	 ) */
		Assert::AreEqual(std::string("!"), notOp->getValue());
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == notOp->getExprNodeValueType());
		std::vector<ExprNode*> notOpChildren = notOp->getChildren();
		Assert::AreEqual(size_t(1), notOpChildren.size());

		/* ((ALLUPPERCASE > 123 + (4567*0)) && (!(1-2147483647 % 3 != MiXeDcAsE)) ) ||   (1<999 / 888) */
		ExprNode* orOp = notOpChildren[0];
		Assert::AreEqual(std::string("||"), orOp->getValue());
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == orOp->getExprNodeValueType());
		std::vector<ExprNode*> orOpChildren = orOp->getChildren();
		Assert::AreEqual(size_t(2), orOpChildren.size());

		/* ((ALLUPPERCASE > 123 + (4567*0)) && (!(1-2147483647 % 3 != MiXeDcAsE)) ) */
		ExprNode* andOp = orOpChildren[0];
		Assert::AreEqual(std::string("&&"), andOp->getValue());
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == andOp->getExprNodeValueType());
		std::vector<ExprNode*> andOpChildren = andOp->getChildren();
		Assert::AreEqual(size_t(2), andOpChildren.size());

		/* (ALLUPPERCASE > 123 + (4567*0))*/
		ExprNode* gtOp = andOpChildren[0];
		Assert::AreEqual(std::string(">"), gtOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == gtOp->getExprNodeValueType());
		std::vector<ExprNode*> gtOpChildren = gtOp->getChildren();
		Assert::AreEqual(size_t(2), gtOpChildren.size());

		/* ALLUPPERCASE */
		Assert::AreEqual(std::string("ALLUPPERCASE"), gtOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == gtOpChildren[0]->getExprNodeValueType());

		/* 123 + (4567*0) */
		ExprNode* plusOp = gtOpChildren[1];
		Assert::AreEqual(std::string("+"), plusOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == plusOp->getExprNodeValueType());
		std::vector<ExprNode*> plusOpChildren = plusOp->getChildren();
		Assert::AreEqual(size_t(2), plusOpChildren.size());

		/* 123 */
		Assert::AreEqual(std::string("123"), plusOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == plusOpChildren[0]->getExprNodeValueType());

		/* (4567*0) */
		ExprNode* multOp = plusOpChildren[1];
		Assert::AreEqual(std::string("*"), multOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == multOp->getExprNodeValueType());
		std::vector<ExprNode*> multOpChildren = multOp->getChildren();
		Assert::AreEqual(size_t(2), multOpChildren.size());
		Assert::AreEqual(std::string("4567"), multOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == multOpChildren[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("0"), multOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == multOpChildren[1]->getExprNodeValueType());

		/* (!(1-2147483647 % 3 != MiXeDcAsE)) */
		ExprNode* notOp2 = andOpChildren[1];
		Assert::AreEqual(std::string("!"), notOp2->getValue());
		Assert::IsTrue(ExprNodeValueType::LOGICAL_OPERATOR == notOp2->getExprNodeValueType());
		std::vector<ExprNode*> notOp2Children = notOp2->getChildren();
		Assert::AreEqual(size_t(1), notOp2Children.size());

		/* (1-2147483647 % 3 != MiXeDcAsE) */
		ExprNode* notEqOp = notOp2Children[0];
		Assert::AreEqual(std::string("!="), notEqOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == notEqOp->getExprNodeValueType());
		std::vector<ExprNode*> notEqOpChildren = notEqOp->getChildren();
		Assert::AreEqual(size_t(2), notEqOpChildren.size());

		/* 1-2147483647 % 3 */
		ExprNode* minusOp = notEqOpChildren[0];
		Assert::AreEqual(std::string("-"), minusOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == minusOp->getExprNodeValueType());
		std::vector<ExprNode*> minusOpChildren = minusOp->getChildren();
		Assert::AreEqual(size_t(2), minusOpChildren.size());

		/* 1 */
		Assert::AreEqual(std::string("1"), minusOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == minusOpChildren[0]->getExprNodeValueType());

		/* 2147483647 % 3 */
		ExprNode* modOp = minusOpChildren[1];
		Assert::AreEqual(std::string("%"), modOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == modOp->getExprNodeValueType());
		std::vector<ExprNode*> modOpChildren = modOp->getChildren();
		Assert::AreEqual(size_t(2), modOpChildren.size());
		Assert::AreEqual(std::string("2147483647"), modOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == modOpChildren[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("3"), modOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == modOpChildren[1]->getExprNodeValueType());

		/* MiXeDcAsE */
		Assert::AreEqual(std::string("MiXeDcAsE"), notEqOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == notEqOpChildren[1]->getExprNodeValueType());

		/* (1<999 / 888) */
		ExprNode* ltOp = orOpChildren[1];
		Assert::AreEqual(std::string("<"), ltOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == ltOp->getExprNodeValueType());
		std::vector<ExprNode*> ltOpChildren = ltOp->getChildren();
		Assert::AreEqual(size_t(2), ltOpChildren.size());

		/* 1 */
		Assert::AreEqual(std::string("1"), ltOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == ltOpChildren[0]->getExprNodeValueType());

		/* 999 / 888 */
		ExprNode* divideOp = ltOpChildren[1];
		Assert::AreEqual(std::string("/"), divideOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == divideOp->getExprNodeValueType());
		std::vector<ExprNode*> divideOpChildren = divideOp->getChildren();
		Assert::AreEqual(size_t(2), divideOpChildren.size());
		Assert::AreEqual(std::string("999"), divideOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == divideOpChildren[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("888"), divideOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == divideOpChildren[1]->getExprNodeValueType());

		/* Test stmtLst in while container */
		std::vector<StmtNode*> stmtsInWhile = stmtLstNodeWhile->getStmtNodes();
		Assert::AreEqual(size_t(1), stmtsInWhile.size());

		/* read fl123ag ; */
		ReadNode* readNode = (ReadNode*)stmtsInWhile[0];
		Assert::IsTrue(StatementType::READ_TYPE == readNode->getStmtType());
		Assert::AreEqual(std::string("fl123ag"), *readNode->getModifiesVars().begin());
	}

	TEST_METHOD(parse_matchWhile_nestedWhile_success) {
		const char* source = "   procedure procedure123name \n "
			"{ while ( x==1	 )\n { "
			" read r;"
			"   while (y == 2) {\t"
			"		print print;}"
			" }  \n"
			" read fl123ag 	;  "
			"} \n ";

		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(1), procNodes.size());
		Assert::AreEqual(std::string("procedure123name"), procNodes[0]->getProcName());

		/* Test statements */
		StmtLstNode* stmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> statements = stmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(2), statements.size());

		/* Test while stmt */
		WhileNode* outerWhileNode = (WhileNode*)statements[0];
		Assert::IsTrue(StatementType::WHILE_TYPE == outerWhileNode->getStmtType());
		StmtLstNode* outerWhileStmtLstNode = outerWhileNode->getChildStmtLst()[0];

		/* Test stmtLst in OUTER while container */
		std::vector<StmtNode*> stmtsInOuterWhile = outerWhileStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(2), stmtsInOuterWhile.size());

		ReadNode* readNode1 = (ReadNode*)stmtsInOuterWhile[0];
		Assert::IsTrue(StatementType::READ_TYPE == readNode1->getStmtType());
		Assert::AreEqual(std::string("r"), *readNode1->getModifiesVars().begin());

		WhileNode* innerWhileNode = (WhileNode*)stmtsInOuterWhile[1];
		Assert::IsTrue(StatementType::WHILE_TYPE == innerWhileNode->getStmtType());
		StmtLstNode* innerWhileStmtLstNode = innerWhileNode->getChildStmtLst()[0];

		/* Test stmtLst in INNER while container */
		std::vector<StmtNode*> stmtsInInnerWhile = innerWhileStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), stmtsInInnerWhile.size());

		PrintNode* printNode = (PrintNode*)stmtsInInnerWhile[0];
		Assert::IsTrue(StatementType::PRINT_TYPE == printNode->getStmtType());
		Assert::AreEqual(std::string("print"), *printNode->getUsesVars().begin());

		/* read fl123ag ; */
		ReadNode* readNode2 = (ReadNode*)statements[1];
		Assert::IsTrue(StatementType::READ_TYPE == readNode2->getStmtType());
		Assert::AreEqual(std::string("fl123ag"), *readNode2->getModifiesVars().begin());
	}

	TEST_METHOD(parse_matchWhile_containsIfStmt_success) {
		const char* source = "   procedure procedure123name \n "
			"{ while ( x==1	 )\n { "
			" read r;"
			"   if (y == 2) then {\t"
			"		print print;}"
			" else { print pretty	;}"
			" }  \n"
			" read fl123ag 	;  "
			"} \n ";

		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(1), procNodes.size());
		Assert::AreEqual(std::string("procedure123name"), procNodes[0]->getProcName());

		/* Test statements */
		StmtLstNode* stmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> statements = stmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(2), statements.size());

		/* Test while stmt */
		WhileNode* whileNode = (WhileNode*)statements[0];
		Assert::IsTrue(StatementType::WHILE_TYPE == whileNode->getStmtType());
		StmtLstNode* whileStmtLstNode = whileNode->getChildStmtLst()[0];

		/* Test stmtLst in while container */
		std::vector<StmtNode*> stmtsInWhile = whileStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(2), stmtsInWhile.size());

		ReadNode* readNode1 = (ReadNode*)stmtsInWhile[0];
		Assert::IsTrue(StatementType::READ_TYPE == readNode1->getStmtType());
		Assert::AreEqual(std::string("r"), *readNode1->getModifiesVars().begin());

		IfNode* ifNode = (IfNode*)stmtsInWhile[1];
		Assert::IsTrue(StatementType::IF_TYPE == ifNode->getStmtType());
		std::vector<StmtLstNode*> ifStmtLstNode = ifNode->getChildStmtLst();
		Assert::AreEqual(size_t(2), ifStmtLstNode.size());

		/* Test then clause */
		/* print print; */
		StmtLstNode* thenStmtLstNode = ifStmtLstNode[0];
		std::vector<StmtNode*> thenStmts = thenStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), thenStmts.size());

		PrintNode* printNode1 = (PrintNode*)thenStmts[0];
		Assert::IsTrue(StatementType::PRINT_TYPE == printNode1->getStmtType());
		Assert::AreEqual(std::string("print"), *printNode1->getUsesVars().begin());

		/* Test else clause */
		/*  print pretty	; */
		StmtLstNode* elseStmtLstNode = ifStmtLstNode[1];
		std::vector<StmtNode*> elseStmts = elseStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), elseStmts.size());

		PrintNode* printNode2 = (PrintNode*)elseStmts[0];
		Assert::IsTrue(StatementType::PRINT_TYPE == printNode2->getStmtType());
		Assert::AreEqual(std::string("pretty"), *printNode2->getUsesVars().begin());

		/* read fl123ag ; */
		ReadNode* readNode2 = (ReadNode*)statements[1];
		Assert::IsTrue(StatementType::READ_TYPE == readNode2->getStmtType());
		Assert::AreEqual(std::string("fl123ag"), *readNode2->getModifiesVars().begin());
	}

	TEST_METHOD(parse_matchWhile_condExprMissingLeftBracket_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  y + 2 > 5) "
			"{ flag    = 123;   count =    someVar123	; "
			" read flag ; print COUNT	; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_LEFT_BRACKET.c_str(), ex.what());
		}

		const char* source1 = "   procedure procedure123name \n "
			"{ while  !(y + 2 > 5)) "
			"{ flag    = 123;   count =    someVar123	; "
			" read flag ; print COUNT	; } "
			"} \n ";

		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_LEFT_BRACKET.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_condExprMissingRightBracket_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  (y + 2 > 5 "
			"{ flag    = 123;   count =    someVar123	; "
			" read flag ; print COUNT	; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_RIGHT_BRACKET.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_stmtLstMissingLeftCurly_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  (y + 2 > 5 )"
			" flag    = 123;   count =    someVar123	; "
			" read flag ; print COUNT	; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_LEFT_CURLY.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_stmtLstMissingRightCurly_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  (y + 2 > 5 )"
			"{ flag    = 123;   count =    someVar123	; "
			" read flag ; print COUNT	;  "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			// The last right curly (for procedure) is taken as the while's right curly
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_missingStmtLst_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  (y + 2 > 5 )"
			"{  } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_invalidCond_invalidRelOp_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  (y + 2 & 5 )"
			"{ print x; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_REL_EXPR.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_invalidCond_invalidRelFactor_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  (y + 2 > ! )"
			"{ print x; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_REL_EXPR.c_str(), ex.what());
		}

		const char* source1 = "   procedure procedure123name \n "
			"{ while  (y + ! > 5 )"
			"{ print x; } "
			"} \n ";

		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_REL_EXPR.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_invalidCond_missingLeftBracketAfterNotOp_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  (!y + 2 > 5) )"
			"{ print x; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_LEFT_BRACKET.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_invalidCond_missingCondExprAfterNotOp_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  (!( ) )"
			"{ print x; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_REL_EXPR.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_invalidCond_invalidCondExprAfterNotOp_parserExceptionThrown) {
		const char* source1 = "   procedure procedure123name \n "
			"{ while  (!( invalidCond ) )"
			"{ print x; } "
			"} \n ";

		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_REL_EXPR.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_invalidCond_missingRightBracketAfterNotOpCondExpr_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  (!( x==1 )"
			"{ print x; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_RIGHT_BRACKET.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_invalidCond_invalidFirstCondExpr_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  (( x+1 ) && (y==0))"
			"{ print x; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_REL_EXPR.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_invalidCond_missingFirstCondExpr_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  ((  ) && (y==0))"
			"{ print x; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_REL_EXPR.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_invalidCond_firstCondExprMissingRightBracket_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  ((x==1   && (y==0))"
			"{ print x; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_RIGHT_BRACKET.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_invalidCond_missingLogOp_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  ((x==1)    (y==0))"
			"{ print x; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_COND_EXPR.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_invalidCond_invalidLogOp_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  ((x==1)  $  (y==0))"
			"{ print x; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_COND_EXPR.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_invalidCond_secondCondExprMissingLeftBracket_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  ((x==1)   || y==0))"
			"{ print x; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_LEFT_BRACKET.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_invalidCond_invalidSecondCondExpr_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  (( x==1 ) && (y*0))"
			"{ print x; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_REL_EXPR.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_invalidCond_missingSecondCondExpr_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  (( y==0 ) && ())"
			"{ print x; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_REL_EXPR.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchWhile_invalidCond_secondCondExprMissingRightBracket_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ while  ((x==1 )  && (y==0	)"
			"{ print x; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_RIGHT_BRACKET.c_str(), ex.what());
		}
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
		Assert::IsTrue(StatementType::IF_TYPE == ifNode->getStmtType());
		std::vector<StmtLstNode*> childStmtLst = ifNode->getChildStmtLst();
		Assert::AreEqual(size_t(2), childStmtLst.size());

		/* Test cond expr */
		/* ( y == 5 * 2)*/
		ExprNode* eqOp = ifNode->getCondExpr();
		Assert::AreEqual(std::string("=="), eqOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == eqOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenEqOp = eqOp->getChildren();
		Assert::AreEqual(size_t(2), childrenEqOp.size());

		/* y */
		Assert::AreEqual(std::string("y"), childrenEqOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == childrenEqOp[0]->getExprNodeValueType());

		/* 5 * 2*/
		ExprNode* multOp = childrenEqOp[1];
		Assert::AreEqual(std::string("*"), multOp->getValue());
		Assert::IsTrue(ExprNodeValueType::ARITHMETIC_OPERATOR == multOp->getExprNodeValueType());
		std::vector<ExprNode*> childrenMultOp = multOp->getChildren();
		Assert::AreEqual(size_t(2), childrenMultOp.size());
		Assert::AreEqual(std::string("5"), childrenMultOp[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == childrenMultOp[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("2"), childrenMultOp[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == childrenMultOp[1]->getExprNodeValueType());

		/* Test then clause */
		/* read fl123ag
		 cenX = 99*/
		StmtLstNode* stmtLstNodeThen = childStmtLst[0];
		std::vector<StmtNode*> stmtsThen = stmtLstNodeThen->getStmtNodes();
		Assert::AreEqual(size_t(2), stmtsThen.size());

		ReadNode* readNode = (ReadNode*)stmtsThen[0];
		Assert::AreEqual(std::string("fl123ag"), *readNode->getModifiesVars().begin());

		AssignNode* assignNode = (AssignNode*)stmtsThen[1];
		Assert::AreEqual(std::string("cenX"), *assignNode->getModifiesVars().begin());

		ExprNode* expr = assignNode->getExpr();
		Assert::AreEqual(std::string("99"), expr->getValue());
		Assert::IsTrue(expr->getChildren().empty());

		/* Test else clause */
		/* print COUNT*/
		StmtLstNode* stmtLstNodeElse = childStmtLst[1];
		std::vector<StmtNode*> stmtsElse = stmtLstNodeElse->getStmtNodes();
		Assert::AreEqual(size_t(1), stmtsElse.size());

		PrintNode* printNode = (PrintNode*)stmtsElse[0];
		Assert::AreEqual(std::string("COUNT"), *printNode->getUsesVars().begin());
	}

	TEST_METHOD(parse_matchIf_nestedIf_success) {
		const char* source = "   procedure procedure123name \n "
			"{ if ( x==1	 ) then\n { "
			"   if (y != 2) then {\t"
			"		print print;}else{ print p;}"
			" }else{  \n"
			" read fl123ag 	; } "
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

		/* Test OUTER if-else-then clause */
		IfNode* outerIfNode = (IfNode*)statements[0];
		Assert::IsTrue(StatementType::IF_TYPE == outerIfNode->getStmtType());
		std::vector<StmtLstNode*> outerChildStmtLst = outerIfNode->getChildStmtLst();
		Assert::AreEqual(size_t(2), outerChildStmtLst.size());

		/* Test OUTER cond expr */
		/* ( x==1	 ) */
		ExprNode* eqOp = outerIfNode->getCondExpr();
		Assert::AreEqual(std::string("=="), eqOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == eqOp->getExprNodeValueType());
		std::vector<ExprNode*> eqOpChildren = eqOp->getChildren();
		Assert::AreEqual(size_t(2), eqOpChildren.size());
		Assert::AreEqual(std::string("x"), eqOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == eqOpChildren[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("1"), eqOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == eqOpChildren[1]->getExprNodeValueType());

		/* Test OUTER then clause */
		/*  if (y != 2) then {\t"
				print print;}else{ print p;} */
		StmtLstNode* outerThenStmtLstNode = outerChildStmtLst[0];
		std::vector<StmtNode*> outerThenStmts = outerThenStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), outerThenStmts.size());

		/* Test INNER if-else-then clause */
		/* if (y != 2) then {\t
			print print;}else{ print p;} */
		IfNode* innerIfNode = (IfNode*)outerThenStmts[0];
		Assert::IsTrue(StatementType::IF_TYPE == innerIfNode->getStmtType());
		std::vector<StmtLstNode*> innerChildStmtLst = innerIfNode->getChildStmtLst();
		Assert::AreEqual(size_t(2), innerChildStmtLst.size());

		/* Test INNER cond expr */
		/* (y != 2) */
		ExprNode* neqOp = innerIfNode->getCondExpr();
		Assert::AreEqual(std::string("!="), neqOp->getValue());
		Assert::IsTrue(ExprNodeValueType::REL_OPERATOR == neqOp->getExprNodeValueType());
		std::vector<ExprNode*> neqOpChildren = neqOp->getChildren();
		Assert::AreEqual(size_t(2), neqOpChildren.size());
		Assert::AreEqual(std::string("y"), neqOpChildren[0]->getValue());
		Assert::IsTrue(ExprNodeValueType::VAR_NAME == neqOpChildren[0]->getExprNodeValueType());
		Assert::AreEqual(std::string("2"), neqOpChildren[1]->getValue());
		Assert::IsTrue(ExprNodeValueType::CONST_VALUE == neqOpChildren[1]->getExprNodeValueType());

		/* Test INNER then clause */
		/* print print; */
		StmtLstNode* innerThenStmtLstNode = innerChildStmtLst[0];
		std::vector<StmtNode*> innerThenStmts = innerThenStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), innerThenStmts.size());
		PrintNode* thenPrintNode = (PrintNode*)innerThenStmts[0];
		Assert::IsTrue(StatementType::PRINT_TYPE == thenPrintNode->getStmtType());
		Assert::AreEqual(std::string("print"), *thenPrintNode->getUsesVars().begin());

		/* Test INNER else clause */
		/* print p; */
		StmtLstNode* innerElseStmtLstNode = innerChildStmtLst[1];
		std::vector<StmtNode*> innerElseStmts = innerElseStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), innerElseStmts.size());

		PrintNode* elsePrintNode = (PrintNode*)innerElseStmts[0];
		Assert::IsTrue(StatementType::PRINT_TYPE == elsePrintNode->getStmtType());
		Assert::AreEqual(std::string("p"), *elsePrintNode->getUsesVars().begin());

		/* Test OUTER else clause */
		/* read fl123ag 	; */
		StmtLstNode* outerElseStmtLstNode = outerChildStmtLst[1];
		std::vector<StmtNode*> outerElseStmts = outerElseStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), outerElseStmts.size());
		ReadNode* readNode = (ReadNode*)outerElseStmts[0];
		Assert::IsTrue(StatementType::READ_TYPE == readNode->getStmtType());
		Assert::AreEqual(std::string("fl123ag"), *readNode->getModifiesVars().begin());
	}

	TEST_METHOD(parse_matchIf_containsWhileStmt_success) {
		const char* source = "   procedure procedure123name \n "
			"{   if (y == 2) then {\t"
			"		print print;}"
			" else {"
			" while ( x==1	 )\n { "
			" read r; }"
			" read fl123ag	;}"
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

		IfNode* ifNode = (IfNode*)statements[0];
		Assert::IsTrue(StatementType::IF_TYPE == ifNode->getStmtType());
		std::vector<StmtLstNode*> ifChildStmtLst = ifNode->getChildStmtLst();
		Assert::AreEqual(size_t(2), ifChildStmtLst.size());

		/* Test then clause */
		/* print print; */
		StmtLstNode* thenStmtLstNode = ifChildStmtLst[0];
		std::vector<StmtNode*> thenStmts = thenStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), thenStmts.size());

		PrintNode* printNode = (PrintNode*)thenStmts[0];
		Assert::IsTrue(StatementType::PRINT_TYPE == printNode->getStmtType());
		Assert::AreEqual(std::string("print"), *printNode->getUsesVars().begin());

		/* Test else clause */
		/*  while ( x==1	 )\n {
			 read r; }"
			 read fl123ag	; */
		StmtLstNode* elseStmtLstNode = ifChildStmtLst[1];
		std::vector<StmtNode*> elseStmts = elseStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(2), elseStmts.size());

		/* Test while stmt */
		WhileNode* whileNode = (WhileNode*)elseStmts[0];
		Assert::IsTrue(StatementType::WHILE_TYPE == whileNode->getStmtType());
		StmtLstNode* whileStmtLstNode = whileNode->getChildStmtLst()[0];

		/* Test stmtLst in while container */
		std::vector<StmtNode*> stmtsInWhile = whileStmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), stmtsInWhile.size());

		ReadNode* readNode1 = (ReadNode*)stmtsInWhile[0];
		Assert::IsTrue(StatementType::READ_TYPE == readNode1->getStmtType());
		Assert::AreEqual(std::string("r"), *readNode1->getModifiesVars().begin());

		/* read fl123ag ; */
		ReadNode* readNode2 = (ReadNode*)elseStmts[1];
		Assert::IsTrue(StatementType::READ_TYPE == readNode2->getStmtType());
		Assert::AreEqual(std::string("fl123ag"), *readNode2->getModifiesVars().begin());
	}
	TEST_METHOD(parse_matchIf_invalidCond_condExprMissingLeftBracket_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ if  y == 5 * 2)  then "
			"{ read fl123ag \n	; cenX = 99 ; } "
			" else { print COUNT; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_LEFT_BRACKET.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchIf_invalidCond_condExprMissingRightBracket_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2  then "
			"{ read fl123ag \n	; cenX = 99 ; } "
			" else { print COUNT; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_RIGHT_BRACKET.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchIf_missingThenKeyword_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2)   "
			"{ read fl123ag \n	; cenX = 99 ; } "
			" else { print COUNT; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_THEN.c_str(), ex.what());
		}

		const char* source1 = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2)   "
			"} \n ";

		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_THEN.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchIf_invalidThenKeyword_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2) tHeN  "
			"{ read fl123ag \n	; cenX = 99 ; } "
			" else { print COUNT; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_THEN.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchIf_thenStmtLstMissingLeftCurly_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2) then  "
			" read fl123ag \n	; cenX = 99 ; } "
			" else { print COUNT; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_LEFT_CURLY.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchIf_missingThenStmtLst_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2) then  "
			" {  } "
			" else { print COUNT; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}

		const char* source1 = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2) then  "
			" else { print COUNT; } "
			"} \n ";

		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_LEFT_CURLY.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchIf_invalidThenStmtLst_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2) then  "
			" { x = x & 1; } "
			" else { print COUNT; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			/* Since '&' is not a term or expr operator, "x = x" is regarded as a valid stmt.
				Thus, the parser is expecting a semicolon next. */
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}

		const char* source1 = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2) then  "
			" { wh1le  123; } "
			" else { print COUNT; } "
			"} \n ";

		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchIf_thenStmtLstMissingRightCurly_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2) then  "
			"{	"
			" read fl123ag \n	; cenX = 99 ;  "
			" else { print COUNT; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			/* Since the parser hasn't encountered a right curly,
				it would assume that "else ..." is the next stmt. */
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchIf_missingElseKeyword_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2) then  "
			"{	"
			" read fl123ag \n	; cenX = 99 ;  }"
			"  { print COUNT; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_ELSE.c_str(), ex.what());
		}

		const char* source1 = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2) then  "
			"{	"
			" read fl123ag \n	; cenX = 99 ;  }"
			"} \n ";

		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_ELSE.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchIf_invalidElseKeyword_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2) then  "
			"{ read fl123ag \n	; cenX = 99 ; } "
			" ELSE { print COUNT; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_ELSE.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchIf_elseStmtLstMissingLeftCurly_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2) then  "
			"{ read fl123ag \n	; cenX = 99 ; } "
			" else  print COUNT; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_LEFT_CURLY.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchIf_missingElseStmtLst_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2) then  "
			" { print COUNT; } "
			" else {  } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}

		const char* source1 = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2) then { print COUNT; } "
			" else  "
			"} \n ";

		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_LEFT_CURLY.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchIf_invalidElseStmtLst_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2) then  "
			" { print COUNT; } "
			" else {  x = x & 1; } "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			/* Since '&' is not a term or expr operator, "x = x" is regarded as a valid stmt.
				Thus, the parser is expecting a semicolon next. */
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}

		const char* source1 = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2) then  "
			" {  print COUNT;} "
			" else {  wh1le  123;} "
			"} \n ";

		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchIf_elseStmtLstMissingRightCurly_parserExceptionThrown) {
		const char* source = "   procedure procedure123name \n "
			"{ if  (y == 5 * 2) then  "
			"{	"
			" read fl123ag \n	; cenX = 99 ;  "
			" else { print COUNT;  "
			"} \n ";

		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			/* The if stmt is parsed successfully,
				but since the parser has not found a right curly to close off the procedure,
				it would still be expecting a stmt. */
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}

	/* Note that parser does not do semantic check for call stmt.
	This means that calling a semantically invalid procedure name will still be parsed successfully. */
	TEST_METHOD(parse_matchCall_syntacticallyValid_semanticallyValid_success) {
		const char* source =
			" procedure proc1 {call proc2;}\n"
			"procedure proc2 {read x;}";

		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(2), procNodes.size());

		/* 1st procedure */
		/* procedure proc1 {call proc2;} */
		Assert::AreEqual(std::string("proc1"), procNodes[0]->getProcName());
		StmtLstNode* proc1StmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> proc1Stmts = proc1StmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), proc1Stmts.size());
		StmtNode* proc1CallNode = proc1Stmts[0];
		Assert::IsTrue(StatementType::CALL_TYPE == proc1CallNode->getStmtType());
		Assert::AreEqual(std::string("proc2"), proc1CallNode->getProcCalled());

		/* 2nd procedure */
		/* procedure proc2 {read x;} */
		Assert::AreEqual(std::string("proc2"), procNodes[1]->getProcName());
		StmtLstNode* proc2StmtLstNode = procNodes[1]->getStmtLstNode();
		std::vector<StmtNode*> proc2Stmts = proc2StmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), proc2Stmts.size());
		ReadNode* readNode = (ReadNode*)proc2Stmts[0];
		Assert::IsTrue(StatementType::READ_TYPE == readNode->getStmtType());
		Assert::AreEqual(std::string("x"), *readNode->getModifiesVars().begin());
	}

	TEST_METHOD(parse_matchCall_syntacticallyValid_semanticallyInvalid_nonExistentProc_success) {
		const char* source = "procedure proc1 {call proc2;}";
		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(1), procNodes.size());

		/* procedure proc1 {call proc2;} */
		Assert::AreEqual(std::string("proc1"), procNodes[0]->getProcName());
		StmtLstNode* proc1StmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> proc1Stmts = proc1StmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), proc1Stmts.size());
		StmtNode* proc1CallNode = proc1Stmts[0];
		Assert::IsTrue(StatementType::CALL_TYPE == proc1CallNode->getStmtType());
		Assert::AreEqual(std::string("proc2"), proc1CallNode->getProcCalled());
	}

	TEST_METHOD(parse_matchCall_syntacticallyValid_semanticallyInvalid_selfCall_success) {
		const char* source = "procedure proc1 {call proc1;}";
		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(1), procNodes.size());

		/* procedure proc1 {call proc1;} */
		Assert::AreEqual(std::string("proc1"), procNodes[0]->getProcName());
		StmtLstNode* proc1StmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> proc1Stmts = proc1StmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), proc1Stmts.size());
		StmtNode* proc1CallNode = proc1Stmts[0];
		Assert::IsTrue(StatementType::CALL_TYPE == proc1CallNode->getStmtType());
		Assert::AreEqual(std::string("proc1"), proc1CallNode->getProcCalled());
	}

	TEST_METHOD(parse_matchCall_syntacticallyValid_semanticallyInvalid_cyclicCall_success) {
		const char* source =
			" procedure proc1 {call proc2;}\n"
			"procedure proc2 {call proc1;}";
		SourceAST ast = Parser::parse(source);
		std::vector<ProcedureNode*> procNodes = ast.getRoot()->getProcedureNodes();

		/* Test procedureNodes */
		Assert::AreEqual(size_t(2), procNodes.size());

		/* 1st procedure */
		/* procedure proc1 {call proc2;} */
		Assert::AreEqual(std::string("proc1"), procNodes[0]->getProcName());
		StmtLstNode* proc1StmtLstNode = procNodes[0]->getStmtLstNode();
		std::vector<StmtNode*> proc1Stmts = proc1StmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), proc1Stmts.size());
		StmtNode* proc1CallNode = proc1Stmts[0];
		Assert::IsTrue(StatementType::CALL_TYPE == proc1CallNode->getStmtType());
		Assert::AreEqual(std::string("proc2"), proc1CallNode->getProcCalled());

		/* 2nd procedure */
		/* procedure proc2 {call proc1;} */
		Assert::AreEqual(std::string("proc2"), procNodes[1]->getProcName());
		StmtLstNode* proc2StmtLstNode = procNodes[1]->getStmtLstNode();
		std::vector<StmtNode*> proc2Stmts = proc2StmtLstNode->getStmtNodes();
		Assert::AreEqual(size_t(1), proc2Stmts.size());
		StmtNode* callNode = proc2Stmts[0];
		Assert::IsTrue(StatementType::CALL_TYPE == callNode->getStmtType());
		Assert::AreEqual(std::string("proc1"), callNode->getProcCalled());
	}

	/* Note that parser does not do semantic check for call stmt.
	This means that exception will only be raised when there is a syntax error. */
	TEST_METHOD(parse_matchCall_missingProcName_parserExceptionThrown) {
		const char* source = "   procedure proc1  "
			"{ print x1;print y1 ; call } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_PROC_NAME_IN_CALL_STMT.c_str(), ex.what());
		}

		const char* source1 = "   procedure proc1  "
			"{ print x1;print y1 ; call		; print V; }";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_PROC_NAME_IN_CALL_STMT.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchCall_invalidProcName_parserExceptionThrown) {
		const char* source = "   procedure proc  "
			"{ print x1; call 1invalidName ; } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_PROC_NAME_IN_CALL_STMT.c_str(), ex.what());
		}

		const char* source1 = "   procedure proc1  "
			"{ print x1;print y1 ; call invalid_name; }";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchCall_missingSemicolon_parserExceptionThrown) {
		const char* source = "   procedure proc  "
			"{ print x1; call vAliD123nAmE call vAliD123nAmEaGAIN ;}  ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}

		const char* source1 = "   procedure proc  "
			"{ print x1; call vAliD123nAmE ; call vAliD123nAmEaGAIN  } ";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}
	}

	TEST_METHOD(parse_matchCall_tooManySemicolon_parserExceptionThrown) {
		const char* source = "   procedure proc  "
			"{ print x1; call vAliD123nAmE ; ; } ";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser::parse(source);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}

		const char* source1 = "   procedure proc  "
			"{ print x1; call vAliD123nAmE ;; read x1 ; }";
		auto wrapperFunc1 = [&source1] { Parser::parse(source1); };
		Assert::ExpectException<ParserException>(wrapperFunc1);
		try {
			Parser::parse(source1);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::INVALID_STMT.c_str(), ex.what());
		}
	}
	};
}
