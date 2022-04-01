#pragma once

#include "../../Instruction.h"
#include "../../../Common/Types.h"

class GetAllInstruction : public Instruction {
protected:
	std::string synonym;

public:
	/* Default Constructor */
	GetAllInstruction(std::string synonym);
	virtual EvaluatedTable execute() = 0;
};
