#pragma once

#include "GetAllInstruction.h"

class GetAllVarInstruction : public GetAllInstruction {
public:
	EvaluatedTable execute() override;
	GetAllVarInstruction(std::string synonym);
};
