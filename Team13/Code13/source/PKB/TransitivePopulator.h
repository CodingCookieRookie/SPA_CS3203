#pragma once

#include <string>
#include <vector>

#include "../Common/Types.h"
#include "./Calls.h"
#include "./CallsT.h"
#include "./Container.h"
#include "./Entity.h"
#include "./Follows.h"
#include "./FollowsT.h"
#include "./ModifiesP.h"
#include "./ModifiesS.h"
#include "./Parent.h"
#include "./ParentT.h"
#include "./UsesP.h"
#include "./UsesS.h"

class TransitivePopulator {
private:
	static void populateContainerInfo();
	static void populateTransitiveProcsInfo();
	static void populateTransitiveStmtsInfo();
public:
	static void populateRecursiveInfo();
};
