#pragma once

#include "GetAllInstruction.h"

class GetAllPrintInstruction : public GetAllInstruction {
public:
	EvaluatedTable execute() override;
	GetAllPrintInstruction(std::string synonym);
};
