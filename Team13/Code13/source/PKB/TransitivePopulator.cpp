//#include <string>
//#include <vector>
//
//#include "TransitivePopulator.h"
//
//void TransitivePopulator::populateContainerInfo() {
//	std::vector<StmtIndex> containerStmtsIndices = pkbGetter->getAllContainerStmts();
//
//	Container::populate();
//	for (auto& containerStmtIndex : containerStmtsIndices) {
//		auto& subStmtsIndices = Container::getStmtsInContainer(containerStmtIndex);
//		if (subStmtsIndices.size() > 0) {
//			ModifiesS::populateFromSubSynonyms(containerStmtIndex, subStmtsIndices);
//			UsesS::populateFromSubSynonyms(containerStmtIndex, subStmtsIndices);
//		}
//	}
//}
//
//void TransitivePopulator::populateTransitiveStmtsInfo() {
//	FollowsT::populate();
//	ParentT::populate();
//}
//
//void TransitivePopulator::populateRS1ProcsFromStmts() {
//	auto procStmtsInfo = pkbGetter->getAllProcStmts();
//	for (size_t idx = 0; idx < std::get<0>(procStmtsInfo).size(); idx++) {
//		auto procIndex = std::get<0>(procStmtsInfo).at(idx);
//		auto stmtIndex = std::get<1>(procStmtsInfo).at(idx);
//
//		std::vector<VarIndex> usesVarIndices = UsesS::getFromLeftArg(stmtIndex);
//		for (VarIndex varIndex : usesVarIndices) {
//			UsesP::insert(procIndex, varIndex);
//		}
//
//		std::vector<VarIndex> modifiesVarIndices = ModifiesS::getFromLeftArg(stmtIndex);
//		for (VarIndex varIndex : modifiesVarIndices) {
//			ModifiesP::insert(procIndex, varIndex);
//		}
//	}
//}
//
//void TransitivePopulator::populateRS1ProcsFromNestedCalls() {
//	CallsT::populate();
//	auto calleeCallers = CallsT::getPredSucTable();
//	for (auto& entry : calleeCallers) {
//		auto callerProcIndex = entry.first;
//		auto calleeProcsIndices = entry.second;
//		if (calleeProcsIndices.size() > 0) {
//			ModifiesP::populateFromSubSynonyms(callerProcIndex, calleeProcsIndices);
//			UsesP::populateFromSubSynonyms(callerProcIndex, calleeProcsIndices);
//		}
//	}
//}
//
//void TransitivePopulator::populateRS1TransitiveCallsStmts() {
//	auto callStmtsIndices = pkbGetter->getStmtIdxFromType(StatementType::CALL_TYPE);
//	for (auto callStmtIndex : callStmtsIndices) {
//		std::string procName = pkbGetter->getAttributeNameByStmtIdx(callStmtIndex);
//		ProcIndex procIndex = pkbGetter->getNameIdxEntityIndex(EntityType::PROCEDURE, procName);
//
//		std::vector<VarIndex> usesVarIndices = UsesP::getFromLeftArg(procIndex);
//		for (VarIndex varIndex : usesVarIndices) {
//			UsesS::insert(callStmtIndex, varIndex);
//		}
//
//		std::vector<VarIndex> modifiesVarIndices = ModifiesP::getFromLeftArg(procIndex);
//		for (VarIndex varIndex : modifiesVarIndices) {
//			ModifiesS::insert(callStmtIndex, varIndex);
//		}
//	}
//
//	populateContainerInfo();
//}
//
//void TransitivePopulator::populateTransitiveProcsInfo() {
//	populateRS1ProcsFromStmts();
//	populateRS1ProcsFromNestedCalls();
//	populateRS1TransitiveCallsStmts();
//}
//
//void TransitivePopulator::populateRecursiveInfo() {
//	populateContainerInfo();
//	populateTransitiveStmtsInfo();
//	populateTransitiveProcsInfo();
//}
