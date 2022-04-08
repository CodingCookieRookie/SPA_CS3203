#pragma once

#include "GetAllInstruction.h"

class GetAllConstInstruction : public GetAllInstruction {
public:
	EvaluatedTable execute() override;
	GetAllConstInstruction(std::string synonym, PKBGetter* pkbGetter);
};
