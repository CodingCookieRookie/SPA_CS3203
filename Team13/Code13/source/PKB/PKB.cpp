#include "./PKB.h"

PKB::PKB() {
	patternAssign = PatternAssign();
	containerEntityToPatternMap[EntityType::IF] = PatternIf();
	containerEntityToPatternMap[EntityType::WHILE] = PatternWhile();
}

PKB::~PKB() {}
