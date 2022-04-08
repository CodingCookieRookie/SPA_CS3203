#include "./GetAllInstruction.h"

GetAllInstruction::GetAllInstruction(std::string synonym, PKBGetter* pkbGetter) :
	synonym(synonym), pkbGetter(pkbGetter) {};

std::unordered_set<std::string> GetAllInstruction::getSynonyms() {
	return { synonym };
}