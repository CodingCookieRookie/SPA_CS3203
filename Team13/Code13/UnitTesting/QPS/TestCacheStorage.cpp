#include "stdafx.h"
#include "CppUnitTest.h"

#include "../source/QPS/Processors/CacheStorage.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
	TEST_CLASS(TestCacheStorage) {
private:
	StmtIndex stmtIndex1 = { 1 };
	StmtIndex stmtIndex2 = { 2 };
	StmtIndex stmtIndex3 = { 3 };
	StmtIndex stmtIndex4 = { 4 };
	CacheStorage* cacheStorage;

	TEST_METHOD_INITIALIZE(init) {
		cacheStorage = new CacheStorage();
	}

public:
	TEST_METHOD(insertSuccessors_getFromLeftArg) {
		cacheStorage->insertSuccessors(stmtIndex1, std::unordered_set<StmtIndex>{stmtIndex2, stmtIndex4});
		cacheStorage->insertSuccessors(stmtIndex2, std::unordered_set<StmtIndex>{stmtIndex3, stmtIndex4});
		Assert::IsTrue(std::vector<StmtIndex>{stmtIndex2, stmtIndex4} == cacheStorage->getFromLeftArg(stmtIndex1));
		Assert::IsTrue(std::vector<StmtIndex>{stmtIndex3, stmtIndex4} == cacheStorage->getFromLeftArg(stmtIndex2));
	};

	TEST_METHOD(insertPredecesors_getFromRightArg) {
		cacheStorage->insertPredecessors(stmtIndex1, std::unordered_set<StmtIndex>{stmtIndex2, stmtIndex4});
		cacheStorage->insertPredecessors(stmtIndex2, std::unordered_set<StmtIndex>{stmtIndex3, stmtIndex4});
		Assert::IsTrue(std::vector<StmtIndex>{stmtIndex2, stmtIndex4} == cacheStorage->getFromRightArg(stmtIndex1));
		Assert::IsTrue(std::vector<StmtIndex>{stmtIndex3, stmtIndex4} == cacheStorage->getFromRightArg(stmtIndex2));
	};

	TEST_METHOD(insertSuccessors_isPredecessorFullyComputed) {
		cacheStorage->insertSuccessors(stmtIndex1, std::unordered_set<StmtIndex>{stmtIndex2, stmtIndex4});
		Assert::IsTrue(cacheStorage->isPredecessorFullyComputed(stmtIndex1));
		Assert::IsFalse(cacheStorage->isPredecessorFullyComputed(stmtIndex2));
	};

	TEST_METHOD(insertSuccessors_isSuccessorFullyComputed) {
		cacheStorage->insertPredecessors(stmtIndex1, std::unordered_set<StmtIndex>{stmtIndex2, stmtIndex4});
		Assert::IsTrue(cacheStorage->isSuccessorFullyComputed(stmtIndex1));
		Assert::IsFalse(cacheStorage->isSuccessorFullyComputed(stmtIndex2));
	};
	};
}
