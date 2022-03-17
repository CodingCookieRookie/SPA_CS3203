#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Common/Types.h"

template<class T, typename SynonymIndex>
class RS1 {
protected:
	RS1();
	static std::unordered_map<VarIndex, std::unordered_set<SynonymIndex>> varSynonymTable;
	static std::unordered_map<SynonymIndex, std::unordered_set<VarIndex>> synonymVarTable;

public:
	static void insert(SynonymIndex index, VarIndex varIndex);
	static bool contains(SynonymIndex index, VarIndex varIndex);
	static std::vector<SynonymIndex> getFromVariable(VarIndex varIndex);
	static std::vector<VarIndex> getVariables(SynonymIndex index);
	static std::tuple<std::vector<SynonymIndex>, std::vector<VarIndex>> getAllSynonymVarInfo();
	static void populateFromSubSynonyms(SynonymIndex& synonym, std::unordered_set<SynonymIndex>& subSynonymIndices);
	static void performCleanUp();
};

template<class T, typename SynonymIndex> std::unordered_map<VarIndex, std::unordered_set<SynonymIndex>> RS1<T, SynonymIndex>::varSynonymTable = {};
template<class T, typename SynonymIndex> std::unordered_map<SynonymIndex, std::unordered_set<VarIndex>> RS1<T, SynonymIndex>::synonymVarTable = {};

template<class T, typename SynonymIndex>
RS1<T, SynonymIndex>::RS1() {};

template<class T, typename SynonymIndex>
void RS1<T, SynonymIndex>::insert(SynonymIndex index, VarIndex varIndex) {
	varSynonymTable[varIndex].insert(index);
	synonymVarTable[index].insert(varIndex);
};

template<class T, typename SynonymIndex>
bool RS1<T, SynonymIndex>::contains(SynonymIndex index, VarIndex varIndex) {
	if (synonymVarTable.find(index) == synonymVarTable.end()) {
		return false;
	}

	std::unordered_set<VarIndex> variables = synonymVarTable[index];
	if (variables.find(varIndex) == variables.end()) {
		return false;
	}

	return true;
};

template<class T, typename SynonymIndex>
std::vector<SynonymIndex> RS1<T, SynonymIndex>::getFromVariable(VarIndex varIndex) {
	std::vector<int> synonyms;
	for (auto synonym : varSynonymTable[varIndex]) {
		synonyms.push_back(synonym);
	}
	return synonyms;
};

template<class T, typename SynonymIndex>
std::vector<VarIndex> RS1<T, SynonymIndex>::getVariables(SynonymIndex index) {
	std::vector<int> variables;
	for (auto variable : synonymVarTable[index]) {
		variables.push_back(variable);
	}
	return variables;
};

template<class T, typename SynonymIndex>
std::tuple<std::vector<SynonymIndex>, std::vector<VarIndex>> RS1<T, SynonymIndex>::getAllSynonymVarInfo() {
	std::vector<int> synonyms;
	std::vector<int> variables;
	for (auto synonymVarEntry : synonymVarTable) {
		for (auto& varIndex : synonymVarEntry.second) {
			synonyms.push_back(synonymVarEntry.first);
			variables.push_back(varIndex);
		}
	}
	return std::make_tuple(synonyms, variables);
};

template<class T, typename SynonymIndex>
void RS1<T, SynonymIndex>::populateFromSubSynonyms(SynonymIndex& synonym, std::unordered_set<SynonymIndex>& subSynonymIndices) {
	for (auto subSynonymIndex : subSynonymIndices) {
		for (auto var : synonymVarTable[subSynonymIndex]) {
			insert(synonym, var);
		}
	}
}

template<class T, typename SynonymIndex>
void RS1<T, SynonymIndex>::performCleanUp() {
	varSynonymTable = {};
	synonymVarTable = {};
}
