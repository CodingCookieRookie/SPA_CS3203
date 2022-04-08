#include "QPSCommons.h"

const std::unordered_set<PqlReferenceType> QPSCommons::SYNONYM_REFERENCE_TYPES = {
	PqlReferenceType::SYNONYM, PqlReferenceType::PROC_NAME, PqlReferenceType::STMT_NUM,
	PqlReferenceType::VAR_NAME, PqlReferenceType::VALUE
};

bool QPSCommons::isSynonymRef(const PqlReference& reference) {
	return SYNONYM_REFERENCE_TYPES.find(reference.first) != SYNONYM_REFERENCE_TYPES.end();
}

bool isStatementEntity(EntityType entityType) {
	/* TODO: refactor this into a table */
	return entityType == EntityType::ASSIGN
		|| entityType == EntityType::STMT
		|| entityType == EntityType::IF
		|| entityType == EntityType::PRINT
		|| entityType == EntityType::READ
		|| entityType == EntityType::STMT
		|| entityType == EntityType::WHILE
		|| entityType == EntityType::CALL;
}
