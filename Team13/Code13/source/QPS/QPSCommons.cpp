#include "QPSCommons.h"

bool isSynonymRef(PqlReference reference) {
	return reference.first == PqlReferenceType::SYNONYM
		|| reference.first == PqlReferenceType::PROC_NAME
		|| reference.first == PqlReferenceType::STMT_NUM
		|| reference.first == PqlReferenceType::VAR_NAME
		|| reference.first == PqlReferenceType::VALUE;
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
