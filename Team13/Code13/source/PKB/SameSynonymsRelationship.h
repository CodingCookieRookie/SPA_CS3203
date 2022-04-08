#pragma once

#include "Relationship.h"

template<typename SynonymIndex>
class SameSynonymsRelationship : public Relationship<SynonymIndex, SynonymIndex> {
public:
	SameSynonymsRelationship();
};

template<typename SynonymIndex>
SameSynonymsRelationship<SynonymIndex>::SameSynonymsRelationship() {};
