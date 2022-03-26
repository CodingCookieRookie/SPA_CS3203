#include "QPSCommons.h"

bool isSynonymRef(PqlReference reference) {
	return reference.first == PqlReferenceType::SYNONYM;
}

bool isStatementEntity(EntityType entityType) {
	/* TODO: refactor this into a table */
	return entityType == EntityType::ASSIGN ||
		entityType == EntityType::STMT ||
		entityType == EntityType::IF ||
		entityType == EntityType::PRINT ||
		entityType == EntityType::READ ||
		entityType == EntityType::STMT ||
		entityType == EntityType::WHILE ||
		entityType == EntityType::CALL;
}
