#pragma once

#ifndef FOLLOWST_H
#define FOLLOWST_H

#include "./RS2.h"
#include "./Follows.h"

class FollowsT : public RS2<FollowsT> {
private:
	static void insert(StmtIndex predecessor, std::unordered_set<StmtIndex, StmtIndex::HashFunction> successors);
	static std::unordered_set<StmtIndex, StmtIndex::HashFunction> getAllSuccessors(StmtIndex predecessor);
public:
	static void populate();
};

#endif
