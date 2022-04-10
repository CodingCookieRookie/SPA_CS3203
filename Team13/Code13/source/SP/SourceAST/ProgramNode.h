#pragma once

#include "ProcedureNode.h"
#include "SourceASTNode.h"

class ProgramNode : public SourceASTNode {
private:
	std::vector<ProcedureNode*> procedureNodes;
	RelationshipMaps relationshipMaps;
	EntityMaps entityMaps;
public:
	ProgramNode();
	void addProcedure(ProcedureNode* procedureNode);
	void process() override;

	std::vector<ProcedureNode*> getProcedureNodes();
	RelationshipMaps getRelationshipMaps();
	EntityMaps getEntityMaps();
};
