#pragma once

#ifndef FOLLOWS_H
#define FOLLOWS_H

#include "./RS2.h"

class Follows : public RS2<Follows> {
public:
	static void insert(StmtIndex predecessor, StmtIndex successor);
};

#endif
