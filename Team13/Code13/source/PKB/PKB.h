#pragma once

#include "../Common/Types.h"
#include "../PKB/Calls.h"
#include "../PKB/CallsT.h"
#include "../PKB/Follows.h"
#include "../PKB/FollowsT.h"
#include "../PKB/ModifiesP.h"
#include "../PKB/ModifiesS.h"
#include "../PKB/Next.h"
#include "../PKB/Parent.h"
#include "../PKB/ParentT.h"
#include "../PKB/Relationship.h"
#include "../PKB/UsesP.h"
#include "../PKB/UsesS.h"
#include "./Attribute.h"
#include "./Constant.h"
#include "./NameIndexEntity.h"
#include "./PatternAssign.h"
#include "./PatternContainer.h"
#include "./PatternContainerTypes.h"
#include "./Procedure.h"
#include "./ProcedureStatementMapper.h"
#include "./Statement.h"
#include "./Variable.h"

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
