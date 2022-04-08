#include "ParsedGetAll.h"

ParsedGetAll::ParsedGetAll(EntityType type, std::string synonym) :
	type(type), synonym(synonym) {}

/* Factory Pattern */
Instruction* ParsedGetAll::toInstruction(PKBGetter* pkbGetter) const {
	Instruction* instruction = nullptr;
	switch (type) {
	case EntityType::STMT:
		instruction = new GetAllStmtInstruction(synonym, pkbGetter);
		break;
	case EntityType::READ:
		instruction = new GetAllReadInstruction(synonym, pkbGetter);
		break;
	case EntityType::PRINT:
		instruction = new GetAllPrintInstruction(synonym, pkbGetter);
		break;
	case EntityType::CALL:
		instruction = new GetAllCallInstruction(synonym, pkbGetter);
		break;
	case EntityType::WHILE:
		instruction = new GetAllWhileInstruction(synonym, pkbGetter);
		break;
	case EntityType::IF:
		instruction = new GetAllIfInstruction(synonym, pkbGetter);
		break;
	case EntityType::ASSIGN:
		instruction = new GetAllAssignInstruction(synonym, pkbGetter);
		break;
	case EntityType::VARIABLE:
		instruction = new GetAllVarInstruction(synonym, pkbGetter);
		break;
	case EntityType::CONSTANT:
		instruction = new GetAllConstInstruction(synonym, pkbGetter);
		break;
	case EntityType::PROCEDURE:
		instruction = new GetAllProcInstruction(synonym, pkbGetter);
		break;
	}
	return instruction;
}
