#pragma once

#include "RS.h"

template<class T, typename SynonymIndex>
class RS1 : public RS<RS1<T, SynonymIndex>, SynonymIndex, VarIndex> {
public:
	RS1();
	static void populateFromSubSynonyms(SynonymIndex& synonym, std::unordered_set<SynonymIndex>& subSynonymIndices);
};

template<class T, typename SynonymIndex>
RS1<T, SynonymIndex>::RS1() {};

/* TO DO: Remove this method once transitive population is refactored, then old internal tables are not required anymore*/
template<class T, typename SynonymIndex>
void RS1<T, SynonymIndex>::populateFromSubSynonyms(SynonymIndex& synonym, std::unordered_set<SynonymIndex>& subSynonymIndices) {
	for (auto subSynonymIndex : subSynonymIndices) {
		for (auto var : leftRightSynonymTable[subSynonymIndex]) {
			insert(synonym, var);
		}
	}
}
