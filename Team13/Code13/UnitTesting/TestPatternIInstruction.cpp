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
	TEST_CLASS(TestPatternIInstruction) {
private:
	PKB* pkb;
	PKBGetter* pkbGetter;
	PKBInserter* pkbInserter;

	TEST_METHOD_INITIALIZE(init) {
		pkb = new PKB();
		pkbGetter = new PKBGetter(pkb);
		pkbInserter = new PKBInserter(pkb);
	}

public:
	// Pattern ifs(v, "_", "_") or Pattern ifs("x", "_", "_") or Pattern ifs("_", "_", "_")
	TEST_METHOD(execute_lhsSynonym_IdentOutOfBoundsEvTableFalse) {
		// if (x > 0)
		// Pattern if("v", "_", "_")
		// 1. Setup:
		std::string synonym = "ifs";
		PqlPatternType pqlPatternType = PqlPatternType::PATTERN_I;
		PqlReference entRef = std::make_pair(PqlReferenceType::IDENT, "v");
		Instruction* instruction = new PatternIInstruction(synonym, entRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "ifs" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::IF_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertPatternContainer(StatementType::IF_TYPE, stmt, varIndex);

		// Check PKB populated
		std::vector<int> allStmts = pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIndex);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: ifs Values: \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.toString());
	}
	TEST_METHOD(execute_lhsSynonym) {
		// if (x > 0)
		// Pattern if(v, "_", "_")
		// 1. Setup:
		std::string synonym = "ifs";
		PqlPatternType pqlPatternType = PqlPatternType::PATTERN_I;
		PqlReference entRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::WILDCARD, "");
		Instruction* instruction = new PatternIInstruction(synonym, entRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "ifs", "v" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::IF_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertPatternContainer(StatementType::IF_TYPE, stmt, varIndex);

		// Check PKB populated
		std::vector<int> allStmts = pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIndex);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: ifs Values: 2 \nSynonym: v Values: 2 \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsIdent) {
		// if (x > 0)
		// Pattern if("x", "_", "_")
		// 1. Setup:
		std::string synonym = "ifs";
		PqlPatternType pqlPatternType = PqlPatternType::PATTERN_I;
		PqlReference entRef = std::make_pair(PqlReferenceType::IDENT, "x");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::WILDCARD, "");
		Instruction* instruction = new PatternIInstruction(synonym, entRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "ifs" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::IF_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertPatternContainer(StatementType::IF_TYPE, stmt, varIndex);

		// Check PKB populated
		std::vector<int> allStmts = pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIndex);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: ifs Values: 2 \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.toString());
	}

	TEST_METHOD(execute_lhsWildCard) {
		// if (x > 0)
		// Pattern if("x", "_", "_")
		// 1. Setup:
		std::string synonym = "ifs";
		PqlPatternType pqlPatternType = PqlPatternType::PATTERN_I;
		PqlReference entRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::WILDCARD, "");
		Instruction* instruction = new PatternIInstruction(synonym, entRef, pkbGetter);

		std::unordered_set<std::string> expectedSynonyms{ "ifs" };
		Assert::IsTrue(instruction->getSynonyms() == expectedSynonyms);

		// PKB inserts pattern
		pkbInserter->insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = pkbInserter->insertStmt(StatementType::IF_TYPE);
		pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "y");
		VarIndex varIndex = pkbInserter->insertNameIdxEntity(EntityType::VARIABLE, "x");
		pkbInserter->insertPatternContainer(StatementType::IF_TYPE, stmt, varIndex);

		// Check PKB populated
		std::vector<int> allStmts = pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIndex);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: ifs Values: 2 \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.toString());
	}
	};
}
