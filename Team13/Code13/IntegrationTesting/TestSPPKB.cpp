#include "stdafx.h"
#include "CppUnitTest.h"

#include "SP/Parser.h"
#include "SP/DesignExtractor.h"
#include "PKB/Entity.h"
#include "PKB/ExpressionProcessor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace IntegrationTesting {
	TEST_CLASS(TestSPPKB) {
private:
	const char* sampleSource1 = "procedure computeAverage { \n "
		" read num1; \n "
		" read num2; \n "
		" read num3; \n "
		" sum = num1 + num2 + num3; \n "
		" ave = sum / 3; \n "
		" print ave; \n "
		" }";

	const char* sampleSource2 =
		"procedure printAscending{ \n "
		"	read num1; \n "
		"	read num2; \n "
		"	noSwap = 0; \n "

		"	if (num1 > num2) then { \n "
		"	  temp = num1; \n "
		"	  num1 = num2; \n "
		"	  num2 = temp; \n "
		"	} \n "
		" else { \n "
		"	noSwap = 1; \n "
		"  } \n "
		"  print num1; \n "
		"  print num2; \n "
		"  print noSwap; \n "
		"	}";

	const char* sampleSource3 =
		"procedure sumDigits{ \n "
		"	read number; \n "
		"	sum = 0; \n "

		"	while (number > 0) { \n "
		"		digit = number % 10; \n "
		"		sum = sum + digit; \n "
		"		number = number / 10; \n "
		"	} \n "

		"	print sum; \n "
		"}";

	const char* mixedStmtsSource = "procedure computeCentroid { \n "
		" count = 0; cenX = 0; cenY = 0; x = 5; \n "
		" cenY = x + 3; \n "
		" while ((x != 0) && (y != 0)) { \n "
		"    count = count + 1; \n "
		"    cenX = cenX + x; \n "
		"    cenY = cenY + y; }  \n "
		" if (count == 0) then{ \n "
		"    flag = 1;  \n "
		" }  \n "
		" else { \n "
		"    cenX = cenX / 5; \n "
		"    print count; \n "
		" } \n "
		" normSq = cenX * cenX; }";

	const char* nestedWhileIfSource =
		"procedure Peter{ \n "
		"    print present; \n "
		"    future = 8 * present; \n "
		"    y = present - 4; \n "
		"    z = future + 2; \n "
		"    while (present < 0) { \n "
		"        x = z - present; \n "
		"        if (present > x) then { \n "
		"            present = z - 8 - (x * y) * z; \n "
		"            x = x - 1; } \n "
		"		else { \n "
		"          y = present * y * z; \n "
		"		} \n "
		"	} \n "
		"    future = x * y * z * present * future; \n "
		"    print future; \n "
		"		}";

	const char* multipleBracketsSource =
		"procedure sumDigits{ \n "
		"	read number; \n "
		"	sum = 0; \n "

		"	while ((!(number != 0)) && ((number <= 0) || (number == 0))) { \n "
		"		digit = (((number)) % 10); \n "
		"		sum = ((sum + digit)); \n "
		"		number = (number) / 10; \n "
		"	} \n "

		"	print sum; \n "
		"}";

	const char* sameKeywordNameSource =
		"procedure procedure{ \n "
		"	read read; \n "
		"	if = 0; \n "
		"	print = 1; \n "
		"	read = 1; \n "
		"	while = 1; \n "
		"	procedure = 1; \n "
		"	print print; \n "
		"}";

	const char* longAssignmentExprSource =
		"procedure main { \n "
		"	x = x + y / z % a * b / c % d + f - g + z % a * b / c % d + \n "
		"f - g - x + y / z % a * b / c % d + f - g + z % a * b / c % d + f - g + \n "
		"z % a * b / c % d + f - g + z % a * b / c % d + f - 9; \n "
		"}";

	const char* longVarNameSource =
		"procedure main { \n "
		"	read extremelyVeryLongVariableNameThatHasManyCharactersAndIsVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLong; \n "
		"}";

	const char* multipleNestingSource =
		"procedure Peter{ \n "
		"    while (present < 0) { \n "
		"        if (present > x) then { \n "
		"            present = z - 8 - (x * y) * z; \n "
		"				if (present > x) then { \n "
		"				     present = z - 8 - (x * y) * z; \n "
		"				     x = x - 1; } \n "
		"				else { \n "
		"					 y = present * y * z; \n "
		"					 while (present < 0) { \n "
		"						y = present * y * z; \n "
		"					 } \n "
		"				} \n "
		"            x = x - 1; } \n "
		"		else { \n "
		"          y = present * y * z; \n "
		"		} \n "
		"	} \n "
		"}";

	const char* differentCasingNamesSource =
		"procedure mAin { \n "
		"	v = 1; \n "
		"	V = 1; \n "
		"	v1 = 1; \n "
		"	V1 = 1; \n "
		"}";

	const char* multipleWhitespacesSource =
		"      procedure     main                     { \n "
		"	read      v; \n "
		"	    v =        0  ; \n "
		"	   print    v     ; \n "
		"     }     ";

	TEST_METHOD_CLEANUP(cleanUpTables) {
		Container::performCleanUp();
		Entity::performCleanUp();
		Uses::performCleanUp();
		Modifies::performCleanUp();
		Pattern::performCleanUp();
		Parent::performCleanUp();
		ParentT::performCleanUp();
		Follows::performCleanUp();
		FollowsT::performCleanUp();
	}

	TEST_METHOD_INITIALIZE(initTables) {
		Container::performCleanUp();
		Entity::performCleanUp();
		Uses::performCleanUp();
		Modifies::performCleanUp();
		Pattern::performCleanUp();
		Parent::performCleanUp();
		ParentT::performCleanUp();
		Follows::performCleanUp();
		FollowsT::performCleanUp();
	}
public:

	TEST_METHOD(blackBoxSampleSource1_checkEntities) {
		std::string varName1 = "num1";
		std::string varName2 = "ave";
		std::string procName = "computeAverage";

		SourceAST ast = Parser::parse(sampleSource1);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(6), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::callType).size());
		Assert::AreEqual(size_t(3), Entity::getStmtIdxFromType(StatementType::readType).size());
		Assert::AreEqual(size_t(2), Entity::getStmtIdxFromType(StatementType::assignType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::printType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::whileType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::ifType).size());

		Assert::AreEqual(size_t(5), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[4]));

		Assert::AreEqual(size_t(1), Entity::getAllConsts().size());
	}

	TEST_METHOD(blackBoxSampleSource1_checkRelationships) {
		std::string varName1 = "num1";
		std::string varName2 = "sum";
		std::string varName3 = "num2";

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(4);
		StmtIndex stmtIdx3 = StmtIndex(5);

		SourceAST ast = Parser::parse(sampleSource1);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(5), std::get<0>(Modifies::getAllStmtVarInfo()).size());
		Assert::IsTrue(Modifies::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Modifies::contains(stmtIdx2, Entity::getVarIdx(varName2)));

		Assert::AreEqual(size_t(5), std::get<0>(Uses::getAllStmtVarInfo()).size());
		Assert::IsTrue(Uses::contains(stmtIdx2, Entity::getVarIdx(varName1)));

		Assert::IsTrue(Follows::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::AreEqual(size_t(5), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx3));
		Assert::AreEqual(size_t(15), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getAllAssignStmtVarsPatternInfo()).size());
		Assert::AreEqual(size_t(1), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName3), true).size());
		Assert::AreEqual(size_t(1), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("num1+num2"), true).size());
		Assert::AreEqual(size_t(0), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("num1+num3"), true).size());
	}

	TEST_METHOD(blackBoxSampleSource2_checkEntities) {
		std::string varName1 = "num1";
		std::string varName2 = "temp";
		std::string procName = "printAscending";

		SourceAST ast = Parser::parse(sampleSource2);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(11), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::callType).size());
		Assert::AreEqual(size_t(2), Entity::getStmtIdxFromType(StatementType::readType).size());
		Assert::AreEqual(size_t(5), Entity::getStmtIdxFromType(StatementType::assignType).size());
		Assert::AreEqual(size_t(3), Entity::getStmtIdxFromType(StatementType::printType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::whileType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::ifType).size());

		Assert::AreEqual(size_t(4), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[3]));

		Assert::AreEqual(size_t(2), Entity::getAllConsts().size());
	}

	TEST_METHOD(blackBoxSampleSource2_checkRelationships) {
		std::string varName1 = "num1";
		std::string varName2 = "num2";
		std::string varName3 = "temp";
		std::string varName4 = "noSwap";

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(4);
		StmtIndex stmtIdx3 = StmtIndex(5);
		StmtIndex stmtIdx4 = StmtIndex(6);
		StmtIndex stmtIdx5 = StmtIndex(7);
		StmtIndex stmtIdx6 = StmtIndex(8);
		StmtIndex stmtIdx7 = StmtIndex(9);

		SourceAST ast = Parser::parse(sampleSource2);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(11), std::get<0>(Modifies::getAllStmtVarInfo()).size());
		Assert::IsTrue(Modifies::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Modifies::contains(stmtIdx2, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Modifies::contains(stmtIdx2, Entity::getVarIdx(varName2)));
		Assert::IsTrue(Modifies::contains(stmtIdx2, Entity::getVarIdx(varName4)));

		Assert::AreEqual(size_t(9), std::get<0>(Uses::getAllStmtVarInfo()).size());
		Assert::IsTrue(Uses::contains(stmtIdx2, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Uses::contains(stmtIdx3, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Uses::contains(stmtIdx4, Entity::getVarIdx(varName2)));
		Assert::AreEqual(size_t(3), Uses::getStatements(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(8), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx2, stmtIdx7));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::AreEqual(size_t(24), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx3, stmtIdx5));

		Assert::AreEqual(size_t(4), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx2, stmtIdx4));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx2, stmtIdx5));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx2, stmtIdx6));
		Assert::AreEqual(size_t(4), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(5), std::get<0>(Pattern::getAllAssignStmtVarsPatternInfo()).size());
		Assert::AreEqual(size_t(1), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName3), true).size());
	}

	TEST_METHOD(blackBoxSampleSource3_checkEntities) {
		std::string varName1 = "number";
		std::string varName2 = "digit";
		std::string procName = "sumDigits";

		SourceAST ast = Parser::parse(sampleSource3);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(7), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::callType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::readType).size());
		Assert::AreEqual(size_t(4), Entity::getStmtIdxFromType(StatementType::assignType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::printType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::whileType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::ifType).size());

		Assert::AreEqual(size_t(3), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[2]));

		Assert::AreEqual(size_t(2), Entity::getAllConsts().size());
	}

	TEST_METHOD(blackBoxSampleSource3_checkRelationships) {
		std::string varName1 = "number";
		std::string varName2 = "digit";

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(3);
		StmtIndex stmtIdx3 = StmtIndex(4);
		StmtIndex stmtIdx4 = StmtIndex(5);
		StmtIndex stmtIdx5 = StmtIndex(6);
		StmtIndex stmtIdx6 = StmtIndex(7);

		SourceAST ast = Parser::parse(sampleSource3);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(8), std::get<0>(Modifies::getAllStmtVarInfo()).size());
		Assert::IsTrue(Modifies::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Modifies::contains(stmtIdx2, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Modifies::contains(stmtIdx2, Entity::getVarIdx(varName2)));

		Assert::AreEqual(size_t(8), std::get<0>(Uses::getAllStmtVarInfo()).size());
		Assert::IsTrue(Uses::contains(stmtIdx2, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Uses::contains(stmtIdx3, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Uses::contains(stmtIdx4, Entity::getVarIdx(varName2)));
		Assert::AreEqual(size_t(3), Uses::getStatements(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(5), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx2, stmtIdx6));
		Assert::AreEqual(size_t(9), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx3, stmtIdx5));

		Assert::AreEqual(size_t(3), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx2, stmtIdx4));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx2, stmtIdx5));
		Assert::AreEqual(size_t(3), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(4), std::get<0>(Pattern::getAllAssignStmtVarsPatternInfo()).size());
		Assert::AreEqual(size_t(1), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName1), true).size());
		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix(varName1), true)).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix("number%10"), true)).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix("10%number"), true)).size());
	}

	TEST_METHOD(mixedStmtsSource_checkEntities) {
		std::string varName1 = "count";
		std::string varName2 = "normSq";
		std::string procName = "computeCentroid";

		SourceAST ast = Parser::parse(mixedStmtsSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(14), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::callType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::readType).size());
		Assert::AreEqual(size_t(11), Entity::getStmtIdxFromType(StatementType::assignType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::printType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::whileType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::ifType).size());

		Assert::AreEqual(size_t(7), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[6]));

		Assert::AreEqual(size_t(4), Entity::getAllConsts().size());
	}

	TEST_METHOD(mixedStmtsSource_checkRelationships) {
		std::string varName1 = "count";
		std::string varName2 = "x";
		std::string varName3 = "flag";

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(6);
		StmtIndex stmtIdx3 = StmtIndex(7);
		StmtIndex stmtIdx4 = StmtIndex(8);
		StmtIndex stmtIdx5 = StmtIndex(10);
		StmtIndex stmtIdx6 = StmtIndex(11);
		StmtIndex stmtIdx7 = StmtIndex(12);
		StmtIndex stmtIdx8 = StmtIndex(14);

		SourceAST ast = Parser::parse(mixedStmtsSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(16), std::get<0>(Modifies::getAllStmtVarInfo()).size());
		Assert::IsTrue(Modifies::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Modifies::contains(stmtIdx2, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Modifies::contains(stmtIdx5, Entity::getVarIdx(varName3)));
		Assert::AreEqual(size_t(3), Modifies::getStatements(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(16), std::get<0>(Uses::getAllStmtVarInfo()).size());
		Assert::IsTrue(Uses::contains(stmtIdx2, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Uses::contains(stmtIdx2, Entity::getVarIdx(varName2)));
		Assert::IsTrue(Uses::contains(stmtIdx3, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Uses::contains(stmtIdx4, Entity::getVarIdx(varName2)));
		Assert::AreEqual(size_t(4), Uses::getStatements(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(10), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx2, stmtIdx5));
		Assert::AreEqual(size_t(32), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx2, stmtIdx8));

		Assert::AreEqual(size_t(6), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx5, stmtIdx6));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx5, stmtIdx7));
		Assert::AreEqual(size_t(6), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(11), std::get<0>(Pattern::getAllAssignStmtVarsPatternInfo()).size());
		Assert::AreEqual(size_t(1), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName1), true).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix(varName1), true)).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix("x+3"), true)).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix("3+x"), true)).size());
	}

	TEST_METHOD(nestedWhileIfSource_checkEntities) {
		std::string varName1 = "present";
		std::string varName2 = "x";
		std::string procName = "Peter";

		SourceAST ast = Parser::parse(nestedWhileIfSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(12), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::callType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::readType).size());
		Assert::AreEqual(size_t(8), Entity::getStmtIdxFromType(StatementType::assignType).size());
		Assert::AreEqual(size_t(2), Entity::getStmtIdxFromType(StatementType::printType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::whileType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::ifType).size());

		Assert::AreEqual(size_t(5), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[4]));

		Assert::AreEqual(size_t(5), Entity::getAllConsts().size());
	}

	TEST_METHOD(nestedWhileIfSource_checkRelationships) {
		std::string varName1 = "present";
		std::string varName2 = "future";

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(5);
		StmtIndex stmtIdx5 = StmtIndex(6);
		StmtIndex stmtIdx6 = StmtIndex(7);
		StmtIndex stmtIdx7 = StmtIndex(8);
		StmtIndex stmtIdx8 = StmtIndex(10);
		StmtIndex stmtIdx9 = StmtIndex(11);

		SourceAST ast = Parser::parse(nestedWhileIfSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(14), std::get<0>(Modifies::getAllStmtVarInfo()).size());
		Assert::IsTrue(Modifies::contains(stmtIdx2, Entity::getVarIdx(varName2)));
		Assert::IsTrue(Modifies::contains(stmtIdx4, Entity::getVarIdx(varName1)));
		Assert::AreEqual(size_t(3), Modifies::getStatements(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(27), std::get<0>(Uses::getAllStmtVarInfo()).size());
		Assert::IsTrue(Uses::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Uses::contains(stmtIdx2, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Uses::contains(stmtIdx3, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Uses::contains(stmtIdx6, Entity::getVarIdx(varName1)));
		Assert::AreEqual(size_t(8), Uses::getStatements(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(8), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx4, stmtIdx9));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx5, stmtIdx6));
		Assert::AreEqual(size_t(23), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx2, stmtIdx9));
		Assert::IsFalse(FollowsT::containsSuccessor(stmtIdx7, stmtIdx8));

		Assert::AreEqual(size_t(5), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx4, stmtIdx5));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx4, stmtIdx6));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx6, stmtIdx7));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx6, stmtIdx8));
		Assert::AreEqual(size_t(8), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx4, stmtIdx7));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx4, stmtIdx8));

		Assert::AreEqual(size_t(8), std::get<0>(Pattern::getAllAssignStmtVarsPatternInfo()).size());
		Assert::AreEqual(size_t(2), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName1), true).size());
		Assert::AreEqual(size_t(1), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("z-8"), true).size());
		Assert::AreEqual(size_t(5), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix(varName1), true)).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix("z-8-(x*y)"), true)).size());
		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix("x*y"), true)).size());
		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix("x*y*z"), true)).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix("8-(x*y)"), true)).size());
	}

	TEST_METHOD(multipleBracketsSource_checkEntities) {
		std::string varName1 = "number";
		std::string varName2 = "digit";
		std::string procName = "sumDigits";

		SourceAST ast = Parser::parse(multipleBracketsSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(7), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::callType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::readType).size());
		Assert::AreEqual(size_t(4), Entity::getStmtIdxFromType(StatementType::assignType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::printType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::whileType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::ifType).size());

		Assert::AreEqual(size_t(3), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[2]));

		Assert::AreEqual(size_t(2), Entity::getAllConsts().size());
	}

	TEST_METHOD(multipleBracketsSource_checkRelationships) {
		std::string varName1 = "number";
		std::string varName2 = "sum";

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(5);
		StmtIndex stmtIdx5 = StmtIndex(6);

		SourceAST ast = Parser::parse(multipleBracketsSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(8), std::get<0>(Modifies::getAllStmtVarInfo()).size());
		Assert::IsTrue(Modifies::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::AreEqual(size_t(3), Modifies::getStatements(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(8), std::get<0>(Uses::getAllStmtVarInfo()).size());
		Assert::IsFalse(Uses::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsFalse(Uses::contains(stmtIdx2, Entity::getVarIdx(varName2)));
		Assert::IsTrue(Uses::contains(stmtIdx3, Entity::getVarIdx(varName1)));
		Assert::AreEqual(size_t(3), Uses::getStatements(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(5), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx4, stmtIdx5));
		Assert::AreEqual(size_t(9), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx3));

		Assert::AreEqual(size_t(3), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::AreEqual(size_t(3), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(4), std::get<0>(Pattern::getAllAssignStmtVarsPatternInfo()).size());
		Assert::AreEqual(size_t(1), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName1), true).size());
		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix(varName1), true)).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix("number%10"), true)).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix("(((number))%10)"), true)).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix("10%number"), true)).size());
	}

	TEST_METHOD(sameKeywordNameSource_checkEntities) {
		std::string varName1 = "read";
		std::string varName2 = "if";
		std::string varName3 = "procedure";
		std::string procName = "procedure";

		SourceAST ast = Parser::parse(sameKeywordNameSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(7), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::callType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::readType).size());
		Assert::AreEqual(size_t(5), Entity::getStmtIdxFromType(StatementType::assignType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::printType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::whileType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::ifType).size());

		Assert::AreEqual(size_t(5), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[1]));
		Assert::AreEqual(varName3, Entity::getVarName(Entity::getAllVars()[4]));

		Assert::AreEqual(size_t(2), Entity::getAllConsts().size());
	}

	TEST_METHOD(sameKeywordNameSource_checkRelationships) {
		std::string varName1 = "read";
		std::string varName2 = "if";
		std::string varName3 = "print";

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(7);

		SourceAST ast = Parser::parse(sameKeywordNameSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(6), std::get<0>(Modifies::getAllStmtVarInfo()).size());
		Assert::IsTrue(Modifies::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::AreEqual(size_t(2), Modifies::getStatements(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(1), std::get<0>(Uses::getAllStmtVarInfo()).size());
		Assert::IsTrue(Uses::contains(stmtIdx3, Entity::getVarIdx(varName3)));
		Assert::IsFalse(Uses::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::AreEqual(size_t(1), Uses::getStatements(Entity::getVarIdx(varName3)).size());

		Assert::AreEqual(size_t(6), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::AreEqual(size_t(21), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx3));

		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(5), std::get<0>(Pattern::getAllAssignStmtVarsPatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName1), true).size());
		Assert::AreEqual(size_t(1), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("1"), true).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix(varName1), true)).size());
		Assert::AreEqual(size_t(4), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix("1"), true)).size());
	}

	TEST_METHOD(longAssignmentExprSource_checkEntities) {
		std::string varName1 = "x";
		std::string varName2 = "y";
		std::string procName = "main";

		SourceAST ast = Parser::parse(longAssignmentExprSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(1), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::callType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::readType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::assignType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::printType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::whileType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::ifType).size());

		Assert::AreEqual(size_t(9), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[8]));

		Assert::AreEqual(size_t(1), Entity::getAllConsts().size());
	}

	TEST_METHOD(longAssignmentExprSource_checkRelationships) {
		std::string varName1 = "x";
		std::string varName2 = "d";

		StmtIndex stmtIdx = StmtIndex(1);

		SourceAST ast = Parser::parse(longAssignmentExprSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(1), std::get<0>(Modifies::getAllStmtVarInfo()).size());
		Assert::IsTrue(Modifies::contains(stmtIdx, Entity::getVarIdx(varName1)));
		Assert::AreEqual(size_t(1), Modifies::getStatements(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(9), std::get<0>(Uses::getAllStmtVarInfo()).size());
		Assert::IsTrue(Uses::contains(stmtIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Uses::contains(stmtIdx, Entity::getVarIdx(varName2)));
		Assert::AreEqual(size_t(1), Uses::getStatements(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(0), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAllAssignStmtVarsPatternInfo()).size());
		Assert::AreEqual(size_t(1), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName1), true).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix(varName1), true)).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix("x+y"), true)).size());
	}

	TEST_METHOD(longVarNameSource_checkEntities) {
		std::string varName = "extremelyVeryLongVariableNameThatHasManyCharactersAndIsVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLong";
		std::string procName = "main";

		SourceAST ast = Parser::parse(longVarNameSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(1), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::readType).size());

		Assert::AreEqual(size_t(1), Entity::getAllVars().size());
		Assert::AreEqual(varName, Entity::getVarName(Entity::getAllVars()[0]));

		Assert::AreEqual(size_t(0), Entity::getAllConsts().size());
	}

	TEST_METHOD(longVarNameSource_checkRelationships) {
		std::string varName = "extremelyVeryLongVariableNameThatHasManyCharactersAndIsVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLong";

		StmtIndex stmtIdx = StmtIndex(1);

		SourceAST ast = Parser::parse(longVarNameSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(1), std::get<0>(Modifies::getAllStmtVarInfo()).size());
		Assert::IsTrue(Modifies::contains(stmtIdx, Entity::getVarIdx(varName)));
		Assert::AreEqual(size_t(1), Modifies::getStatements(Entity::getVarIdx(varName)).size());

		Assert::AreEqual(size_t(0), std::get<0>(Uses::getAllStmtVarInfo()).size());
		Assert::AreEqual(size_t(0), Uses::getStatements(Entity::getVarIdx(varName)).size());

		Assert::AreEqual(size_t(0), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllAssignStmtVarsPatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName), ExpressionProcessor::convertInfixToPostFix(varName), true).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix(varName), true)).size());
	}

	TEST_METHOD(multipleNestingSource_checkEntities) {
		std::string varName1 = "present";
		std::string varName2 = "y";
		std::string procName = "Peter";

		SourceAST ast = Parser::parse(multipleNestingSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(11), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::callType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::readType).size());
		Assert::AreEqual(size_t(7), Entity::getStmtIdxFromType(StatementType::assignType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::printType).size());
		Assert::AreEqual(size_t(2), Entity::getStmtIdxFromType(StatementType::whileType).size());
		Assert::AreEqual(size_t(2), Entity::getStmtIdxFromType(StatementType::ifType).size());

		Assert::AreEqual(size_t(4), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[3]));

		Assert::AreEqual(size_t(3), Entity::getAllConsts().size());
	}

	TEST_METHOD(multipleNestingSource_checkRelationships) {
		std::string varName1 = "present";
		std::string varName2 = "y";
		std::string varName3 = "x";

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		StmtIndex stmtIdx6 = StmtIndex(6);
		StmtIndex stmtIdx7 = StmtIndex(7);
		StmtIndex stmtIdx8 = StmtIndex(8);
		StmtIndex stmtIdx9 = StmtIndex(9);
		StmtIndex stmtIdx10 = StmtIndex(10);

		SourceAST ast = Parser::parse(multipleNestingSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(17), std::get<0>(Modifies::getAllStmtVarInfo()).size());
		Assert::IsTrue(Modifies::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Modifies::contains(stmtIdx1, Entity::getVarIdx(varName3)));
		Assert::IsTrue(Modifies::contains(stmtIdx2, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Modifies::contains(stmtIdx3, Entity::getVarIdx(varName1)));
		Assert::IsTrue(Modifies::contains(stmtIdx4, Entity::getVarIdx(varName1)));
		Assert::IsFalse(Modifies::contains(stmtIdx6, Entity::getVarIdx(varName1)));
		Assert::IsFalse(Modifies::contains(stmtIdx7, Entity::getVarIdx(varName1)));
		Assert::IsFalse(Modifies::contains(stmtIdx8, Entity::getVarIdx(varName1)));
		Assert::IsFalse(Modifies::contains(stmtIdx9, Entity::getVarIdx(varName1)));
		Assert::AreEqual(size_t(3), Container::getContainersOfStmt(stmtIdx5).size());
		Assert::IsTrue(Modifies::contains(stmtIdx5, Entity::getVarIdx(varName1)));
		Assert::AreEqual(size_t(5), Modifies::getStatements(Entity::getVarIdx(varName1)).size());
		Assert::AreEqual(size_t(7), Modifies::getStatements(Entity::getVarIdx(varName2)).size());

		Assert::AreEqual(size_t(32), std::get<0>(Uses::getAllStmtVarInfo()).size());
		Assert::IsTrue(Uses::contains(stmtIdx1, Entity::getVarIdx(varName2)));
		Assert::IsTrue(Uses::contains(stmtIdx2, Entity::getVarIdx(varName2)));
		Assert::IsTrue(Uses::contains(stmtIdx3, Entity::getVarIdx(varName2)));
		Assert::IsTrue(Uses::contains(stmtIdx4, Entity::getVarIdx(varName2)));
		Assert::IsFalse(Uses::contains(stmtIdx6, Entity::getVarIdx(varName2)));
		Assert::IsTrue(Uses::contains(stmtIdx7, Entity::getVarIdx(varName2)));
		Assert::IsTrue(Uses::contains(stmtIdx8, Entity::getVarIdx(varName2)));
		Assert::IsTrue(Uses::contains(stmtIdx9, Entity::getVarIdx(varName2)));
		Assert::AreEqual(size_t(7), Uses::getStatements(Entity::getVarIdx(varName1)).size());
		Assert::AreEqual(size_t(9), Uses::getStatements(Entity::getVarIdx(varName2)).size());

		Assert::AreEqual(size_t(4), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx3, stmtIdx10));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx5, stmtIdx6));
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx6, stmtIdx7));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx7, stmtIdx8));
		Assert::AreEqual(size_t(5), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx3, stmtIdx10));
		Assert::IsFalse(FollowsT::containsSuccessor(stmtIdx6, stmtIdx7));

		Assert::AreEqual(size_t(10), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx2, stmtIdx4));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx4, stmtIdx5));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx4, stmtIdx6));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx4, stmtIdx7));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx4, stmtIdx8));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx8, stmtIdx9));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx2, stmtIdx10));
		Assert::IsFalse(Parent::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::AreEqual(size_t(25), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx2, stmtIdx4));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx4, stmtIdx5));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx4, stmtIdx6));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx4, stmtIdx7));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx4, stmtIdx8));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx8, stmtIdx9));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx2, stmtIdx10));
		Assert::IsFalse(ParentT::containsSuccessor(stmtIdx3, stmtIdx4));

		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx1, stmtIdx3));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx1, stmtIdx4));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx1, stmtIdx5));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx1, stmtIdx6));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx1, stmtIdx7));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx1, stmtIdx8));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx1, stmtIdx9));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx1, stmtIdx10));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx2, stmtIdx5));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx2, stmtIdx6));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx2, stmtIdx7));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx2, stmtIdx8));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx2, stmtIdx9));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx4, stmtIdx9));

		Assert::AreEqual(size_t(7), std::get<0>(Pattern::getAllAssignStmtVarsPatternInfo()).size());
		Assert::AreEqual(size_t(3), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName1), true).size());
		Assert::AreEqual(size_t(0), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("sent"), true).size());
		Assert::AreEqual(size_t(0), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("y*z"), true).size());
		Assert::AreEqual(size_t(3), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("present*y"), true).size());
		Assert::AreEqual(size_t(3), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix(varName1), true)).size());
	}

	TEST_METHOD(differentCasingNamesSource_checkEntities) {
		std::string varName1 = "v";
		std::string varName2 = "V1";
		std::string procName = "mAin";

		SourceAST ast = Parser::parse(differentCasingNamesSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));
		Assert::IsFalse("main" == Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(4), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::callType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::readType).size());
		Assert::AreEqual(size_t(4), Entity::getStmtIdxFromType(StatementType::assignType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::printType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::whileType).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::ifType).size());

		Assert::AreEqual(size_t(4), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[3]));

		Assert::AreEqual(size_t(1), Entity::getAllConsts().size());
	}

	TEST_METHOD(differentCasingNamesSource_checkRelationships) {
		std::string varName = "v";

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);

		SourceAST ast = Parser::parse(differentCasingNamesSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(4), std::get<0>(Modifies::getAllStmtVarInfo()).size());
		Assert::IsTrue(Modifies::contains(stmtIdx1, Entity::getVarIdx(varName)));
		Assert::AreEqual(size_t(1), Modifies::getStatements(Entity::getVarIdx(varName)).size());

		Assert::AreEqual(size_t(0), std::get<0>(Uses::getAllStmtVarInfo()).size());

		Assert::AreEqual(size_t(3), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::AreEqual(size_t(6), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx4));

		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(4), std::get<0>(Pattern::getAllAssignStmtVarsPatternInfo()).size());
		Assert::AreEqual(size_t(1), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName), ExpressionProcessor::convertInfixToPostFix("1"), true).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix(varName), true)).size());
	}

	TEST_METHOD(multipleWhitespacesSource_checkEntities) {
		std::string varName = "v";
		std::string procName = "main";

		SourceAST ast = Parser::parse(multipleWhitespacesSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(3), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::readType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::assignType).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::printType).size());

		Assert::AreEqual(size_t(1), Entity::getAllVars().size());
		Assert::AreEqual(varName, Entity::getVarName(Entity::getAllVars()[0]));

		Assert::AreEqual(size_t(1), Entity::getAllConsts().size());
	}

	TEST_METHOD(multipleWhitespacesSource_checkRelationships) {
		std::string varName = "v";

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);

		SourceAST ast = Parser::parse(multipleWhitespacesSource);
		DesignExtractor::extract(ast);

		Assert::AreEqual(size_t(2), std::get<0>(Modifies::getAllStmtVarInfo()).size());
		Assert::IsTrue(Modifies::contains(stmtIdx1, Entity::getVarIdx(varName)));
		Assert::AreEqual(size_t(2), Modifies::getStatements(Entity::getVarIdx(varName)).size());

		Assert::AreEqual(size_t(1), std::get<0>(Uses::getAllStmtVarInfo()).size());
		Assert::AreEqual(size_t(1), Uses::getStatements(Entity::getVarIdx(varName)).size());

		Assert::AreEqual(size_t(2), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(3), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAllAssignStmtVarsPatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName), ExpressionProcessor::convertInfixToPostFix(varName), true).size());
		Assert::AreEqual(size_t(1), Pattern::getStmtsFromVarPattern(Entity::getVarIdx(varName), ExpressionProcessor::convertInfixToPostFix("0"), true).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getStmtsFromPattern(ExpressionProcessor::convertInfixToPostFix(varName), true)).size());
	}

	TEST_METHOD(missingSemicolon_parserExceptionThrown_checkPKBTables) {
		const char* source =
			"procedure main { \n "
			"	read v; \n "
			"	print v \n "
			"}";
		auto wrapperFunc = [&source] { Parser::parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			SourceAST ast = Parser::parse(source);
			DesignExtractor::extract(ast);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}

		Assert::AreEqual(size_t(0), Entity::getAllProcs().size());
		Assert::AreEqual(size_t(0), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getAllVars().size());
		Assert::AreEqual(size_t(0), Entity::getAllConsts().size());
		Assert::AreEqual(size_t(0), std::get<0>(Modifies::getAllStmtVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Uses::getAllStmtVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllAssignStmtVarsPatternInfo()).size());
	}
	};
}
