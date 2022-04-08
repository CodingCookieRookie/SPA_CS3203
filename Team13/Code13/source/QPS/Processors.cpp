#include "Processors.h"

Processors::Processors(
	NextTProcessor* nextTProcessor, AffectsProcessor* affectsProcessor, AffectsTProcessor* affectsTProcessor) :
	nextTProcessor(nextTProcessor),
	affectsProcessor(affectsProcessor),
	affectsTProcessor(affectsTProcessor) {}

NextTProcessor* Processors::getNextTProcessor() const {
	return nextTProcessor;
}

AffectsProcessor* Processors::getAffectsProcessor() const {
	return affectsProcessor;
}

AffectsTProcessor* Processors::getAffectsTProcessor() const {
	return affectsTProcessor;
}
