#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Common/Types.h"
#include "BidirectionalTable.h"

template<class T, typename SynonymIndex>
class RS1 {
protected:
	RS1();
	static BidirectionalTable<VarIndex, SynonymIndex> bidirectionalTable;
	static std::unordered_map<VarIndex, std::unordered_set<SynonymIndex>> varSynonymTable;
	static std::unordered_map<SynonymIndex, std::unordered_set<VarIndex>> synonymVarTable;

public:
	static void insert(SynonymIndex synonymIndex, VarIndex varIndex);
	static bool contains(SynonymIndex synonymIndex, VarIndex varIndex);
	static std::vector<SynonymIndex> getFromVariable(VarIndex varIndex);
	static std::vector<VarIndex> getVariables(SynonymIndex synonymIndex);
	static std::tuple<std::vector<SynonymIndex>, std::vector<VarIndex>> getAllSynonymVarInfo();
	static void populateFromSubSynonyms(SynonymIndex& synonym, std::unordered_set<SynonymIndex>& subSynonymIndices);
	static void performCleanUp();
};

template<class T, typename SynonymIndex>
BidirectionalTable<VarIndex, SynonymIndex> RS1<T, SynonymIndex>::bidirectionalTable = BidirectionalTable<VarIndex, SynonymIndex>();

template<class T, typename SynonymIndex>
std::unordered_map<VarIndex, std::unordered_set<SynonymIndex>> RS1<T, SynonymIndex>::varSynonymTable = {};

template<class T, typename SynonymIndex>
std::unordered_map<SynonymIndex, std::unordered_set<VarIndex>> RS1<T, SynonymIndex>::synonymVarTable = {};

template<class T, typename SynonymIndex>
RS1<T, SynonymIndex>::RS1() {};

template<class T, typename SynonymIndex>
void RS1<T, SynonymIndex>::insert(SynonymIndex synonymIndex, VarIndex varIndex) {
	/* TO DO: Remove once transitive population is refactored, and does not need to access internal tables */
	varSynonymTable[varIndex].insert(synonymIndex);
	synonymVarTable[synonymIndex].insert(varIndex);

	bidirectionalTable.insert(varIndex, synonymIndex);
};

template<class T, typename SynonymIndex>
bool RS1<T, SynonymIndex>::contains(SynonymIndex synonymIndex, VarIndex varIndex) {
	/* TO DO: Remove once transitive population is refactored, and does not need to access internal tables,
	then uncomment updated code*/
	std::unordered_set<VarIndex> variables = synonymVarTable[synonymIndex];
	return variables.find(varIndex) != variables.end();

	//return bidirectionalTable.contains(varIndex, synonymIndex);
};

template<class T, typename SynonymIndex>
std::vector<SynonymIndex> RS1<T, SynonymIndex>::getFromVariable(VarIndex varIndex) {
	return bidirectionalTable.getFromLeftArg(varIndex);
};

template<class T, typename SynonymIndex>
std::vector<VarIndex> RS1<T, SynonymIndex>::getVariables(SynonymIndex synonymIndex) {
	return bidirectionalTable.getFromRightArg(synonymIndex);
};

template<class T, typename SynonymIndex>
std::tuple<std::vector<SynonymIndex>, std::vector<VarIndex>> RS1<T, SynonymIndex>::getAllSynonymVarInfo() {
	/* TO DO: Remove once transitive population is refactored, and does not need to access internal tables,
	then uncomment updated code*/
	std::vector<SynonymIndex> synonyms;
	std::vector<VarIndex> variables;
	for (auto synonymVarEntry : synonymVarTable) {
		for (auto& varIndex : synonymVarEntry.second) {
			synonyms.push_back(synonymVarEntry.first);
			variables.push_back(varIndex);
		}
	}
	return std::make_tuple(synonyms, variables);

	//return bidirectionalTable.getAll();
};

/* TO DO: Remove this method once transitive population is refactored, then old internal tables are not required anymore*/
template<class T, typename SynonymIndex>
void RS1<T, SynonymIndex>::populateFromSubSynonyms(SynonymIndex& synonym, std::unordered_set<SynonymIndex>& subSynonymIndices) {
	for (auto subSynonymIndex : subSynonymIndices) {
		for (auto var : synonymVarTable[subSynonymIndex]) {
			insert(synonym, var);
		}
	}
}

/* TO DO: Remove all clean up code once tables are non static */
template<class T, typename SynonymIndex>
void RS1<T, SynonymIndex>::performCleanUp() {
	varSynonymTable = {};
	synonymVarTable = {};
	bidirectionalTable = BidirectionalTable<VarIndex, SynonymIndex>();
}
