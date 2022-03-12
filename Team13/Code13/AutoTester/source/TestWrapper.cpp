#include "TestWrapper.h"
#include "AbstractWrapper.h"


// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
  if (wrapper == 0) wrapper = new TestWrapper;
  return wrapper;
}
// Do not modify the following line
volatile bool AbstractWrapper::GlobalStop = false;
AbstractWrapper::~AbstractWrapper() = default;

// a default constructor
TestWrapper::TestWrapper() {
	// create any objects here as instance variables of this class
	// as well as any initialization required for your spa program
}

TestWrapper::~TestWrapper() = default;

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
	// call your parser to do the parsing
	// ...rest of your code...
	std::string fileContent = getFileContent(filename);

	try {
		SourceAST ast = Parser::parse(fileContent);
		DesignExtractor::extract(ast);
	} catch (ParserException& ex) {
		std::cerr << ex.what() << std::endl;
		exit(EXIT_FAILURE);
	} catch (LexerException& ex) {
		std::cerr << ex.what() << std::endl;
		exit(EXIT_FAILURE);
	} catch (ExpressionException& ex) {
		std::cerr << ex.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

std::string TestWrapper::getFileContent(std::string& filename) {
	std::ifstream inputFile(filename);
	if (!inputFile.is_open()) {
		exit(EXIT_FAILURE);
	}
	return std::string((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results){
	// call your evaluator to evaluate the query here
	// ...code to evaluate query...
	try {
		ParsedQuery parsedQuery = PQLParser::parseQuery(query);
		EvaluatedTable evTable = PQLEvaluator::evaluate(parsedQuery);
		EvaluatedTable projectedEvTable = PQLEvaluator::selectProjection(evTable, parsedQuery);
		results = PQLResultProjector::resolveTableToResults(projectedEvTable, parsedQuery.getColumns(), parsedQuery.getDeclarations());
	} catch (QPSException& ex) {
		return;
	} catch (LexerException& ex) {
		return;
	} catch (ExpressionException& ex) {
		return;
	}
	// store the answers to the query in the results list (it is initially empty)
	// each result must be a string.
}

