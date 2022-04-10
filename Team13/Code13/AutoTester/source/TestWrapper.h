#ifndef TESTWRAPPER_H
#define TESTWRAPPER_H

#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

// include your other headers here
#include "AbstractWrapper.h"
#include "../../source/PKB/PKBGetter.h"
#include "../../source/PKB/PKBInserter.h"
#include "../../source/SP/ASTValidator.h"
#include "../../source/SP/DesignExtractor.h"
#include "../../source/SP/Parser.h"
#include "../../source/QPS/Parser/PQLParser.h"
#include "../../source/QPS/Evaluator/PQLEvaluator.h"
#include "../../source/QPS/ResultProjector/PQLResultProjector.h"

class TestWrapper : public AbstractWrapper {
private:
	Parser parser;
	PKB pkb;
	PKBGetter* pkbGetter;
	PKBInserter* pkbInserter;
	std::string getFileContent(std::string& filename);

public:
	// default constructor
	TestWrapper();

	// destructor
	~TestWrapper();

	// method for parsing the SIMPLE source
	virtual void parse(std::string filename);

	// method for evaluating a query
	virtual void evaluate(std::string query, std::list<std::string>& results);
};

#endif
