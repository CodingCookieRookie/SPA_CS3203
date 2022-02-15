#pragma once

#ifndef PARENT_H
#define PARENT_H

#include "./RS2.h"

class Parent : public RS2<Parent> {
public:
	static void insert(StmtIndex predecessor, StmtIndex successor);
};

#endif
