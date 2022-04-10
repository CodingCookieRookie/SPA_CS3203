#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include "../../Common/ExprNode.h"
#include "../../Common/Types.h"
#include "../RsEntitiesMapsTypes.h"

/* Forward definition for compatibility */
class StmtLstNode;
class ExprNode;
class StmtNode;

/* A vector of the ast's stmtNodes.
Needed to populate PKB stmt table in order.
Declared here to avoid circular dependency with RsEntitiesMapsTypes.h. */
typedef std::vector<StmtNode*> StmtNodes;

struct RelationshipMaps {
	/* Maps prevIndex to the stmtIndex that follows the former */
	RelationshipMap followsMap;

	/* Maps stmtIndex to the variable that it modifies */
	RelationshipMap modifiesMap;

	/* Maps stmtIndex to the variables that it uses */
	RelationshipMap usesMap;

	/* Maps parent's stmtIndex to its children's stmtIndices */
	RelationshipMap parentChildMap;

	/* Maps call stmtIndex to the procIndex that it calls */
	RelationshipMap callStmtToProcIndexCalledMap;

	/* Maps procIndex of a call stmt to the procIndex that it calls */
	RelationshipMap procIndexCallerToProcIndexCalledMap;

	/* Intermediate map that maps call stmtIndex to the procedure name that it calls */
	std::unordered_map<StmtIndex, std::string> callStmtToProcNameCalledMap;
};

struct EntityMaps {
	StmtNodes stmtNodes;
	StmtTypeMap stmtTypeMap;
	PatternMap patternMap;
	ConstSet constSet;
	ProcNameToIndexMap procNameToIndexMap;
	SortedProcIndexToNameMap sortedProcIndexToNameMap;
	ProcStmtMap procStmtMap;
	StmtProcMap stmtProcMap;
	VarNameToIndexMap varNameToIndexMap;
	SortedVarIndexToNameMap sortedVarIndexToNameMap;
};

class SourceASTNode {
public:
	SourceASTNode();
	~SourceASTNode();

	virtual void process();
	virtual void process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps);
};
