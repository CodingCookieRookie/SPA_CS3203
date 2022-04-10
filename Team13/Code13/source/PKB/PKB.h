#pragma once

#include "../Common/Types.h"
#include "./Relationship/Calls.h"
#include "./Relationship/CallsT.h"
#include "./Relationship/Follows.h"
#include "./Relationship/FollowsT.h"
#include "./Relationship/ModifiesP.h"
#include "./Relationship/ModifiesS.h"
#include "./Relationship/Next.h"
#include "./Relationship/Parent.h"
#include "./Relationship/ParentT.h"
#include "./Relationship/Relationship.h"
#include "./Relationship/UsesP.h"
#include "./Relationship/UsesS.h"
#include "./Attribute/Attribute.h"
#include "./Entity/Constant.h"
#include "./Entity/NameIndexEntity.h"
#include "./Pattern/PatternAssign.h"
#include "./Pattern/PatternContainer.h"
#include "./Pattern/PatternContainerTypes.h"
#include "./Entity/Procedure.h"
#include "./Entity/ProcedureStatementMapper.h"
#include "./Entity/Statement.h"
#include "./Entity/Variable.h"

class PKB {
private:
	/* Main Entities */
	std::unordered_map<EntityType, NameIndexEntityBase*> nameIdxEntityToInstanceMap;
	Constant* constant;
	Statement* statement;

	/* Other Entity related information */
	ProcedureStatementMapper* procStmtMapper;

	/* Attributes */
	Attribute* attribute;

	/* Pattern */
	PatternAssign* patternAssign;
	std::unordered_map<StatementType, PatternContainer*> containerEntityToInstanceMap;

	/* Relationships */
	std::unordered_map<RelationshipType, Relationship<SynonymIndex, SynonymIndex>*> rsToInstanceMap;

public:
	PKB();
	~PKB();
	friend class PKBGetter;
	friend class PKBInserter;
};
