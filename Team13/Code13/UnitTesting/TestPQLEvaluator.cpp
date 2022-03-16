#include "stdafx.h"
#include "CppUnitTest.h"

#include <algorithm>
#include <string>

#include "../source/PKB/Follows.h"
#include "../source/PKB/FollowsT.h"
#include "../source/PKB/Parent.h"
#include "../source/PKB/ParentT.h"
#include "../source/PKB/RS2.h"
#include "../source/QPS/PQLEvaluator.h"
#include "../source/QPS/PQLParser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPQLEvaluator) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Attribute::performCleanUp();
		Entity::performCleanUp();
		FollowsT::performCleanUp();
		Follows::performCleanUp();
		ParentT::performCleanUp();
		Parent::performCleanUp();
	}
public:

	TEST_METHOD(evaluateQuery_declarationAndSelectOnly_designEntitiesExtracted) {
		// "stmt s; if ifs; Select s";
		typedef std::pair<PqlEntityType, std::string> DECLARATION;

		DECLARATION pair1 = DECLARATION(PqlEntityType::Stmt, "s");
		DECLARATION pair2 = DECLARATION(PqlEntityType::If, "ifs");

		std::vector<PQL_VARIABLE> declarations;
		declarations.push_back(pair1);
		declarations.push_back(pair2);

		std::vector<PqlReference> columns;
		columns.emplace_back(PqlReferenceType::synonym, "s");

		std::vector<ParsedRelationship> relationships;
		std::vector<ParsedPattern> patterns;
		std::vector<ParsedWith> withs;
		ParsedQuery pq1 = ParsedQuery(declarations, columns, relationships, patterns, withs);

		Assert::AreEqual(size_t(2), pq1.getDeclarations().size());
		Assert::IsFalse(pq1.getColumns().empty());

		std::unordered_set<std::string> selects = pq1.getColumns();
		Assert::IsTrue(selects.find("s") != selects.end());
	}
	};
}
