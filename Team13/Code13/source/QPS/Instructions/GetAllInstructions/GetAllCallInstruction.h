#pragma once

#include "GetAllInstruction.h"

class GetAllCallInstruction : public GetAllInstruction {
public:
	EvaluatedTable execute() override;
	GetAllCallInstruction(std::string synonym, PKBGetter* pkbGetter);
};
