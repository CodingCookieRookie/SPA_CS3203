#pragma once
#include <vector>
#include <unordered_map>

#include "QPSCommons.h"
#include "../Exception/SPAException.h"

class Instruction {
private:
	INSTRUCTION_TYPE type;
	std::vector<std::string> arguments;

public:

	/* Constructor for an Instruction object */
	Instruction(INSTRUCTION_TYPE type) {
		this->type = type;
	}

	/* Constructor for an Instruction object */
	Instruction(INSTRUCTION_TYPE type, std::vector<std::string> arguments) {
		this->type = type;
		this->arguments = arguments;
	}

	/* Executes instruction by calling the PKB */
	void execute(Instruction& instr);

	/* Getter for type */
	INSTRUCTION_TYPE getType() {
		return this->type;
	};

	/* Getter for arguments */
	std::vector<std::string> getArgs() {
		return this->arguments;
	};

};
