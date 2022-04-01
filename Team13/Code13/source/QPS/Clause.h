#pragma once

#include "Instruction.h"
#include "QPSCommons.h"
#include "WithInstruction.h"
#include "ModifiesInstruction.h"
#include "UsesInstruction.h"
#include "ParentInstruction.h"
#include "ParentStarInstruction.h"
#include "FollowsInstruction.h"
#include "FollowsStarInstruction.h"
#include "CallsInstruction.h"
#include "CallsStarInstruction.h"
#include "NextInstruction.h"
#include "PatternInstruction.h"
#include "Instructions\GetAllInstructions\GetAllAssignInstruction.h"
#include "Instructions\GetAllInstructions\GetAllCallInstruction.h"
#include "Instructions\GetAllInstructions\GetAllConstInstruction.h"
#include "Instructions\GetAllInstructions\GetAllIfInstruction.h"
#include "Instructions\GetAllInstructions\GetAllPrintInstruction.h"
#include "Instructions\GetAllInstructions\GetAllProcInstruction.h"
#include "Instructions\GetAllInstructions\GetAllReadInstruction.h"
#include "Instructions\GetAllInstructions\GetAllStmtInstruction.h"
#include "Instructions\GetAllInstructions\GetAllVarInstruction.h"
#include "Instructions\GetAllInstructions\GetAllWhileInstruction.h"

class Clause {
public:
	virtual Instruction* toInstruction() {
		return nullptr;
	}
};
