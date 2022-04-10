#pragma once

#include <string>
#include <vector>

#include "../../Common/Types.h"
#include "../Attribute/Attribute.h"

class NameIndexEntityBase {
public:
	virtual NameIdxEntityIndex insert(std::string entityName, Attribute* attribute) = 0;
	virtual bool contains(std::string& entityName) = 0;
	virtual std::string getName(NameIdxEntityIndex entityIdx) = 0;
	virtual NameIdxEntityIndex getIndex(std::string entityName) = 0;
	virtual std::vector<NameIdxEntityIndex> getAll() = 0;
};
