#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/PKB/Pattern.h"
#include "../source/QPS/PQLEvaluator.h"
#include "../source/QPS/PQLParser.h"
#include "PKB/ExpressionProcessor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestPatternIInstruction) {
private:
	TEST_METHOD_CLEANUP(cleanUpTables) {
		Attribute::performCleanUp();
		Entity::performCleanUp();
		Pattern::performCleanUp();
	}
public:
	// Pattern ifs(v, "_", "_") or Pattern ifs("x", "_", "_") or Pattern ifs("_", "_", "_")
	TEST_METHOD(execute_lhsSynonym_IdentOutOfBoundsEvTableFalse) {
		// if (x > 0)
		// Pattern if("v", "_", "_")
		// 1. Setup:
		std::string synonym = "ifs";
		PqlPatternType pqlPatternType = PqlPatternType::PatternI;
		PqlReference entRef = std::make_pair(PqlReferenceType::ident, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::wildcard, "");
		Instruction* instruction = new PatternInstruction(synonym, pqlPatternType, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::ifType);
		Entity::insertVar("y");
		VarIndex varIndex = Entity::insertVar("x");
		Pattern::insertIfInfo(stmt, varIndex);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getIfStmtsFromVar(varIndex);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(0), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: ifs Values: \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.getTableString());
	}
	TEST_METHOD(execute_lhsSynonym) {
		// if (x > 0)
		// Pattern if(v, "_", "_")
		// 1. Setup:
		std::string synonym = "ifs";
		PqlPatternType pqlPatternType = PqlPatternType::PatternI;
		PqlReference entRef = std::make_pair(PqlReferenceType::synonym, "v");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::wildcard, "");
		Instruction* instruction = new PatternInstruction(synonym, pqlPatternType, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::ifType);
		Entity::insertVar("y");
		VarIndex varIndex = Entity::insertVar("x");
		Pattern::insertIfInfo(stmt, varIndex);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getIfStmtsFromVar(varIndex);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 2\nSynonym: ifs Values: 2 \nSynonym: v Values: 2 \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsIdent) {
		// if (x > 0)
		// Pattern if("x", "_", "_")
		// 1. Setup:
		std::string synonym = "ifs";
		PqlPatternType pqlPatternType = PqlPatternType::PatternI;
		PqlReference entRef = std::make_pair(PqlReferenceType::ident, "x");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::wildcard, "");
		Instruction* instruction = new PatternInstruction(synonym, pqlPatternType, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::ifType);
		Entity::insertVar("y");
		VarIndex varIndex = Entity::insertVar("x");
		Pattern::insertIfInfo(stmt, varIndex);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getIfStmtsFromVar(varIndex);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: ifs Values: 2 \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.getTableString());
	}

	TEST_METHOD(execute_lhsWildCard) {
		// if (x > 0)
		// Pattern if("x", "_", "_")
		// 1. Setup:
		std::string synonym = "ifs";
		PqlPatternType pqlPatternType = PqlPatternType::PatternI;
		PqlReference entRef = std::make_pair(PqlReferenceType::wildcard, "");
		PqlExpression expressionSpec = std::make_pair(PqlExpressionType::wildcard, "");
		Instruction* instruction = new PatternInstruction(synonym, pqlPatternType, entRef, expressionSpec);

		// PKB inserts pattern
		Entity::insertStmt(StatementType::printType);
		StmtIndex stmt = Entity::insertStmt(StatementType::ifType);
		Entity::insertVar("y");
		VarIndex varIndex = Entity::insertVar("x");
		Pattern::insertIfInfo(stmt, varIndex);

		// Check PKB populated
		std::vector<int> allStmts = Pattern::getIfStmtsFromVar(varIndex);
		Assert::AreEqual(size_t(1), allStmts.size());

		// 2. Main test:
		EvaluatedTable evTable = instruction->execute();
		Assert::AreEqual(size_t(1), evTable.getNumRow());
		std::string expected = "Table String: size: 1\nSynonym: ifs Values: 2 \n";   // for this iteration we leave empty vectors still (does not affect end res)
		Assert::AreEqual(expected, evTable.getTableString());
	}
	};
}
