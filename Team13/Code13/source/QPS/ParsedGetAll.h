#pragma once

#include "Clause.h"

class ParsedGetAll : public Clause {
private:
	EntityType type;
	std::string synonym;
public:
	ParsedGetAll(EntityType type, std::string synonym);
	Instruction* toInstruction(PKBGetter* pkbGetter) const;
};
