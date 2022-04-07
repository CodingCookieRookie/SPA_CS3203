#pragma once

#include "NextTProcessor.h"
#include "AffectsProcessor.h"
#include "AffectsTProcessor.h"

class Processors {
private:
	NextTProcessor* nextTProcessor;
	AffectsProcessor* affectsProcessor;
	AffectsTProcessor* affectsTProcessor;

public:
	Processors(NextTProcessor* nextTProcessor, AffectsProcessor* affectsProcessor, AffectsTProcessor* affectsTProcessor);
	NextTProcessor* getNextTProcessor();
	AffectsProcessor* getAffectsProcessor();
	AffectsTProcessor* getAffectsTProcessor();

};
