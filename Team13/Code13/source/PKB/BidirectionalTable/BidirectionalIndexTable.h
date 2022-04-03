#pragma once

#include <string>
#include <unordered_map>
#include <vector>

template<typename T>
class BidirectionalIndexTable {
private:
	size_t getTableSize();
	std::unordered_map<std::string, T> stringIndexTable;
	std::unordered_map<T, std::string> indexStringTable;

public:
	BidirectionalIndexTable();
	T insert(std::string& string);
	bool contains(std::string& string);
	std::string getStringFromIndex(T element);
	T getIndexFromString(std::string string);
	std::vector<T> getAll();
};

template<typename T>
BidirectionalIndexTable<T>::BidirectionalIndexTable() {};

template<typename T>
size_t BidirectionalIndexTable<T>::getTableSize() {
	return stringIndexTable.size();
}

template<typename T>
T BidirectionalIndexTable<T>::insert(std::string& string) {
	if (stringIndexTable.find(string) == stringIndexTable.end()) {
		T nextIndex = T(getTableSize() + 1);
		stringIndexTable[string] = nextIndex;
		indexStringTable[nextIndex] = string;
	}

	return stringIndexTable[string];
}

template<typename T>
bool BidirectionalIndexTable<T>::contains(std::string& string) {
	return stringIndexTable.find(string) != stringIndexTable.end();
}

template<typename T>
std::string BidirectionalIndexTable<T>::getStringFromIndex(T element) {
	return indexStringTable[element];
}

template<typename T>
T BidirectionalIndexTable<T>::getIndexFromString(std::string string) {
	return stringIndexTable[string];
}

template<typename T>
std::vector<T> BidirectionalIndexTable<T>::getAll() {
	std::vector<T> res;

	for (auto& entry : indexStringTable) {
		res.push_back(entry.first);
	}

	return res;
}
