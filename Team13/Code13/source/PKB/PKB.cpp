#include <string>
#include <vector>

#include "PKB.h"

void PKB::populateContainerInfo() {
	std::vector<StmtIndex> containerStmts = Entity::getAllContainerStmts();

	Container::populate();
	for (auto& containerStmt : containerStmts) {
		auto& subStmts = Container::getStmtsInContainer(containerStmt);
		if (subStmts.size() > 0) {
			ModifiesS::populateFromSubSynonyms(containerStmt, subStmts);
			UsesS::populateFromSubSynonyms(containerStmt, subStmts);
		}
	}
}

void PKB::populateTransitiveStmtsInfo() {
	FollowsT::populate();
	ParentT::populate();
}

void PKB::populateTransitiveProcsInfo() {
	CallsT::populate();
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
	populateContainerInfo();
	populateTransitiveStmtsInfo();
	populateTransitiveProcsInfo();
}
