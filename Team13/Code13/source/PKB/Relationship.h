#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Common/Types.h"
#include "./BidirectionalTable/BidirectionalTableTwoWaySet.h"

template<typename LeftIndexType, typename RightIndexType>
class Relationship {
protected:
	BidirectionalTableTwoWaySet<LeftIndexType, RightIndexType> bidirectionalTable;
	std::unordered_map<LeftIndexType, std::unordered_set<RightIndexType>> leftRightSynonymTable;
	std::unordered_map<RightIndexType, std::unordered_set<LeftIndexType>> rightLeftSynonymTable;

public:
	Relationship();
	void insert(LeftIndexType leftIndex, RightIndexType rightIndex);
	bool contains(LeftIndexType leftIndex, RightIndexType rightIndex);
	std::vector<RightIndexType> getFromLeftArg(LeftIndexType leftIndex);
	std::vector<LeftIndexType> getFromRightArg(RightIndexType rightIndex);
	std::tuple<std::vector<LeftIndexType>, std::vector<RightIndexType>> getAllInfo();
};

template<typename LeftIndexType, typename RightIndexType>
Relationship<LeftIndexType, RightIndexType>::Relationship() {}

template<typename LeftIndexType, typename RightIndexType>
void Relationship<LeftIndexType, RightIndexType>::insert(LeftIndexType leftIndex, RightIndexType rightIndex) {
	bidirectionalTable.insert(leftIndex, rightIndex);
};

template<typename LeftIndexType, typename RightIndexType>
bool Relationship<LeftIndexType, RightIndexType>::contains(LeftIndexType leftIndex, RightIndexType rightIndex) {
	return bidirectionalTable.contains(leftIndex, rightIndex);
};

template<typename LeftIndexType, typename RightIndexType>
std::vector<RightIndexType> Relationship<LeftIndexType, RightIndexType>::getFromLeftArg(LeftIndexType leftIndex) {
	return bidirectionalTable.getFromLeftArg(leftIndex);
};

template<typename LeftIndexType, typename RightIndexType>
std::vector<LeftIndexType> Relationship<LeftIndexType, RightIndexType>::getFromRightArg(RightIndexType rightIndex) {
	return bidirectionalTable.getFromRightArg(rightIndex);
};

template<typename LeftIndexType, typename RightIndexType>
std::tuple<std::vector<LeftIndexType>, std::vector<RightIndexType>> Relationship<LeftIndexType, RightIndexType>::getAllInfo() {
	return bidirectionalTable.getAll();
};
