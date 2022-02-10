#pragma once
#include <vector>
#include <unordered_map>

#include "QPSCommons.h"
#include "../Exception/SPAException.h"

class Instruction {
private:
	InstructionType type;
	std::vector<std::string> arguments; // has to be generalised

public:

	/* Constructor for an Instruction object */
	Instruction(InstructionType type) : type(type) {}

	/* Constructor for an Instruction object */
	Instruction(InstructionType type, std::vector<std::string> arguments) : type(type), arguments(arguments) {}

	/* TODO: To generalise. Executes instruction by calling the PKB */
	void execute(Instruction& instr);

	/* Getter for type */
	InstructionType getType() {
		return type;
	};

	/* Getter for arguments */
	std::vector<std::string> getArgs() {
		return arguments;
	};

};
