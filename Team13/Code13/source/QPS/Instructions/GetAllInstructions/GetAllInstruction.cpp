#include "./GetAllInstruction.h"

GetAllInstruction::GetAllInstruction(std::string synonym, PKBGetter* pkbGetter) :
	synonym(synonym), pkbGetter(pkbGetter) {};

EvaluatedTable GetAllInstruction::execute() {
	return EvaluatedTable();
}
