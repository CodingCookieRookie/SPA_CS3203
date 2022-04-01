#include "./GetAllInstruction.h"

GetAllInstruction::GetAllInstruction(std::string synonym) : synonym(synonym) {};

EvaluatedTable GetAllInstruction::execute() {
	return EvaluatedTable();
}
