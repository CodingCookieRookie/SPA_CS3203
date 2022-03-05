#include "CppUnitTest.h"
#include "stdafx.h"

#include "SP/SourceASTNode.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestSourceASTNode) {
public:

	TEST_METHOD(assignNode_getUsesModifiesConsts_oneUsesVar_oneModifiesVar_oneConst_success) {
		/* count = countA + 1
					+
			countA		1
		*/
		ExprNode* root = new ExprNode(ExprNodeValueType::arithmeticOperator, "+");
		root->addChild(new ExprNode(ExprNodeValueType::varName, "countA"));
		root->addChild(new ExprNode(ExprNodeValueType::constValue, "1"));
		AssignNode* assignNode = new AssignNode("count", root);

		/* getUsesVars */
		std::unordered_set<std::string> usesVars = assignNode->getUsesVars();
		Assert::AreEqual(size_t(1), usesVars.size());
		Assert::IsTrue(usesVars.find("countA") != usesVars.end());

		/* getModifiesVars */
		std::unordered_set<std::string> modifiesVars = assignNode->getModifiesVars();
		Assert::AreEqual(size_t(1), modifiesVars.size());
		Assert::IsTrue(modifiesVars.find("count") != modifiesVars.end());

		/* getConsts */
		std::unordered_set<std::string> consts = assignNode->getConsts();
		Assert::AreEqual(size_t(1), consts.size());
		Assert::IsTrue(consts.find("1") != consts.end());
	}

	TEST_METHOD(assignNode_getUsesModifiesConsts_multipleUsesVars_oneModifiesVar_multipleConsts_success) {
		/* z = (z + a123 * 2) % ((100 - b0b) / 3)
							  %
					+					/
				z		*			-		3
					a123	2	100   b0b
		*/
		ExprNode* root = new ExprNode(ExprNodeValueType::arithmeticOperator, "%");
		ExprNode* plus = new ExprNode(ExprNodeValueType::arithmeticOperator, "+");
		ExprNode* divide = new ExprNode(ExprNodeValueType::arithmeticOperator, "/");
		root->addChild(plus);
		root->addChild(divide);

		/* left subtree */
		ExprNode* z = new ExprNode(ExprNodeValueType::varName, "z");
		ExprNode* multiply = new ExprNode(ExprNodeValueType::arithmeticOperator, "*");
		plus->addChild(z);
		plus->addChild(multiply);
		multiply->addChild(new ExprNode(ExprNodeValueType::varName, "a123"));
		multiply->addChild(new ExprNode(ExprNodeValueType::constValue, "2"));

		/* right subtree */
		ExprNode* minus = new ExprNode(ExprNodeValueType::arithmeticOperator, "-");
		ExprNode* three = new ExprNode(ExprNodeValueType::constValue, "3");
		divide->addChild(minus);
		divide->addChild(three);
		minus->addChild(new ExprNode(ExprNodeValueType::constValue, "100"));
		minus->addChild(new ExprNode(ExprNodeValueType::varName, "b0b"));

		AssignNode* assignNode = new AssignNode("z", root);

		/* getUsesVars */
		std::unordered_set<std::string> usesVars = assignNode->getUsesVars();
		Assert::AreEqual(size_t(3), usesVars.size());
		Assert::IsTrue(usesVars.find("z") != usesVars.end());
		Assert::IsTrue(usesVars.find("a123") != usesVars.end());
		Assert::IsTrue(usesVars.find("b0b") != usesVars.end());

		/* getModifiesVars */
		std::unordered_set<std::string> modifiesVars = assignNode->getModifiesVars();
		Assert::AreEqual(size_t(1), modifiesVars.size());
		Assert::IsTrue(modifiesVars.find("z") != modifiesVars.end());

		/* getConsts */
		std::unordered_set<std::string> consts = assignNode->getConsts();
		Assert::AreEqual(size_t(3), consts.size());
		Assert::IsTrue(consts.find("2") != consts.end());
		Assert::IsTrue(consts.find("100") != consts.end());
		Assert::IsTrue(consts.find("3") != consts.end());
	}

	TEST_METHOD(whileNode_getUsesVarsAndConsts_oneUsesVar_oneConst_success) {
		/*
			while (cenX != 0) {
				z = (z + a123 * 2) % ((100 - b0b) / 3);
				read readVar;
				print printVar;
			}
		*/

		// z = (z + a123 * 2) % ((100 - b0b) / 3);
		ExprNode* root = new ExprNode(ExprNodeValueType::arithmeticOperator, "%");
		ExprNode* plus = new ExprNode(ExprNodeValueType::arithmeticOperator, "+");
		ExprNode* divide = new ExprNode(ExprNodeValueType::arithmeticOperator, "/");
		root->addChild(plus);
		root->addChild(divide);

		/* left subtree */
		ExprNode* z = new ExprNode(ExprNodeValueType::varName, "z");
		ExprNode* multiply = new ExprNode(ExprNodeValueType::arithmeticOperator, "*");
		plus->addChild(z);
		plus->addChild(multiply);
		multiply->addChild(new ExprNode(ExprNodeValueType::varName, "a123"));
		multiply->addChild(new ExprNode(ExprNodeValueType::constValue, "2"));

		/* right subtree */
		ExprNode* minus = new ExprNode(ExprNodeValueType::arithmeticOperator, "-");
		ExprNode* three = new ExprNode(ExprNodeValueType::constValue, "3");
		divide->addChild(minus);
		divide->addChild(three);
		minus->addChild(new ExprNode(ExprNodeValueType::constValue, "100"));
		minus->addChild(new ExprNode(ExprNodeValueType::varName, "b0b"));

		AssignNode* assignNode = new AssignNode("z", root);

		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(assignNode);
		stmtLstNode->addStmtNode(new ReadNode("readVar"));
		stmtLstNode->addStmtNode(new PrintNode("printVar"));

		/* Cond expr */
		/* while (cenX != 0) */
		ExprNode* condExpr = new ExprNode(ExprNodeValueType::relOperator, "!=");
		condExpr->addChild(new ExprNode(ExprNodeValueType::varName, "cenX"));
		condExpr->addChild(new ExprNode(ExprNodeValueType::constValue, "0"));

		WhileNode* whileNode = new WhileNode(condExpr, stmtLstNode);

		/* getUsesVars */
		std::unordered_set<std::string> usesVars = whileNode->getUsesVars();
		Assert::AreEqual(size_t(1), usesVars.size());
		Assert::IsTrue(usesVars.find("cenX") != usesVars.end());

		/* getConsts */
		std::unordered_set<std::string> consts = whileNode->getConsts();
		Assert::AreEqual(size_t(1), consts.size());
		Assert::IsTrue(consts.find("0") != consts.end());
	}

	TEST_METHOD(whileNode_getUsesVarsAndConsts_multipleUsesVars_multipleConsts_success) {
		/*
			while ((cenX < 0) && (cenY + 15 >= 0)) {
				print printVar;
			}
		*/

		/* Cond expr */
		/* (cenX != 0) */
		ExprNode* ltOp = new ExprNode(ExprNodeValueType::relOperator, "<");
		ltOp->addChild(new ExprNode(ExprNodeValueType::varName, "cenX"));
		ltOp->addChild(new ExprNode(ExprNodeValueType::constValue, "0"));

		/* (cenY + 15 >= 0)*/
		ExprNode* plusOp = new ExprNode(ExprNodeValueType::arithmeticOperator, "+");
		plusOp->addChild(new ExprNode(ExprNodeValueType::varName, "cenY"));
		plusOp->addChild(new ExprNode(ExprNodeValueType::constValue, "15"));

		ExprNode* gteOp = new ExprNode(ExprNodeValueType::relOperator, ">=");
		gteOp->addChild(plusOp);
		gteOp->addChild(new ExprNode(ExprNodeValueType::constValue, "0"));

		/* while ((cenX < 0) && (cenY + 15 >= 0)) */
		ExprNode* andOp = new ExprNode(ExprNodeValueType::logicalOperator, "&&");
		andOp->addChild(ltOp);
		andOp->addChild(gteOp);

		/* StmtLstNode */
		/* print printVar; */
		StmtLstNode* stmtLstNode = new StmtLstNode();
		stmtLstNode->addStmtNode(new PrintNode("printVar"));

		WhileNode* whileNode = new WhileNode(andOp, stmtLstNode);

		/* getUsesVars */
		std::unordered_set<std::string> usesVars = whileNode->getUsesVars();
		Assert::AreEqual(size_t(2), usesVars.size());
		Assert::IsTrue(usesVars.find("cenX") != usesVars.end());
		Assert::IsTrue(usesVars.find("cenY") != usesVars.end());

		/* getConsts */
		std::unordered_set<std::string> consts = whileNode->getConsts();
		Assert::AreEqual(size_t(2), consts.size());
		Assert::IsTrue(consts.find("0") != consts.end());
		Assert::IsTrue(consts.find("15") != consts.end());
	}

	TEST_METHOD(whileNode_getUsesVars_oneRelExpr_twoOperands_success) {
		/* Note that the semantic check for (empty) stmtLstNode is not done in SourceASTNode.
		Thus, assigning an empty stmtLstNode to a whileNode's condExpr will not raise any error here. */

		/* No variable */
		/* while (1!=0) */
		ExprNode* neqOp = new ExprNode(ExprNodeValueType::relOperator, "!=");
		neqOp->addChild(new ExprNode(ExprNodeValueType::constValue, "1"));
		neqOp->addChild(new ExprNode(ExprNodeValueType::constValue, "0"));

		WhileNode* whileNode1 = new WhileNode(neqOp, new StmtLstNode());
		Assert::AreEqual(size_t(0), whileNode1->getUsesVars().size());

		/* One variable */
		/* while (i==0) */
		ExprNode* eqOp = new ExprNode(ExprNodeValueType::relOperator, "==");
		eqOp->addChild(new ExprNode(ExprNodeValueType::varName, "i"));
		eqOp->addChild(new ExprNode(ExprNodeValueType::constValue, "0"));

		WhileNode* whileNode2 = new WhileNode(eqOp, new StmtLstNode());
		std::unordered_set<std::string> usesVars2 = whileNode2->getUsesVars();
		Assert::AreEqual(size_t(1), usesVars2.size());
		Assert::IsTrue(usesVars2.find("i") != usesVars2.end());

		/* >1 variables, but duplicate */
		/* while (i>i) */
		ExprNode* gtOp = new ExprNode(ExprNodeValueType::relOperator, ">");
		gtOp->addChild(new ExprNode(ExprNodeValueType::varName, "i"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::varName, "i"));

		WhileNode* whileNode3 = new WhileNode(gtOp, new StmtLstNode());
		std::unordered_set<std::string> usesVars3 = whileNode3->getUsesVars();
		Assert::AreEqual(size_t(1), usesVars3.size());
		Assert::IsTrue(usesVars3.find("i") != usesVars3.end());

		/* >1 variables */
		/* while (i<j) */
		ExprNode* ltOp = new ExprNode(ExprNodeValueType::relOperator, "<");
		ltOp->addChild(new ExprNode(ExprNodeValueType::varName, "i"));
		ltOp->addChild(new ExprNode(ExprNodeValueType::varName, "j"));

		WhileNode* whileNode4 = new WhileNode(ltOp, new StmtLstNode());
		std::unordered_set<std::string> usesVars4 = whileNode4->getUsesVars();
		Assert::AreEqual(size_t(2), usesVars4.size());
		Assert::IsTrue(usesVars4.find("i") != usesVars4.end());
		Assert::IsTrue(usesVars4.find("j") != usesVars4.end());
	}

	TEST_METHOD(containerNode_getUsesVars_oneRelExpr_success) {
		/* while ( y + 2 > 5 * z) */
		ExprNode* neqOp = new ExprNode(ExprNodeValueType::relOperator, ">");
		neqOp->addChild(new ExprNode(ExprNodeValueType::arithmeticOperator, "+"));
		neqOp->addChild(new ExprNode(ExprNodeValueType::arithmeticOperator, "*"));
		neqOp->getChildren()[0]->addChild(new ExprNode(ExprNodeValueType::varName, "y"));
		neqOp->getChildren()[0]->addChild(new ExprNode(ExprNodeValueType::constValue, "2"));
		neqOp->getChildren()[1]->addChild(new ExprNode(ExprNodeValueType::constValue, "5"));
		neqOp->getChildren()[1]->addChild(new ExprNode(ExprNodeValueType::varName, "z"));

		/* Note that the semantic check for (empty) stmtLstNode is not done in SourceASTNode.
		Thus, assigning an empty stmtLstNode to a whileNode's condExpr will not raise any error here. */
		WhileNode* whileNode = new WhileNode(neqOp, new StmtLstNode());
		std::unordered_set<std::string> usesVars = whileNode->getUsesVars();
		Assert::AreEqual(size_t(2), usesVars.size());
		Assert::IsTrue(usesVars.find("y") != usesVars.end());
		Assert::IsTrue(usesVars.find("z") != usesVars.end());
	}

	TEST_METHOD(containerNode_getUsesVars_oneRelExpr_veryNestedExpr_success) {
		/*  if ( y + 2 > (b0b - (c * 10 - ALL %( Z + (1*0)) )	)) */
		ExprNode* gtOp = new ExprNode(ExprNodeValueType::relOperator, ">");
		gtOp->addChild(new ExprNode(ExprNodeValueType::arithmeticOperator, "+"));
		gtOp->addChild(new ExprNode(ExprNodeValueType::arithmeticOperator, "-"));

		/* y + 2 */
		ExprNode* plusOp1 = gtOp->getChildren()[0];
		plusOp1->addChild(new ExprNode(ExprNodeValueType::varName, "y"));
		plusOp1->addChild(new ExprNode(ExprNodeValueType::constValue, "2"));

		/* (b0b - (c * 10 - ALL %( Z + (1*0)) )	) */
		ExprNode* minusOp1 = gtOp->getChildren()[1];
		minusOp1->addChild(new ExprNode(ExprNodeValueType::varName, "b0b"));
		minusOp1->addChild(new ExprNode(ExprNodeValueType::arithmeticOperator, "-"));

		/* (c * 10 - ALL %( Z + (1*0)) ) */
		ExprNode* minusOp2 = plusOp1->getChildren()[1];
		minusOp2->addChild(new ExprNode(ExprNodeValueType::arithmeticOperator, "*"));
		minusOp2->addChild(new ExprNode(ExprNodeValueType::arithmeticOperator, "%"));

		/* c * 10 */
		minusOp2->getChildren()[0]->addChild(new ExprNode(ExprNodeValueType::varName, "c"));
		minusOp2->getChildren()[0]->addChild(new ExprNode(ExprNodeValueType::constValue, "10"));

		/* ALL %( Z + (1*0)) */
		ExprNode* modOp = minusOp2->getChildren()[1];
		modOp->addChild(new ExprNode(ExprNodeValueType::varName, "ALL"));
		modOp->addChild(new ExprNode(ExprNodeValueType::arithmeticOperator, "+"));

		/* Z + (1*0) */
		ExprNode* plusOp2 = modOp->getChildren()[1];
		plusOp2->addChild(new ExprNode(ExprNodeValueType::varName, "Z"));
		plusOp2->addChild(new ExprNode(ExprNodeValueType::arithmeticOperator, "*"));

		/* 1*0 */
		plusOp2->getChildren()[1]->addChild(new ExprNode(ExprNodeValueType::constValue, "1"));
		plusOp2->getChildren()[1]->addChild(new ExprNode(ExprNodeValueType::constValue, "0"));

		/* Note that the semantic check for (empty) stmtLstNode is not done in SourceASTNode.
		Thus, assigning an empty stmtLstNode to an ifNode's condExpr will not raise any error here. */
		IfNode* ifNode = new IfNode(gtOp, new StmtLstNode(), new StmtLstNode());
		std::unordered_set<std::string> usesVars = ifNode->getUsesVars();
		Assert::AreEqual(size_t(5), usesVars.size());
		Assert::IsTrue(usesVars.find("y") != usesVars.end());
		Assert::IsTrue(usesVars.find("b0b") != usesVars.end());
		Assert::IsTrue(usesVars.find("c") != usesVars.end());
		Assert::IsTrue(usesVars.find("ALL") != usesVars.end());
		Assert::IsTrue(usesVars.find("Z") != usesVars.end());
	}

	TEST_METHOD(containerNode_getUsesVars_nestedNOTOp_success) {
		/* while (!(	!(x >= 0)	)	) */
		ExprNode* outerNotOp = new ExprNode(ExprNodeValueType::logicalOperator, "!");
		ExprNode* innerNotOp = new ExprNode(ExprNodeValueType::logicalOperator, "!");
		outerNotOp->addChild(innerNotOp);
		innerNotOp->addChild(new ExprNode(ExprNodeValueType::relOperator, ">="));
		innerNotOp->getChildren()[0]->addChild(new ExprNode(ExprNodeValueType::varName, "x"));
		innerNotOp->getChildren()[0]->addChild(new ExprNode(ExprNodeValueType::constValue, "0"));

		WhileNode* whileNode = new WhileNode(outerNotOp, new StmtLstNode());
		std::unordered_set<std::string> usesVars = whileNode->getUsesVars();
		Assert::AreEqual(size_t(1), usesVars.size());
		Assert::IsTrue(usesVars.find("x") != usesVars.end());
	}

	TEST_METHOD(containerNode_getUsesVars_OROpNestedInNOTOpCondExpr_success) {
		/* if ( !( (y * 1 == 5 - z) || (x != 0)  )    ) */
		ExprNode* notOp = new ExprNode(ExprNodeValueType::logicalOperator, "!");
		ExprNode* orOp = new ExprNode(ExprNodeValueType::logicalOperator, "||");
		notOp->addChild(orOp);
		orOp->addChild(new ExprNode(ExprNodeValueType::relOperator, "=="));
		orOp->addChild(new ExprNode(ExprNodeValueType::relOperator, "!="));

		/* (y * 1 == 5 - z) */
		ExprNode* eqOp = orOp->getChildren()[0];
		eqOp->addChild(new ExprNode(ExprNodeValueType::arithmeticOperator, "*"));
		eqOp->getChildren()[0]->addChild(new ExprNode(ExprNodeValueType::varName, "y"));
		eqOp->getChildren()[0]->addChild(new ExprNode(ExprNodeValueType::constValue, "1"));
		eqOp->addChild(new ExprNode(ExprNodeValueType::arithmeticOperator, "-"));
		eqOp->getChildren()[1]->addChild(new ExprNode(ExprNodeValueType::constValue, "5"));
		eqOp->getChildren()[1]->addChild(new ExprNode(ExprNodeValueType::varName, "z"));

		/* (x != 0) */
		ExprNode* neqOp = orOp->getChildren()[1];
		neqOp->addChild(new ExprNode(ExprNodeValueType::varName, "x"));
		neqOp->addChild(new ExprNode(ExprNodeValueType::constValue, "0"));

		/* Note that the semantic check for (empty) stmtLstNode is not done in SourceASTNode.
		Thus, assigning an empty stmtLstNode to an ifNode's condExpr will not raise any error here. */
		IfNode* ifNode = new IfNode(notOp, new StmtLstNode(), new StmtLstNode());
		std::unordered_set<std::string> usesVars = ifNode->getUsesVars();
		Assert::AreEqual(size_t(3), usesVars.size());
		Assert::IsTrue(usesVars.find("y") != usesVars.end());
		Assert::IsTrue(usesVars.find("z") != usesVars.end());
		Assert::IsTrue(usesVars.find("x") != usesVars.end());
	}

	TEST_METHOD(assignNode_getPattern_differentExpressions_success) {
		/* Test expression tree containing a single node */
		ExprNode* leafNode1 = new ExprNode(ExprNodeValueType::constValue, "1");
		AssignNode* assignNode = new AssignNode("y", leafNode1);
		Assert::AreEqual(std::string(" 1 "), assignNode->getPattern());

		ExprNode* leafNodeX = new ExprNode(ExprNodeValueType::varName, "x");
		assignNode = new AssignNode("hello", leafNodeX);
		Assert::AreEqual(std::string(" x "), assignNode->getPattern());

		/* Test expression trees containing only one operator */
		ExprNode* leafNode2 = new ExprNode(ExprNodeValueType::constValue, "2");
		ExprNode* leafNodeA = new ExprNode(ExprNodeValueType::varName, "asdf");
		ExprNode* plusNode = new ExprNode(ExprNodeValueType::arithmeticOperator, "+");
		plusNode->addChild(leafNode2);
		plusNode->addChild(leafNodeA);
		assignNode = new AssignNode("y", plusNode);
		Assert::AreEqual(std::string(" 2 asdf + "), assignNode->getPattern());

		ExprNode* leafNodeXyz = new ExprNode(ExprNodeValueType::varName, "xyz");
		ExprNode* leafNode123 = new ExprNode(ExprNodeValueType::constValue, "123");
		ExprNode* modNode = new ExprNode(ExprNodeValueType::arithmeticOperator, "%");
		modNode->addChild(leafNodeXyz);
		modNode->addChild(leafNode123);
		assignNode = new AssignNode("qwerty", modNode);
		Assert::AreEqual(std::string(" xyz 123 % "), assignNode->getPattern());

		/* Test expression trees that can have more than one operator */
		ExprNode* leafNodeAbc = new ExprNode(ExprNodeValueType::varName, "abc");
		ExprNode* leafNode12345 = new ExprNode(ExprNodeValueType::constValue, "12345");
		ExprNode* leafNode436474 = new ExprNode(ExprNodeValueType::constValue, "436474");
		ExprNode* multiplyNode = new ExprNode(ExprNodeValueType::arithmeticOperator, "*");
		multiplyNode->addChild(leafNodeAbc);
		multiplyNode->addChild(leafNode12345);
		ExprNode* divNode = new ExprNode(ExprNodeValueType::arithmeticOperator, "/");
		divNode->addChild(multiplyNode);
		divNode->addChild(leafNode436474);
		assignNode = new AssignNode("abc123", divNode);
		Assert::AreEqual(std::string(" abc 12345 * 436474 / "), assignNode->getPattern());

		ExprNode* leafNodeAlice = new ExprNode(ExprNodeValueType::varName, "alice");
		ExprNode* leafNodeBob = new ExprNode(ExprNodeValueType::varName, "bob");
		ExprNode* leafNodeCharlie = new ExprNode(ExprNodeValueType::varName, "charlie");
		ExprNode* plusNode2 = new ExprNode(ExprNodeValueType::arithmeticOperator, "+");
		plusNode2->addChild(leafNodeAlice);
		plusNode2->addChild(leafNodeBob);
		ExprNode* minusNode = new ExprNode(ExprNodeValueType::arithmeticOperator, "-");
		minusNode->addChild(leafNodeCharlie);
		minusNode->addChild(plusNode2);
		assignNode = new AssignNode("david", minusNode);
		Assert::AreEqual(std::string(" charlie alice bob + - "), assignNode->getPattern());
	}
	};
}
