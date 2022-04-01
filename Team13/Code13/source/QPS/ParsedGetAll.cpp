#include "ParsedGetAll.h"

ParsedGetAll::ParsedGetAll(EntityType type, std::string synonym) :
	type(type), synonym(synonym) {}

/* Factory Pattern */
Instruction* ParsedGetAll::toInstruction() const {
	Instruction* instruction = nullptr;
	switch (type) {
	case EntityType::STMT:
		instruction = new GetAllStmtInstruction(synonym);
		break;
	case EntityType::READ:
		instruction = new GetAllReadInstruction(synonym);
		break;
	case EntityType::PRINT:
		instruction = new GetAllPrintInstruction(synonym);
		break;
	case EntityType::CALL:
		instruction = new GetAllCallInstruction(synonym);
		break;
	case EntityType::WHILE:
		instruction = new GetAllWhileInstruction(synonym);
		break;
	case EntityType::IF:
		instruction = new GetAllIfInstruction(synonym);
		break;
	case EntityType::ASSIGN:
		instruction = new GetAllAssignInstruction(synonym);
		break;
	case EntityType::VARIABLE:
		instruction = new GetAllVarInstruction(synonym);
		break;
	case EntityType::CONSTANT:
		instruction = new GetAllConstInstruction(synonym);
		break;
	case EntityType::PROCEDURE:
		instruction = new GetAllProcInstruction(synonym);
		break;
	}
	return instruction;
}
