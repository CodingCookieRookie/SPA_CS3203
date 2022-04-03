#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "BidirectionalTable.h"

template<class T, typename LeftIndexType, typename RightIndexType>
class RS {
protected:
	static BidirectionalTable<LeftIndexType, RightIndexType> bidirectionalTable;
	static std::unordered_map<LeftIndexType, std::unordered_set<RightIndexType>> leftRightSynonymTable;
	static std::unordered_map<RightIndexType, std::unordered_set<LeftIndexType>> rightLeftSynonymTable;

public:
	static void insert(LeftIndexType leftIndex, RightIndexType rightIndex);
	static bool contains(LeftIndexType leftIndex, RightIndexType rightIndex);
	static std::vector<RightIndexType> getFromLeftArg(LeftIndexType leftIndex);
	static std::vector<LeftIndexType> getFromRightArg(RightIndexType rightIndex);
	static std::tuple<std::vector<LeftIndexType>, std::vector<RightIndexType>> getAllInfo();
	static void performCleanUp();
};

template<class T, typename LeftIndexType, typename RightIndexType>
BidirectionalTable<LeftIndexType, RightIndexType> RS<T, LeftIndexType, RightIndexType>::bidirectionalTable
= BidirectionalTable<RightIndexType, LeftIndexType>();

template<class T, typename LeftIndexType, typename RightIndexType>
std::unordered_map<RightIndexType, std::unordered_set<LeftIndexType>> RS<T, LeftIndexType, RightIndexType>::rightLeftSynonymTable = {};

template<class T, typename LeftIndexType, typename RightIndexType>
std::unordered_map<LeftIndexType, std::unordered_set<RightIndexType>> RS<T, LeftIndexType, RightIndexType>::leftRightSynonymTable = {};

template<class T, typename LeftIndexType, typename RightIndexType>
void RS<T, LeftIndexType, RightIndexType>::insert(LeftIndexType leftIndex, RightIndexType rightIndex) {
	/* TO DO: Remove once transitive population is refactored, and does not need to access internal tables */
	rightLeftSynonymTable[rightIndex].insert(leftIndex);
	leftRightSynonymTable[leftIndex].insert(rightIndex);

	bidirectionalTable.insert(rightIndex, leftIndex);
};

template<class T, typename LeftIndexType, typename RightIndexType>
bool RS<T, LeftIndexType, RightIndexType>::contains(LeftIndexType leftIndex, RightIndexType rightIndex) {
	/* TO DO: Remove once transitive population is refactored, and does not need to access internal tables,
	then uncomment updated code*/
	std::unordered_set<RightIndexType> variables = leftRightSynonymTable[leftIndex];
	return variables.find(rightIndex) != variables.end();

	//return bidirectionalTable.contains(rightIndex, leftIndex);
};

template<class T, typename LeftIndexType, typename RightIndexType>
std::vector<RightIndexType> RS<T, LeftIndexType, RightIndexType>::getFromLeftArg(LeftIndexType leftIndex) {
	return bidirectionalTable.getFromRightArg(leftIndex);
};

template<class T, typename LeftIndexType, typename RightIndexType>
std::vector<LeftIndexType> RS<T, LeftIndexType, RightIndexType>::getFromRightArg(RightIndexType rightIndex) {
	return bidirectionalTable.getFromLeftArg(rightIndex);
};

template<class T, typename LeftIndexType, typename RightIndexType>
std::tuple<std::vector<LeftIndexType>, std::vector<RightIndexType>> RS<T, LeftIndexType, RightIndexType>::getAllInfo() {
	/* TO DO: Remove once transitive population is refactored, and does not need to access internal tables,
	then uncomment updated code*/
	std::vector<LeftIndexType> synonyms;
	std::vector<RightIndexType> variables;
	for (auto synonymVarEntry : leftRightSynonymTable) {
		for (auto& rightIndex : synonymVarEntry.second) {
			synonyms.push_back(synonymVarEntry.first);
			variables.push_back(rightIndex);
		}
	}
	return std::make_tuple(synonyms, variables);

	//return bidirectionalTable.getAll();
};

/* TO DO: Remove all clean up code once tables are non static */
template<class T, typename LeftIndexType, typename RightIndexType>
void RS<T, LeftIndexType, RightIndexType>::performCleanUp() {
	rightLeftSynonymTable = {};
	leftRightSynonymTable = {};
	bidirectionalTable = BidirectionalTable<LeftIndexType, RightIndexType>();
}
