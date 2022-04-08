#pragma once

#include "Instructions/GetAllInstructions/GetAllAssignInstruction.h"
#include "Instructions/GetAllInstructions/GetAllCallInstruction.h"
#include "Instructions/GetAllInstructions/GetAllConstInstruction.h"
#include "Instructions/GetAllInstructions/GetAllIfInstruction.h"
#include "Instructions/GetAllInstructions/GetAllPrintInstruction.h"
#include "Instructions/GetAllInstructions/GetAllProcInstruction.h"
#include "Instructions/GetAllInstructions/GetAllReadInstruction.h"
#include "Instructions/GetAllInstructions/GetAllStmtInstruction.h"
#include "Instructions/GetAllInstructions/GetAllVarInstruction.h"
#include "Instructions/GetAllInstructions/GetAllWhileInstruction.h"

class ParsedGetAll {
private:
	EntityType type;
	std::string synonym;
public:
	ParsedGetAll(EntityType type, std::string synonym);
	Instruction* toInstruction(PKBGetter* pkbGetter) const;
};
