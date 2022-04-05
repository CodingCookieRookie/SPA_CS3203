#pragma once

#include <string>
#include <vector>

#include "../Common/Types.h"
#include "./Attribute.h"
#include "./BidirectionalTable/BidirectionalIndexTable.h"

template <EntityType entityType, typename Index>
class NameIndexEntity {
private:
	BidirectionalIndexTable<Index> entityIdxBidirectionalTable;

public:
	NameIndexEntity();
	Index insert(std::string entityName);
	bool contains(std::string& entityName);
	std::string getName(Index idx);
	Index getIndex(std::string entityName);
	std::vector<Index> getAll();
};

template<EntityType entityType, typename Index>
NameIndexEntity<entityType, Index>::NameIndexEntity() {};

template<EntityType entityType, typename Index>
Index NameIndexEntity<entityType, Index>::insert(std::string entityName) {
	NameIndex nameIdx = Attribute::insertNameValue(entityName);

	if (!entityIdxBidirectionalTable.contains(entityName)) {
		Index entityIdx = entityIdxBidirectionalTable.insert(entityName);
		Attribute::insertNameIdxEntityByName(entityType, entityIdx, nameIdx);
		return entityIdx;
	}

	return entityIdxBidirectionalTable.getIndexFromString(entityName);
}

template <EntityType entityType, typename Index>
bool NameIndexEntity<entityType, Index>::contains(std::string& entityName) {
	return entityIdxBidirectionalTable.contains(entityName);
}

template<EntityType entityType, typename Index>
std::string NameIndexEntity<entityType, Index>::getName(Index entityIdx) {
	return entityIdxBidirectionalTable.getStringFromIndex(entityIdx);
}

template<EntityType entityType, typename Index>
Index NameIndexEntity<entityType, Index>::getIndex(std::string entityName) {
	return entityIdxBidirectionalTable.getIndexFromString(entityName);
}

template<EntityType entityType, typename Index>
std::vector<Index> NameIndexEntity<entityType, Index>::getAll() {
	return entityIdxBidirectionalTable.getAll();
}
