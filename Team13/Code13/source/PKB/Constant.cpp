#include "./Constant.h"

Constant::Constant() {}

void Constant::insertConst(ConstValue constant) {
	constTable.insert(constant);
}

std::vector<ConstValue> Constant::getAllConsts() {
	std::vector<ConstValue> res;

	for (auto& constant : constTable) {
		res.push_back(constant);
	}

	return res;
}
