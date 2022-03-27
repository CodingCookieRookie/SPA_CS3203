#pragma once

#include "Instruction.h"
#include "QPSCommons.h"
#include "WithInstruction.h"
#include "ModifiesInstruction.h"
#include "UsesInstruction.h"

class Clause {
public:
	virtual Instruction* toInstruction() {
		return nullptr;
	}
};
