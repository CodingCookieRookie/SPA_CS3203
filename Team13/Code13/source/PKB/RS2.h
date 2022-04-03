#pragma once

#include "RS.h"

template<class T, typename SynonymIndex>
class RS2 : public RS<RS2<T, SynonymIndex>, SynonymIndex, SynonymIndex> {
public:
	static std::unordered_map<SynonymIndex, std::unordered_set<SynonymIndex>> getPredSucTable();
};

template<class T, typename SynonymIndex>
std::unordered_map<SynonymIndex, std::unordered_set<SynonymIndex>> RS2<T, SynonymIndex>::getPredSucTable() {
	std::unordered_map<SynonymIndex, std::unordered_set<SynonymIndex>> data;
	for (auto predSucEntry : leftRightSynonymTable) {
		for (auto successor : predSucEntry.second) {
			data[predSucEntry.first].insert(successor);
		}
	}
	return data;
};
