#ifndef TESTWRAPPER_H
#define TESTWRAPPER_H

#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

// include your other headers here
#include "AbstractWrapper.h"
#include "SP/DesignExtractor.h"
#include "SP/Parser.h"
#include "QPS-NEW/PQLParser.h"
#include "QPS-NEW/PQLEvaluator.h"
#include "QPS-NEW/PQLResultProjector.h"

class TestWrapper : public AbstractWrapper {
private:
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
