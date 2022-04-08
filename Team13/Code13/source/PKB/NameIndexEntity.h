#pragma once

#include "./BidirectionalTable/BidirectionalIndexTable.h"
#include "./NameIndexEntityBase.h"

template <EntityType entityType>
class NameIndexEntity : public NameIndexEntityBase {
private:
	BidirectionalIndexTable<NameIdxEntityIndex> entityIdxBidirectionalTable;

public:
	NameIndexEntity();
	~NameIndexEntity();
	NameIdxEntityIndex insert(std::string entityName, Attribute* attribute) override;
	bool contains(std::string& entityName) override;
	std::string getName(NameIdxEntityIndex entityIdx) override;
	NameIdxEntityIndex getIndex(std::string entityName) override;
	std::vector<NameIdxEntityIndex> getAll() override;
};

template<EntityType entityType>
NameIndexEntity<entityType>::NameIndexEntity() {};

template<EntityType entityType>
NameIndexEntity<entityType>::~NameIndexEntity() {};

template<EntityType entityType>
NameIdxEntityIndex NameIndexEntity<entityType>::insert(std::string entityName, Attribute* attribute) {
	NameIndex nameIdx = attribute->insertNameValue(entityName);

	if (!entityIdxBidirectionalTable.contains(entityName)) {
		NameIdxEntityIndex entityIdx = entityIdxBidirectionalTable.insert(entityName);
		attribute->insertNameIdxEntityByName(entityType, entityIdx, nameIdx);
		return entityIdx;
	}

	return entityIdxBidirectionalTable.getIndexFromString(entityName);
}

template <EntityType entityType>
bool NameIndexEntity<entityType>::contains(std::string& entityName) {
	return entityIdxBidirectionalTable.contains(entityName);
}

template<EntityType entityType>
std::string NameIndexEntity<entityType>::getName(NameIdxEntityIndex entityIdx) {
	return entityIdxBidirectionalTable.getStringFromIndex(entityIdx);
}

template<EntityType entityType>
NameIdxEntityIndex NameIndexEntity<entityType>::getIndex(std::string entityName) {
	return entityIdxBidirectionalTable.getIndexFromString(entityName);
}

template<EntityType entityType>
std::vector<NameIdxEntityIndex> NameIndexEntity<entityType>::getAll() {
	return entityIdxBidirectionalTable.getAll();
}
