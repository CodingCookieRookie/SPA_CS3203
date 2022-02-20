#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include <algorithm>

#include "../source/QPS-NEW/PQLEvaluator.h"
#include "../source/QPS-NEW/PQLParser.h"
#include "../source/PKB/RS2.h"
#include "../source/PKB/Follows.h"
#include "../source/PKB/FollowsT.h"
#include "../source/PKB/Parent.h"
#include "../source/PKB/ParentT.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestPQLEvaluator)
	{
	private:
		TEST_METHOD_CLEANUP(cleanUpTables) {
			Entity::performCleanUp();
			FollowsT::performCleanUp();
			Follows::performCleanUp();
			ParentT::performCleanUp();
			Parent::performCleanUp();
		}
	public:

		TEST_METHOD(evaluateQuery_declarationAndSelectOnly_designEntitiesExtracted)
		{
			// "stmt s; if ifs; Select s";
			typedef std::pair<PqlEntityType, std::string> DECLARATION;

			DECLARATION pair1 = DECLARATION(PqlEntityType::Stmt, "s");
			DECLARATION pair2 = DECLARATION(PqlEntityType::If, "ifs");

			std::vector<PQL_VARIABLE> declarations;
			declarations.push_back(pair1);
			declarations.push_back(pair2);

			std::vector<std::string> columns;
			columns.push_back("s");

			std::vector<ParsedRelationship> relationships;
			std::vector<ParsedPattern> patterns;
			ParsedQuery pq1 = ParsedQuery(declarations, columns, relationships, patterns);

			PQLEvaluator pqlEvaluator(pq1);

			Assert::AreEqual(size_t(2), pq1.getDeclarations().size());
			Assert::IsFalse(pq1.getColumns().empty());
			Assert::AreEqual(std::string("s"), pq1.getColumns()[0]);
		}
	};
}
