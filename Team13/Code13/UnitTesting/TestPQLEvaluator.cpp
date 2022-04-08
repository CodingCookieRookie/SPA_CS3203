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
		bool tupleSelect = false;
		ParsedQuery pq1 = ParsedQuery(declarations, columns, relationships, patterns, withs, tupleSelect);

		Assert::IsFalse(pq1.getAttributes().empty());

		PKBGetter* pkbGetter = nullptr;
		std::vector<Instruction*> selectInstructions = pq1.getSelectInstructions(pkbGetter);
		Assert::AreEqual(size_t(1), selectInstructions.size());
		Assert::IsTrue(std::any_of(selectInstructions.begin(), selectInstructions.end(), [](Instruction* instruction) {
			std::unordered_set<std::string> synonyms = instruction->getSynonyms();
			return synonyms.find("s") != synonyms.end();
			}));
	}
	};
}
