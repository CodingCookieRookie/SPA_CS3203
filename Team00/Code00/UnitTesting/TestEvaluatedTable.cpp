#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>

#include "../source/QPS-NEW/EvaluatedTable.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestEvaluatedTable) {
public:
    TEST_METHOD(innerJoinMerge_emptyLHS_RHSCopiedOver) {
        EvaluatedTable evTable1;

        std::unordered_map<std::string, PqlEntityType> entities = { {"s", PqlEntityType::Stmt} };
        std::unordered_map<std::string, std::vector<int>> table = {
            {"s", { 1, 3, 6, 7, 10 } } };
        int numRow = 5;
        EvaluatedTable evTable2(entities, table, numRow);

        evTable1.innerJoinMerge(evTable2);
        Assert::AreEqual(size_t(1), evTable1.getEntities().size());
        Assert::AreEqual(std::string("s"), (evTable1.getEntities().begin())->first);
        Assert::AreEqual(5, evTable1.getNumRow());
    }

    TEST_METHOD(innerJoinMerge_noCommonColumns_crossProduct) {
        std::unordered_map<std::string, PqlEntityType> leftEntities = {
            {"s1", PqlEntityType::Stmt},
            {"v1", PqlEntityType::Variable}
        };
        std::unordered_map<std::string, std::vector<int>> leftTable = {
            {"s1", { 1, 3 } },
            {"v1", { 2, 4 } },
        };
        int leftNumRow = 2;
        EvaluatedTable leftEvTable(leftEntities, leftTable, leftNumRow);

        std::unordered_map<std::string, PqlEntityType> rightEntities = {
            {"s2", PqlEntityType::Stmt},
            {"v2", PqlEntityType::Variable}
        };
        std::unordered_map<std::string, std::vector<int>> rightTable = {
            {"s2", { 5, 7, 9} },
            {"v2", { 6, 8, 10} },
        };
        int rightNumRow = 3;
        EvaluatedTable rightEvTable(rightEntities, rightTable, rightNumRow);

        leftEvTable.innerJoinMerge(rightEvTable);
        Assert::AreEqual(size_t(4), leftEvTable.getEntities().size());
        std::unordered_map<std::string, PqlEntityType> entities = leftEvTable.getEntities();
        Assert::IsTrue(entities.find(std::string("s1")) != entities.end());
        Assert::IsTrue(entities.find(std::string("v1")) != entities.end());
        Assert::IsTrue(entities.find(std::string("s2")) != entities.end());
        Assert::IsTrue(entities.find(std::string("v2")) != entities.end());
        Assert::AreEqual(6, leftEvTable.getNumRow());
    }

    TEST_METHOD(innerJoinMerge_commonColumnExists_innerJoin) {
        std::unordered_map<std::string, PqlEntityType> leftEntities = {
            {"s", PqlEntityType::Stmt},
            {"v1", PqlEntityType::Variable}
        };
        std::unordered_map<std::string, std::vector<int>> leftTable = {
            {"s", { 1, 3, 4} },
            {"v1", { 2, 4, 4} },
        };
        int leftNumRow = 3;
        EvaluatedTable leftEvTable(leftEntities, leftTable, leftNumRow);

        std::unordered_map<std::string, PqlEntityType> rightEntities = {
            {"s", PqlEntityType::Stmt},
            {"v2", PqlEntityType::Variable}
        };
        std::unordered_map<std::string, std::vector<int>> rightTable = {
            {"s", { 1, 3, 5} },
            {"v2", { 6, 8, 10} },
        };
        int rightNumRow = 3;
        EvaluatedTable rightEvTable(rightEntities, rightTable, rightNumRow);

        leftEvTable.innerJoinMerge(rightEvTable);
        Assert::AreEqual(size_t(3), leftEvTable.getEntities().size());
        std::unordered_map<std::string, PqlEntityType> entities = leftEvTable.getEntities();
        Assert::IsTrue(entities.find(std::string("s")) != entities.end());
        Assert::IsTrue(entities.find(std::string("v1")) != entities.end());
        Assert::IsTrue(entities.find(std::string("v2")) != entities.end());
        Assert::AreEqual(2, leftEvTable.getNumRow());
    }
    };
}
