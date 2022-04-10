#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "PKB/PKBInserter.h"
#include "PKB/PKBGetter.h"
#include "QPS/PQLEvaluator.h"
#include "QPS/PQLParser.h"
#include "PKB/ExpressionProcessor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPatternWInstruction) {
private:
	PKB pkb;
	PKBGetter* pkbGetter;
	PKBInserter* pkbInserter;

	TEST_METHOD_INITIALIZE(init) {
		pkb = PKB();
		pkbGetter = new PKBGetter(pkb);
		pkbInserter = new PKBInserter(pkb);
	}

public:
	// Pattern while(v, "_") or Pattern while("x", "_") or Pattern while("_", "_")
	TEST_METHOD(execute_lhsSynonym_IdentOutOfBoundsEvTableFalse) {
		// while (x > 0)
		// Pattern w("v", "_")
		// 1. Setup:
		std::string synonym = "w";
		PqlPatternType pqlPatternType = PqlPatternType::PATTERN_W;
		PqlReference entRef = std::make_pair(PqlReferenceType::IDENT, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::WILDCARD, "");
		Instruction* instruction = new PatternWInstruction(synonym, entRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "w" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertPatternContainer(StatementType::WHILE_TYPE, stmt, varIndex);

		// Check PKB populated
		std::vector<Index> allStmts = pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIndex);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: w Values: \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.toString());
	}
	TEST_METHOD(execute_lhsSynonym) {
		// while(v, "_")
		std::string synonym = "w";
		PqlPatternType pqlPatternType = PqlPatternType::PATTERN_W;
		PqlReference entRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::WILDCARD, "");
		Instruction* instruction = new PatternWInstruction(synonym, entRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "w", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertPatternContainer(StatementType::WHILE_TYPE, stmt, varIndex);

		// Check PKB populated
		std::vector<Index> allStmts = pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIndex);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: v Values: 2 \nSynonym: w Values: 2 \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsIdent) {
		// while ("x", "_")
		std::string synonym = "w";
		PqlPatternType pqlPatternType = PqlPatternType::PATTERN_W;
		PqlReference entRef = std::make_pair(PqlReferenceType::IDENT, "x");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::WILDCARD, "");
		Instruction* instruction = new PatternWInstruction(synonym, entRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "w" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertPatternContainer(StatementType::WHILE_TYPE, stmt, varIndex);

		// Check PKB populated
		std::vector<Index> allStmts = pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIndex);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: w Values: 2 \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsWildCard) {
		// Pattern while ("_", "_")
		std::string synonym = "w";
		PqlPatternType pqlPatternType = PqlPatternType::PATTERN_W;
		PqlReference entRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::WILDCARD, "");
		Instruction* instruction = new PatternWInstruction(synonym, entRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "w" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::WHILE_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertPatternContainer(StatementType::WHILE_TYPE, stmt, varIndex);

		// Check PKB populated
		std::vector<Index> allStmts = pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIndex);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: w Values: 2 \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.toString());
	}
	};
}
