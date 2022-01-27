using namespace std;

#include <iostream>
#include <string>


class PQLAST
{
private:
	vector<string> PQLAST;
public:
	void addToAst(string);
	vector<string> getPQLAST();
};