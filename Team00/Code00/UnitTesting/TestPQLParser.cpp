#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "QPS-NEW/PQLParser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestPQLParser)
	{
	public:
		TEST_METHOD(parseQuery_declarationAndSelectOnly_designEntitiesExtracted)
		{
			std::string query = "stmt s; if ifs; Select s";
			PQLParser pqlParser(query);
			ParsedQuery parsedQuery = pqlParser.parseQuery();
			Assert::AreEqual(size_t(2), parsedQuery.getDeclarations().size());
			Assert::IsFalse(parsedQuery.getColumns().empty());
			Assert::AreEqual(std::string("s"), parsedQuery.getColumns()[0]);
		}

		TEST_METHOD(parseQuery_repeatDeclaration_exceptionThrown)
		{
			std::string query = "stmt s; if s; Select s";
			PQLParser pqlParser(query);
			auto wrapperFunc = [&pqlParser] { pqlParser.parseQuery(); };
			Assert::ExpectException<SPAException>(wrapperFunc);
		}

	};
}