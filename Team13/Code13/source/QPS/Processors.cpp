#include "Processors.h"

Processors::Processors(
	NextTProcessor* nextTProcessor, AffectsProcessor* affectsProcessor, AffectsTProcessor* affectsTProcessor) :
	nextTProcessor(nextTProcessor),
	affectsProcessor(affectsProcessor),
	affectsTProcessor(affectsTProcessor) {}

NextTProcessor* Processors::getNextTProcessor() {
	return nextTProcessor;
}

AffectsProcessor* Processors::getAffectsProcessor() {
	return affectsProcessor;
}

AffectsTProcessor* Processors::getAffectsTProcessor() {
	return affectsTProcessor;
}
