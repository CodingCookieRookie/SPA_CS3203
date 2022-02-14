#include "CppUnitTest.h"
#include "stdafx.h"

#include "SP/SourceASTNode.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestSourceASTNode) {
public:

	TEST_METHOD(assignNode_getUsesModifies_oneUsesVar_oneUsesConst_success) {
		/* count = countA + 1
					+
			countA		1
		*/
		ExprNode* root = new ExprNode(ExprNodeValueType::operand, "+");
		root->addChild(new ExprNode(ExprNodeValueType::varName, "countA"));
		root->addChild(new ExprNode(ExprNodeValueType::constValue, "1"));
		AssignNode* assignNode = new AssignNode("count", root);

		/* getModifiesVars */
		std::unordered_set<std::string> modifiesVars = assignNode->getModifiesVars();
		Assert::AreEqual(size_t(1), modifiesVars.size());
		Assert::IsTrue(modifiesVars.find("count") != modifiesVars.end());

		/* getUsesVars */
		std::unordered_set<std::string> usesVars = assignNode->getUsesVars();
		Assert::AreEqual(size_t(1), usesVars.size());
		Assert::IsTrue(usesVars.find("countA") != usesVars.end());

		/* getUsesConsts */
		std::unordered_set<std::string> usesConsts = assignNode->getUsesConsts();
		Assert::AreEqual(size_t(1), usesConsts.size());
		Assert::IsTrue(usesConsts.find("1") != usesConsts.end());

		delete root;
	}

	TEST_METHOD(assignNode_getUsesModifies_multipleUsesVars_multipleUsesConsts_success) {
		/* z = (z + a123 * 2) % ((100 - b0b) / 3)
							  %
					+					/
				z		*			-		3
					a123	2	100   b0b
		*/
		ExprNode* root = new ExprNode(ExprNodeValueType::operand, "%");
		ExprNode* plus = new ExprNode(ExprNodeValueType::operand, "+");
		ExprNode* divide = new ExprNode(ExprNodeValueType::operand, "/");
		root->addChild(plus);
		root->addChild(divide);

		/* left subtree */
		ExprNode* z = new ExprNode(ExprNodeValueType::varName, "z");
		ExprNode* multiply = new ExprNode(ExprNodeValueType::operand, "*");
		plus->addChild(z);
		plus->addChild(multiply);
		multiply->addChild(new ExprNode(ExprNodeValueType::varName, "a123"));
		multiply->addChild(new ExprNode(ExprNodeValueType::constValue, "2"));

		/* right subtree */
		ExprNode* minus = new ExprNode(ExprNodeValueType::operand, "-");
		ExprNode* three = new ExprNode(ExprNodeValueType::constValue, "3");
		divide->addChild(minus);
		divide->addChild(three);
		minus->addChild(new ExprNode(ExprNodeValueType::constValue, "100"));
		minus->addChild(new ExprNode(ExprNodeValueType::varName, "b0b"));

		AssignNode* assignNode = new AssignNode("z", root);

		/* getModifiesVars */
		std::unordered_set<std::string> modifiesVars = assignNode->getModifiesVars();
		Assert::AreEqual(size_t(1), modifiesVars.size());
		Assert::IsTrue(modifiesVars.find("z") != modifiesVars.end());

		/* getUsesVars */
		std::unordered_set<std::string> usesVars = assignNode->getUsesVars();
		Assert::AreEqual(size_t(3), usesVars.size());
		Assert::IsTrue(usesVars.find("z") != usesVars.end());
		Assert::IsTrue(usesVars.find("a123") != usesVars.end());
		Assert::IsTrue(usesVars.find("b0b") != usesVars.end());

		/* getUsesConsts */
		std::unordered_set<std::string> usesConsts = assignNode->getUsesConsts();
		Assert::AreEqual(size_t(3), usesConsts.size());
		Assert::IsTrue(usesConsts.find("2") != usesConsts.end());
		Assert::IsTrue(usesConsts.find("100") != usesConsts.end());
		Assert::IsTrue(usesConsts.find("3") != usesConsts.end());

		delete root;
	}
	};
}
