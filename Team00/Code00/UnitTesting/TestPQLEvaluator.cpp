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
			typedef std::pair<PQL_VARIABLE_TYPE, std::string> DECLARATION;

			DECLARATION pair1 = DECLARATION(PQL_VARIABLE_TYPE::STMT, "s");
			DECLARATION pair2 = DECLARATION(PQL_VARIABLE_TYPE::IF, "ifs");

			std::vector<PQL_VARIABLE> declarations;
			declarations.push_back(pair1);
			declarations.push_back(pair2);

			std::vector<std::string> columns;
			columns.push_back("s");

			ParsedQuery pq1 = ParsedQuery(declarations, columns);

			PQLEvaluator pqlEvaluator(pq1);
			auto wrapperFunc = [&pqlEvaluator] { pqlEvaluator.evaluate(); };

			// To change once PKB is integrated.
			/*Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::IsFalse(parsedQuery.getColumns().empty());
			Assert::AreEqual(std::string("s"), parsedQuery.getColumns()[0]);*/
		}
	};
}