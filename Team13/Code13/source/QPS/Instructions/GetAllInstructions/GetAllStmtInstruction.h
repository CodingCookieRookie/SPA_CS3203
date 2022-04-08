#pragma once

#include "GetAllInstruction.h"

class GetAllStmtInstruction : public GetAllInstruction {
public:
	EvaluatedTable execute() override;
	GetAllStmtInstruction(std::string synonym, PKBGetter* pkbGetter);
};
