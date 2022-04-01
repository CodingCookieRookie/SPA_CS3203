#pragma once

#include "GetAllInstruction.h"

class GetAllAssignInstruction : public GetAllInstruction {
public:
	EvaluatedTable execute() override;
	GetAllAssignInstruction(std::string synonym);
};
