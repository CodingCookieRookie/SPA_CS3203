#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/PKB/Pattern.h"
#include "../source/QPS/PQLEvaluator.h"
#include "../source/QPS/PQLParser.h"
#include "PKB/ExpressionProcessor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPatternWInstruction) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Attribute::performCleanUp();
		Entity::performCleanUp();
		Pattern::performCleanUp();
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
		Instruction* instruction = new PatternInstruction(synonym, pqlPatternType, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::WHILE_TYPE);
		Entity::insertVar("y");
		VarIndex varIndex = Entity::insertVar("x");
		Pattern::insertWhileInfo(stmt, varIndex);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getWhileStmtsFromVar(varIndex);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: w Values: \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.getTableString());
	}
	TEST_METHOD(execute_lhsSynonym) {
		// while(v, "_")
		std::string synonym = "w";
		PqlPatternType pqlPatternType = PqlPatternType::PATTERN_W;
		PqlReference entRef = std::make_pair(PqlReferenceType::SYNONYM, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::WILDCARD, "");
		Instruction* instruction = new PatternInstruction(synonym, pqlPatternType, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::WHILE_TYPE);
		Entity::insertVar("y");
		VarIndex varIndex = Entity::insertVar("x");
		Pattern::insertWhileInfo(stmt, varIndex);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getWhileStmtsFromVar(varIndex);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: v Values: 2 \nSynonym: w Values: 2 \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsIdent) {
		// while ("x", "_")
		std::string synonym = "w";
		PqlPatternType pqlPatternType = PqlPatternType::PATTERN_W;
		PqlReference entRef = std::make_pair(PqlReferenceType::IDENT, "x");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::WILDCARD, "");
		Instruction* instruction = new PatternInstruction(synonym, pqlPatternType, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::WHILE_TYPE);
		Entity::insertVar("y");
		VarIndex varIndex = Entity::insertVar("x");
		Pattern::insertWhileInfo(stmt, varIndex);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getWhileStmtsFromVar(varIndex);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: w Values: 2 \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsWildCard) {
		// Pattern while ("_", "_")
		std::string synonym = "w";
		PqlPatternType pqlPatternType = PqlPatternType::PATTERN_W;
		PqlReference entRef = std::make_pair(PqlReferenceType::WILDCARD, "");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::WILDCARD, "");
		Instruction* instruction = new PatternInstruction(synonym, pqlPatternType, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::PRINT_TYPE);
		StmtIndex stmt = Entity::insertStmt(StatementType::WHILE_TYPE);
		Entity::insertVar("y");
		VarIndex varIndex = Entity::insertVar("x");
		Pattern::insertWhileInfo(stmt, varIndex);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getWhileStmtsFromVar(varIndex);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: w Values: 2 \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.getTableString());
	}
	};
}
