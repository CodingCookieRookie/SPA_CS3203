#include "QPSCommons.h"

bool isSynonymRef(PqlReference reference) {
	return reference.first == PqlReferenceType::Synonym;
}

bool isStatementEntity(PqlEntityType entityType) {
	/* TODO: refactor this into a table */
	return entityType == PqlEntityType::Assign ||
		entityType == PqlEntityType::Stmt ||
		entityType == PqlEntityType::If ||
		entityType == PqlEntityType::Print ||
		entityType == PqlEntityType::Read ||
		entityType == PqlEntityType::Stmt ||
		entityType == PqlEntityType::While ||
		entityType == PqlEntityType::Call;
}
