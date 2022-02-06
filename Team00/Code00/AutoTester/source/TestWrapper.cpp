#include "TestWrapper.h"
#include "../source/QPS-NEW/PQLParser.h"
#include "../source/QPS-NEW/PQLEvaluator.h"
#include "../source/QPS-NEW/PQLResultProjector.h"

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
	if (wrapper == 0) wrapper = new TestWrapper;
	return wrapper;
}
// Do not modify the following line
volatile bool TestWrapper::GlobalStop = false;

// a default constructor
TestWrapper::TestWrapper() {
	// create any objects here as instance variables of this class
	// as well as any initialization required for your spa program
}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
	// call your parser to do the parsing
  // ...rest of your code...
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results) {
	// call your evaluator to evaluate the query here
	  // ...code to evaluate query...
	PQLParser pqlParser = PQLParser(query);
	ParsedQuery parsedQuery = pqlParser.parseQuery();
	PQLEvaluator pqlEvaluator = PQLEvaluator(parsedQuery);
	EvaluatedTable evTable = pqlEvaluator.evaluate();
	PQLResultProjector resultProjector = PQLResultProjector(evTable);
	results = resultProjector.resolveTableToResults();

	  // store the answers to the query in the results list (it is initially empty)
	  // each result must be a string.
}
