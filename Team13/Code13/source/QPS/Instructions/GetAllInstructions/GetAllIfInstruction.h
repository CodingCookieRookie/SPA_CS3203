#pragma once

#include "GetAllInstruction.h"

class GetAllIfInstruction : public GetAllInstruction {
public:
	EvaluatedTable execute() override;
	GetAllIfInstruction(std::string synonym, PKBGetter* pkbGetter);
};
