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

		delete root;
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

		delete root;
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

		delete root;
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
