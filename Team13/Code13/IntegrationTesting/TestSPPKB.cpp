#include "stdafx.h"
#include "CppUnitTest.h"

#include "PKB/Entity.h"
#include "PKB/ExpressionProcessor.h"
#include "SP/ASTValidator.h"
#include "SP/DesignExtractor.h"
#include "SP/Parser.h"

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

	const char* sampleSource4 =
		"procedure main { \n "
		"	flag = 0; \n "
		"	call computeCentroid; \n "
		"	call printResults; \n "
		" } \n "

		"procedure readPoint{ \n "
		"  read x; \n "
		"  read y; \n "
		"} \n "

		"procedure printResults{ \n "
		"     print flag; \n "
		"     print cenX; \n "
		"     print cenY; \n "
		"     print normSq; \n "
		"} \n "

		"procedure computeCentroid{ \n "
		"     count = 0; \n "
		"     cenX = 0; \n "
		"     cenY = 0; \n "
		"     call readPoint; \n "
		"     while ((x != 0) && (y != 0)) { \n "
		"          count = count + 1; \n "
		"          cenX = cenX + x; \n "
		"          cenY = cenY + y; \n "
		"          call readPoint; \n "
		"	  } \n "
		"     if (count == 0) then { \n "
		"          flag = 1; \n "
		"	  } else { \n "
		"         cenX = cenX / count; \n "
		"         cenY = cenY / count; \n "
		" 	  } \n "
		"      normSq = cenX * cenX + cenY * cenY; \n "
		"} \n ";

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

	const char* multipleProceduresRecursiveCallsSource =
		"procedure first { \n "
		"	call second; \n "
		"	call third; \n "
		"} \n"
		"procedure second { \n "
		"	call fourth; \n "
		"	x = y; \n "
		"} \n"
		"procedure third { \n "
		"	read x; \n "
		"} \n"
		"procedure fourth { \n "
		"	z = 2; \n "
		"} \n";

	TEST_METHOD_CLEANUP(cleanUpTables) {
		Attribute::performCleanUp();
		Container::performCleanUp();
		Calls::performCleanUp();
		CallsT::performCleanUp();
		Entity::performCleanUp();
		Follows::performCleanUp();
		FollowsT::performCleanUp();
		ModifiesP::performCleanUp();
		ModifiesS::performCleanUp();
		Next::performCleanUp();
		Pattern::performCleanUp();
		Parent::performCleanUp();
		ParentT::performCleanUp();
		UsesP::performCleanUp();
		UsesS::performCleanUp();
	}

	TEST_METHOD_INITIALIZE(initTables) {
		Attribute::performCleanUp();
		Container::performCleanUp();
		Calls::performCleanUp();
		CallsT::performCleanUp();
		Entity::performCleanUp();
		Follows::performCleanUp();
		FollowsT::performCleanUp();
		ModifiesP::performCleanUp();
		ModifiesS::performCleanUp();
		Next::performCleanUp();
		Pattern::performCleanUp();
		Parent::performCleanUp();
		ParentT::performCleanUp();
		UsesP::performCleanUp();
		UsesS::performCleanUp();
	}
