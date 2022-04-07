#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS/PQLEvaluator.h"
#include "../source/QPS/PQLParser.h"
#include "../source/QPS/AffectsStarInstruction.h"
#include "../source/QPS/CacheStorage.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	// Affects (a1, a2) or Affects(a1, 1) or Affects (a1, _ )
	// Affects (_, a2) or Affects(_, 1) or Affects (_, _ )
	TEST_CLASS(TestAffectsStarInstruction) {
private:
	CacheStorage* cacheStorage;
	AffectsProcessor* affectsProcessor;
	AffectsTProcessor* affectsTProcessor;
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Attribute::performCleanUp();
		Entity::performCleanUp();
	}

	TEST_METHOD_INITIALIZE(init) {
		cacheStorage = new CacheStorage();
		affectsProcessor = new AffectsProcessor(cacheStorage);
		affectsTProcessor = new AffectsTProcessor(cacheStorage, affectsProcessor);
	}
public:
	TEST_METHOD(execute_lhsSynonymRhsSynonym) {
		/*	x = 1;
			y = x;
			z = y;
		*/
		// 1. Setup:
		PqlReference lhsRef, rhsRef;
		lhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a1");
		rhsRef = std::make_pair(PqlReferenceType::SYNONYM, "a2");
		Instruction* instruction = new AffectsStarInstruction(lhsRef, rhsRef, affectsTProcessor);

		std::unordered_set<std::string> expectedSynonyms{ "a1", "a2" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts modifies
		Entity::insertStmt(StatementType::PRINT_TYPE);   // insert dummy stmt
		StmtIndex stmt = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt2 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		StmtIndex stmt3 = Entity::insertStmt(StatementType::ASSIGN_TYPE);
		Entity::insertVar("randomVar"); // insert dummy var
		VarIndex varIndex = Entity::insertVar("x");
		VarIndex varIndex2 = Entity::insertVar("y");
		ModifiesS::insert(stmt, varIndex);
		UsesS::insert(stmt2, varIndex);
		ModifiesS::insert(stmt2, varIndex2);
		UsesS::insert(stmt3, varIndex2);
		std::unordered_set<StmtIndex> successors;
		successors.insert(stmt2);
		successors.insert(stmt3);
		std::unordered_set<StmtIndex> successors2;
		successors2.insert(stmt3);
		cacheStorage->insertSuccessors(stmt, successors);
		cacheStorage->insertSuccessors(stmt2, successors2);

		Assert::IsTrue(cacheStorage->contains(stmt, stmt2));
		Assert::IsTrue(cacheStorage->contains(stmt2, stmt3));

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		//Assert::AreEqual(size_t(3), evTable.getNumRow());
		//std::string expected = "Table String: size: 2\nSynonym: a1 Values: 2 2 3 \nSynonym: a2 Values: 3 4 4 \n";
		//Assert::AreEqual(expected, evTable.getTableString());
	}
	};
}
