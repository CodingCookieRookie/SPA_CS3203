#include "./PKB.h"

PKB::PKB() {
	/* Main Entities*/
	nameIdxEntityToInstanceMap[EntityType::PROCEDURE] = new Procedure();
	nameIdxEntityToInstanceMap[EntityType::VARIABLE] = new Variable();
	constant = new Constant();
	statement = new Statement();

	/* Other Entity related information */
	procStmtMapper = new ProcedureStatementMapper();

	/* Attributes */
	attribute = new Attribute();

	/* Pattern */
	patternAssign = new PatternAssign();
	containerEntityToInstanceMap[StatementType::IF_TYPE] = new PatternIf();
	containerEntityToInstanceMap[StatementType::WHILE_TYPE] = new PatternWhile();

	/* Relationships */
	rsToInstanceMap[RelationshipType::CALLS] = new Calls();
	rsToInstanceMap[RelationshipType::CALLS_T] = new CallsT();
	rsToInstanceMap[RelationshipType::FOLLOWS] = new Follows();
	rsToInstanceMap[RelationshipType::FOLLOWS_T] = new FollowsT();
	rsToInstanceMap[RelationshipType::MODIFIES_P] = new ModifiesP();
	rsToInstanceMap[RelationshipType::MODIFIES_S] = new ModifiesS();
	rsToInstanceMap[RelationshipType::NEXT] = new Next();
	rsToInstanceMap[RelationshipType::PARENT] = new Parent();
	rsToInstanceMap[RelationshipType::PARENT_T] = new ParentT();
	rsToInstanceMap[RelationshipType::USES_P] = new UsesP();
	rsToInstanceMap[RelationshipType::USES_S] = new UsesS();
}

PKB::~PKB() {}
