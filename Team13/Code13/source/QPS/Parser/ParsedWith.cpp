#include "ParsedWith.h"

const std::unordered_set<PqlReferenceType> ParsedWith::ATTRIB_REFERENCES = {
	PqlReferenceType::PROC_NAME, PqlReferenceType::VAR_NAME,
	PqlReferenceType::VALUE, PqlReferenceType::STMT_NUM
};

const std::unordered_map<EntityType, std::unordered_set<PqlReferenceType>>
ParsedWith::VALID_ATTRIBS = {
	{ EntityType::PROCEDURE, { PqlReferenceType::PROC_NAME } },
	{ EntityType::CALL,		{ PqlReferenceType::PROC_NAME, PqlReferenceType::STMT_NUM } },
	{ EntityType::VARIABLE,	{ PqlReferenceType::VAR_NAME } },
	{ EntityType::READ,		{ PqlReferenceType::VAR_NAME, PqlReferenceType::STMT_NUM } },
	{ EntityType::PRINT,		{ PqlReferenceType::VAR_NAME, PqlReferenceType::STMT_NUM } },
	{ EntityType::CONSTANT,	{ PqlReferenceType::VALUE } },
	{ EntityType::STMT,		{ PqlReferenceType::STMT_NUM } },
	{ EntityType::WHILE,		{ PqlReferenceType::STMT_NUM } },
	{ EntityType::IF,		{ PqlReferenceType::STMT_NUM } },
	{ EntityType::ASSIGN,	{ PqlReferenceType::STMT_NUM } },
};

const std::unordered_map<PqlReferenceType, PqlAttributeType>
ParsedWith::REF_TO_ATTRIB_MAP = {
	{ PqlReferenceType::IDENT,		PqlAttributeType::STRING },
	{ PqlReferenceType::INTEGER,	PqlAttributeType::INTEGER },
	{ PqlReferenceType::PROC_NAME,	PqlAttributeType::STRING },
	{ PqlReferenceType::VAR_NAME,	PqlAttributeType::STRING },
	{ PqlReferenceType::VALUE,		PqlAttributeType::INTEGER },
	{ PqlReferenceType::STMT_NUM,	PqlAttributeType::INTEGER },
};

PqlAttributeType ParsedWith::getAttribType(const PqlReference& pqlRef, const std::unordered_map<std::string, EntityType>& declarationMap) {
	const auto& [refType, synonym] = pqlRef;
	if (ATTRIB_REFERENCES.find(refType) != ATTRIB_REFERENCES.end()) {
		if (declarationMap.find(synonym) == declarationMap.end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		EntityType entType = declarationMap.at(synonym);
		/* Validate that the design entity and attribute are compatible */
		if (VALID_ATTRIBS.find(entType) == VALID_ATTRIBS.end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		if (VALID_ATTRIBS.at(entType).find(refType) == VALID_ATTRIBS.at(entType).end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
	}
	return REF_TO_ATTRIB_MAP.at(refType);
}

ParsedWith::ParsedWith(PqlReference lhs, PqlReference rhs)
	: lhs(lhs), rhs(rhs) {}

PqlReference ParsedWith::getLhs() const {
	return lhs;
}

PqlReference ParsedWith::getRhs() const {
	return rhs;
}

Instruction* ParsedWith::toInstruction(const std::unordered_map<std::string, EntityType>& declarationMap, PKBGetter * pkbGetter) {
	PqlAttributeType leftAttribType = getAttribType(lhs, declarationMap);
	PqlAttributeType rightAttribType = getAttribType(rhs, declarationMap);

	EntityType lhsEntity = declarationMap.find(lhs.second) != declarationMap.end() ? declarationMap.at(lhs.second) : EntityType::CONSTANT;
	EntityType rhsEntity = declarationMap.find(rhs.second) != declarationMap.end() ? declarationMap.at(rhs.second) : EntityType::CONSTANT;

	if (leftAttribType != rightAttribType) {
		/* Incompatible types */
		throw QPSException(QPSException::VALIDATOR_ERROR);
	}

	Instruction* instruction = nullptr;
	switch (leftAttribType) {
	case PqlAttributeType::STRING:
		instruction = new WithStringInstruction(lhs, rhs, lhsEntity, rhsEntity, pkbGetter);
		break;
	case PqlAttributeType::INTEGER:
		instruction = new WithIntegerInstruction(lhs, rhs, lhsEntity, rhsEntity, pkbGetter);
		break;
	default:
		break;
	}
	return instruction;
}
