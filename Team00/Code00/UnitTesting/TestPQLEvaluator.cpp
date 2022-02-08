#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS-NEW/PQLEvaluator.h"
#include "../source/QPS-NEW/PQLParser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestPQLEvaluator)
	{
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
			auto wrapperFunc = [&pqlEvaluator] { pqlEvaluator.evaluate(); };

			// To change once PKB is integrated.
			/*Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::IsFalse(parsedQuery.getColumns().empty());
			Assert::AreEqual(std::string("s"), parsedQuery.getColumns()[0]);*/
		}
	};
}