#pragma once

#include "GetAllInstruction.h"

class GetAllWhileInstruction : public GetAllInstruction {
public:
	EvaluatedTable execute() override;
	GetAllWhileInstruction(std::string synonym, PKBGetter* pkbGetter);
};
