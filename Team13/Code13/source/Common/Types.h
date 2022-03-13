#pragma once

enum class StatementType {
	assignType, whileType, ifType, readType, printType, callType
};

enum class ExprNodeValueType {
	varName, constValue, arithmeticOperator, logicalOperator, relOperator
};

enum class PqlEntityType {
	Stmt, Read, Print, Call, While,
	If, Assign, Variable, Constant, Procedure
};

struct VarIndex {
	int index;

	VarIndex() :index() {}

	VarIndex(int index) {
		this->index = index;
	}

	bool operator==(const VarIndex& p) const {
		return index == p.index;
	}

	int getIndex() {
		return index;
	}
};

struct ProcIndex {
	int index;

	ProcIndex() :index() {}

	ProcIndex(int index) {
		this->index = index;
	}

	bool operator==(const ProcIndex& p) const {
		return index == p.index;
	}

	int getIndex() {
		return index;
	}
};

struct StmtIndex {
	int index;

	StmtIndex() :index() {}

	StmtIndex(int index) {
		this->index = index;
	}

	bool operator==(const StmtIndex& p) const {
		return index == p.index;
	}

	int getIndex() {
		return index;
	}
};

template<>
struct std::hash<VarIndex> {
	size_t operator()(const VarIndex& k) const {
		return std::hash<int>()(k.index);
	}
};

template<>
struct std::hash<ProcIndex> {
	size_t operator()(const ProcIndex& k) const {
		return std::hash<int>()(k.index);
	}
};

template<>
struct std::hash<StmtIndex> {
	size_t operator()(const StmtIndex& k) const {
		return std::hash<int>()(k.index);
	}
};
