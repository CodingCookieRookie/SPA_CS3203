#ifndef TESTWRAPPER_H
#define TESTWRAPPER_H

#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

// include your other headers here
#include "AbstractWrapper.h"
#include "PKB/PKBGetter.h"
#include "PKB/PKBInserter.h"
#include "SP/ASTValidator.h"
#include "SP/DesignExtractor.h"
#include "SP/Parser.h"
#include "QPS/PQLParser.h"
#include "QPS/PQLEvaluator.h"
#include "QPS/PQLResultProjector.h"

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