public:

	TEST_METHOD(blackBoxSampleSource1_checkEntities) {
		std::string varName1 = "num1";
		std::string varName2 = "ave";
		std::string procName = "computeAverage";
		VarIndex constVal = 3;

		ProcIndex procIdx = ProcIndex(1);

		SourceAST ast = Parser::parse(sampleSource1);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(6), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(3), Entity::getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(2), Entity::getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(5), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[4]));

		Assert::AreEqual(size_t(1), Entity::getAllConsts().size());
		Assert::AreEqual(constVal, Entity::getAllConsts()[0]);

		Assert::AreEqual(size_t(6), Entity::getAllProcStmts()[procIdx].size());
		Assert::AreEqual(size_t(6), Entity::getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(blackBoxSampleSource1_checkAttributes) {
		std::string varName1 = "num1";
		std::string varName2 = "ave";
		std::string procName = "computeAverage";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		StmtIndex stmtIdx6 = StmtIndex(6);

		SourceAST ast = Parser::parse(sampleSource1);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(varName1, Attribute::getAttributeNameByStmtIdx(stmtIdx1));
		Assert::AreEqual(varName2, Attribute::getAttributeNameByStmtIdx(stmtIdx6));
		Assert::IsTrue(std::vector{ stmtIdx1 } == Attribute::getEqualNameAttributesFromName(EntityType::READ, varName1));
		Assert::IsTrue(std::vector{ stmtIdx6 } == Attribute::getEqualNameAttributesFromName(EntityType::PRINT, varName2));
		Assert::IsTrue(std::vector{ procIdx } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::IsTrue(std::vector{ 3 } == Attribute::getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsFalse(Attribute::hasEqualIntegerAttribute(EntityType::STMT, 7));
		Assert::IsFalse(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::READ, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 3));
	}

	TEST_METHOD(blackBoxSampleSource1_checkPattern) {
		std::string varName1 = "num1";
		std::string varName2 = "sum";
		std::string varName3 = "num2";

		VarIndex varIdx = VarIndex(1);

		SourceAST ast = Parser::parse(sampleSource1);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName3)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("num1+num2")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("num1+num2+num3")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("num1+((num2))")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("(num1+num2)+num3")).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("num1+num3")).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("num2+num3")).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("num1+(num2+num3)")).size());

		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("num1+num2+num3")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("(num1+num2)+num3")).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("num1+num2")).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName1)).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName3))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("num1+num2"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("num1+num2+num3"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("num1+((num2))"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("(num1+num2)+num3"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("num1+num3"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("num2+num3"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("num1+(num2+num3)"))).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("num1+num2+num3"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("(num1+num2)+num3"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("num1+num2"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getIfStmtsFromVar(varIdx).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getWhileStmtsFromVar(varIdx).size());
	}

	TEST_METHOD(blackBoxSampleSource1_checkRelationships) {
		std::string varName1 = "num1";
		std::string varName2 = "sum";
		std::string varName3 = "num2";

		ProcIndex procIdx = ProcIndex(1);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);

		SourceAST ast = Parser::parse(sampleSource1);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(5), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesS::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx4, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx2, Entity::getVarIdx(varName3)));

		Assert::AreEqual(size_t(5), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesS::contains(stmtIdx4, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx4, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx5, Entity::getVarIdx(varName2)));

		Assert::AreEqual(size_t(5), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName3)));

		Assert::AreEqual(size_t(5), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName3)));

		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(5), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Next::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx4, stmtIdx5));

		Assert::IsTrue(Follows::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx4, stmtIdx5));
		Assert::AreEqual(size_t(5), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx5));
		Assert::AreEqual(size_t(15), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
	}

	TEST_METHOD(blackBoxSampleSource2_checkEntities) {
		std::string varName1 = "num1";
		std::string varName2 = "temp";
		std::string procName = "printAscending";
		VarIndex constVal1 = 0;
		VarIndex constVal2 = 1;

		ProcIndex procIdx = ProcIndex(1);

		SourceAST ast = Parser::parse(sampleSource2);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(11), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(2), Entity::getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(5), Entity::getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(3), Entity::getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(4), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[3]));

		Assert::AreEqual(size_t(2), Entity::getAllConsts().size());
		Assert::AreEqual(constVal1, Entity::getAllConsts()[0]);
		Assert::AreEqual(constVal2, Entity::getAllConsts()[1]);

		Assert::AreEqual(size_t(11), Entity::getAllProcStmts()[procIdx].size());
		Assert::AreEqual(size_t(11), Entity::getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(blackBoxSampleSource2_checkAttributes) {
		std::string varName1 = "num1";
		std::string varName2 = "num2";
		std::string varName3 = "temp";
		std::string varName4 = "noSwap";
		std::string procName = "printAscending";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx9 = StmtIndex(9);
		StmtIndex stmtIdx10 = StmtIndex(10);
		StmtIndex stmtIdx11 = StmtIndex(11);

		SourceAST ast = Parser::parse(sampleSource2);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(varName1, Attribute::getAttributeNameByStmtIdx(stmtIdx1));
		Assert::AreEqual(varName2, Attribute::getAttributeNameByStmtIdx(stmtIdx2));
		Assert::AreEqual(varName1, Attribute::getAttributeNameByStmtIdx(stmtIdx9));
		Assert::AreEqual(varName2, Attribute::getAttributeNameByStmtIdx(stmtIdx10));
		Assert::AreEqual(varName4, Attribute::getAttributeNameByStmtIdx(stmtIdx11));
		Assert::IsTrue(std::vector{ stmtIdx1 } == Attribute::getEqualNameAttributesFromName(EntityType::READ, varName1));
		Assert::IsTrue(std::vector{ stmtIdx2 } == Attribute::getEqualNameAttributesFromName(EntityType::READ, varName2));
		Assert::IsTrue(std::vector{ stmtIdx9 } == Attribute::getEqualNameAttributesFromName(EntityType::PRINT, varName1));
		Assert::IsTrue(std::vector{ stmtIdx10 } == Attribute::getEqualNameAttributesFromName(EntityType::PRINT, varName2));
		Assert::IsTrue(std::vector{ stmtIdx11 } == Attribute::getEqualNameAttributesFromName(EntityType::PRINT, varName4));
		Assert::IsTrue(std::vector{ procIdx } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::IsTrue(std::vector{ 1 } == Attribute::getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 1));
	}

	TEST_METHOD(blackBoxSampleSource2_checkPattern) {
		std::string varName1 = "num1";
		std::string varName2 = "num2";
		std::string varName3 = "temp";

		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);

		SourceAST ast = Parser::parse(sampleSource2);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(5), std::get<0>(Pattern::getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName3)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix("((num1))")).size());

		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName3)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix("((num1))")).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName1)).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName3))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("((num1))"))).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName3))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("((num1))"))).size());

		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(1), Pattern::getIfStmtsFromVar(varIdx1).size());
		Assert::AreEqual(size_t(1), Pattern::getIfStmtsFromVar(varIdx2).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getWhileStmtsFromVar(varIdx1).size());
	}

	TEST_METHOD(blackBoxSampleSource2_checkRelationships) {
		std::string varName1 = "num1";
		std::string varName2 = "num2";
		std::string varName3 = "temp";
		std::string varName4 = "noSwap";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		StmtIndex stmtIdx6 = StmtIndex(6);
		StmtIndex stmtIdx7 = StmtIndex(7);
		StmtIndex stmtIdx8 = StmtIndex(8);
		StmtIndex stmtIdx9 = StmtIndex(9);

		SourceAST ast = Parser::parse(sampleSource2);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(11), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesS::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx4, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx4, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx4, Entity::getVarIdx(varName4)));

		Assert::AreEqual(size_t(9), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesS::contains(stmtIdx4, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx4, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx5, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx6, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx7, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx9, Entity::getVarIdx(varName1)));
		Assert::AreEqual(size_t(3), UsesS::getFromVariable(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(4), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName4)));

		Assert::AreEqual(size_t(4), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName4)));

		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(11), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Next::containsSuccessor(stmtIdx4, stmtIdx5));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx4, stmtIdx8));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx5, stmtIdx6));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx7, stmtIdx9));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx8, stmtIdx9));

		Assert::AreEqual(size_t(8), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx1, stmtIdx4));
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx4, stmtIdx5));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx4, stmtIdx9));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx5, stmtIdx6));
		Assert::AreEqual(size_t(24), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx4));
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx5, stmtIdx7));

		Assert::AreEqual(size_t(4), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx4, stmtIdx5));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx4, stmtIdx6));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx4, stmtIdx7));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx4, stmtIdx8));
		Assert::AreEqual(size_t(4), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
	}

	TEST_METHOD(blackBoxSampleSource3_checkEntities) {
		std::string varName1 = "number";
		std::string varName2 = "digit";
		std::string procName = "sumDigits";
		VarIndex constVal1 = 0;
		VarIndex constVal2 = 10;

		ProcIndex procIdx = ProcIndex(1);

		SourceAST ast = Parser::parse(sampleSource3);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(7), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(4), Entity::getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(3), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[2]));

		Assert::AreEqual(size_t(2), Entity::getAllConsts().size());
		Assert::AreEqual(constVal1, Entity::getAllConsts()[0]);
		Assert::AreEqual(constVal2, Entity::getAllConsts()[1]);

		Assert::AreEqual(size_t(7), Entity::getAllProcStmts()[procIdx].size());
		Assert::AreEqual(size_t(7), Entity::getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(blackBoxSampleSource3_checkAttributes) {
		std::string varName1 = "number";
		std::string varName2 = "sum";
		std::string varName3 = "digit";
		std::string procName = "sumDigits";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx7 = StmtIndex(7);

		SourceAST ast = Parser::parse(sampleSource3);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(varName1, Attribute::getAttributeNameByStmtIdx(stmtIdx1));
		Assert::AreEqual(varName2, Attribute::getAttributeNameByStmtIdx(stmtIdx7));
		Assert::IsTrue(std::vector{ stmtIdx1 } == Attribute::getEqualNameAttributesFromName(EntityType::READ, varName1));
		Assert::IsTrue(std::vector{ stmtIdx7 } == Attribute::getEqualNameAttributesFromName(EntityType::PRINT, varName2));
		Assert::IsTrue(std::vector{ procIdx } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::AreEqual(size_t(0), Attribute::getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT).size());
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::WHILE, 3));
		Assert::IsFalse(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 0));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 0));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 10));
		Assert::IsFalse(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 1));
	}

	TEST_METHOD(blackBoxSampleSource3_checkPattern) {
		std::string varName1 = "number";
		std::string varName2 = "digit";

		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);

		SourceAST ast = Parser::parse(sampleSource3);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(4), std::get<0>(Pattern::getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName1)).size());

		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName1)).size());

		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("number%10"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("number%((10))"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("sum+digit"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("digit+sum"))).size());
		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("10"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("1"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("10%number"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getIfStmtsFromVar(varIdx1).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
		Assert::AreEqual(size_t(1), Pattern::getWhileStmtsFromVar(varIdx1).size());
	}

	TEST_METHOD(blackBoxSampleSource3_checkRelationships) {
		std::string varName1 = "number";
		std::string varName2 = "digit";
		std::string varName3 = "sum";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		StmtIndex stmtIdx6 = StmtIndex(6);
		StmtIndex stmtIdx7 = StmtIndex(7);

		SourceAST ast = Parser::parse(sampleSource3);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(8), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesS::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx2, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx3, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx3, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx3, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx4, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx5, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx6, Entity::getVarIdx(varName1)));

		Assert::AreEqual(size_t(8), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesS::contains(stmtIdx3, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx3, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx3, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx4, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx5, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx5, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx6, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx7, Entity::getVarIdx(varName3)));
		Assert::AreEqual(size_t(3), UsesS::getFromVariable(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(3), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName3)));

		Assert::AreEqual(size_t(3), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName3)));

		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(7), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Next::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx4, stmtIdx5));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx5, stmtIdx6));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx6, stmtIdx3));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx3, stmtIdx7));
		Assert::IsFalse(Next::containsSuccessor(stmtIdx6, stmtIdx7));

		Assert::AreEqual(size_t(5), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx1, stmtIdx3));
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx4, stmtIdx5));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx3, stmtIdx7));
		Assert::AreEqual(size_t(9), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx3));
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx4, stmtIdx6));

		Assert::AreEqual(size_t(3), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx3, stmtIdx5));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx3, stmtIdx6));
		Assert::AreEqual(size_t(3), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx3, stmtIdx5));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx3, stmtIdx6));
	}

	TEST_METHOD(blackBoxSampleSource4_checkEntities) {
		std::string varName1 = "flag";
		std::string varName3 = "y";
		std::string procName1 = "main";
		std::string procName4 = "computeCentroid";
		VarIndex constVal1 = 0;
		VarIndex constVal2 = 1;

		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		ProcIndex procIdx3 = ProcIndex(3);
		ProcIndex procIdx4 = ProcIndex(4);

		SourceAST ast = Parser::parse(sampleSource4);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(4), Entity::getAllProcs().size());
		Assert::AreEqual(procName1, Entity::getProcName(Entity::getAllProcs()[0]));
		Assert::AreEqual(procName4, Entity::getProcName(Entity::getAllProcs()[3]));

		Assert::AreEqual(size_t(23), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(4), Entity::getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(2), Entity::getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(11), Entity::getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(4), Entity::getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(7), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName3, Entity::getVarName(Entity::getAllVars()[2]));

		Assert::AreEqual(size_t(2), Entity::getAllConsts().size());
		Assert::AreEqual(constVal1, Entity::getAllConsts()[0]);
		Assert::AreEqual(constVal2, Entity::getAllConsts()[1]);

		Assert::AreEqual(size_t(3), Entity::getStmtsFromProc(procIdx1).size());
		Assert::AreEqual(size_t(2), Entity::getStmtsFromProc(procIdx2).size());
		Assert::AreEqual(size_t(4), Entity::getStmtsFromProc(procIdx3).size());
		Assert::AreEqual(size_t(14), Entity::getStmtsFromProc(procIdx4).size());
	}

	TEST_METHOD(blackBoxSampleSource4_checkAttributes) {
		std::string varName1 = "flag";
		std::string varName2 = "x";
		std::string varName3 = "y";
		std::string procName1 = "main";
		std::string procName2 = "readPoint";
		std::string procName3 = "printResults";
		std::string procName4 = "computeCentroid";

		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		StmtIndex stmtIdx6 = StmtIndex(6);

		SourceAST ast = Parser::parse(sampleSource4);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(procName4, Attribute::getAttributeNameByStmtIdx(stmtIdx2));
		Assert::AreEqual(procName3, Attribute::getAttributeNameByStmtIdx(stmtIdx3));
		Assert::AreEqual(varName2, Attribute::getAttributeNameByStmtIdx(stmtIdx4));
		Assert::AreEqual(varName3, Attribute::getAttributeNameByStmtIdx(stmtIdx5));
		Assert::AreEqual(varName1, Attribute::getAttributeNameByStmtIdx(stmtIdx6));
		Assert::IsTrue(std::vector{ stmtIdx2 } == Attribute::getEqualNameAttributesFromName(EntityType::CALL, procName4));
		Assert::IsTrue(std::vector{ stmtIdx3 } == Attribute::getEqualNameAttributesFromName(EntityType::CALL, procName3));
		Assert::IsTrue(std::vector{ stmtIdx4 } == Attribute::getEqualNameAttributesFromName(EntityType::READ, varName2));
		Assert::IsTrue(std::vector{ stmtIdx5 } == Attribute::getEqualNameAttributesFromName(EntityType::READ, varName3));
		Assert::IsTrue(std::vector{ stmtIdx6 } == Attribute::getEqualNameAttributesFromName(EntityType::PRINT, varName1));
		Assert::IsTrue(std::vector{ procIdx1 } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName1));
		Assert::IsTrue(std::vector{ procIdx2 } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName2));
		Assert::IsTrue(std::vector{ 1 } == Attribute::getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CALL, 3));
		Assert::IsFalse(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 0));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 1));
	}

	TEST_METHOD(blackBoxSampleSource4_checkPattern) {
		std::string varName1 = "flag";
		std::string varName2 = "x";
		std::string varName3 = "y";
		std::string varName4 = "cenX";
		std::string varName5 = "cenY";

		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);

		SourceAST ast = Parser::parse(sampleSource4);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(11), std::get<0>(Pattern::getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName4), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(2), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName4), ExpressionProcessor::convertInfixToPostFix(varName4)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName5), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(2), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName5), ExpressionProcessor::convertInfixToPostFix(varName5)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName5), ExpressionProcessor::convertInfixToPostFix(varName3)).size());

		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName4), ExpressionProcessor::convertInfixToPostFix("cenX/count")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName4), ExpressionProcessor::convertInfixToPostFix("cenX/(count)")).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(3), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName4))).size());
		Assert::AreEqual(size_t(3), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName5))).size());
		Assert::AreEqual(size_t(4), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());
		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("1"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(4), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getIfStmtsFromVar(varIdx1).size());

		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getWhileStmtsFromVar(varIdx1).size());
		Assert::AreEqual(size_t(1), Pattern::getWhileStmtsFromVar(varIdx2).size());
	}

	TEST_METHOD(blackBoxSampleSource4_checkRelationships) {
		std::string varName1 = "flag";
		std::string varName2 = "x";
		std::string varName3 = "y";
		std::string varName4 = "cenX";
		std::string varName5 = "cenY";

		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		ProcIndex procIdx3 = ProcIndex(3);
		ProcIndex procIdx4 = ProcIndex(4);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		StmtIndex stmtIdx6 = StmtIndex(6);
		StmtIndex stmtIdx7 = StmtIndex(7);
		StmtIndex stmtIdx8 = StmtIndex(8);
		StmtIndex stmtIdx9 = StmtIndex(9);
		StmtIndex stmtIdx14 = StmtIndex(14);
		StmtIndex stmtIdx15 = StmtIndex(15);
		StmtIndex stmtIdx18 = StmtIndex(18);
		StmtIndex stmtIdx19 = StmtIndex(19);
		StmtIndex stmtIdx20 = StmtIndex(20);
		StmtIndex stmtIdx23 = StmtIndex(23);

		SourceAST ast = Parser::parse(sampleSource4);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(32), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesS::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx2, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx2, Entity::getVarIdx(varName4)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx2, Entity::getVarIdx(varName5)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx3, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx4, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx5, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx14, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx14, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx14, Entity::getVarIdx(varName4)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx14, Entity::getVarIdx(varName5)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx18, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx18, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx5, Entity::getVarIdx(varName3)));

		Assert::AreEqual(size_t(32), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::IsFalse(UsesS::contains(stmtIdx2, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx2, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx2, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx2, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesS::contains(stmtIdx2, Entity::getVarIdx(varName5)));
		Assert::IsTrue(UsesS::contains(stmtIdx3, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx3, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesS::contains(stmtIdx3, Entity::getVarIdx(varName5)));
		Assert::IsTrue(UsesS::contains(stmtIdx6, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx7, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesS::contains(stmtIdx8, Entity::getVarIdx(varName5)));
		Assert::IsTrue(UsesS::contains(stmtIdx14, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx14, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx14, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesS::contains(stmtIdx14, Entity::getVarIdx(varName5)));
		Assert::IsTrue(UsesS::contains(stmtIdx19, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesS::contains(stmtIdx19, Entity::getVarIdx(varName5)));
		Assert::AreEqual(size_t(2), UsesS::getFromVariable(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(16), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesP::contains(procIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesP::contains(procIdx1, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesP::contains(procIdx1, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesP::contains(procIdx1, Entity::getVarIdx(varName4)));
		Assert::IsTrue(ModifiesP::contains(procIdx1, Entity::getVarIdx(varName5)));
		Assert::IsTrue(ModifiesP::contains(procIdx2, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesP::contains(procIdx2, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesP::contains(procIdx4, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesP::contains(procIdx4, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesP::contains(procIdx4, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesP::contains(procIdx4, Entity::getVarIdx(varName4)));
		Assert::IsTrue(ModifiesP::contains(procIdx4, Entity::getVarIdx(varName5)));

		Assert::AreEqual(size_t(16), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesP::contains(procIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesP::contains(procIdx1, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesP::contains(procIdx1, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesP::contains(procIdx1, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesP::contains(procIdx1, Entity::getVarIdx(varName5)));
		Assert::IsTrue(UsesP::contains(procIdx3, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesP::contains(procIdx3, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesP::contains(procIdx3, Entity::getVarIdx(varName5)));
		Assert::IsTrue(UsesP::contains(procIdx4, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesP::contains(procIdx4, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesP::contains(procIdx4, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesP::contains(procIdx4, Entity::getVarIdx(varName5)));

		Assert::AreEqual(size_t(3), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Calls::containsSuccessor(procIdx1, procIdx4));
		Assert::IsTrue(Calls::containsSuccessor(procIdx1, procIdx3));
		Assert::IsFalse(Calls::containsSuccessor(procIdx1, procIdx2));
		Assert::IsTrue(Calls::containsSuccessor(procIdx4, procIdx2));
		Assert::AreEqual(size_t(4), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(CallsT::containsSuccessor(procIdx1, procIdx2));
		Assert::IsTrue(CallsT::containsSuccessor(procIdx1, procIdx3));

		Assert::AreEqual(size_t(21), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Next::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsFalse(Next::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx4, stmtIdx5));
		Assert::IsFalse(Next::containsSuccessor(stmtIdx5, stmtIdx6));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx6, stmtIdx7));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx7, stmtIdx8));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx8, stmtIdx9));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx14, stmtIdx15));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx18, stmtIdx14));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx14, stmtIdx19));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx19, stmtIdx20));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx20, stmtIdx23));

		Assert::AreEqual(size_t(16), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx1, stmtIdx3));
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx4, stmtIdx5));
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx5, stmtIdx6));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx14, stmtIdx19));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx19, stmtIdx23));
		Assert::AreEqual(size_t(38), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx3));
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx14, stmtIdx23));

		Assert::AreEqual(size_t(7), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx14, stmtIdx15));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx19, stmtIdx20));
		Assert::AreEqual(size_t(7), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
	}

	TEST_METHOD(mixedStmtsSource_checkEntities) {
		std::string varName1 = "count";
		std::string varName2 = "normSq";
		std::string procName = "computeCentroid";
		VarIndex constVal1 = 0;
		VarIndex constVal2 = 5;
		VarIndex constVal3 = 3;
		VarIndex constVal4 = 1;

		ProcIndex procIdx = ProcIndex(1);

		SourceAST ast = Parser::parse(mixedStmtsSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(14), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(11), Entity::getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(7), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[6]));

		Assert::AreEqual(size_t(4), Entity::getAllConsts().size());
		Assert::AreEqual(constVal1, Entity::getAllConsts()[0]);
		Assert::AreEqual(constVal2, Entity::getAllConsts()[1]);
		Assert::AreEqual(constVal3, Entity::getAllConsts()[2]);
		Assert::AreEqual(constVal4, Entity::getAllConsts()[3]);

		Assert::AreEqual(size_t(14), Entity::getAllProcStmts()[procIdx].size());
		Assert::AreEqual(size_t(14), Entity::getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(mixedStmtsSource_checkAttributes) {
		std::string varName1 = "count";
		std::string varName2 = "normSq";
		std::string procName = "computeCentroid";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx12 = StmtIndex(12);
		StmtIndex stmtIdx13 = StmtIndex(13);

		SourceAST ast = Parser::parse(mixedStmtsSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(varName1, Attribute::getAttributeNameByStmtIdx(stmtIdx13));
		Assert::IsTrue(std::vector{ stmtIdx13 } == Attribute::getEqualNameAttributesFromName(EntityType::PRINT, varName1));
		Assert::IsTrue(std::vector{ procIdx } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::IsTrue(std::vector{ 5, 3, 1 } == Attribute::getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::PRINT, 13));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 1));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 5));
	}

	TEST_METHOD(mixedStmtsSource_checkPattern) {
		std::string varName1 = "count";
		std::string varName2 = "x";
		std::string varName3 = "flag";

		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx4 = VarIndex(4);
		VarIndex varIdx5 = VarIndex(5);

		SourceAST ast = Parser::parse(mixedStmtsSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(11), std::get<0>(Pattern::getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("5")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix("1")).size());

		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("5")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("count+1")).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("1+count")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix("1")).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName3))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("x+3"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("3+x"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("x+3"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("3+x"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("cenX*cenX"))).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(1), Pattern::getIfStmtsFromVar(varIdx1).size());

		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
		Assert::AreEqual(size_t(1), Pattern::getWhileStmtsFromVar(varIdx4).size());
		Assert::AreEqual(size_t(1), Pattern::getWhileStmtsFromVar(varIdx5).size());
	}

	TEST_METHOD(mixedStmtsSource_checkRelationships) {
		std::string varName1 = "count";
		std::string varName2 = "x";
		std::string varName3 = "flag";
		std::string varName4 = "cenX";
		std::string varName5 = "cenY";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx6 = StmtIndex(6);
		StmtIndex stmtIdx7 = StmtIndex(7);
		StmtIndex stmtIdx8 = StmtIndex(8);
		StmtIndex stmtIdx9 = StmtIndex(9);
		StmtIndex stmtIdx10 = StmtIndex(10);
		StmtIndex stmtIdx11 = StmtIndex(11);
		StmtIndex stmtIdx12 = StmtIndex(12);
		StmtIndex stmtIdx13 = StmtIndex(13);
		StmtIndex stmtIdx14 = StmtIndex(14);

		SourceAST ast = Parser::parse(mixedStmtsSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(16), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesS::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx6, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx6, Entity::getVarIdx(varName4)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx6, Entity::getVarIdx(varName5)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx10, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx11, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx12, Entity::getVarIdx(varName4)));
		Assert::AreEqual(size_t(3), ModifiesS::getFromVariable(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(16), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesS::contains(stmtIdx6, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx6, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx6, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesS::contains(stmtIdx6, Entity::getVarIdx(varName5)));
		Assert::IsTrue(UsesS::contains(stmtIdx7, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx8, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx8, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesS::contains(stmtIdx9, Entity::getVarIdx(varName5)));
		Assert::AreEqual(size_t(4), UsesS::getFromVariable(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(6), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName4)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName5)));

		Assert::AreEqual(size_t(5), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName2)));
		Assert::IsFalse(UsesP::contains(procIdx, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName5)));

		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(15), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Next::containsSuccessor(stmtIdx6, stmtIdx7));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx7, stmtIdx8));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx9, stmtIdx6));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx10, stmtIdx11));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx10, stmtIdx12));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx12, stmtIdx13));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx11, stmtIdx14));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx13, stmtIdx14));

		Assert::AreEqual(size_t(10), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx1, stmtIdx6));
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx6, stmtIdx7));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx7, stmtIdx8));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx6, stmtIdx10));
		Assert::AreEqual(size_t(32), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx6));
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx6, stmtIdx14));

		Assert::AreEqual(size_t(6), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx6, stmtIdx7));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx10, stmtIdx11));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx10, stmtIdx12));
		Assert::AreEqual(size_t(6), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
	}

	TEST_METHOD(nestedWhileIfSource_checkEntities) {
		std::string varName1 = "present";
		std::string varName2 = "x";
		std::string procName = "Peter";
		VarIndex constVal1 = 0;
		VarIndex constVal2 = 8;
		VarIndex constVal3 = 4;
		VarIndex constVal4 = 2;
		VarIndex constVal5 = 1;

		ProcIndex procIdx = ProcIndex(1);

		SourceAST ast = Parser::parse(nestedWhileIfSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(12), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(8), Entity::getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(2), Entity::getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(5), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[4]));

		Assert::AreEqual(size_t(5), Entity::getAllConsts().size());
		Assert::AreEqual(constVal1, Entity::getAllConsts()[0]);
		Assert::AreEqual(constVal2, Entity::getAllConsts()[1]);
		Assert::AreEqual(constVal3, Entity::getAllConsts()[2]);
		Assert::AreEqual(constVal4, Entity::getAllConsts()[3]);
		Assert::AreEqual(constVal5, Entity::getAllConsts()[4]);

		Assert::AreEqual(size_t(12), Entity::getAllProcStmts()[procIdx].size());
		Assert::AreEqual(size_t(12), Entity::getStmtsFromProc(procIdx).size());
	}
	TEST_METHOD(nestedWhileIfSource_checkAttributes) {
		std::string varName1 = "present";
		std::string varName2 = "future";
		std::string procName = "Peter";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx12 = StmtIndex(12);

		SourceAST ast = Parser::parse(nestedWhileIfSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(varName1, Attribute::getAttributeNameByStmtIdx(stmtIdx1));
		Assert::AreEqual(varName2, Attribute::getAttributeNameByStmtIdx(stmtIdx12));
		Assert::IsTrue(std::vector{ stmtIdx1 } == Attribute::getEqualNameAttributesFromName(EntityType::PRINT, varName1));
		Assert::IsTrue(std::vector{ stmtIdx12 } == Attribute::getEqualNameAttributesFromName(EntityType::PRINT, varName2));
		Assert::IsTrue(std::vector{ procIdx } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::IsTrue(std::vector{ 8, 4, 2, 1 } == Attribute::getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::PRINT, 1));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::PRINT, 12));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 8));
	}

	TEST_METHOD(nestedWhileIfSource_checkPattern) {
		std::string varName1 = "present";
		std::string varName2 = "future";
		std::string varName3 = "y";
		std::string varName4 = "z";
		std::string varName5 = "x";

		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);
		VarIndex varIdx5 = VarIndex(5);

		SourceAST ast = Parser::parse(nestedWhileIfSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(8), std::get<0>(Pattern::getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(2), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("z-8")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("x*y")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("x*y")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("x*y*z")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("x*y*z")).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("y*z")).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("z-8-(x*y)")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("8")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix(varName3)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName4), ExpressionProcessor::convertInfixToPostFix("2")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName5), ExpressionProcessor::convertInfixToPostFix(varName5)).size());

		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("8*present")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName5), ExpressionProcessor::convertInfixToPostFix("x-1")).size());

		Assert::AreEqual(size_t(5), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("z-8"))).size());
		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("x*y"))).size());
		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("x*y*z"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("z-8-(x*y)"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("8-(x*y)"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("z-8"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("8*present"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("x-1"))).size());

		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(1), Pattern::getIfStmtsFromVar(varIdx1).size());
		Assert::AreEqual(size_t(0), Pattern::getIfStmtsFromVar(varIdx2).size());
		Assert::AreEqual(size_t(1), Pattern::getIfStmtsFromVar(varIdx5).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
		Assert::AreEqual(size_t(1), Pattern::getWhileStmtsFromVar(varIdx1).size());
	}

	TEST_METHOD(nestedWhileIfSource_checkRelationships) {
		std::string varName1 = "present";
		std::string varName2 = "future";
		std::string varName3 = "y";
		std::string varName4 = "z";
		std::string varName5 = "x";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx5 = StmtIndex(5);
		StmtIndex stmtIdx6 = StmtIndex(6);
		StmtIndex stmtIdx7 = StmtIndex(7);
		StmtIndex stmtIdx8 = StmtIndex(8);
		StmtIndex stmtIdx9 = StmtIndex(9);
		StmtIndex stmtIdx10 = StmtIndex(10);
		StmtIndex stmtIdx11 = StmtIndex(11);
		StmtIndex stmtIdx12 = StmtIndex(12);

		SourceAST ast = Parser::parse(nestedWhileIfSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(14), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesS::contains(stmtIdx2, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx3, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx5, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx5, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx5, Entity::getVarIdx(varName5)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx7, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx7, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx7, Entity::getVarIdx(varName5)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx8, Entity::getVarIdx(varName1)));
		Assert::AreEqual(size_t(3), ModifiesS::getFromVariable(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(27), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesS::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx2, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx3, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx5, Entity::getVarIdx(varName1)));
		Assert::IsFalse(UsesS::contains(stmtIdx5, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx5, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx5, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesS::contains(stmtIdx5, Entity::getVarIdx(varName5)));
		Assert::IsTrue(UsesS::contains(stmtIdx7, Entity::getVarIdx(varName1)));
		Assert::IsFalse(UsesS::contains(stmtIdx7, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx7, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx7, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesS::contains(stmtIdx7, Entity::getVarIdx(varName5)));
		Assert::IsTrue(UsesS::contains(stmtIdx8, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx8, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesS::contains(stmtIdx8, Entity::getVarIdx(varName5)));
		Assert::IsTrue(UsesS::contains(stmtIdx12, Entity::getVarIdx(varName2)));
		Assert::AreEqual(size_t(8), UsesS::getFromVariable(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(5), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName4)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName5)));

		Assert::AreEqual(size_t(5), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName5)));

		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(13), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Next::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx5, stmtIdx6));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx6, stmtIdx7));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx7, stmtIdx8));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx7, stmtIdx10));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx8, stmtIdx9));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx9, stmtIdx5));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx10, stmtIdx5));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx5, stmtIdx11));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx11, stmtIdx12));

		Assert::AreEqual(size_t(8), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx5, stmtIdx11));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx6, stmtIdx7));
		Assert::AreEqual(size_t(23), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx2, stmtIdx11));
		Assert::IsFalse(FollowsT::containsSuccessor(stmtIdx8, stmtIdx10));

		Assert::AreEqual(size_t(5), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx5, stmtIdx6));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx5, stmtIdx7));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx7, stmtIdx8));
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx7, stmtIdx10));
		Assert::AreEqual(size_t(8), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx5, stmtIdx8));
		Assert::IsTrue(ParentT::containsSuccessor(stmtIdx5, stmtIdx10));
	}

	TEST_METHOD(multipleBracketsSource_checkEntities) {
		std::string varName1 = "number";
		std::string varName2 = "digit";
		std::string procName = "sumDigits";
		VarIndex constVal1 = 0;
		VarIndex constVal2 = 10;

		ProcIndex procIdx = ProcIndex(1);

		SourceAST ast = Parser::parse(multipleBracketsSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(7), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(4), Entity::getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(3), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[2]));

		Assert::AreEqual(size_t(2), Entity::getAllConsts().size());
		Assert::AreEqual(constVal1, Entity::getAllConsts()[0]);
		Assert::AreEqual(constVal2, Entity::getAllConsts()[1]);

		Assert::AreEqual(size_t(7), Entity::getAllProcStmts()[procIdx].size());
		Assert::AreEqual(size_t(7), Entity::getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(multipleBracketsSource_checkAttributes) {
		std::string varName1 = "number";
		std::string varName2 = "sum";
		std::string varName3 = "digit";
		std::string procName = "sumDigits";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx7 = StmtIndex(7);

		SourceAST ast = Parser::parse(multipleBracketsSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(varName1, Attribute::getAttributeNameByStmtIdx(stmtIdx1));
		Assert::AreEqual(varName2, Attribute::getAttributeNameByStmtIdx(stmtIdx7));
		Assert::IsTrue(std::vector{ stmtIdx1 } == Attribute::getEqualNameAttributesFromName(EntityType::READ, varName1));
		Assert::IsTrue(std::vector{ stmtIdx7 } == Attribute::getEqualNameAttributesFromName(EntityType::PRINT, varName2));
		Assert::IsTrue(std::vector{ procIdx } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::AreEqual(size_t(0), Attribute::getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT).size());
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::WHILE, 3));
		Assert::IsFalse(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsFalse(Attribute::hasEqualIntegerAttribute(EntityType::STMT, 8));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::READ, 1));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::PRINT, 7));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 0));
	}

	TEST_METHOD(multipleBracketsSource_checkPattern) {
		std::string varName1 = "number";
		std::string varName2 = "sum";
		std::string varName3 = "digit";

		VarIndex varIdx = VarIndex(1);

		SourceAST ast = Parser::parse(multipleBracketsSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(4), std::get<0>(Pattern::getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("10")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName3)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix("number%10")).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix("10%number")).size());

		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix("number%10")).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix("10%number")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("sum+digit")).size());

		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName3))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("number%10"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("(((number))%10)"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("10%number"))).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("number%10"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("((0))"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getIfStmtsFromVar(varIdx).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
		Assert::AreEqual(size_t(1), Pattern::getWhileStmtsFromVar(varIdx).size());
	}

	TEST_METHOD(multipleBracketsSource_checkRelationships) {
		std::string varName1 = "number";
		std::string varName2 = "sum";
		std::string varName3 = "digit";

		ProcIndex procIdx = ProcIndex(1);

		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		StmtIndex stmtIdx6 = StmtIndex(6);
		StmtIndex stmtIdx7 = StmtIndex(7);

		SourceAST ast = Parser::parse(multipleBracketsSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(8), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesS::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx2, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx3, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx3, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx3, Entity::getVarIdx(varName3)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx7, Entity::getVarIdx(varName2)));
		Assert::AreEqual(size_t(3), ModifiesS::getFromVariable(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(8), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::IsFalse(UsesS::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsFalse(UsesS::contains(stmtIdx2, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx3, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx3, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx3, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx4, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx5, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx5, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx6, Entity::getVarIdx(varName1)));
		Assert::AreEqual(size_t(3), UsesS::getFromVariable(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(3), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName3)));

		Assert::AreEqual(size_t(3), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName3)));

		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(7), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Next::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx4, stmtIdx5));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx5, stmtIdx6));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx6, stmtIdx3));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx3, stmtIdx7));
		Assert::IsFalse(Next::containsSuccessor(stmtIdx6, stmtIdx7));

		Assert::AreEqual(size_t(5), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx5, stmtIdx6));
		Assert::AreEqual(size_t(9), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx3));

		Assert::AreEqual(size_t(3), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Parent::containsSuccessor(stmtIdx3, stmtIdx5));
		Assert::AreEqual(size_t(3), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
	}

	TEST_METHOD(sameKeywordNameSource_checkEntities) {
		std::string varName1 = "read";
		std::string varName2 = "if";
		std::string varName3 = "procedure";
		std::string procName = "procedure";
		VarIndex constVal1 = 0;
		VarIndex constVal2 = 1;

		ProcIndex procIdx = ProcIndex(1);

		SourceAST ast = Parser::parse(sameKeywordNameSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(7), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(5), Entity::getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(5), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[1]));
		Assert::AreEqual(varName3, Entity::getVarName(Entity::getAllVars()[4]));

		Assert::AreEqual(size_t(2), Entity::getAllConsts().size());
		Assert::AreEqual(constVal1, Entity::getAllConsts()[0]);
		Assert::AreEqual(constVal2, Entity::getAllConsts()[1]);

		Assert::AreEqual(size_t(7), Entity::getAllProcStmts()[procIdx].size());
		Assert::AreEqual(size_t(7), Entity::getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(sameKeywordNameSource_checkAttributes) {
		std::string varName1 = "read";
		std::string varName2 = "print";
		std::string procName = "procedure";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx7 = StmtIndex(7);

		SourceAST ast = Parser::parse(sameKeywordNameSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(varName1, Attribute::getAttributeNameByStmtIdx(stmtIdx1));
		Assert::AreEqual(varName2, Attribute::getAttributeNameByStmtIdx(stmtIdx7));
		Assert::IsTrue(std::vector{ stmtIdx1 } == Attribute::getEqualNameAttributesFromName(EntityType::READ, varName1));
		Assert::IsTrue(std::vector{ stmtIdx7 } == Attribute::getEqualNameAttributesFromName(EntityType::PRINT, varName2));
		Assert::IsTrue(std::vector{ procIdx } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::IsTrue(std::vector{ 1 } == Attribute::getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::READ, 1));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsFalse(Attribute::hasEqualIntegerAttribute(EntityType::PRINT, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 2));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 4));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 5));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 6));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 0));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 1));
	}

	TEST_METHOD(sameKeywordNameSource_checkPattern) {
		std::string varName1 = "read";
		std::string varName2 = "if";
		std::string varName3 = "print";
		std::string varName4 = "while";
		std::string varName5 = "procedure";

		VarIndex varIdx = VarIndex(1);

		SourceAST ast = Parser::parse(sameKeywordNameSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(5), std::get<0>(Pattern::getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName4), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName5), ExpressionProcessor::convertInfixToPostFix("1")).size());

		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName4), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName5), ExpressionProcessor::convertInfixToPostFix("1")).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(4), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("1"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(4), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("1"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getIfStmtsFromVar(varIdx).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getWhileStmtsFromVar(varIdx).size());
	}

	TEST_METHOD(sameKeywordNameSource_checkRelationships) {
		std::string varName1 = "read";
		std::string varName2 = "if";
		std::string varName3 = "print";
		std::string varName4 = "while";
		std::string varName5 = "procedure";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		StmtIndex stmtIdx6 = StmtIndex(6);
		StmtIndex stmtIdx7 = StmtIndex(7);

		SourceAST ast = Parser::parse(sameKeywordNameSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(6), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesS::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx4, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx2, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx3, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx5, Entity::getVarIdx(varName4)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx6, Entity::getVarIdx(varName5)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx7, Entity::getVarIdx(varName3)));
		Assert::AreEqual(size_t(2), ModifiesS::getFromVariable(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(1), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesS::contains(stmtIdx7, Entity::getVarIdx(varName3)));
		Assert::IsFalse(UsesS::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsFalse(UsesS::contains(stmtIdx2, Entity::getVarIdx(varName2)));
		Assert::IsFalse(UsesS::contains(stmtIdx3, Entity::getVarIdx(varName3)));
		Assert::IsFalse(UsesS::contains(stmtIdx4, Entity::getVarIdx(varName1)));
		Assert::IsFalse(UsesS::contains(stmtIdx5, Entity::getVarIdx(varName4)));
		Assert::IsFalse(UsesS::contains(stmtIdx6, Entity::getVarIdx(varName5)));
		Assert::AreEqual(size_t(1), UsesS::getFromVariable(Entity::getVarIdx(varName3)).size());

		Assert::AreEqual(size_t(5), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName4)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName5)));

		Assert::AreEqual(size_t(1), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::IsFalse(UsesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsFalse(UsesP::contains(procIdx, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName3)));
		Assert::IsFalse(UsesP::contains(procIdx, Entity::getVarIdx(varName4)));
		Assert::IsFalse(UsesP::contains(procIdx, Entity::getVarIdx(varName5)));

		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(6), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Next::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx4, stmtIdx5));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx5, stmtIdx6));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx6, stmtIdx7));

		Assert::AreEqual(size_t(6), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx2, stmtIdx7));
		Assert::AreEqual(size_t(21), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx7));

		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
	}

	TEST_METHOD(longAssignmentExprSource_checkEntities) {
		std::string varName1 = "x";
		std::string varName2 = "g";
		std::string procName = "main";
		VarIndex constVal = 9;

		ProcIndex procIdx = ProcIndex(1);

		SourceAST ast = Parser::parse(longAssignmentExprSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(1), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::IF_TYPE).size());

		/* ordering: x, a, y, b, z, c, d, f, g */
		Assert::AreEqual(size_t(9), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[8]));

		Assert::AreEqual(size_t(1), Entity::getAllConsts().size());
		Assert::AreEqual(constVal, Entity::getAllConsts()[0]);

		Assert::AreEqual(size_t(1), Entity::getAllProcStmts()[procIdx].size());
		Assert::AreEqual(size_t(1), Entity::getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(longAssignmentExprSource_checkAttributes) {
		std::string procName = "main";

		ProcIndex procIdx = ProcIndex(1);

		SourceAST ast = Parser::parse(longAssignmentExprSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::IsTrue(std::vector{ procIdx } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::AreEqual(size_t(0), Attribute::getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT).size());
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::STMT, 1));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 1));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 9));
	}

	TEST_METHOD(longAssignmentExprSource_checkPattern) {
		std::string varName1 = "x";
		std::string varName2 = "d";

		VarIndex varIdx = VarIndex(1);

		SourceAST ast = Parser::parse(longAssignmentExprSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName2)).size());

		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("x+y/z%a*b/c%d+f-g+z%a*b/c%d+f-g-x+y/z%a*b/c%d+f-g+z%a*b/c%d+f-g+z%a*b/c%d+f-g+z%a*b/c%d+f-9")).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("x+y"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("x+y/z%a*b/c%d+f-g+z%a*b/c%d+f-g-x+y/z%a*b/c%d+f-g+z%a*b/c%d+f-g+z%a*b/c%d+f-g+z%a*b/c%d+f-9"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getIfStmtsFromVar(varIdx).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getWhileStmtsFromVar(varIdx).size());
	}

	TEST_METHOD(longAssignmentExprSource_checkRelationships) {
		std::string varName1 = "x";
		std::string varName2 = "d";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx = StmtIndex(1);

		SourceAST ast = Parser::parse(longAssignmentExprSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(1), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesS::contains(stmtIdx, Entity::getVarIdx(varName1)));
		Assert::AreEqual(size_t(1), ModifiesS::getFromVariable(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(9), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesS::contains(stmtIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx, Entity::getVarIdx(varName2)));
		Assert::AreEqual(size_t(1), UsesS::getFromVariable(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(1), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName1)));

		Assert::AreEqual(size_t(9), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName2)));

		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(0), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(0), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
	}

	TEST_METHOD(longVarNameSource_checkEntities) {
		std::string varName = "extremelyVeryLongVariableNameThatHasManyCharactersAndIsVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLong";
		std::string procName = "main";

		ProcIndex procIdx = ProcIndex(1);

		SourceAST ast = Parser::parse(longVarNameSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(1), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::READ_TYPE).size());

		Assert::AreEqual(size_t(1), Entity::getAllVars().size());
		Assert::AreEqual(varName, Entity::getVarName(Entity::getAllVars()[0]));

		Assert::AreEqual(size_t(0), Entity::getAllConsts().size());

		Assert::AreEqual(size_t(1), Entity::getAllProcStmts()[procIdx].size());
		Assert::AreEqual(size_t(1), Entity::getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(longVarNameSource_checkAttributes) {
		std::string varName = "extremelyVeryLongVariableNameThatHasManyCharactersAndIsVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLong";
		std::string procName = "main";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx = StmtIndex(1);

		SourceAST ast = Parser::parse(longVarNameSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(varName, Attribute::getAttributeNameByStmtIdx(stmtIdx));
		Assert::IsTrue(std::vector{ procIdx } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::AreEqual(size_t(0), Attribute::getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT).size());
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::STMT, 1));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::READ, 1));
	}

	TEST_METHOD(longVarNameSource_checkPattern) {
		std::string varName = "extremelyVeryLongVariableNameThatHasManyCharactersAndIsVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLong";

		VarIndex varIdx = VarIndex(1);

		SourceAST ast = Parser::parse(longVarNameSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllAssignPatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName), ExpressionProcessor::convertInfixToPostFix(varName)).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName), ExpressionProcessor::convertInfixToPostFix(varName)).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName))).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getIfStmtsFromVar(varIdx).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getWhileStmtsFromVar(varIdx).size());
	}

	TEST_METHOD(longVarNameSource_checkRelationships) {
		std::string varName = "extremelyVeryLongVariableNameThatHasManyCharactersAndIsVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLong";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx = StmtIndex(1);

		SourceAST ast = Parser::parse(longVarNameSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(1), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesS::contains(stmtIdx, Entity::getVarIdx(varName)));
		Assert::AreEqual(size_t(1), ModifiesS::getFromVariable(Entity::getVarIdx(varName)).size());

		Assert::AreEqual(size_t(0), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), UsesS::getFromVariable(Entity::getVarIdx(varName)).size());

		Assert::AreEqual(size_t(1), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName)));

		Assert::AreEqual(size_t(0), std::get<0>(UsesP::getAllSynonymVarInfo()).size());

		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(0), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(0), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
	}

	TEST_METHOD(multipleNestingSource_checkEntities) {
		std::string varName1 = "present";
		std::string varName2 = "y";
		std::string procName = "Peter";
		VarIndex constVal1 = 0;
		VarIndex constVal2 = 8;
		VarIndex constVal3 = 1;

		ProcIndex procIdx = ProcIndex(1);

		SourceAST ast = Parser::parse(multipleNestingSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(11), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(7), Entity::getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(2), Entity::getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(2), Entity::getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(4), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[3]));

		Assert::AreEqual(size_t(3), Entity::getAllConsts().size());
		Assert::AreEqual(constVal1, Entity::getAllConsts()[0]);
		Assert::AreEqual(constVal2, Entity::getAllConsts()[1]);
		Assert::AreEqual(constVal3, Entity::getAllConsts()[2]);

		Assert::AreEqual(size_t(11), Entity::getAllProcStmts()[procIdx].size());
		Assert::AreEqual(size_t(11), Entity::getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(multipleNestingSource_checkAttributes) {
		std::string varName1 = "present";
		std::string varName2 = "y";
		std::string procName = "Peter";

		ProcIndex procIdx = ProcIndex(1);

		SourceAST ast = Parser::parse(multipleNestingSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::IsTrue(std::vector{ procIdx } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::IsTrue(std::vector{ 8, 1 } == Attribute::getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::WHILE, 1));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::IF, 2));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::WHILE, 8));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 1));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 8));
	}

	TEST_METHOD(multipleNestingSource_checkPattern) {
		std::string varName1 = "present";
		std::string varName2 = "y";
		std::string varName3 = "x";
		std::string varName4 = "z";

		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(1);

		SourceAST ast = Parser::parse(multipleNestingSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(7), std::get<0>(Pattern::getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(3), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(2), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(3), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("sent")).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("y*z")).size());
		Assert::AreEqual(size_t(3), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("present*y*z")).size());
		Assert::AreEqual(size_t(3), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("present*y")).size());
		Assert::AreEqual(size_t(2), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("x*y*z")).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("x*(y*z)")).size());

		Assert::AreEqual(size_t(3), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("present*y*z")).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("present*y")).size());
		Assert::AreEqual(size_t(2), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("z-8-x*y*z")).size());
		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("x*(y*z)")).size());

		Assert::AreEqual(size_t(3), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(5), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(4), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName3))).size());
		Assert::AreEqual(size_t(5), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName4))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("y*z"))).size());
		Assert::AreEqual(size_t(3), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("present*y*z"))).size());
		Assert::AreEqual(size_t(3), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("present*y"))).size());
		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("x*y*z"))).size());

		Assert::AreEqual(size_t(3), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("present*y*z"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("present*y"))).size());
		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("z-8-x*y*z"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("x*(y*z)"))).size());

		Assert::AreEqual(size_t(4), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(2), Pattern::getIfStmtsFromVar(varIdx1).size());
		Assert::AreEqual(size_t(2), Pattern::getIfStmtsFromVar(varIdx2).size());

		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
		Assert::AreEqual(size_t(2), Pattern::getWhileStmtsFromVar(varIdx1).size());
	}

	TEST_METHOD(multipleNestingSource_checkRelationships) {
		std::string varName1 = "present";
		std::string varName2 = "y";
		std::string varName3 = "x";
		std::string varName4 = "z";

		ProcIndex procIdx = ProcIndex(1);
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
		StmtIndex stmtIdx11 = StmtIndex(11);

		SourceAST ast = Parser::parse(multipleNestingSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(17), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesS::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx1, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx1, Entity::getVarIdx(varName3)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx1, Entity::getVarIdx(varName4)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx2, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx2, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx2, Entity::getVarIdx(varName3)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx2, Entity::getVarIdx(varName4)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx3, Entity::getVarIdx(varName1)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx3, Entity::getVarIdx(varName2)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx3, Entity::getVarIdx(varName3)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx3, Entity::getVarIdx(varName4)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx4, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx4, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx4, Entity::getVarIdx(varName3)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx4, Entity::getVarIdx(varName4)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx5, Entity::getVarIdx(varName1)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx6, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx6, Entity::getVarIdx(varName3)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx7, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx7, Entity::getVarIdx(varName2)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx8, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx8, Entity::getVarIdx(varName2)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx9, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx9, Entity::getVarIdx(varName2)));
		Assert::AreEqual(size_t(5), ModifiesS::getFromVariable(Entity::getVarIdx(varName1)).size());
		Assert::AreEqual(size_t(7), ModifiesS::getFromVariable(Entity::getVarIdx(varName2)).size());

		Assert::AreEqual(size_t(32), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesS::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx1, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx1, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx1, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesS::contains(stmtIdx2, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx2, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx2, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx2, Entity::getVarIdx(varName4)));
		Assert::IsFalse(UsesS::contains(stmtIdx3, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx3, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx3, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx3, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesS::contains(stmtIdx4, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx4, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx4, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx4, Entity::getVarIdx(varName4)));
		Assert::IsFalse(UsesS::contains(stmtIdx5, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx5, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx5, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx5, Entity::getVarIdx(varName4)));
		Assert::IsFalse(UsesS::contains(stmtIdx6, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx6, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx7, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx7, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx7, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesS::contains(stmtIdx8, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesS::contains(stmtIdx8, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesS::contains(stmtIdx8, Entity::getVarIdx(varName4)));
		Assert::IsTrue(UsesS::contains(stmtIdx9, Entity::getVarIdx(varName2)));
		Assert::AreEqual(size_t(7), UsesS::getFromVariable(Entity::getVarIdx(varName1)).size());
		Assert::AreEqual(size_t(9), UsesS::getFromVariable(Entity::getVarIdx(varName2)).size());

		Assert::AreEqual(size_t(3), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName3)));
		Assert::IsFalse(ModifiesP::contains(procIdx, Entity::getVarIdx(varName4)));

		Assert::AreEqual(size_t(4), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName2)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName4)));

		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(14), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Next::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx2, stmtIdx11));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx4, stmtIdx5));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx5, stmtIdx6));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx4, stmtIdx7));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx7, stmtIdx8));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx8, stmtIdx9));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx9, stmtIdx8));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx8, stmtIdx10));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx6, stmtIdx10));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx10, stmtIdx1));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx11, stmtIdx1));

		Assert::AreEqual(size_t(4), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx3, stmtIdx10));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx3, stmtIdx4));
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
		Assert::AreEqual(size_t(3), ParentT::getPredecessors(stmtIdx5).size());

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
	}

	TEST_METHOD(differentCasingNamesSource_checkEntities) {
		std::string varName1 = "v";
		std::string varName2 = "V1";
		std::string procName = "mAin";
		VarIndex constVal = 1;

		ProcIndex procIdx = ProcIndex(1);

		SourceAST ast = Parser::parse(differentCasingNamesSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));
		Assert::IsFalse("main" == Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(4), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(4), Entity::getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(4), Entity::getAllVars().size());
		Assert::AreEqual(varName1, Entity::getVarName(Entity::getAllVars()[0]));
		Assert::AreEqual(varName2, Entity::getVarName(Entity::getAllVars()[3]));

		Assert::AreEqual(size_t(1), Entity::getAllConsts().size());
		Assert::AreEqual(constVal, Entity::getAllConsts()[0]);

		Assert::AreEqual(size_t(4), Entity::getAllProcStmts()[procIdx].size());
		Assert::AreEqual(size_t(4), Entity::getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(differentCasingNamesSource_checkAttributes) {
		std::string varName1 = "v";
		std::string varName2 = "V1";
		std::string procName = "mAin";

		ProcIndex procIdx = ProcIndex(1);

		SourceAST ast = Parser::parse(differentCasingNamesSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::IsTrue(std::vector{ procIdx } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::IsTrue(std::vector{ 1 } == Attribute::getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsFalse(Attribute::hasEqualIntegerAttribute(EntityType::STMT, 5));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 1));
	}

	TEST_METHOD(differentCasingNamesSource_checkPattern) {
		std::string varName1 = "v";
		std::string varName2 = "V";
		std::string varName3 = "v1";
		std::string varName4 = "V1";

		VarIndex varIdx = VarIndex(1);

		SourceAST ast = Parser::parse(differentCasingNamesSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(4), std::get<0>(Pattern::getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName4), ExpressionProcessor::convertInfixToPostFix("1")).size());

		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName2), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName4), ExpressionProcessor::convertInfixToPostFix("1")).size());

		Assert::AreEqual(size_t(4), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("1"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());

		Assert::AreEqual(size_t(4), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("1"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getIfStmtsFromVar(varIdx).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getWhileStmtsFromVar(varIdx).size());
	}

	TEST_METHOD(differentCasingNamesSource_checkRelationships) {
		std::string varName1 = "v";
		std::string varName2 = "V";
		std::string varName3 = "v1";
		std::string varName4 = "V1";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);

		SourceAST ast = Parser::parse(differentCasingNamesSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(4), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesS::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx2, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx2, Entity::getVarIdx(varName2)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx1, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx3, Entity::getVarIdx(varName3)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx3, Entity::getVarIdx(varName4)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx4, Entity::getVarIdx(varName4)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx4, Entity::getVarIdx(varName3)));
		Assert::AreEqual(size_t(1), ModifiesS::getFromVariable(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(0), std::get<0>(UsesS::getAllSynonymVarInfo()).size());

		Assert::AreEqual(size_t(4), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName4)));

		Assert::AreEqual(size_t(0), std::get<0>(UsesP::getAllSynonymVarInfo()).size());

		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(3), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Next::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx3, stmtIdx4));

		Assert::AreEqual(size_t(3), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx1, stmtIdx3));
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx1, stmtIdx4));
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx2, stmtIdx4));
		Assert::AreEqual(size_t(6), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx3));
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx1, stmtIdx4));
		Assert::IsTrue(FollowsT::containsSuccessor(stmtIdx2, stmtIdx4));

		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
	}

	TEST_METHOD(multipleWhitespacesSource_checkEntities) {
		std::string varName = "v";
		std::string procName = "main";
		VarIndex constVal = 0;

		ProcIndex procIdx = ProcIndex(1);

		SourceAST ast = Parser::parse(multipleWhitespacesSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(1), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(3), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(1), Entity::getAllVars().size());
		Assert::AreEqual(varName, Entity::getVarName(Entity::getAllVars()[0]));

		Assert::AreEqual(size_t(1), Entity::getAllConsts().size());
		Assert::AreEqual(constVal, Entity::getAllConsts()[0]);

		Assert::AreEqual(size_t(3), Entity::getAllProcStmts()[procIdx].size());
		Assert::AreEqual(size_t(3), Entity::getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(multipleWhitespacesSource_checkAttributes) {
		std::string varName = "v";
		std::string procName = "main";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);

		SourceAST ast = Parser::parse(multipleWhitespacesSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(varName, Attribute::getAttributeNameByStmtIdx(stmtIdx1));
		Assert::IsFalse(varName == Attribute::getAttributeNameByStmtIdx(stmtIdx2));
		Assert::AreEqual(varName, Attribute::getAttributeNameByStmtIdx(stmtIdx3));
		Assert::IsTrue(std::vector{ stmtIdx1 } == Attribute::getEqualNameAttributesFromName(EntityType::READ, varName));
		Assert::IsTrue(std::vector{ stmtIdx3 } == Attribute::getEqualNameAttributesFromName(EntityType::PRINT, varName));
		Assert::IsTrue(std::vector{ procIdx } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::AreEqual(size_t(0), Attribute::getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT).size());
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::STMT, 2));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::READ, 1));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 2));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::PRINT, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 0));
	}

	TEST_METHOD(multipleWhitespacesSource_checkPattern) {
		std::string varName = "v";

		VarIndex varIdx = VarIndex(1);

		SourceAST ast = Parser::parse(multipleWhitespacesSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName), ExpressionProcessor::convertInfixToPostFix(varName)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName), ExpressionProcessor::convertInfixToPostFix("0")).size());

		Assert::AreEqual(size_t(0), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName), ExpressionProcessor::convertInfixToPostFix(varName)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName), ExpressionProcessor::convertInfixToPostFix("0")).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getIfStmtsFromVar(varIdx).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getWhileStmtsFromVar(varIdx).size());
	}

	TEST_METHOD(multipleWhitespacesSource_checkRelationships) {
		std::string varName = "v";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);

		SourceAST ast = Parser::parse(multipleWhitespacesSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(2), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesS::contains(stmtIdx1, Entity::getVarIdx(varName)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx2, Entity::getVarIdx(varName)));
		Assert::AreEqual(size_t(2), ModifiesS::getFromVariable(Entity::getVarIdx(varName)).size());

		Assert::AreEqual(size_t(1), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesS::contains(stmtIdx3, Entity::getVarIdx(varName)));
		Assert::AreEqual(size_t(1), UsesS::getFromVariable(Entity::getVarIdx(varName)).size());

		Assert::AreEqual(size_t(1), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesP::contains(procIdx, Entity::getVarIdx(varName)));

		Assert::AreEqual(size_t(1), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesP::contains(procIdx, Entity::getVarIdx(varName)));

		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(2), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Next::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx2, stmtIdx3));

		Assert::AreEqual(size_t(2), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::AreEqual(size_t(3), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
	}

	TEST_METHOD(multipleProceduresRecursiveCallsSource_checkEntities) {
		std::string varName = "x";
		std::string procName = "first";

		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		ProcIndex procIdx3 = ProcIndex(3);
		ProcIndex procIdx4 = ProcIndex(4);

		SourceAST ast = Parser::parse(multipleProceduresRecursiveCallsSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(4), Entity::getAllProcs().size());
		Assert::AreEqual(procName, Entity::getProcName(Entity::getAllProcs()[0]));

		Assert::AreEqual(size_t(6), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(1), Entity::getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(2), Entity::getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(3), Entity::getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(0), Entity::getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(3), Entity::getAllVars().size());
		Assert::AreEqual(varName, Entity::getVarName(Entity::getAllVars()[0]));

		Assert::AreEqual(size_t(1), Entity::getAllConsts().size());

		Assert::AreEqual(size_t(2), Entity::getStmtsFromProc(procIdx1).size());
		Assert::AreEqual(size_t(2), Entity::getStmtsFromProc(procIdx2).size());
		Assert::AreEqual(size_t(1), Entity::getStmtsFromProc(procIdx3).size());
		Assert::AreEqual(size_t(1), Entity::getStmtsFromProc(procIdx4).size());
	}

	TEST_METHOD(multipleProceduresRecursiveCallsSource_checkAttributes) {
		std::string varName = "x";
		std::string procName1 = "first";
		std::string procName2 = "second";
		std::string procName3 = "third";
		std::string procName4 = "fourth";

		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		ProcIndex procIdx3 = ProcIndex(3);
		ProcIndex procIdx4 = ProcIndex(4);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);

		SourceAST ast = Parser::parse(multipleProceduresRecursiveCallsSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(procName2, Attribute::getAttributeNameByStmtIdx(stmtIdx1));
		Assert::AreEqual(procName3, Attribute::getAttributeNameByStmtIdx(stmtIdx2));
		Assert::AreEqual(procName4, Attribute::getAttributeNameByStmtIdx(stmtIdx3));
		Assert::IsTrue(std::vector{ stmtIdx1 } == Attribute::getEqualNameAttributesFromName(EntityType::CALL, procName2));
		Assert::IsTrue(std::vector{ stmtIdx2 } == Attribute::getEqualNameAttributesFromName(EntityType::CALL, procName3));
		Assert::IsTrue(std::vector{ stmtIdx3 } == Attribute::getEqualNameAttributesFromName(EntityType::CALL, procName4));
		Assert::IsTrue(std::vector{ stmtIdx5 } == Attribute::getEqualNameAttributesFromName(EntityType::READ, varName));
		Assert::IsTrue(std::vector{ procIdx1 } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName1));
		Assert::IsTrue(std::vector{ procIdx2 } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName2));
		Assert::IsTrue(std::vector{ procIdx3 } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName3));
		Assert::IsTrue(std::vector{ procIdx4 } == Attribute::getEqualNameAttributesFromName(EntityType::PROCEDURE, procName4));
		Assert::IsTrue(std::vector{ 2 } == Attribute::getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::STMT, 2));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CALL, 1));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CALL, 2));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CALL, 3));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 4));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::READ, 5));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::ASSIGN, 6));
		Assert::IsTrue(Attribute::hasEqualIntegerAttribute(EntityType::CONSTANT, 2));
	}

	TEST_METHOD(multipleProceduresRecursiveCallsSource_checkPattern) {
		std::string varName1 = "x";
		std::string varName2 = "y";
		std::string varName3 = "z";

		VarIndex varIdx = VarIndex(1);

		SourceAST ast = Parser::parse(multipleProceduresRecursiveCallsSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(2), std::get<0>(Pattern::getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprPartialMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix("2")).size());

		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName1), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(1), Pattern::getAssignStmtsFromVarExprFullMatch(Entity::getVarIdx(varName3), ExpressionProcessor::convertInfixToPostFix("2")).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("2"))).size());

		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(1), std::get<0>(Pattern::getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("2"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getIfStmtsFromVar(varIdx).size());

		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
		Assert::AreEqual(size_t(0), Pattern::getWhileStmtsFromVar(varIdx).size());
	}

	TEST_METHOD(multipleProceduresRecursiveCallsSource_checkRelationships) {
		std::string varName1 = "x";
		std::string varName2 = "y";
		std::string varName3 = "z";

		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		ProcIndex procIdx3 = ProcIndex(3);
		ProcIndex procIdx4 = ProcIndex(4);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);
		StmtIndex stmtIdx4 = StmtIndex(4);
		StmtIndex stmtIdx5 = StmtIndex(5);
		StmtIndex stmtIdx6 = StmtIndex(6);

		SourceAST ast = Parser::parse(multipleProceduresRecursiveCallsSource);
		PKB* pkb = new PKB();
		PKBInserter* pkbInserter = new PKBInserter(pkb);
		DesignExtractor::extract(ast, pkbInserter);

		Assert::AreEqual(size_t(7), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesS::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsFalse(ModifiesS::contains(stmtIdx1, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx1, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx2, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx3, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx4, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesS::contains(stmtIdx5, Entity::getVarIdx(varName1)));
		Assert::AreEqual(size_t(4), ModifiesS::getFromVariable(Entity::getVarIdx(varName1)).size());

		Assert::AreEqual(size_t(2), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesS::contains(stmtIdx1, Entity::getVarIdx(varName2)));
		Assert::IsFalse(UsesS::contains(stmtIdx1, Entity::getVarIdx(varName1)));
		Assert::IsFalse(UsesS::contains(stmtIdx1, Entity::getVarIdx(varName3)));
		Assert::IsFalse(UsesS::contains(stmtIdx2, Entity::getVarIdx(varName1)));
		Assert::IsFalse(UsesS::contains(stmtIdx2, Entity::getVarIdx(varName2)));
		Assert::IsFalse(UsesS::contains(stmtIdx2, Entity::getVarIdx(varName3)));
		Assert::IsFalse(UsesS::contains(stmtIdx3, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesS::contains(stmtIdx4, Entity::getVarIdx(varName2)));
		Assert::IsFalse(UsesS::contains(stmtIdx5, Entity::getVarIdx(varName1)));
		Assert::AreEqual(size_t(2), UsesS::getFromVariable(Entity::getVarIdx(varName2)).size());

		Assert::AreEqual(size_t(6), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(ModifiesP::contains(procIdx1, Entity::getVarIdx(varName1)));
		Assert::IsFalse(ModifiesP::contains(procIdx1, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesP::contains(procIdx1, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesP::contains(procIdx2, Entity::getVarIdx(varName1)));
		Assert::IsFalse(ModifiesP::contains(procIdx2, Entity::getVarIdx(varName2)));
		Assert::IsTrue(ModifiesP::contains(procIdx2, Entity::getVarIdx(varName3)));
		Assert::IsTrue(ModifiesP::contains(procIdx3, Entity::getVarIdx(varName1)));
		Assert::IsTrue(ModifiesP::contains(procIdx4, Entity::getVarIdx(varName3)));

		Assert::AreEqual(size_t(2), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::IsTrue(UsesP::contains(procIdx1, Entity::getVarIdx(varName2)));
		Assert::IsFalse(UsesP::contains(procIdx1, Entity::getVarIdx(varName1)));
		Assert::IsFalse(UsesP::contains(procIdx1, Entity::getVarIdx(varName3)));
		Assert::IsTrue(UsesP::contains(procIdx2, Entity::getVarIdx(varName2)));
		Assert::IsFalse(UsesP::contains(procIdx3, Entity::getVarIdx(varName1)));
		Assert::IsFalse(UsesP::contains(procIdx4, Entity::getVarIdx(varName3)));

		Assert::AreEqual(size_t(3), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Calls::containsSuccessor(procIdx1, procIdx2));
		Assert::IsTrue(Calls::containsSuccessor(procIdx1, procIdx3));
		Assert::IsTrue(Calls::containsSuccessor(procIdx2, procIdx4));
		Assert::IsFalse(Calls::containsSuccessor(procIdx1, procIdx4));
		Assert::AreEqual(size_t(4), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(CallsT::containsSuccessor(procIdx1, procIdx4));
		Assert::IsTrue(CallsT::containsSuccessor(procIdx1, procIdx2));
		Assert::IsTrue(CallsT::containsSuccessor(procIdx1, procIdx3));
		Assert::IsTrue(CallsT::containsSuccessor(procIdx2, procIdx4));

		Assert::AreEqual(size_t(2), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Next::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsFalse(Next::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Next::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::IsFalse(Next::containsSuccessor(stmtIdx4, stmtIdx5));
		Assert::IsFalse(Next::containsSuccessor(stmtIdx5, stmtIdx6));

		Assert::AreEqual(size_t(2), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx1, stmtIdx2));
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx2, stmtIdx3));
		Assert::IsTrue(Follows::containsSuccessor(stmtIdx3, stmtIdx4));
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx4, stmtIdx5));
		Assert::IsFalse(Follows::containsSuccessor(stmtIdx5, stmtIdx6));
		Assert::AreEqual(size_t(2), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());

		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
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
			PKB* pkb = new PKB();
			PKBInserter* pkbInserter = new PKBInserter(pkb);
			DesignExtractor::extract(ast, pkbInserter);
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}

		Assert::AreEqual(size_t(0), Entity::getAllProcs().size());
		Assert::AreEqual(size_t(0), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getAllVars().size());
		Assert::AreEqual(size_t(0), Entity::getAllConsts().size());
		Assert::AreEqual(size_t(0), Entity::getAllProcStmts().size());
		Assert::AreEqual(size_t(0), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllAssignPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
	}

	TEST_METHOD(callNonExistentProcedure_ASTExceptionThrown_checkPKBTables) {
		const char* source =
			"procedure main { \n "
			"	call Petr; \n "
			"} \n"
			"procedure Peter { \n "
			"	read x; \n "
			"}";
		auto wrapperFunc = [&source] { SourceAST ast = Parser::parse(source); ASTValidator::validateAST(ast); };
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			SourceAST ast = Parser::parse(source);
			ASTValidator::validateAST(ast);
			PKB* pkb = new PKB();
			PKBInserter* pkbInserter = new PKBInserter(pkb);
			DesignExtractor::extract(ast, pkbInserter);
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CALL_NON_EXISTENT_PROC_NAME_ERROR.c_str(), ex.what());
		}

		Assert::AreEqual(size_t(0), Entity::getAllProcs().size());
		Assert::AreEqual(size_t(0), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getAllVars().size());
		Assert::AreEqual(size_t(0), Entity::getAllConsts().size());
		Assert::AreEqual(size_t(0), Entity::getAllProcStmts().size());
		Assert::AreEqual(size_t(0), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllAssignPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
	}

	TEST_METHOD(duplicateProc_ASTExceptionThrown_checkPKBTables) {
		const char* source =
			"procedure main { \n "
			"	print a; \n "
			"} \n"
			"procedure main { \n "
			"	read a; \n "
			"}";
		auto wrapperFunc = [&source] { SourceAST ast = Parser::parse(source); ASTValidator::validateAST(ast); };
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			SourceAST ast = Parser::parse(source);
			ASTValidator::validateAST(ast);
			PKB* pkb = new PKB();
			PKBInserter* pkbInserter = new PKBInserter(pkb);
			DesignExtractor::extract(ast, pkbInserter);
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::DUPLICATE_PROC_NAMES_ERROR.c_str(), ex.what());
		}

		Assert::AreEqual(size_t(0), Entity::getAllProcs().size());
		Assert::AreEqual(size_t(0), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getAllVars().size());
		Assert::AreEqual(size_t(0), Entity::getAllConsts().size());
		Assert::AreEqual(size_t(0), Entity::getAllProcStmts().size());
		Assert::AreEqual(size_t(0), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllAssignPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
	}

	TEST_METHOD(selfCall_insideNestedContainerStmts_ASTExceptionThrown_checkPKBTables) {
		const char* source =
			"procedure main { \n "
			"	while (x==y) { \n "
			"      if (x==y) then { read x; } else { \n"
			"          call main; }} \n"
			"}";
		auto wrapperFunc = [&source] { SourceAST ast = Parser::parse(source); ASTValidator::validateAST(ast); };
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			SourceAST ast = Parser::parse(source);
			ASTValidator::validateAST(ast);
			PKB* pkb = new PKB();
			PKBInserter* pkbInserter = new PKBInserter(pkb);
			DesignExtractor::extract(ast, pkbInserter);
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CYCLIC_CALL_ERROR.c_str(), ex.what());
		}

		Assert::AreEqual(size_t(0), Entity::getAllProcs().size());
		Assert::AreEqual(size_t(0), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getAllVars().size());
		Assert::AreEqual(size_t(0), Entity::getAllConsts().size());
		Assert::AreEqual(size_t(0), Entity::getAllProcStmts().size());
		Assert::AreEqual(size_t(0), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllAssignPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
	}

	TEST_METHOD(cyclicCall_twoProcs_ASTExceptionThrown_checkPKBTables) {
		const char* source =
			"procedure proc { \n "
			"	call main;  \n "
			"} \n "
			"procedure main{ \n "
			"   while (x == y) {  \n "
			"		if (x == y) then { read x; } else {  \n "
			"	       call proc;  \n "
			"		}  \n "
			"   }  \n "
			"}";

		auto wrapperFunc = [&source] { SourceAST ast = Parser::parse(source); ASTValidator::validateAST(ast); };
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			SourceAST ast = Parser::parse(source);
			ASTValidator::validateAST(ast);
			PKB* pkb = new PKB();
			PKBInserter* pkbInserter = new PKBInserter(pkb);
			DesignExtractor::extract(ast, pkbInserter);
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CYCLIC_CALL_ERROR.c_str(), ex.what());
		}

		Assert::AreEqual(size_t(0), Entity::getAllProcs().size());
		Assert::AreEqual(size_t(0), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getAllVars().size());
		Assert::AreEqual(size_t(0), Entity::getAllConsts().size());
		Assert::AreEqual(size_t(0), Entity::getAllProcStmts().size());
		Assert::AreEqual(size_t(0), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllAssignPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
	}

	TEST_METHOD(cyclicCall_transitiveCalls_ASTExceptionThrown_checkPKBTables) {
		const char* source =
			"procedure proc1{ \n "
			"	while (x == y) { \n "
			"		if (x == y) then { read x; } else { \n "
			"			call proc2; \n "
			"		} \n "
			"	} \n "
			"} \n "
			"procedure proc2{ \n "
			"	call proc3; call proc3; \n "
			"} \n "
			"procedure proc3{ \n "
			"	if ((x == y) && (z > 0)) then { \n "
			"		 print a; } else { \n "
			"		 read y; \n "
			"	} \n "
			"	if (x != z) then { \n "
			"		call proc1; } else { \n "
			"		read a; \n "
			"	} \n "
			"}";

		auto wrapperFunc = [&source] { SourceAST ast = Parser::parse(source); ASTValidator::validateAST(ast); };
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			SourceAST ast = Parser::parse(source);
			ASTValidator::validateAST(ast);
			PKB* pkb = new PKB();
			PKBInserter* pkbInserter = new PKBInserter(pkb);
			DesignExtractor::extract(ast, pkbInserter);
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CYCLIC_CALL_ERROR.c_str(), ex.what());
		}

		Assert::AreEqual(size_t(0), Entity::getAllProcs().size());
		Assert::AreEqual(size_t(0), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getAllVars().size());
		Assert::AreEqual(size_t(0), Entity::getAllConsts().size());
		Assert::AreEqual(size_t(0), Entity::getAllProcStmts().size());
		Assert::AreEqual(size_t(0), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllAssignPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
	}

	TEST_METHOD(cyclicCall_transitiveCalls_disjointCallsGraphs_ASTExceptionThrown_checkPKBTables) {
		const char* source =
			"procedure proc1{\n"
			"	while (x == y) { \n"
			"		if (x == y) then { read x; } else { \n"
			"			call proc2; \n"
			"		} \n"
			"	} \n"
			"} \n"
			"procedure proc2{ \n"
			"	call proc3; call proc3; \n"
			"} \n"
			"procedure proc3 { call proc1; } \n"
			"procedure proc4 { read proc1; } \n"
			"procedure proc5{ \n"
			"	if ((x == y) && (z > 0)) then { \n"
			"		  print a; } else { \n"
			"		read y; \n"
			"	} \n"
			"	if (x != z) then { \n"
			"		call proc4; } else { \n"
			"		read a; \n"
			"	} \n"
			"}";

		auto wrapperFunc = [&source] { SourceAST ast = Parser::parse(source); ASTValidator::validateAST(ast); };
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			SourceAST ast = Parser::parse(source);
			ASTValidator::validateAST(ast);
			PKB* pkb = new PKB();
			PKBInserter* pkbInserter = new PKBInserter(pkb);
			DesignExtractor::extract(ast, pkbInserter);
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CYCLIC_CALL_ERROR.c_str(), ex.what());
		}

		Assert::AreEqual(size_t(0), Entity::getAllProcs().size());
		Assert::AreEqual(size_t(0), Entity::getAllStmts().size());
		Assert::AreEqual(size_t(0), Entity::getAllVars().size());
		Assert::AreEqual(size_t(0), Entity::getAllConsts().size());
		Assert::AreEqual(size_t(0), Entity::getAllProcStmts().size());
		Assert::AreEqual(size_t(0), std::get<0>(ModifiesS::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(UsesS::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ModifiesP::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(UsesP::getAllSynonymVarInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Calls::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(CallsT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Next::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Follows::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(FollowsT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Parent::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(ParentT::getAllPredecessorSuccessorInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllAssignPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllIfPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(Pattern::getAllWhilePatternInfo()).size());
	}
	};
}
