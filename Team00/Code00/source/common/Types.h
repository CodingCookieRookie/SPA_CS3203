#pragma once

#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <iostream>

using namespace std;

enum class StatementType {
	assignType, whileType, ifType, readType, printType, callType
};

struct VarIndex {
	int index;

	VarIndex():index() {}

	VarIndex(int index) {
		this->index = index;
	}

	bool operator==(const VarIndex& p) const {
		return index == p.index;
	}

	struct HashFunction {
		size_t operator()(const VarIndex& k) const {
			return hash<int>()(k.index);
		}
	};
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

	struct HashFunction {
		size_t operator()(const ProcIndex& k) const {
			return hash<int>()(k.index);
		}
	};
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

	struct HashFunction {
		size_t operator()(const StmtIndex& k) const {
			return hash<int>()(k.index);
		}
	};
};

#endif
