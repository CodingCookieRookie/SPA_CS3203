#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include <algorithm>

#include "../source/QPS-NEW/PQLEvaluator.h"
#include "../source/QPS-NEW/PQLParser.h"
#include "../source/PKB/RS2.h"
#include "../source/PKB/Follows.h"
#include "../source/PKB/FollowsT.h"
#include "../source/PKB/Parent.h"
#include "../source/PKB/ParentT.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	// Modifies (a/r/s/a1, v) or Modifies(a/r/s/a1, "x) or Modifies (a/r/s/a1, _ ) 
	// Modifies (1, v)	=> true or Modifies (1, _ ) (under statement)
	// Modifies (p/p1, v)	or Modifies (p/p1, _ )	proc
	TEST_CLASS(TestInstructionModifies)
	{
	private:
		TEST_METHOD_CLEANUP(cleanUpTables) {
			Entity::performCleanUp();
			FollowsT::performCleanUp();
			Follows::performCleanUp();
			ParentT::performCleanUp();
			Parent::performCleanUp();
		}
	public:
	};
}
