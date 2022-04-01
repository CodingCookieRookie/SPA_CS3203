#pragma once

#include "GetAllInstruction.h"

class GetAllProcInstruction : public GetAllInstruction {
public:
	EvaluatedTable execute() override;
	GetAllProcInstruction(std::string synonym);
};
