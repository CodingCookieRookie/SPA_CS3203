#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "PKB.h"

void PKB::populateRS1ContainerInfo() {
	auto containerStmts = Entity::getStmtIdxFromType(StatementType::ifType);
	for (auto& stmtIdx : Entity::getStmtIdxFromType(StatementType::whileType)) {
		containerStmts.push_back(stmtIdx);
	}

	for (auto& containerStmt : containerStmts) {
		auto subStmts = Container::getStmtsInContainer(containerStmt);
		Modifies::populateForContainers(containerStmt, subStmts);
		Uses::populateForContainers(containerStmt, subStmts);
	}
}

void PKB::populateFollowsT() {
	auto followsPredSucTable = Follows::getPredSucTable();
	FollowsT::populate(followsPredSucTable);
}

void PKB::populateParentT() {
	auto parentPredSucTable = Parent::getPredSucTable();
	ParentT::populate(parentPredSucTable);
}

void PKB::populateRecursiveInfo() {
	Container::populate();
	populateFollowsT();
	populateParentT();
	populateRS1ContainerInfo();
}
