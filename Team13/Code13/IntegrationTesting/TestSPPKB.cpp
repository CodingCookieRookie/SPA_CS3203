#include "stdafx.h"
#include "CppUnitTest.h"

#include "PKB/ExpressionProcessor.h"
#include "PKB/PKBGetter.h"
#include "PKB/PKBInserter.h"
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

	PKB pkb;
	PKBGetter* pkbGetter;
	PKBInserter* pkbInserter;

	TEST_METHOD_INITIALIZE(init) {
		pkb = PKB();
		pkbGetter = new PKBGetter(pkb);
		pkbInserter = new PKBInserter(pkb);
	}

public:

	TEST_METHOD(blackBoxSampleSource1_checkEntities) {
		std::string varName1 = "num1";
		std::string varName2 = "ave";
		std::string procName = "computeAverage";
		VarIndex constVal = 3;

		ProcIndex procIdx = ProcIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(sampleSource1);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(1), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(procName, pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE)[0]));

		Assert::AreEqual(size_t(6), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(3), pkbGetter->getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(2), pkbGetter->getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(5), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(varName1, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[0]));
		Assert::AreEqual(varName2, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[4]));

		Assert::AreEqual(size_t(1), pkbGetter->getAllConsts().size());
		Assert::AreEqual(constVal, pkbGetter->getAllConsts()[0]);

		Assert::AreEqual(size_t(6), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(6), pkbGetter->getStmtsFromProc(procIdx).size());
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

		Parser parser;
		SourceAST ast = parser.parse(sampleSource1);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(varName1, pkbGetter->getAttributeNameByStmtIdx(stmtIdx1));
		Assert::AreEqual(varName2, pkbGetter->getAttributeNameByStmtIdx(stmtIdx6));
		Assert::IsTrue(std::vector{ stmtIdx1 } == pkbGetter->getEqualNameAttributesFromName(EntityType::READ, varName1));
		Assert::IsTrue(std::vector{ stmtIdx6 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PRINT, varName2));
		Assert::IsTrue(std::vector{ procIdx } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::IsTrue(std::vector{ 3 } == pkbGetter->getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsFalse(pkbGetter->hasEqualIntegerAttribute(EntityType::STMT, 7));
		Assert::IsFalse(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::READ, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 3));
	}

	TEST_METHOD(blackBoxSampleSource1_checkPattern) {
		std::string varName1 = "num1";
		std::string varName2 = "sum";
		std::string varName3 = "num2";

		VarIndex varIdx = VarIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(sampleSource1);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix(varName3)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("num1+num2")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("num1+num2+num3")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("num1+((num2))")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("(num1+num2)+num3")).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("num1+num3")).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("num2+num3")).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("num1+(num2+num3)")).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("num1+num2+num3")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("(num1+num2)+num3")).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("num1+num2")).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix(varName1)).size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName3))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("num1+num2"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("num1+num2+num3"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("num1+((num2))"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("(num1+num2)+num3"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("num1+num3"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("num2+num3"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("num1+(num2+num3)"))).size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("num1+num2+num3"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("(num1+num2)+num3"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("num1+num2"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIdx).size());
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

		Parser parser;
		SourceAST ast = parser.parse(sampleSource1);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));

		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));

		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));

		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());

		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx4, stmtIdx5));

		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx1, stmtIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx4, stmtIdx5));
		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx1, stmtIdx5));
		Assert::AreEqual(size_t(15), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
	}

	TEST_METHOD(blackBoxSampleSource2_checkEntities) {
		std::string varName1 = "num1";
		std::string varName2 = "temp";
		std::string procName = "printAscending";
		VarIndex constVal1 = 0;
		VarIndex constVal2 = 1;

		ProcIndex procIdx = ProcIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(sampleSource2);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(1), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(procName, pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE)[0]));

		Assert::AreEqual(size_t(11), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(2), pkbGetter->getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(5), pkbGetter->getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(3), pkbGetter->getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(4), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(varName1, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[0]));
		Assert::AreEqual(varName2, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[3]));

		Assert::AreEqual(size_t(2), pkbGetter->getAllConsts().size());
		Assert::AreEqual(constVal1, pkbGetter->getAllConsts()[0]);
		Assert::AreEqual(constVal2, pkbGetter->getAllConsts()[1]);

		Assert::AreEqual(size_t(11), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(11), pkbGetter->getStmtsFromProc(procIdx).size());
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

		Parser parser;
		SourceAST ast = parser.parse(sampleSource2);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(varName1, pkbGetter->getAttributeNameByStmtIdx(stmtIdx1));
		Assert::AreEqual(varName2, pkbGetter->getAttributeNameByStmtIdx(stmtIdx2));
		Assert::AreEqual(varName1, pkbGetter->getAttributeNameByStmtIdx(stmtIdx9));
		Assert::AreEqual(varName2, pkbGetter->getAttributeNameByStmtIdx(stmtIdx10));
		Assert::AreEqual(varName4, pkbGetter->getAttributeNameByStmtIdx(stmtIdx11));
		Assert::IsTrue(std::vector{ stmtIdx1 } == pkbGetter->getEqualNameAttributesFromName(EntityType::READ, varName1));
		Assert::IsTrue(std::vector{ stmtIdx2 } == pkbGetter->getEqualNameAttributesFromName(EntityType::READ, varName2));
		Assert::IsTrue(std::vector{ stmtIdx9 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PRINT, varName1));
		Assert::IsTrue(std::vector{ stmtIdx10 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PRINT, varName2));
		Assert::IsTrue(std::vector{ stmtIdx11 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PRINT, varName4));
		Assert::IsTrue(std::vector{ procIdx } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::IsTrue(std::vector{ 1 } == pkbGetter->getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 1));
	}

	TEST_METHOD(blackBoxSampleSource2_checkPattern) {
		std::string varName1 = "num1";
		std::string varName2 = "num2";
		std::string varName3 = "temp";

		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);

		Parser parser;
		SourceAST ast = parser.parse(sampleSource2);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix(varName3)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix("((num1))")).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix(varName3)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix("((num1))")).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix(varName1)).size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName3))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("((num1))"))).size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName3))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("((num1))"))).size());

		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx1).size());
		Assert::AreEqual(size_t(1), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx2).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIdx1).size());
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

		Parser parser;
		SourceAST ast = parser.parse(sampleSource2);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(11), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));

		Assert::AreEqual(size_t(9), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx9, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::AreEqual(size_t(3), pkbGetter->getRSInfoFromRightArg(RelationshipType::USES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)).size());

		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));

		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());

		Assert::AreEqual(size_t(11), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx4, stmtIdx5));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx4, stmtIdx8));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx5, stmtIdx6));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx7, stmtIdx9));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx8, stmtIdx9));

		Assert::AreEqual(size_t(8), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx1, stmtIdx4));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx4, stmtIdx5));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx4, stmtIdx9));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx5, stmtIdx6));
		Assert::AreEqual(size_t(24), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx1, stmtIdx4));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx5, stmtIdx7));

		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx4, stmtIdx5));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx4, stmtIdx6));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx4, stmtIdx7));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx4, stmtIdx8));
		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
	}

	TEST_METHOD(blackBoxSampleSource3_checkEntities) {
		std::string varName1 = "number";
		std::string varName2 = "digit";
		std::string procName = "sumDigits";
		VarIndex constVal1 = 0;
		VarIndex constVal2 = 10;

		ProcIndex procIdx = ProcIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(sampleSource3);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(1), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(procName, pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE)[0]));

		Assert::AreEqual(size_t(7), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(4), pkbGetter->getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(3), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(varName1, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[0]));
		Assert::AreEqual(varName2, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[2]));

		Assert::AreEqual(size_t(2), pkbGetter->getAllConsts().size());
		Assert::AreEqual(constVal1, pkbGetter->getAllConsts()[0]);
		Assert::AreEqual(constVal2, pkbGetter->getAllConsts()[1]);

		Assert::AreEqual(size_t(7), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(7), pkbGetter->getStmtsFromProc(procIdx).size());
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

		Parser parser;
		SourceAST ast = parser.parse(sampleSource3);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(varName1, pkbGetter->getAttributeNameByStmtIdx(stmtIdx1));
		Assert::AreEqual(varName2, pkbGetter->getAttributeNameByStmtIdx(stmtIdx7));
		Assert::IsTrue(std::vector{ stmtIdx1 } == pkbGetter->getEqualNameAttributesFromName(EntityType::READ, varName1));
		Assert::IsTrue(std::vector{ stmtIdx7 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PRINT, varName2));
		Assert::IsTrue(std::vector{ procIdx } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::AreEqual(size_t(0), pkbGetter->getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT).size());
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::WHILE, 3));
		Assert::IsFalse(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 0));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 0));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 10));
		Assert::IsFalse(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 1));
	}

	TEST_METHOD(blackBoxSampleSource3_checkPattern) {
		std::string varName1 = "number";
		std::string varName2 = "digit";

		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(2);

		Parser parser;
		SourceAST ast = parser.parse(sampleSource3);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix(varName1)).size());

		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix(varName1)).size());

		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("number%10"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("number%((10))"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("sum+digit"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("digit+sum"))).size());
		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("10"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("1"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("10%number"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx1).size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIdx1).size());
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

		Parser parser;
		SourceAST ast = parser.parse(sampleSource3);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(8), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));

		Assert::AreEqual(size_t(8), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::AreEqual(size_t(3), pkbGetter->getRSInfoFromRightArg(RelationshipType::USES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)).size());

		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));

		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());

		Assert::AreEqual(size_t(7), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx4));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx4, stmtIdx5));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx5, stmtIdx6));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx6, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx7));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx6, stmtIdx7));

		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx1, stmtIdx3));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx3, stmtIdx4));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx4, stmtIdx5));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx3, stmtIdx7));
		Assert::AreEqual(size_t(9), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx1, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx4, stmtIdx6));

		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx3, stmtIdx4));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx3, stmtIdx5));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx3, stmtIdx6));
		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx3, stmtIdx4));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx3, stmtIdx5));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx3, stmtIdx6));
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

		Parser parser;
		SourceAST ast = parser.parse(sampleSource4);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(4), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(procName1, pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE)[0]));
		Assert::AreEqual(procName4, pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE)[3]));

		Assert::AreEqual(size_t(23), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(4), pkbGetter->getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(2), pkbGetter->getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(11), pkbGetter->getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(4), pkbGetter->getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(7), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(varName1, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[0]));
		Assert::AreEqual(varName3, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[2]));

		Assert::AreEqual(size_t(2), pkbGetter->getAllConsts().size());
		Assert::AreEqual(constVal1, pkbGetter->getAllConsts()[0]);
		Assert::AreEqual(constVal2, pkbGetter->getAllConsts()[1]);

		Assert::AreEqual(size_t(3), pkbGetter->getStmtsFromProc(procIdx1).size());
		Assert::AreEqual(size_t(2), pkbGetter->getStmtsFromProc(procIdx2).size());
		Assert::AreEqual(size_t(4), pkbGetter->getStmtsFromProc(procIdx3).size());
		Assert::AreEqual(size_t(14), pkbGetter->getStmtsFromProc(procIdx4).size());
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

		Parser parser;
		SourceAST ast = parser.parse(sampleSource4);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(procName4, pkbGetter->getAttributeNameByStmtIdx(stmtIdx2));
		Assert::AreEqual(procName3, pkbGetter->getAttributeNameByStmtIdx(stmtIdx3));
		Assert::AreEqual(varName2, pkbGetter->getAttributeNameByStmtIdx(stmtIdx4));
		Assert::AreEqual(varName3, pkbGetter->getAttributeNameByStmtIdx(stmtIdx5));
		Assert::AreEqual(varName1, pkbGetter->getAttributeNameByStmtIdx(stmtIdx6));
		Assert::IsTrue(std::vector{ stmtIdx2 } == pkbGetter->getEqualNameAttributesFromName(EntityType::CALL, procName4));
		Assert::IsTrue(std::vector{ stmtIdx3 } == pkbGetter->getEqualNameAttributesFromName(EntityType::CALL, procName3));
		Assert::IsTrue(std::vector{ stmtIdx4 } == pkbGetter->getEqualNameAttributesFromName(EntityType::READ, varName2));
		Assert::IsTrue(std::vector{ stmtIdx5 } == pkbGetter->getEqualNameAttributesFromName(EntityType::READ, varName3));
		Assert::IsTrue(std::vector{ stmtIdx6 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PRINT, varName1));
		Assert::IsTrue(std::vector{ procIdx1 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName1));
		Assert::IsTrue(std::vector{ procIdx2 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName2));
		Assert::IsTrue(std::vector{ 1 } == pkbGetter->getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CALL, 3));
		Assert::IsFalse(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 0));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 1));
	}

	TEST_METHOD(blackBoxSampleSource4_checkPattern) {
		std::string varName1 = "flag";
		std::string varName2 = "x";
		std::string varName3 = "y";
		std::string varName4 = "cenX";
		std::string varName5 = "cenY";

		Parser parser;
		SourceAST ast = parser.parse(sampleSource4);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(11), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(2), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4), ExpressionProcessor::convertInfixToPostFix(varName4)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(2), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5), ExpressionProcessor::convertInfixToPostFix(varName5)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5), ExpressionProcessor::convertInfixToPostFix(varName3)).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4), ExpressionProcessor::convertInfixToPostFix("cenX/count")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4), ExpressionProcessor::convertInfixToPostFix("cenX/(count)")).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName4))).size());
		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName5))).size());
		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());
		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("1"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());

		VarIndex varIdx1 = pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1);
		VarIndex varIdx2 = pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2);

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx1).size());

		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIdx1).size());
		Assert::AreEqual(size_t(1), pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIdx2).size());
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

		Parser parser;
		SourceAST ast = parser.parse(sampleSource4);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(32), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx14, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx14, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx14, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx14, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx18, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx18, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));

		Assert::AreEqual(size_t(32), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx8, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx14, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx14, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx14, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx14, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx19, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx19, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::AreEqual(size_t(2), pkbGetter->getRSInfoFromRightArg(RelationshipType::USES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)).size());

		Assert::AreEqual(size_t(16), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));

		Assert::AreEqual(size_t(16), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));

		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::CALLS, procIdx1, procIdx4));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::CALLS, procIdx1, procIdx3));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::CALLS, procIdx1, procIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::CALLS, procIdx4, procIdx2));
		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::CALLS_T, procIdx1, procIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::CALLS_T, procIdx1, procIdx3));

		Assert::AreEqual(size_t(21), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx4));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx4, stmtIdx5));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx5, stmtIdx6));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx6, stmtIdx7));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx7, stmtIdx8));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx8, stmtIdx9));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx14, stmtIdx15));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx18, stmtIdx14));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx14, stmtIdx19));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx19, stmtIdx20));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx20, stmtIdx23));

		Assert::AreEqual(size_t(16), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx1, stmtIdx3));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx3, stmtIdx4));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx4, stmtIdx5));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx5, stmtIdx6));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx14, stmtIdx19));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx19, stmtIdx23));
		Assert::AreEqual(size_t(38), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx1, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx14, stmtIdx23));

		Assert::AreEqual(size_t(7), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx14, stmtIdx15));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx19, stmtIdx20));
		Assert::AreEqual(size_t(7), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
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

		Parser parser;
		SourceAST ast = parser.parse(mixedStmtsSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(1), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(procName, pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE)[0]));

		Assert::AreEqual(size_t(14), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(11), pkbGetter->getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(7), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(varName1, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[0]));
		Assert::AreEqual(varName2, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[6]));

		Assert::AreEqual(size_t(4), pkbGetter->getAllConsts().size());
		Assert::AreEqual(constVal1, pkbGetter->getAllConsts()[0]);
		Assert::AreEqual(constVal2, pkbGetter->getAllConsts()[1]);
		Assert::AreEqual(constVal3, pkbGetter->getAllConsts()[2]);
		Assert::AreEqual(constVal4, pkbGetter->getAllConsts()[3]);

		Assert::AreEqual(size_t(14), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(14), pkbGetter->getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(mixedStmtsSource_checkAttributes) {
		std::string varName1 = "count";
		std::string varName2 = "normSq";
		std::string procName = "computeCentroid";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx12 = StmtIndex(12);
		StmtIndex stmtIdx13 = StmtIndex(13);

		Parser parser;
		SourceAST ast = parser.parse(mixedStmtsSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(varName1, pkbGetter->getAttributeNameByStmtIdx(stmtIdx13));
		Assert::IsTrue(std::vector{ stmtIdx13 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PRINT, varName1));
		Assert::IsTrue(std::vector{ procIdx } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::IsTrue(std::vector{ 5, 3, 1 } == pkbGetter->getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::PRINT, 13));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 1));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 5));
	}

	TEST_METHOD(mixedStmtsSource_checkPattern) {
		std::string varName1 = "count";
		std::string varName2 = "x";
		std::string varName3 = "flag";
		std::string varName4 = "y";

		Parser parser;
		SourceAST ast = parser.parse(mixedStmtsSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(11), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("5")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix("1")).size());

		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("5")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("count+1")).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("1+count")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix("1")).size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName3))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("x+3"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("3+x"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("x+3"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("3+x"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("cenX*cenX"))).size());

		VarIndex varIdx1 = pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1);
		VarIndex varIdx2 = pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2);
		VarIndex varIdx4 = pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4);

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx1).size());

		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIdx2).size());
		Assert::AreEqual(size_t(1), pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIdx4).size());
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

		Parser parser;
		SourceAST ast = parser.parse(mixedStmtsSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(16), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx10, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx11, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx12, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::AreEqual(size_t(3), pkbGetter->getRSInfoFromRightArg(RelationshipType::MODIFIES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)).size());

		Assert::AreEqual(size_t(16), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx8, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx8, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx9, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::AreEqual(size_t(4), pkbGetter->getRSInfoFromRightArg(RelationshipType::USES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)).size());

		Assert::AreEqual(size_t(6), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));

		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());

		Assert::AreEqual(size_t(15), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx6, stmtIdx7));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx7, stmtIdx8));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx9, stmtIdx6));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx10, stmtIdx11));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx10, stmtIdx12));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx12, stmtIdx13));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx11, stmtIdx14));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx13, stmtIdx14));

		Assert::AreEqual(size_t(10), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx1, stmtIdx6));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx6, stmtIdx7));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx7, stmtIdx8));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx6, stmtIdx10));
		Assert::AreEqual(size_t(32), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx1, stmtIdx6));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx6, stmtIdx14));

		Assert::AreEqual(size_t(6), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx6, stmtIdx7));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx10, stmtIdx11));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx10, stmtIdx12));
		Assert::AreEqual(size_t(6), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
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

		Parser parser;
		SourceAST ast = parser.parse(nestedWhileIfSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(1), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(procName, pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE)[0]));

		Assert::AreEqual(size_t(12), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(8), pkbGetter->getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(2), pkbGetter->getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(5), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(varName1, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[0]));
		Assert::AreEqual(varName2, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[4]));

		Assert::AreEqual(size_t(5), pkbGetter->getAllConsts().size());
		Assert::AreEqual(constVal1, pkbGetter->getAllConsts()[0]);
		Assert::AreEqual(constVal2, pkbGetter->getAllConsts()[1]);
		Assert::AreEqual(constVal3, pkbGetter->getAllConsts()[2]);
		Assert::AreEqual(constVal4, pkbGetter->getAllConsts()[3]);
		Assert::AreEqual(constVal5, pkbGetter->getAllConsts()[4]);

		Assert::AreEqual(size_t(12), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(12), pkbGetter->getStmtsFromProc(procIdx).size());
	}
	TEST_METHOD(nestedWhileIfSource_checkAttributes) {
		std::string varName1 = "present";
		std::string varName2 = "future";
		std::string procName = "Peter";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx12 = StmtIndex(12);

		Parser parser;
		SourceAST ast = parser.parse(nestedWhileIfSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(varName1, pkbGetter->getAttributeNameByStmtIdx(stmtIdx1));
		Assert::AreEqual(varName2, pkbGetter->getAttributeNameByStmtIdx(stmtIdx12));
		Assert::IsTrue(std::vector{ stmtIdx1 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PRINT, varName1));
		Assert::IsTrue(std::vector{ stmtIdx12 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PRINT, varName2));
		Assert::IsTrue(std::vector{ procIdx } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::IsTrue(std::vector{ 8, 4, 2, 1 } == pkbGetter->getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::PRINT, 1));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::PRINT, 12));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 8));
	}

	TEST_METHOD(nestedWhileIfSource_checkPattern) {
		std::string varName1 = "present";
		std::string varName2 = "future";
		std::string varName3 = "y";
		std::string varName4 = "z";
		std::string varName5 = "x";

		Parser parser;
		SourceAST ast = parser.parse(nestedWhileIfSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(8), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(2), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("z-8")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("x*y")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("x*y")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("x*y*z")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("x*y*z")).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("y*z")).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("z-8-(x*y)")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("8")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix(varName3)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4), ExpressionProcessor::convertInfixToPostFix("2")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5), ExpressionProcessor::convertInfixToPostFix(varName5)).size());

		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("8*present")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5), ExpressionProcessor::convertInfixToPostFix("x-1")).size());

		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("z-8"))).size());
		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("x*y"))).size());
		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("x*y*z"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("z-8-(x*y)"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("8-(x*y)"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("z-8"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("8*present"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("x-1"))).size());

		VarIndex varIdx1 = pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1);
		VarIndex varIdx2 = pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2);
		VarIndex varIdx5 = pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5);

		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx1).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx2).size());
		Assert::AreEqual(size_t(1), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx5).size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIdx1).size());
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

		Parser parser;
		SourceAST ast = parser.parse(nestedWhileIfSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(14), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx8, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::AreEqual(size_t(3), pkbGetter->getRSInfoFromRightArg(RelationshipType::MODIFIES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)).size());

		Assert::AreEqual(size_t(27), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx8, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx8, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx8, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx12, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::AreEqual(size_t(8), pkbGetter->getRSInfoFromRightArg(RelationshipType::USES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)).size());

		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));

		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());

		Assert::AreEqual(size_t(13), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx5, stmtIdx6));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx6, stmtIdx7));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx7, stmtIdx8));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx7, stmtIdx10));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx8, stmtIdx9));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx9, stmtIdx5));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx10, stmtIdx5));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx5, stmtIdx11));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx11, stmtIdx12));

		Assert::AreEqual(size_t(8), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx1, stmtIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx2, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx5, stmtIdx11));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx6, stmtIdx7));
		Assert::AreEqual(size_t(23), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx1, stmtIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx2, stmtIdx11));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx8, stmtIdx10));

		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx5, stmtIdx6));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx5, stmtIdx7));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx7, stmtIdx8));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx7, stmtIdx10));
		Assert::AreEqual(size_t(8), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx5, stmtIdx8));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx5, stmtIdx10));
	}

	TEST_METHOD(multipleBracketsSource_checkEntities) {
		std::string varName1 = "number";
		std::string varName2 = "digit";
		std::string procName = "sumDigits";
		VarIndex constVal1 = 0;
		VarIndex constVal2 = 10;

		ProcIndex procIdx = ProcIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(multipleBracketsSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(1), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(procName, pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE)[0]));

		Assert::AreEqual(size_t(7), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(4), pkbGetter->getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(3), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(varName1, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[0]));
		Assert::AreEqual(varName2, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[2]));

		Assert::AreEqual(size_t(2), pkbGetter->getAllConsts().size());
		Assert::AreEqual(constVal1, pkbGetter->getAllConsts()[0]);
		Assert::AreEqual(constVal2, pkbGetter->getAllConsts()[1]);

		Assert::AreEqual(size_t(7), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(7), pkbGetter->getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(multipleBracketsSource_checkAttributes) {
		std::string varName1 = "number";
		std::string varName2 = "sum";
		std::string varName3 = "digit";
		std::string procName = "sumDigits";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx7 = StmtIndex(7);

		Parser parser;
		SourceAST ast = parser.parse(multipleBracketsSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(varName1, pkbGetter->getAttributeNameByStmtIdx(stmtIdx1));
		Assert::AreEqual(varName2, pkbGetter->getAttributeNameByStmtIdx(stmtIdx7));
		Assert::IsTrue(std::vector{ stmtIdx1 } == pkbGetter->getEqualNameAttributesFromName(EntityType::READ, varName1));
		Assert::IsTrue(std::vector{ stmtIdx7 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PRINT, varName2));
		Assert::IsTrue(std::vector{ procIdx } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::AreEqual(size_t(0), pkbGetter->getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT).size());
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::WHILE, 3));
		Assert::IsFalse(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsFalse(pkbGetter->hasEqualIntegerAttribute(EntityType::STMT, 8));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::READ, 1));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::PRINT, 7));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 0));
	}

	TEST_METHOD(multipleBracketsSource_checkPattern) {
		std::string varName1 = "number";
		std::string varName2 = "sum";
		std::string varName3 = "digit";

		VarIndex varIdx = VarIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(multipleBracketsSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("10")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix(varName3)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix("number%10")).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix("10%number")).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix("number%10")).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix("10%number")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("sum+digit")).size());

		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName3))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("number%10"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("(((number))%10)"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("10%number"))).size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("number%10"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("((0))"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx).size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIdx).size());
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

		Parser parser;
		SourceAST ast = parser.parse(multipleBracketsSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(8), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::AreEqual(size_t(3), pkbGetter->getRSInfoFromRightArg(RelationshipType::MODIFIES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)).size());

		Assert::AreEqual(size_t(8), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::AreEqual(size_t(3), pkbGetter->getRSInfoFromRightArg(RelationshipType::USES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)).size());

		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));

		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());

		Assert::AreEqual(size_t(7), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx4));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx4, stmtIdx5));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx5, stmtIdx6));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx6, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx7));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx6, stmtIdx7));

		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx1, stmtIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx2, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx5, stmtIdx6));
		Assert::AreEqual(size_t(9), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx1, stmtIdx3));

		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx3, stmtIdx5));
		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
	}

	TEST_METHOD(sameKeywordNameSource_checkEntities) {
		std::string varName1 = "read";
		std::string varName2 = "if";
		std::string varName3 = "procedure";
		std::string procName = "procedure";
		VarIndex constVal1 = 0;
		VarIndex constVal2 = 1;

		ProcIndex procIdx = ProcIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(sameKeywordNameSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(1), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(procName, pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE)[0]));

		Assert::AreEqual(size_t(7), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(5), pkbGetter->getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(5), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(varName1, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[0]));
		Assert::AreEqual(varName2, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[1]));
		Assert::AreEqual(varName3, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[4]));

		Assert::AreEqual(size_t(2), pkbGetter->getAllConsts().size());
		Assert::AreEqual(constVal1, pkbGetter->getAllConsts()[0]);
		Assert::AreEqual(constVal2, pkbGetter->getAllConsts()[1]);

		Assert::AreEqual(size_t(7), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(7), pkbGetter->getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(sameKeywordNameSource_checkAttributes) {
		std::string varName1 = "read";
		std::string varName2 = "print";
		std::string procName = "procedure";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx7 = StmtIndex(7);

		Parser parser;
		SourceAST ast = parser.parse(sameKeywordNameSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(varName1, pkbGetter->getAttributeNameByStmtIdx(stmtIdx1));
		Assert::AreEqual(varName2, pkbGetter->getAttributeNameByStmtIdx(stmtIdx7));
		Assert::IsTrue(std::vector{ stmtIdx1 } == pkbGetter->getEqualNameAttributesFromName(EntityType::READ, varName1));
		Assert::IsTrue(std::vector{ stmtIdx7 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PRINT, varName2));
		Assert::IsTrue(std::vector{ procIdx } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::IsTrue(std::vector{ 1 } == pkbGetter->getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::READ, 1));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsFalse(pkbGetter->hasEqualIntegerAttribute(EntityType::PRINT, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 2));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 4));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 5));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 6));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 0));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 1));
	}

	TEST_METHOD(sameKeywordNameSource_checkPattern) {
		std::string varName1 = "read";
		std::string varName2 = "if";
		std::string varName3 = "print";
		std::string varName4 = "while";
		std::string varName5 = "procedure";

		VarIndex varIdx = VarIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(sameKeywordNameSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5), ExpressionProcessor::convertInfixToPostFix("1")).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("0")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5), ExpressionProcessor::convertInfixToPostFix("1")).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("1"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("1"))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIdx).size());
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

		Parser parser;
		SourceAST ast = parser.parse(sameKeywordNameSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(6), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::AreEqual(size_t(2), pkbGetter->getRSInfoFromRightArg(RelationshipType::MODIFIES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)).size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));
		Assert::AreEqual(size_t(1), pkbGetter->getRSInfoFromRightArg(RelationshipType::USES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)).size());

		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName5)));

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());

		Assert::AreEqual(size_t(6), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx4));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx4, stmtIdx5));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx5, stmtIdx6));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx6, stmtIdx7));

		Assert::AreEqual(size_t(6), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx1, stmtIdx2));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx2, stmtIdx7));
		Assert::AreEqual(size_t(21), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx1, stmtIdx7));

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
	}

	TEST_METHOD(longAssignmentExprSource_checkEntities) {
		std::string varName1 = "x";
		std::string varName2 = "g";
		std::string procName = "main";
		VarIndex constVal = 9;

		ProcIndex procIdx = ProcIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(longAssignmentExprSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(1), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(procName, pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE)[0]));

		Assert::AreEqual(size_t(1), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::IF_TYPE).size());

		/* ordering: x, a, y, b, z, c, d, f, g */
		Assert::AreEqual(size_t(9), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(varName1, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[0]));
		Assert::AreEqual(varName2, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[8]));

		Assert::AreEqual(size_t(1), pkbGetter->getAllConsts().size());
		Assert::AreEqual(constVal, pkbGetter->getAllConsts()[0]);

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(longAssignmentExprSource_checkAttributes) {
		std::string procName = "main";

		ProcIndex procIdx = ProcIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(longAssignmentExprSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::IsTrue(std::vector{ procIdx } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::AreEqual(size_t(0), pkbGetter->getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT).size());
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::STMT, 1));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 1));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 9));
	}

	TEST_METHOD(longAssignmentExprSource_checkPattern) {
		std::string varName1 = "x";
		std::string varName2 = "d";

		VarIndex varIdx = VarIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(longAssignmentExprSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix(varName2)).size());

		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("x+y/z%a*b/c%d+f-g+z%a*b/c%d+f-g-x+y/z%a*b/c%d+f-g+z%a*b/c%d+f-g+z%a*b/c%d+f-g+z%a*b/c%d+f-9")).size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("x+y"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("x+y/z%a*b/c%d+f-g+z%a*b/c%d+f-g-x+y/z%a*b/c%d+f-g+z%a*b/c%d+f-g+z%a*b/c%d+f-g+z%a*b/c%d+f-9"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIdx).size());
	}

	TEST_METHOD(longAssignmentExprSource_checkRelationships) {
		std::string varName1 = "x";
		std::string varName2 = "d";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx = StmtIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(longAssignmentExprSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::AreEqual(size_t(1), pkbGetter->getRSInfoFromRightArg(RelationshipType::MODIFIES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)).size());

		Assert::AreEqual(size_t(9), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::AreEqual(size_t(1), pkbGetter->getRSInfoFromRightArg(RelationshipType::USES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)).size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));

		Assert::AreEqual(size_t(9), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
	}

	TEST_METHOD(longVarNameSource_checkEntities) {
		std::string varName = "extremelyVeryLongVariableNameThatHasManyCharactersAndIsVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLong";
		std::string procName = "main";

		ProcIndex procIdx = ProcIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(longVarNameSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(1), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(procName, pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE)[0]));

		Assert::AreEqual(size_t(1), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::READ_TYPE).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(varName, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[0]));

		Assert::AreEqual(size_t(0), pkbGetter->getAllConsts().size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(longVarNameSource_checkAttributes) {
		std::string varName = "extremelyVeryLongVariableNameThatHasManyCharactersAndIsVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLong";
		std::string procName = "main";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx = StmtIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(longVarNameSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(varName, pkbGetter->getAttributeNameByStmtIdx(stmtIdx));
		Assert::IsTrue(std::vector{ procIdx } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::AreEqual(size_t(0), pkbGetter->getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT).size());
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::STMT, 1));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::READ, 1));
	}

	TEST_METHOD(longVarNameSource_checkPattern) {
		std::string varName = "extremelyVeryLongVariableNameThatHasManyCharactersAndIsVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLong";

		VarIndex varIdx = VarIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(longVarNameSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName), ExpressionProcessor::convertInfixToPostFix(varName)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName), ExpressionProcessor::convertInfixToPostFix(varName)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName))).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIdx).size());
	}

	TEST_METHOD(longVarNameSource_checkRelationships) {
		std::string varName = "extremelyVeryLongVariableNameThatHasManyCharactersAndIsVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLong";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx = StmtIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(longVarNameSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName)));
		Assert::AreEqual(size_t(1), pkbGetter->getRSInfoFromRightArg(RelationshipType::MODIFIES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName)).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getRSInfoFromRightArg(RelationshipType::USES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName)).size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName)));

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
	}

	TEST_METHOD(multipleNestingSource_checkEntities) {
		std::string varName1 = "present";
		std::string varName2 = "x";
		std::string procName = "Peter";
		VarIndex constVal1 = 0;
		VarIndex constVal2 = 8;
		VarIndex constVal3 = 1;

		ProcIndex procIdx = ProcIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(multipleNestingSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(1), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(procName, pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE)[0]));

		Assert::AreEqual(size_t(11), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(7), pkbGetter->getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(2), pkbGetter->getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(2), pkbGetter->getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(4), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(varName1, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[0]));
		Assert::AreEqual(varName2, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[2]));

		Assert::AreEqual(size_t(3), pkbGetter->getAllConsts().size());
		Assert::AreEqual(constVal1, pkbGetter->getAllConsts()[0]);
		Assert::AreEqual(constVal2, pkbGetter->getAllConsts()[1]);
		Assert::AreEqual(constVal3, pkbGetter->getAllConsts()[2]);

		Assert::AreEqual(size_t(11), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(11), pkbGetter->getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(multipleNestingSource_checkAttributes) {
		std::string varName1 = "present";
		std::string varName2 = "y";
		std::string procName = "Peter";

		ProcIndex procIdx = ProcIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(multipleNestingSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::IsTrue(std::vector{ procIdx } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::IsTrue(std::vector{ 8, 1 } == pkbGetter->getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::WHILE, 1));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::IF, 2));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::WHILE, 8));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 1));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 8));
	}

	TEST_METHOD(multipleNestingSource_checkPattern) {
		std::string varName1 = "present";
		std::string varName2 = "y";
		std::string varName3 = "x";
		std::string varName4 = "z";

		VarIndex varIdx1 = VarIndex(1);
		VarIndex varIdx2 = VarIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(multipleNestingSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(7), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(3), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix(varName1)).size());
		Assert::AreEqual(size_t(2), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(3), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("sent")).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("y*z")).size());
		Assert::AreEqual(size_t(3), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("present*y*z")).size());
		Assert::AreEqual(size_t(3), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("present*y")).size());
		Assert::AreEqual(size_t(2), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("x*y*z")).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("x*(y*z)")).size());

		Assert::AreEqual(size_t(3), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("present*y*z")).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("present*y")).size());
		Assert::AreEqual(size_t(2), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("z-8-x*y*z")).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("x*(y*z)")).size());

		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());
		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName3))).size());
		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName4))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("y*z"))).size());
		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("present*y*z"))).size());
		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("present*y"))).size());
		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("x*y*z"))).size());

		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("present*y*z"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("present*y"))).size());
		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("z-8-x*y*z"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("x*(y*z)"))).size());

		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(2), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx1).size());
		Assert::AreEqual(size_t(2), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx2).size());

		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
		Assert::AreEqual(size_t(2), pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIdx1).size());
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

		Parser parser;
		SourceAST ast = parser.parse(multipleNestingSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(17), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx8, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx8, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx9, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx9, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::AreEqual(size_t(5), pkbGetter->getRSInfoFromRightArg(RelationshipType::MODIFIES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)).size());
		Assert::AreEqual(size_t(7), pkbGetter->getRSInfoFromRightArg(RelationshipType::MODIFIES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)).size());

		Assert::AreEqual(size_t(32), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx6, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx7, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx8, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx8, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx8, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx9, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::AreEqual(size_t(7), pkbGetter->getRSInfoFromRightArg(RelationshipType::USES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)).size());
		Assert::AreEqual(size_t(9), pkbGetter->getRSInfoFromRightArg(RelationshipType::USES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)).size());

		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));

		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());

		Assert::AreEqual(size_t(14), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx11));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx4));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx4, stmtIdx5));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx5, stmtIdx6));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx4, stmtIdx7));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx7, stmtIdx8));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx8, stmtIdx9));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx9, stmtIdx8));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx8, stmtIdx10));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx6, stmtIdx10));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx10, stmtIdx1));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx11, stmtIdx1));

		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx3, stmtIdx10));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx3, stmtIdx4));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx5, stmtIdx6));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx6, stmtIdx7));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx7, stmtIdx8));
		Assert::AreEqual(size_t(5), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx3, stmtIdx10));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx6, stmtIdx7));

		Assert::AreEqual(size_t(10), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx1, stmtIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx2, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx2, stmtIdx4));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx4, stmtIdx5));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx4, stmtIdx6));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx4, stmtIdx7));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx4, stmtIdx8));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx8, stmtIdx9));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx2, stmtIdx10));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::PARENT, stmtIdx3, stmtIdx4));

		Assert::AreEqual(size_t(25), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx1, stmtIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx2, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx2, stmtIdx4));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx4, stmtIdx5));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx4, stmtIdx6));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx4, stmtIdx7));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx4, stmtIdx8));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx8, stmtIdx9));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx2, stmtIdx10));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx3, stmtIdx4));
		Assert::AreEqual(size_t(3), pkbGetter->getRSInfoFromRightArg(RelationshipType::PARENT_T, stmtIdx5).size());

		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx1, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx1, stmtIdx4));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx1, stmtIdx5));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx1, stmtIdx6));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx1, stmtIdx7));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx1, stmtIdx8));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx1, stmtIdx9));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx1, stmtIdx10));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx2, stmtIdx5));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx2, stmtIdx6));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx2, stmtIdx7));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx2, stmtIdx8));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx2, stmtIdx9));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::PARENT_T, stmtIdx4, stmtIdx9));
	}

	TEST_METHOD(differentCasingNamesSource_checkEntities) {
		std::string varName1 = "v";
		std::string varName2 = "V1";
		std::string procName = "mAin";
		VarIndex constVal = 1;

		ProcIndex procIdx = ProcIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(differentCasingNamesSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(1), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(procName, pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE)[0]));
		Assert::IsFalse("main" == pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE)[0]));

		Assert::AreEqual(size_t(4), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(4), pkbGetter->getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(4), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(varName1, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[0]));
		Assert::AreEqual(varName2, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[3]));

		Assert::AreEqual(size_t(1), pkbGetter->getAllConsts().size());
		Assert::AreEqual(constVal, pkbGetter->getAllConsts()[0]);

		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(4), pkbGetter->getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(differentCasingNamesSource_checkAttributes) {
		std::string varName1 = "v";
		std::string varName2 = "V1";
		std::string procName = "mAin";

		ProcIndex procIdx = ProcIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(differentCasingNamesSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::IsTrue(std::vector{ procIdx } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::IsTrue(std::vector{ 1 } == pkbGetter->getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::STMT, 3));
		Assert::IsFalse(pkbGetter->hasEqualIntegerAttribute(EntityType::STMT, 5));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 1));
	}

	TEST_METHOD(differentCasingNamesSource_checkPattern) {
		std::string varName1 = "v";
		std::string varName2 = "V";
		std::string varName3 = "v1";
		std::string varName4 = "V1";

		VarIndex varIdx = VarIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(differentCasingNamesSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4), ExpressionProcessor::convertInfixToPostFix("1")).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix("1")).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4), ExpressionProcessor::convertInfixToPostFix("1")).size());

		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("1"))).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName1))).size());

		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("1"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIdx).size());
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

		Parser parser;
		SourceAST ast = parser.parse(differentCasingNamesSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::AreEqual(size_t(1), pkbGetter->getRSInfoFromRightArg(RelationshipType::MODIFIES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());

		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName4)));

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());

		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx4));

		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx1, stmtIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx2, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx3, stmtIdx4));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx1, stmtIdx3));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx1, stmtIdx4));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx2, stmtIdx4));
		Assert::AreEqual(size_t(6), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx1, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx1, stmtIdx4));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS_T, stmtIdx2, stmtIdx4));

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
	}

	TEST_METHOD(multipleWhitespacesSource_checkEntities) {
		std::string varName = "v";
		std::string procName = "main";
		VarIndex constVal = 0;

		ProcIndex procIdx = ProcIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(multipleWhitespacesSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(1), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(procName, pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE)[0]));

		Assert::AreEqual(size_t(3), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(varName, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[0]));

		Assert::AreEqual(size_t(1), pkbGetter->getAllConsts().size());
		Assert::AreEqual(constVal, pkbGetter->getAllConsts()[0]);

		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(3), pkbGetter->getStmtsFromProc(procIdx).size());
	}

	TEST_METHOD(multipleWhitespacesSource_checkAttributes) {
		std::string varName = "v";
		std::string procName = "main";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);

		Parser parser;
		SourceAST ast = parser.parse(multipleWhitespacesSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(varName, pkbGetter->getAttributeNameByStmtIdx(stmtIdx1));
		Assert::IsFalse(varName == pkbGetter->getAttributeNameByStmtIdx(stmtIdx2));
		Assert::AreEqual(varName, pkbGetter->getAttributeNameByStmtIdx(stmtIdx3));
		Assert::IsTrue(std::vector{ stmtIdx1 } == pkbGetter->getEqualNameAttributesFromName(EntityType::READ, varName));
		Assert::IsTrue(std::vector{ stmtIdx3 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PRINT, varName));
		Assert::IsTrue(std::vector{ procIdx } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName));
		Assert::AreEqual(size_t(0), pkbGetter->getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT).size());
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::STMT, 2));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::READ, 1));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 2));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::PRINT, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 0));
	}

	TEST_METHOD(multipleWhitespacesSource_checkPattern) {
		std::string varName = "v";

		VarIndex varIdx = VarIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(multipleWhitespacesSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName), ExpressionProcessor::convertInfixToPostFix(varName)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName), ExpressionProcessor::convertInfixToPostFix("0")).size());

		Assert::AreEqual(size_t(0), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName), ExpressionProcessor::convertInfixToPostFix(varName)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName), ExpressionProcessor::convertInfixToPostFix("0")).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("0"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIdx).size());
	}

	TEST_METHOD(multipleWhitespacesSource_checkRelationships) {
		std::string varName = "v";

		ProcIndex procIdx = ProcIndex(1);
		StmtIndex stmtIdx1 = StmtIndex(1);
		StmtIndex stmtIdx2 = StmtIndex(2);
		StmtIndex stmtIdx3 = StmtIndex(3);

		Parser parser;
		SourceAST ast = parser.parse(multipleWhitespacesSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName)));
		Assert::AreEqual(size_t(2), pkbGetter->getRSInfoFromRightArg(RelationshipType::MODIFIES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName)).size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName)));
		Assert::AreEqual(size_t(1), pkbGetter->getRSInfoFromRightArg(RelationshipType::USES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName)).size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName)));

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName)));

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());

		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3));

		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx1, stmtIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx2, stmtIdx3));
		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
	}

	TEST_METHOD(multipleProceduresRecursiveCallsSource_checkEntities) {
		std::string varName = "x";
		std::string procName = "first";

		ProcIndex procIdx1 = ProcIndex(1);
		ProcIndex procIdx2 = ProcIndex(2);
		ProcIndex procIdx3 = ProcIndex(3);
		ProcIndex procIdx4 = ProcIndex(4);

		Parser parser;
		SourceAST ast = parser.parse(multipleProceduresRecursiveCallsSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(4), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(procName, pkbGetter->getNameIdxEntityName(EntityType::PROCEDURE, pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE)[0]));

		Assert::AreEqual(size_t(6), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtIdxFromType(StatementType::READ_TYPE).size());
		Assert::AreEqual(size_t(2), pkbGetter->getStmtIdxFromType(StatementType::ASSIGN_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::PRINT_TYPE).size());
		Assert::AreEqual(size_t(3), pkbGetter->getStmtIdxFromType(StatementType::CALL_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::WHILE_TYPE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getStmtIdxFromType(StatementType::IF_TYPE).size());

		Assert::AreEqual(size_t(3), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(varName, pkbGetter->getNameIdxEntityName(EntityType::VARIABLE, pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE)[0]));

		Assert::AreEqual(size_t(1), pkbGetter->getAllConsts().size());

		Assert::AreEqual(size_t(2), pkbGetter->getStmtsFromProc(procIdx1).size());
		Assert::AreEqual(size_t(2), pkbGetter->getStmtsFromProc(procIdx2).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtsFromProc(procIdx3).size());
		Assert::AreEqual(size_t(1), pkbGetter->getStmtsFromProc(procIdx4).size());
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

		Parser parser;
		SourceAST ast = parser.parse(multipleProceduresRecursiveCallsSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(procName2, pkbGetter->getAttributeNameByStmtIdx(stmtIdx1));
		Assert::AreEqual(procName3, pkbGetter->getAttributeNameByStmtIdx(stmtIdx2));
		Assert::AreEqual(procName4, pkbGetter->getAttributeNameByStmtIdx(stmtIdx3));
		Assert::IsTrue(std::vector{ stmtIdx1 } == pkbGetter->getEqualNameAttributesFromName(EntityType::CALL, procName2));
		Assert::IsTrue(std::vector{ stmtIdx2 } == pkbGetter->getEqualNameAttributesFromName(EntityType::CALL, procName3));
		Assert::IsTrue(std::vector{ stmtIdx3 } == pkbGetter->getEqualNameAttributesFromName(EntityType::CALL, procName4));
		Assert::IsTrue(std::vector{ stmtIdx5 } == pkbGetter->getEqualNameAttributesFromName(EntityType::READ, varName));
		Assert::IsTrue(std::vector{ procIdx1 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName1));
		Assert::IsTrue(std::vector{ procIdx2 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName2));
		Assert::IsTrue(std::vector{ procIdx3 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName3));
		Assert::IsTrue(std::vector{ procIdx4 } == pkbGetter->getEqualNameAttributesFromName(EntityType::PROCEDURE, procName4));
		Assert::IsTrue(std::vector{ 2 } == pkbGetter->getEqualIntegerAttributes(EntityType::CONSTANT, EntityType::STMT));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::STMT, 2));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CALL, 1));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CALL, 2));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CALL, 3));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 4));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::READ, 5));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::ASSIGN, 6));
		Assert::IsTrue(pkbGetter->hasEqualIntegerAttribute(EntityType::CONSTANT, 2));
	}

	TEST_METHOD(multipleProceduresRecursiveCallsSource_checkPattern) {
		std::string varName1 = "x";
		std::string varName2 = "y";
		std::string varName3 = "z";

		VarIndex varIdx = VarIndex(1);

		Parser parser;
		SourceAST ast = parser.parse(multipleProceduresRecursiveCallsSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprPartialMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix("2")).size());

		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1), ExpressionProcessor::convertInfixToPostFix(varName2)).size());
		Assert::AreEqual(size_t(1), pkbGetter->getAssignStmtsFromVarExprFullMatch(pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3), ExpressionProcessor::convertInfixToPostFix("2")).size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprPartialMatch(ExpressionProcessor::convertInfixToPostFix("2"))).size());

		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix(varName2))).size());
		Assert::AreEqual(size_t(1), std::get<0>(pkbGetter->getAssignStmtsFromExprFullMatch(ExpressionProcessor::convertInfixToPostFix("2"))).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::IF_TYPE, varIdx).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
		Assert::AreEqual(size_t(0), pkbGetter->getPatternContainerStmtsFromVar(StatementType::WHILE_TYPE, varIdx).size());
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

		Parser parser;
		SourceAST ast = parser.parse(multipleProceduresRecursiveCallsSource);
		DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
		designExtractor->extract();

		Assert::AreEqual(size_t(7), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::AreEqual(size_t(4), pkbGetter->getRSInfoFromRightArg(RelationshipType::MODIFIES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)).size());

		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_S, stmtIdx5, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::AreEqual(size_t(2), pkbGetter->getRSInfoFromRightArg(RelationshipType::USES_S, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)).size());

		Assert::AreEqual(size_t(6), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_P, procIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));

		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx1, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx2, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName2)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx3, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName1)));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::USES_P, procIdx4, pkbGetter->getNameIdxEntityIndex(EntityType::VARIABLE, varName3)));

		Assert::AreEqual(size_t(3), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::CALLS, procIdx1, procIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::CALLS, procIdx1, procIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::CALLS, procIdx2, procIdx4));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::CALLS, procIdx1, procIdx4));
		Assert::AreEqual(size_t(4), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::CALLS_T, procIdx1, procIdx4));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::CALLS_T, procIdx1, procIdx2));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::CALLS_T, procIdx1, procIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::CALLS_T, procIdx2, procIdx4));

		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx1, stmtIdx2));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx2, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx3, stmtIdx4));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx4, stmtIdx5));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::NEXT, stmtIdx5, stmtIdx6));

		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx1, stmtIdx2));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx2, stmtIdx3));
		Assert::IsTrue(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx3, stmtIdx4));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx4, stmtIdx5));
		Assert::IsFalse(pkbGetter->getRSContainsInfo(RelationshipType::FOLLOWS, stmtIdx5, stmtIdx6));
		Assert::AreEqual(size_t(2), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());

		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
	}

	TEST_METHOD(missingSemicolon_parserExceptionThrown_checkPKBTables) {
		const char* source =
			"procedure main { \n "
			"	read v; \n "
			"	print v \n "
			"}";
		auto wrapperFunc = [&source] { Parser parser; parser.parse(source); };
		Assert::ExpectException<ParserException>(wrapperFunc);
		try {
			Parser parser;
			SourceAST ast = parser.parse(source);
			PKB& pkb = PKB();
			PKBInserter* pkbInserter = new PKBInserter(pkb);
			DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
			designExtractor->extract();
		} catch (ParserException& ex) {
			Assert::AreEqual(ParserException::MISSING_SEMICOLON.c_str(), ex.what());
		}

		Assert::AreEqual(size_t(0), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllConsts().size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
	}

	TEST_METHOD(callNonExistentProcedure_ASTExceptionThrown_checkPKBTables) {
		const char* source =
			"procedure main { \n "
			"	call Petr; \n "
			"} \n"
			"procedure Peter { \n "
			"	read x; \n "
			"}";
		auto wrapperFunc = [&source] {
			Parser parser;
			SourceAST ast = parser.parse(source);
			ASTValidator::validateAST(ast);
		};
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			Parser parser;
			SourceAST ast = parser.parse(source);
			ASTValidator::validateAST(ast);
			PKB& pkb = PKB();
			PKBInserter* pkbInserter = new PKBInserter(pkb);
			DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
			designExtractor->extract();
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CALL_NON_EXISTENT_PROC_NAME_ERROR.c_str(), ex.what());
		}

		Assert::AreEqual(size_t(0), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllConsts().size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
	}

	TEST_METHOD(duplicateProc_ASTExceptionThrown_checkPKBTables) {
		const char* source =
			"procedure main { \n "
			"	print a; \n "
			"} \n"
			"procedure main { \n "
			"	read a; \n "
			"}";
		auto wrapperFunc = [&source] {
			Parser parser;
			SourceAST ast = parser.parse(source);
			ASTValidator::validateAST(ast);
		};
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			Parser parser;
			SourceAST ast = parser.parse(source);
			ASTValidator::validateAST(ast);
			PKB& pkb = PKB();
			PKBInserter* pkbInserter = new PKBInserter(pkb);
			DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
			designExtractor->extract();
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::DUPLICATE_PROC_NAMES_ERROR.c_str(), ex.what());
		}

		Assert::AreEqual(size_t(0), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllConsts().size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
	}

	TEST_METHOD(selfCall_insideNestedContainerStmts_ASTExceptionThrown_checkPKBTables) {
		const char* source =
			"procedure main { \n "
			"	while (x==y) { \n "
			"      if (x==y) then { read x; } else { \n"
			"          call main; }} \n"
			"}";
		auto wrapperFunc = [&source] {
			Parser parser;
			SourceAST ast = parser.parse(source);
			ASTValidator::validateAST(ast);
		};
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			Parser parser;
			SourceAST ast = parser.parse(source);
			ASTValidator::validateAST(ast);
			PKB& pkb = PKB();
			PKBInserter* pkbInserter = new PKBInserter(pkb);
			DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
			designExtractor->extract();
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CYCLIC_CALL_ERROR.c_str(), ex.what());
		}

		Assert::AreEqual(size_t(0), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllConsts().size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
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

		auto wrapperFunc = [&source] {
			Parser parser;
			SourceAST ast = parser.parse(source);
			ASTValidator::validateAST(ast);
		};
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			Parser parser;
			SourceAST ast = parser.parse(source);
			ASTValidator::validateAST(ast);
			PKB& pkb = PKB();
			PKBInserter* pkbInserter = new PKBInserter(pkb);
			DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
			designExtractor->extract();
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CYCLIC_CALL_ERROR.c_str(), ex.what());
		}

		Assert::AreEqual(size_t(0), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllConsts().size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
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

		auto wrapperFunc = [&source] {
			Parser parser;
			SourceAST ast = parser.parse(source);
			ASTValidator::validateAST(ast);
		};
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			Parser parser;
			SourceAST ast = parser.parse(source);
			ASTValidator::validateAST(ast);
			PKB& pkb = PKB();
			PKBInserter* pkbInserter = new PKBInserter(pkb);
			DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
			designExtractor->extract();
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CYCLIC_CALL_ERROR.c_str(), ex.what());
		}

		Assert::AreEqual(size_t(0), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllConsts().size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
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

		auto wrapperFunc = [&source] {
			Parser parser;
			SourceAST ast = parser.parse(source);
			ASTValidator::validateAST(ast);
		};
		Assert::ExpectException<ASTException>(wrapperFunc);
		try {
			Parser parser;
			SourceAST ast = parser.parse(source);
			ASTValidator::validateAST(ast);
			PKB& pkb = PKB();
			PKBInserter* pkbInserter = new PKBInserter(pkb);
			DesignExtractor* designExtractor = new DesignExtractor(ast, pkbInserter);
			designExtractor->extract();
		} catch (ASTException& ex) {
			Assert::AreEqual(ASTException::CYCLIC_CALL_ERROR.c_str(), ex.what());
		}

		Assert::AreEqual(size_t(0), pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllStmts().size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE).size());
		Assert::AreEqual(size_t(0), pkbGetter->getAllConsts().size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllProcStmts()).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_S)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_S)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::MODIFIES_P)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::USES_P)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::CALLS_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::NEXT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::FOLLOWS_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getRSAllInfo(RelationshipType::PARENT_T)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllAssignPatternInfo()).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::IF_TYPE)).size());
		Assert::AreEqual(size_t(0), std::get<0>(pkbGetter->getAllPatternContainerInfo(StatementType::WHILE_TYPE)).size());
	}
	};
}
