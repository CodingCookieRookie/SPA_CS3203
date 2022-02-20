#include <string>
#include <vector>

#include "PKB.h"

void PKB::populateRS1ContainerInfo() {
	auto statements = Entity::getAllStmts();
	std::vector<StmtIndex> containerStmts;
	for (auto& stmt : statements) {
		if (Entity::isContainerStmt(stmt)) {
			containerStmts.push_back(stmt);
		}
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
