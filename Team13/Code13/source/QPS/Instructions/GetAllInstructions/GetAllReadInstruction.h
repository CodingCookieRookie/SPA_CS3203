#pragma once

#include "GetAllInstruction.h"

class GetAllReadInstruction : public GetAllInstruction {
public:
	EvaluatedTable execute() override;
	GetAllReadInstruction(std::string synonym);
};
