#pragma once

#include <unordered_set>

#include "../Common/Types.h"

class Constant {
private:
	std::unordered_set<ConstValue> constTable;

public:
	Constant();
	void insertConst(ConstValue constant);
	std::vector<ConstValue> getAllConsts();
};
