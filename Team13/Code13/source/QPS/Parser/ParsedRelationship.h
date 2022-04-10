#pragma once

#include "../../PKB/PKBGetter.h"
#include "../Instructions/GetAllInstructions/GetAllAssignInstruction.h"
#include "../Instructions/GetAllInstructions/GetAllCallInstruction.h"
#include "../Instructions/GetAllInstructions/GetAllConstInstruction.h"
#include "../Instructions/GetAllInstructions/GetAllIfInstruction.h"
#include "../Instructions/GetAllInstructions/GetAllPrintInstruction.h"
#include "../Instructions/GetAllInstructions/GetAllReadInstruction.h"
#include "../Instructions/GetAllInstructions/GetAllStmtInstruction.h"
#include "../Instructions/GetAllInstructions/GetAllVarInstruction.h"
#include "../Instructions/GetAllInstructions/GetAllWhileInstruction.h"
#include "../Instructions/RelationshipInstruction/AffectsInstruction.h"
#include "../Instructions/RelationshipInstruction/AffectsTInstruction.h"
#include "../Instructions/RelationshipInstruction/CallsInstruction.h"
#include "../Instructions/RelationshipInstruction/CallsTInstruction.h"
#include "../Instructions/RelationshipInstruction/FollowsInstruction.h"
#include "../Instructions/RelationshipInstruction/FollowsTInstruction.h"
#include "../Instructions/RelationshipInstruction/ModifiesInstruction.h"
#include "../Instructions/RelationshipInstruction/NextInstruction.h"
#include "../Instructions/RelationshipInstruction/NextTInstruction.h"
#include "../Instructions/RelationshipInstruction/ParentInstruction.h"
#include "../Instructions/RelationshipInstruction/ParentTInstruction.h"
#include "ParsedGetAll.h"
#include "../Processors/Processors.h"
#include "../QPSCommons.h"
#include "../Instructions/RelationshipInstruction/UsesInstruction.h"

class ParsedRelationship {
private:
	static const std::unordered_set<PqlReferenceType> STMT_REF;
	static const std::unordered_set<PqlReferenceType> ENT_REF;
	static const std::unordered_set<PqlReferenceType> STMT_REF_NO_WILDCARD;
	static const std::unordered_set<PqlReferenceType> ENT_REF_NO_WILDCARD;

	static const std::unordered_set<EntityType> STMT_ENTITIES;
	static const std::unordered_set<EntityType> ASSIGN_ENTITIES;
	static const std::unordered_set<EntityType> VAR_ENTITIES;
	static const std::unordered_set<EntityType> PROC_ENTITIES;

	/* These two unordered_maps validate the type of argument, e.g. stmtRef := '_' | synonym | INTEGER
	The leftArg map also provides a mapping from the generic Modifies/Uses to the specific P or S variants */
	static const std::unordered_map<PqlRelationshipType,
		std::vector<std::pair<PqlRelationshipType, std::unordered_set<PqlReferenceType>>>> VALID_LEFT_ARGS;
	static const std::unordered_map<PqlRelationshipType, std::unordered_set<PqlReferenceType>> VALID_RIGHT_ARGS;

	/* If the argument is a synonym, these two unordered_maps validate the design entity that the synonym is declared as */
	static const std::unordered_map<PqlRelationshipType, std::unordered_set<EntityType>> VALID_LEFT_DESIGN_ENTITIES;
	static const std::unordered_map<PqlRelationshipType, std::unordered_set<EntityType>> VALID_RIGHT_DESIGN_ENTITIES;

	static const std::unordered_set<PqlReferenceType> SYNONYM_REFERENCE_TYPES;

	PqlRelationshipType relationshipType;
	PqlReference lhsRef;
	PqlReference rhsRef;

	void validate(const std::unordered_map<std::string, EntityType>& declarationMap);
	Instruction* getArgFilter(const PqlReference& pqlRef, const std::unordered_map<std::string, EntityType>& declarationMap, PKBGetter* pkbGetter);
public:
	ParsedRelationship(PqlRelationshipType relationshipType,
		PqlReference lhsRef, PqlReference rhsRef);
	PqlRelationshipType getRelationshipType() const;
	PqlReference getLhs() const;
	PqlReference getRhs() const;

	Instruction* getLeftFilter(const std::unordered_map<std::string, EntityType>& declarationMap, PKBGetter* pkbGetter);
	Instruction* getRightFilter(const std::unordered_map<std::string, EntityType>& declarationMap, PKBGetter* pkbGetter);
	Instruction* toInstruction(const std::unordered_map<std::string, EntityType>& declarationMap, PKBGetter* pkbGetter, Processors processors);
};
