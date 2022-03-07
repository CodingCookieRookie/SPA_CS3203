#include <string>
#include <vector>

#include "PKB.h"

void PKB::populateFollowsT() {
	auto followsPredSucTable = Follows::getPredSucTable();
	if (followsPredSucTable.size() > 0) {
		FollowsT::populate(followsPredSucTable);
	}
}

void PKB::populateParentT() {
	auto parentPredSucTable = Parent::getPredSucTable();
	if (parentPredSucTable.size() > 0) {
		ParentT::populate(parentPredSucTable);
	}
}

void PKB::populateCallsT() {
	auto callsPredSucTable = Calls::getPredSucTable();
	if (callsPredSucTable.size() > 0) {
		CallsT::populate(callsPredSucTable);
	}
}

void PKB::populateRS1ContainerInfo() {
	auto statements = Entity::getAllStmts();
	std::vector<StmtIndex> containerStmts;

	for (auto& stmt : statements) {
		if (Entity::isContainerStmt(stmt)) {
			containerStmts.push_back(stmt);
		}
	}

	for (auto& containerStmt : containerStmts) {
		auto& subStmts = Container::getStmtsInContainer(containerStmt);
		if (subStmts.size() > 0) {
			ModifiesS::populateFromSubSynonyms(containerStmt, subStmts);
			UsesS::populateFromSubSynonyms(containerStmt, subStmts);
		}
	}
}

void PKB::populateRS1TransitiveProcInfo() {
	auto calleeCallers = CallsT::getPredSucTable();
	for (auto& entry : calleeCallers) {
		auto caller = entry.first;
		auto callees = entry.second;
		if (callees.size() > 0) {
			ModifiesP::populateFromSubSynonyms(caller, callees);
			UsesP::populateFromSubSynonyms(caller, callees);
		}
	}
}

void PKB::populateRecursiveInfo() {
	Container::populate();
	populateFollowsT();
	populateParentT();
	populateCallsT();
	populateRS1ContainerInfo();
	populateRS1TransitiveProcInfo();
}
