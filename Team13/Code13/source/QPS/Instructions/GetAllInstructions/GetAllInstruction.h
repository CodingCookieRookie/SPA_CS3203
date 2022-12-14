#pragma once

#include "../Instruction.h"
#include "../../../Common/Types.h"

class GetAllInstruction : public Instruction {
protected:
	std::string synonym;
	PKBGetter* pkbGetter;

public:
	/* Default Constructor */
	GetAllInstruction(std::string synonym, PKBGetter* pkbGetter);
	virtual std::unordered_set<std::string> getSynonyms();
};
