#pragma once

#include <string>
#include <vector>

#include "../Common/Types.h"
#include "./Container.h"
#include "./Entity.h"
#include "./Follows.h"
#include "./FollowsT.h"
#include "./Modifies.h"
#include "./Parent.h"
#include "./ParentT.h"
#include "./Uses.h"

class PKB {
private:
	static void populateFollowsT();
	static void populateParentT();
	static void populateRS1ContainerInfo();
public:
	static void populateRecursiveInfo();
};
