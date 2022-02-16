#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "./Container.h"
#include "./Entity.h"
#include "./Follows.h"
#include "./FollowsT.h"
#include "./Modifies.h"
#include "./Parent.h"
#include "./ParentT.h"
#include "./Uses.h"
#include "../common/Types.h"

class PKB {
private:
	static void populateFollowsT();
	static void populateParentT();
	static void populateRS1ContainerInfo();
public:
	static void populateRecursiveInfo();
};
