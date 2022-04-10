#pragma once

#include "../Instructions/WithInstruction/WithInstruction.h"
#include "../../PKB/PKBGetter.h"

class ParsedWith {
private:
	/* Tables for type-validating the with-clause */
	static const std::unordered_set<PqlReferenceType> ATTRIB_REFERENCES;
	static const std::unordered_map<EntityType, std::unordered_set<PqlReferenceType>> VALID_ATTRIBS;
	static const std::unordered_map<PqlReferenceType, PqlAttributeType> REF_TO_ATTRIB_MAP;

	PqlReference lhs;
	PqlReference rhs;

	PqlAttributeType getAttribType(const PqlReference& pqlRef, const std::unordered_map<std::string, EntityType>& declarationMap);
public:
	ParsedWith(PqlReference lhs, PqlReference rhs);
	PqlReference getLhs() const;
	PqlReference getRhs() const;
	Instruction* toInstruction(const std::unordered_map<std::string, EntityType>& declarationMap, PKBGetter* pkbGetter);
};
