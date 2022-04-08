#include "stdafx.h"
#include "CppUnitTest.h"

#include <algorithm>
#include <string>

#include "../source/QPS/PQLEvaluator.h"
#include "../source/QPS/PQLParser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPQLEvaluator) {
private:

public:

	TEST_METHOD(evaluateQuery_declarationAndSelectOnly_designEntitiesExtracted) {
		// "stmt s; if ifs; Select s";
		typedef std::pair<EntityType, std::string> DECLARATION;

		DECLARATION pair1 = DECLARATION(EntityType::STMT, "s");
		DECLARATION pair2 = DECLARATION(EntityType::IF, "ifs");

		std::vector<PqlEntity> declarations;
		declarations.push_back(pair1);
		declarations.push_back(pair2);

		std::vector<PqlReference> columns;
		columns.emplace_back(PqlReferenceType::SYNONYM, "s");

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
