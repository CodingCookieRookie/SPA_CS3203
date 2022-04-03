#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

template<typename T, typename U>
class BidirectionalTableOneWaySet {
private:
	size_t size = 0;
	std::unordered_map<T, std::unordered_set<U>> TUTable;
	std::unordered_map<U, T> UTTable;

public:
	size_t getSize();
	BidirectionalTableOneWaySet();
	void insert(T leftArg, U rightArg);
	bool contains(U rightArg);
	std::vector<U> getFromLeftArg(T leftArg);
	T getFromRightArg(U rightArg);
	std::tuple<std::vector<T>, std::vector<U>> getAll();
};

template<typename T, typename U>
BidirectionalTableOneWaySet<T, U>::BidirectionalTableOneWaySet() {};

template<typename T, typename U>
size_t BidirectionalTableOneWaySet<T, U>::getSize() {
	return size;
}

template<typename T, typename U>
void BidirectionalTableOneWaySet<T, U>::insert(T leftArg, U rightArg) {
	size += 1;
	TUTable[leftArg].insert(rightArg);
	UTTable[rightArg] = leftArg;
}

template<typename T, typename U>
bool BidirectionalTableOneWaySet<T, U>::contains(U rightArg) {
	return UTTable.find(rightArg) != UTTable.end();
}

template<typename T, typename U>
std::vector<U> BidirectionalTableOneWaySet<T, U>::getFromLeftArg(T leftArg) {
	std::unordered_set<U> USet = TUTable[leftArg];
	return std::vector<U>(USet.begin(), USet.end());
}

template<typename T, typename U>
T BidirectionalTableOneWaySet<T, U>::getFromRightArg(U rightArg) {
	return UTTable[rightArg];
}

template<typename T, typename U>
std::tuple<std::vector<T>, std::vector<U>> BidirectionalTableOneWaySet<T, U>::getAll() {
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
