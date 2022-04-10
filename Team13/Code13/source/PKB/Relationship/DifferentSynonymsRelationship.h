#pragma once

#include "Relationship.h"

template<typename SynonymIndex>
class DifferentSynonymsRelationship : public Relationship<SynonymIndex, VarIndex> {
public:
	DifferentSynonymsRelationship();
};

template<typename SynonymIndex>
DifferentSynonymsRelationship<SynonymIndex>::DifferentSynonymsRelationship() {};
