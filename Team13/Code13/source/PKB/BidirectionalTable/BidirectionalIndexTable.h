#pragma once

#include <string>
#include <unordered_map>
#include <vector>

template<typename Index>
class BidirectionalIndexTable {
private:
	size_t getTableSize();
	std::unordered_map<std::string, Index> stringIndexTable;
	std::unordered_map<Index, std::string> indexStringTable;

public:
	BidirectionalIndexTable();
	Index insert(std::string& string);
	bool contains(std::string& string);
	std::string getStringFromIndex(Index element);
	Index getIndexFromString(std::string string);
	std::vector<Index> getAll();
};

template<typename Index>
BidirectionalIndexTable<Index>::BidirectionalIndexTable() {};

template<typename Index>
size_t BidirectionalIndexTable<Index>::getTableSize() {
	return stringIndexTable.size();
}

template<typename Index>
Index BidirectionalIndexTable<Index>::insert(std::string& string) {
	if (stringIndexTable.find(string) == stringIndexTable.end()) {
		Index nextIndex = Index(getTableSize() + 1);
		stringIndexTable[string] = nextIndex;
		indexStringTable[nextIndex] = string;
	}

	return stringIndexTable[string];
}

template<typename Index>
bool BidirectionalIndexTable<Index>::contains(std::string& string) {
	return stringIndexTable.find(string) != stringIndexTable.end();
}

template<typename Index>
std::string BidirectionalIndexTable<Index>::getStringFromIndex(Index element) {
	return indexStringTable[element];
}

template<typename Index>
Index BidirectionalIndexTable<Index>::getIndexFromString(std::string string) {
	return stringIndexTable[string];
}

template<typename Index>
std::vector<Index> BidirectionalIndexTable<Index>::getAll() {
	std::vector<Index> res;

	for (auto& entry : indexStringTable) {
		res.push_back(entry.first);
	}

	return res;
}
