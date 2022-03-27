#include <string>
#include <vector>

#include "TransitivePopulator.h"

void TransitivePopulator::populateContainerInfo() {
	std::vector<StmtIndex> containerStmtsIndices = Entity::getAllContainerStmts();

	Container::populate();
	for (auto& containerStmtIndex : containerStmtsIndices) {
		auto& subStmtsIndices = Container::getStmtsInContainer(containerStmtIndex);
		if (subStmtsIndices.size() > 0) {
			ModifiesS::populateFromSubSynonyms(containerStmtIndex, subStmtsIndices);
			UsesS::populateFromSubSynonyms(containerStmtIndex, subStmtsIndices);
		}
	}
}

void TransitivePopulator::populateTransitiveStmtsInfo() {
	FollowsT::populate();
	ParentT::populate();
}

void TransitivePopulator::populateRS1ProcsFromStmts() {
	auto procStmtsTable = Entity::getAllProcStmts();
	for (auto entry : procStmtsTable) {
		auto procIndex = entry.first;
		auto stmtIndices = entry.second;

		for (auto stmtIndex : stmtIndices) {
			std::vector<VarIndex> usesVarIndices = UsesS::getVariables(stmtIndex);
			for (VarIndex varIndex : usesVarIndices) {
				UsesP::insert(procIndex, varIndex);
			}

			std::vector<VarIndex> modifiesVarIndices = ModifiesS::getVariables(stmtIndex);
			for (VarIndex varIndex : modifiesVarIndices) {
				ModifiesP::insert(procIndex, varIndex);
			}
		}
	}
}

void TransitivePopulator::populateRS1ProcsFromNestedCalls() {
	CallsT::populate();
	auto calleeCallers = CallsT::getPredSucTable();
	for (auto& entry : calleeCallers) {
		auto callerProcIndex = entry.first;
		auto calleeProcsIndices = entry.second;
		if (calleeProcsIndices.size() > 0) {
			ModifiesP::populateFromSubSynonyms(callerProcIndex, calleeProcsIndices);
			UsesP::populateFromSubSynonyms(callerProcIndex, calleeProcsIndices);
		}
	}
}

void TransitivePopulator::populateRS1TransitiveCallsStmts() {
	auto callStmtsIndices = Entity::getStmtIdxFromType(StatementType::CALL_TYPE);
	for (auto callStmtIndex : callStmtsIndices) {
		std::string procName = Attribute::getAttributeNameByStmtIdx(callStmtIndex);
		ProcIndex procIndex = Entity::getProcIdx(procName);

		std::vector<VarIndex> usesVarIndices = UsesP::getVariables(procIndex);
		for (VarIndex varIndex : usesVarIndices) {
			UsesS::insert(callStmtIndex, varIndex);
		}

		std::vector<VarIndex> modifiesVarIndices = ModifiesP::getVariables(procIndex);
		for (VarIndex varIndex : modifiesVarIndices) {
			ModifiesS::insert(callStmtIndex, varIndex);
		}
	}

	populateContainerInfo();
}

void TransitivePopulator::populateTransitiveProcsInfo() {
	populateRS1ProcsFromStmts();
	populateRS1ProcsFromNestedCalls();
	populateRS1TransitiveCallsStmts();
}

void TransitivePopulator::populateRecursiveInfo() {
	populateContainerInfo();
	populateTransitiveStmtsInfo();
	populateTransitiveProcsInfo();
}
