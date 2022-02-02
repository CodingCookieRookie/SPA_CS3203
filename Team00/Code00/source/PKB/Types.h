#pragma once

#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <iostream>

using namespace std;

struct VAR_IDX {
	int index;

	VAR_IDX():index() {}

	VAR_IDX(int index) {
		this->index = index;
	}

	bool operator==(const VAR_IDX& p) const {
		return index == p.index;
	}

	struct hash_fn
	{
		size_t operator()(const VAR_IDX& k) const
		{
			return hash<int>()(k.index);
		}
	};
};

struct PROC_IDX {
	int index;

	PROC_IDX() :index() {}

	PROC_IDX(int index) {
		this->index = index;
	}

	bool operator==(const PROC_IDX& p) const {
		return index == p.index;
	}

	struct hash_fn
	{
		size_t operator()(const PROC_IDX& k) const
		{
			return hash<int>()(k.index);
		}
	};
};

struct STMT_IDX {
	int index;

	STMT_IDX() :index() {}

	STMT_IDX(int index) {
		this->index = index;
	}

	bool operator==(const STMT_IDX& p) const {
		return index == p.index;
	}

	struct hash_fn
	{
		size_t operator()(const STMT_IDX& k) const
		{
			return hash<int>()(k.index);
		}
	};
};

#endif