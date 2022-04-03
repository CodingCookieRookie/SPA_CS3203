#pragma once

#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

template<typename T, typename U>
class BidirectionalTableTwoWaySet {
private:
	std::unordered_map<T, std::unordered_set<U>> TUTable;
	std::unordered_map<U, std::unordered_set<T>> UTTable;

public:
	BidirectionalTableTwoWaySet();
	void insert(T leftArg, U rightArg);
	bool contains(T leftArg, U rightArg);
	std::vector<U> getFromLeftArg(T leftArg);
	std::vector<T> getFromRightArg(U rightArg);
	std::tuple<std::vector<T>, std::vector<U>> getAll();
};

template<typename T, typename U>
BidirectionalTableTwoWaySet<T, U>::BidirectionalTableTwoWaySet() {};

template<typename T, typename U>
void BidirectionalTableTwoWaySet<T, U>::insert(T leftArg, U rightArg) {
	TUTable[leftArg].insert(rightArg);
	UTTable[rightArg].insert(leftArg);
}

template<typename T, typename U>
bool BidirectionalTableTwoWaySet<T, U>::contains(T leftArg, U rightArg) {
	std::unordered_set<T> TSet = UTTable[rightArg];
	return TSet.find(leftArg) != TSet.end();
}

template<typename T, typename U>
std::vector<U> BidirectionalTableTwoWaySet<T, U>::getFromLeftArg(T leftArg) {
	std::unordered_set<U> USet = TUTable[leftArg];
	return std::vector<U>(USet.begin(), USet.end());
}

template<typename T, typename U>
std::vector<T> BidirectionalTableTwoWaySet<T, U>::getFromRightArg(U rightArg) {
	std::unordered_set<T> TSet = UTTable[rightArg];
	return std::vector<T>(TSet.begin(), TSet.end());
}

template<typename T, typename U>
std::tuple<std::vector<T>, std::vector<U>> BidirectionalTableTwoWaySet<T, U>::getAll() {
	std::vector<T> TList;
	std::vector<U> UList;
	for (auto TUEntry : TUTable) {
		for (auto& U : TUEntry.second) {
			TList.push_back(TUEntry.first);
			UList.push_back(U);
		}
	}
	return std::make_tuple(TList, UList);
}
